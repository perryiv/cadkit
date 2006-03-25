
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
//  GLFeedback: The feedback buffer renderer class.
//
/////////////////////////////////////////////////////////////////////////////

#include "OsgTools/IO/GLFeedback.h"

#include "Usul/Errors/Assert.h"
#include "Usul/Headers/OpenGL.h"
#include "Usul/Math/MinMax.h"

#include "osg/Vec3d"

#include <cstdlib>
#include <memory.h>

using namespace OsgTools::IO;

/////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
/////////////////////////////////////////////////////////////////////////////

GLFeedback::GLFeedback() : 
	sortDirection ( BACK_TO_FRONT ),
	sortOption ( MIN_Z ),
	backFaces ( true ),
	_primitive ( 0x0 ),
	_numPrimitives ( 0 ),
	_maxBufferSize ( 0 ),
	_actualBufferSize ( 0 ),
	_buffer ( 0x0 ),
	_clientSuppliedBuffer ( false )
{
}


/////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
/////////////////////////////////////////////////////////////////////////////

GLFeedback::~GLFeedback()
{
	// Delete the arrays.
	this->_deleteBuffer();
	this->_deletePrimitives();
}


/////////////////////////////////////////////////////////////////////////////
//
//  Delete the feedback buffer if it's ours.
//
/////////////////////////////////////////////////////////////////////////////

void GLFeedback::_deleteBuffer()
{
	// If we have a feedback buffer and it's ours...
	if ( _buffer && !_clientSuppliedBuffer )
	{
		// Should be true.
		USUL_ASSERT ( _maxBufferSize > 0 );

		// Delete the buffer. Use free() because we used calloc() to allocate it.
		::free ( _buffer );

		// Reset these.
		_buffer = 0x0;
		_maxBufferSize = 0;
		_actualBufferSize = 0;
	}
}



/////////////////////////////////////////////////////////////////////////////
//
//  Delete the primitive array.
//
/////////////////////////////////////////////////////////////////////////////

void GLFeedback::_deletePrimitives()
{
	// Delete the array if it's not NULL.
	if ( _primitive ) delete [] _primitive;

	// Reset these.
	_primitive = 0x0;
	_numPrimitives = 0;
}


/////////////////////////////////////////////////////////////////////////////
//
//  Calculate the size the feedback buffer needs to be.
//
/////////////////////////////////////////////////////////////////////////////

bool GLFeedback::_calculateBufferSize ( int &size ) const
{
	size = 524288 * 8; // TODO. I was thinking of a SgGetVertexCount visitor.
	return true;
}


/////////////////////////////////////////////////////////////////////////////
//
//  Count the primitives in the feedback buffer. Return -1 if an error.
//	It's ok to have zero.
//
/////////////////////////////////////////////////////////////////////////////

int GLFeedback::_countPrimitives() const
{
	USUL_ASSERT ( _maxBufferSize > 0 );
	USUL_ASSERT ( _actualBufferSize >= 0 && _actualBufferSize <= _maxBufferSize );
	USUL_ASSERT ( _buffer );

	// Initialize.
	int numPrimitives = 0, numVertices, token;
	GLfloat *buffer = _buffer;
	GLfloat *endOfBuffer = _buffer + _actualBufferSize;

	// While we're not at the end of the buffer...
	while ( buffer < endOfBuffer )
	{
		// Get the token and increment the buffer pointer.
		token = static_cast < int > ( *buffer );
		++buffer;

		// Switch off the token.
		switch ( token )
		{
		case GL_LINE_TOKEN:
		case GL_LINE_RESET_TOKEN:

			buffer += 14;					// Offset.
			++numPrimitives;				// Incerment count.
			break;

		case GL_POLYGON_TOKEN:

			numVertices = static_cast < int > ( *buffer );			// Vertex count in polygon.
			++buffer;						// Offset for the number of vertices.
			buffer += ( 7 * numVertices );	// Offset for the actual vertices.
			++numPrimitives;				// Increment count.
			break;

		case GL_POINT_TOKEN:

			buffer += 7;					// Offset.
			++numPrimitives;				// Incerment count.
			break;

		case GL_PASS_THROUGH_TOKEN:

			++buffer;						// Ignore it.
			break;

		default:

			USUL_ASSERT ( 0 ); // What token is this?
			return -1;
		}
	}

	// Return the count. It's ok for it to be zero.
	return numPrimitives;
}


/////////////////////////////////////////////////////////////////////////////
//
//  Calculate the debth of each primitve. This also assigns the pointers in 
//	the primitve array to the dta in the feedback buffer.
//
/////////////////////////////////////////////////////////////////////////////

void GLFeedback::_calculatePrimitiveDebths()
{
	USUL_ASSERT ( _maxBufferSize > 0 );
	USUL_ASSERT ( _actualBufferSize >= 0 && _actualBufferSize <= _maxBufferSize );
	USUL_ASSERT ( _buffer );
	USUL_ASSERT ( _primitive );
	USUL_ASSERT ( _numPrimitives >= 0 && _numPrimitives < _actualBufferSize );

	// Declare and initialize.
	int index = 0, token, numVertices;//, i;
	GLfloat *buffer = _buffer;//, temp;
	GLfloat *endOfBuffer = _buffer + _actualBufferSize;
	GL3DColorFeedback *vertex;
  osg::Vec3d v01, v12, normal;

	// While we're not at the end of the buffer...
	while ( buffer < endOfBuffer )
	{
		// Save this primitive's location.
		_primitive[index].buffer = buffer;

		// Get the token and increment the buffer pointer.
		token = static_cast < int > ( *buffer );
		++buffer;

		// Switch off the token.
		switch ( token )
		{
		case GL_LINE_TOKEN:
		case GL_LINE_RESET_TOKEN:

			// Get the vertex.
			vertex = (GL3DColorFeedback *) buffer;

			// Calculate the "debth".
			switch ( sortOption )
			{
			case AVERAGE:	_primitive[index].depth = ( vertex[0].z + vertex[1].z ) * 0.5f; break;
			case MIN_Z:		_primitive[index].depth = Usul::Math::minimum ( vertex[0].z, vertex[1].z ); break;
			case MAX_Z:		_primitive[index].depth = Usul::Math::maximum ( vertex[0].z, vertex[1].z ); break;
			default : USUL_ASSERT ( 0 );
			}

			buffer += 14;	// Offset.
			++index;		// Increment the primitive counter.
			break;

		case GL_POLYGON_TOKEN:

			numVertices = static_cast < int > ( *buffer );						// Get the number of vertices.
			++buffer;									// Increment the buffer pointer.
			vertex = (GL3DColorFeedback *) buffer;		// Grab the first vertex.

			// First time this isn't true I will know that I need to use a for-loop.
			USUL_ASSERT ( numVertices == 3 );

			// Calculate the "debth".
			switch ( sortOption )
			{
			case AVERAGE: _primitive[index].depth = ( vertex[0].z + vertex[1].z + vertex[2].z ) / numVertices; break;
			case MIN_Z:	  _primitive[index].depth = Usul::Math::minimum ( vertex[0].z, vertex[1].z, vertex[2].z ); break;
			case MAX_Z:   _primitive[index].depth = Usul::Math::maximum ( vertex[0].z, vertex[1].z, vertex[2].z ); break;
			default: USUL_ASSERT ( 0 );
			}

			// If the flag says to then determine if the triangle is facing the view.
			if ( !backFaces )
			{
				// These are the vectors from vertex 0 to vertex 1, and vertex 1 to vertex 2.
				v01.set ( vertex[1].x - vertex[0].x, vertex[1].y - vertex[0].y, vertex[1].z - vertex[0].z );
				v12.set ( vertex[2].x - vertex[1].x, vertex[2].y - vertex[1].y, vertex[2].z - vertex[1].z );
				
				// Calculate the normal for the triangle from the vectors.
				//v12.cross ( v01, normal );
				normal = v01 ^ v12;

				// We use this triangle iff the normal is facing the view (i.e., if the z-value is positive).
				if ( normal[2] > 0.0 ) ++index;
				else --_numPrimitives;
			}

			// If we are not culling the back faces then we always use this triangle.
			else ++index;

			// Offset to the next primitive.
			buffer += ( 7 * numVertices );
			break;

		case GL_POINT_TOKEN:

			vertex = (GL3DColorFeedback *) buffer;		// Get the buffer.
			_primitive[index].depth = vertex[0].z;		// Assign the debth.
			buffer += 7;								// Offset.
			++index;									// Increment the primitive counter.
			break;

		case GL_PASS_THROUGH_TOKEN:

			++buffer;									// Ignore it.
			break;

		default:

			// This should have been caught earlier.
			USUL_ASSERT ( 0 );
		}
	}

	// Should be true.
	USUL_ASSERT ( _numPrimitives >= 0 ); // Could have back-face culled them all away.
	USUL_ASSERT ( index == _numPrimitives );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Reallocate the feedback buffer if needed.
//
/////////////////////////////////////////////////////////////////////////////

bool GLFeedback::_reallocateBufferIfNeeded ( int &size )
{
	USUL_ASSERT ( !_clientSuppliedBuffer );
	USUL_ASSERT ( size >= 0 );

	// If the size is different...
	if ( size != _maxBufferSize )
	{
		// Delete the existing buffer.
		this->_deleteBuffer();

		// Allocate the buffer. 
		// We can't use calloc() because it is not cross platform.
		// If the new size is zero then we just set to NULL.
		GLfloat *buffer = (GLfloat *) ::malloc ( size * sizeof (GLfloat) );
		
		// If we have a buffer, set all values to zero.
		if( buffer )
			::memset ( buffer, 0, size );

		// Check allocation.
		if ( size > 0 && buffer == 0x0 )
		{
			USUL_ASSERT ( 0 ); // Too many vertices?
			return false;
		}

		// Set these.
		_maxBufferSize = size;
		_actualBufferSize = 0;
		_buffer = buffer;
	}

	// If we get here then we did good.
	return true;
}


/////////////////////////////////////////////////////////////////////////////
//
//  Reallocate the feedback buffer if needed.
//
/////////////////////////////////////////////////////////////////////////////

bool GLFeedback::_reallocatePrimitivesIfNeeded ( int &numPrimitives )
{
	USUL_ASSERT ( !_clientSuppliedBuffer );
	USUL_ASSERT ( numPrimitives >= 0 );
	USUL_ASSERT ( numPrimitives < _actualBufferSize );

	// If the size is different...
	if ( numPrimitives != _numPrimitives )
	{
		// Delete the existing buffer.
		this->_deletePrimitives();

		// Allocate the buffer. If the new size is zero then we just set to NULL.
		Primitive *primitive = ( numPrimitives > 0 ) ? new Primitive[numPrimitives] : 0x0;

		// Check allocation.
		if ( numPrimitives > 0 && primitive == 0x0 )
		{
			USUL_ASSERT ( 0 ); // Too many primitives?
			return false;
		}

		// Set these.
		_numPrimitives = numPrimitives;
		_primitive = primitive;
	}

	// If we get here then we did good.
	return true;
}


/////////////////////////////////////////////////////////////////////////////
//
//  Initialize the next rendering.
//
/////////////////////////////////////////////////////////////////////////////

GLFeedback::Result GLFeedback::preRender()
{
	// Initialize this.
	_actualBufferSize = 0;

	// If the client did not supply a feedback buffer...
	if ( !_clientSuppliedBuffer )
	{
		// Get the total vertex count.
		int bufferSize;
		if ( !this->_calculateBufferSize ( bufferSize ) ) return FAILED_TO_CALCULATE_BUFFER_SIZE;

		// Reallocate the buffer if we need to.
		if ( !this->_reallocateBufferIfNeeded ( bufferSize ) ) return FAILED_TO_ALLOCATE_BUFFER;
	}

	// When we get to here we should have a good buffer.
	USUL_ASSERT ( _maxBufferSize > 0 );
	USUL_ASSERT ( _actualBufferSize == 0 );
	USUL_ASSERT ( _buffer );

	// Set the feedback buffer.
	::glFeedbackBuffer ( _maxBufferSize, GL_3D_COLOR, _buffer );

	// Switch to feedback mode. Should return zero indicating that we 
	// were in rendering mode.
	USUL_ASSERT ( ::glRenderMode ( GL_FEEDBACK ) == 0 );

	// It worked!
	return NO_RENDER_ERROR;
}


/////////////////////////////////////////////////////////////////////////////
//
//  Finalize the rendering.
//
/////////////////////////////////////////////////////////////////////////////

GLFeedback::Result GLFeedback::postRender()
{
	USUL_ASSERT ( _maxBufferSize > 0 );
	USUL_ASSERT ( _actualBufferSize == 0 );
	USUL_ASSERT ( _buffer );

	// Draw everything right now.
	::glFlush();

	// Return to rendering mode. This will return the number of values in 
	// the feedback buffer.
	_actualBufferSize = ::glRenderMode ( GL_RENDER );

	// Should be true.
	USUL_ASSERT ( _actualBufferSize <= _maxBufferSize );

	// Check for errors. It's ok if it's zero. 
	// That just means that the window is blank.
	if ( _actualBufferSize < 0 )
	{
		return BUFFER_TOO_SMALL;
	}

	// Prepare the array of primitive.
	return ( this->_preparePrimitives() );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Prepare the primitive array.
//
/////////////////////////////////////////////////////////////////////////////

GLFeedback::Result GLFeedback::_preparePrimitives()
{
	USUL_ASSERT ( _maxBufferSize > 0 );
	USUL_ASSERT ( _actualBufferSize >= 0 && _actualBufferSize <= _maxBufferSize );
	USUL_ASSERT ( _buffer );

	// Count the primitives and check for errors.
	int numPrimitives = this->_countPrimitives();
	if ( numPrimitives < 0 ) return FAILED_TO_COUNT_PRIMITIVES;
 
	// Allocate the primitive array if needed.
	if ( !this->_reallocatePrimitivesIfNeeded ( numPrimitives ) ) return FAILED_TO_ALLOCATE_PRIMITIVES;

	// Should be true.
	USUL_ASSERT ( _primitive );
	USUL_ASSERT ( _numPrimitives == numPrimitives );

	// Calculate the average debth of each primitive.
	// This will also assign the primitive pointers into the buffer.
	this->_calculatePrimitiveDebths();

	// If we are supposed to sort...
		 if ( sortDirection == BACK_TO_FRONT ) ::qsort ( _primitive, _numPrimitives, sizeof ( Primitive ), &GLFeedback::_compareBackToFront );
	else if ( sortDirection == FRONT_TO_BACK ) ::qsort ( _primitive, _numPrimitives, sizeof ( Primitive ), &GLFeedback::_compareFrontToBack );

	// Note: if we don't sort then the primitve array is good as is.

	// It worked!
	return NO_RENDER_ERROR;
}


/////////////////////////////////////////////////////////////////////////////
//
//  Callback for qsort() to sort the array of primitives.
//
/////////////////////////////////////////////////////////////////////////////

int GLFeedback::_compareBackToFront ( const void *a, const void *b )
{
	USUL_ASSERT ( a && b );

	// Get the primitives.
	Primitive *primA = (Primitive *) a;
	Primitive *primB = (Primitive *) b;

	// Calculate the difference in their debths.
	GLfloat diff = primB->depth - primA->depth;

	// Return the result.
	if ( diff > 0.0 ) return 1;
	else if ( diff < 0.0 ) return -1;
	else return 0;
}


/////////////////////////////////////////////////////////////////////////////
//
//  Callback for qsort() to sort the array of primitives.
//
/////////////////////////////////////////////////////////////////////////////

int GLFeedback::_compareFrontToBack ( const void *a, const void *b )
{
	USUL_ASSERT ( a && b );

	// Get the primitives.
	Primitive *primA = (Primitive *) a;
	Primitive *primB = (Primitive *) b;

	// Calculate the difference in their debths.
	GLfloat diff = primB->depth - primA->depth;

	// Return the result.
	if ( diff > 0.0 ) return -1;
	else if ( diff < 0.0 ) return 1;
	else return 0;
}

