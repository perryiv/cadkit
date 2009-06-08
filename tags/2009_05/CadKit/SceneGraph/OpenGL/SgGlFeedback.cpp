
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  SgGlFeedback: The feedback buffer renderer class.
//
///////////////////////////////////////////////////////////////////////////////

#include "SgGlPrecompiled.h"
#include "SgGlFeedback.h"
#include "SgGlConstants.h"
#include "SgGlInternalMacros.h"

#include "Standard/SlPrint.h"
#include "Standard/SlConstants.h"
#include "Standard/SlTrace.h"
#include "Standard/SlVec3.h"
#include "Standard/SlMinMax.h"

#include "SceneGraph/Core/SgAllNodes.h"

#include <limits>


using namespace CadKit;

#define DEFAULT_BUFFER_SIZE 1024

SG_IMPLEMENT_DYNAMIC_VISITOR(SgGlFeedback,SgGlRenderer);


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

SgGlFeedback::SgGlFeedback() : SgGlRenderer(), 
  sortDirection ( BACK_TO_FRONT ),
  sortOption ( MIN_Z ),
  _primitive ( NULL ),
  _numPrimitives ( 0 ),
  _bufferSize ( 0 ),
  _numValuesInBuffer ( 0 ),
  _buffer ( NULL ),
  _flags ( 0 )
{
  SL_PRINT2 ( "SgGlFeedback::SgGlFeedback(), this = %X\n", this );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

SgGlFeedback::~SgGlFeedback()
{
  SL_PRINT2 ( "SgGlFeedback::~SgGlFeedback(), this = %X\n", this );

  // Delete the arrays.
  this->_deleteBuffer();
  this->_deletePrimitives();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Calculate the size the feedback buffer needs to be.
//
///////////////////////////////////////////////////////////////////////////////

bool SgGlFeedback::_calculateBufferSize ( GLint &size, SgNode &scene ) const
{
  SL_PRINT ( "SgGlFeedback::_calculateBufferSize()" );
  SL_ASSERT ( this );
  SL_ASSERT ( _bufferSize >= 0 );
  SL_ASSERT ( sizeof ( GLuint ) >= sizeof ( GLint ) );
  
  // Initialize this.
  size = 0;

  // If the current buffer size is zero then set it to the default size.
  // Otherwise, we double it. We stop if the GLint will overflow.
  GLuint temp = ( 0 == _bufferSize ) ? DEFAULT_BUFFER_SIZE : _bufferSize * 2;

  // Check for overflow of GLint. We assume that a GLuint will always safely 
  // hold the maximum value of a GLint.
  bool newSizeOk = false;
  switch ( sizeof ( GLint ) )
  {
  case 2: newSizeOk = ( temp <= static_cast<GLuint>( std::numeric_limits<SlInt16>::max() ) ); break;
  case 4: newSizeOk = ( temp <= static_cast<GLuint>( std::numeric_limits<SlInt32>::max() ) ); break;
  case 8: newSizeOk = ( temp <= static_cast<GLuint>( std::numeric_limits<SlInt64>::max() ) ); break;
  default:

    SL_ASSERT ( 0 ); // What kind of an OS is this?
    return false;
  }

  // If we can safely assign the new size then do it.
  if ( newSizeOk )
    size = static_cast<GLint> ( temp );

  // Return the result.
  return newSizeOk;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Reallocate the feedback buffer if needed.
//
///////////////////////////////////////////////////////////////////////////////

bool SgGlFeedback::_reallocateBufferIfNeeded ( GLint &size )
{
  SL_PRINT ( "SgGlFeedback::_reallocateBufferIfNeeded()" );
  SL_ASSERT ( this );
  SL_ASSERT ( false == this->hasFlags ( _CLIENT_OWNS_BUFFER ) );
  SL_ASSERT ( size >= 0 );

  // If the size is different...
  if ( size != _bufferSize )
  {
    // Delete the existing buffer.
    this->_deleteBuffer();

    // Allocate the buffer. We use calloc() because it initializes the values to zero.
    // If the new size is zero then we just set to NULL.
    GLfloat *buffer = ( size > 0 ) ? (GLfloat *) ::calloc ( size, sizeof (GLfloat) ) : NULL;

    // Check allocation.
    if ( size > 0 && NULL == buffer )
    {
      SL_ASSERT ( 0 ); // Too many vertices?
      return false;
    }

    // Set these.
    _bufferSize = size;
    _numValuesInBuffer = 0;
    _buffer = buffer;
  }

  // If we get here then we did good.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Delete the feedback buffer if it's ours.
//
///////////////////////////////////////////////////////////////////////////////

void SgGlFeedback::_deleteBuffer()
{
  SL_PRINT ( "SgGlFeedback::_deleteBuffer()" );
  SL_ASSERT ( this );

  // If we have a feedback buffer and it's ours...
  if ( _buffer && false == this->hasFlags ( _CLIENT_OWNS_BUFFER ) )
  {
    // Should be true.
    SL_ASSERT ( _bufferSize > 0 );

    // Delete the buffer. Use free() because we used calloc() to allocate it.
    ::free ( _buffer );

    // Reset these.
    _buffer = NULL;
    _bufferSize = 0;
    _numValuesInBuffer = 0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize the next rendering.
//
///////////////////////////////////////////////////////////////////////////////

bool SgGlFeedback::preRender ( SgNode &scene )
{
  SL_PRINT ( "SgGlFeedback::preRender()" );
  SL_ASSERT ( this );

  // Initialize this.
  _numValuesInBuffer = 0;

  // Make our context the current one.
  if ( false == this->_makeContextCurrent() ) 
    return false;

  // If the client did not supply a feedback buffer...
  if ( false == this->hasFlags ( _CLIENT_OWNS_BUFFER ) )
  {
    // Get the total vertex count.
    GLint bufferSize;
    if ( false == this->_calculateBufferSize ( bufferSize, scene ) ) 
      return false;

    // Reallocate the buffer if we need to.
    if ( false == this->_reallocateBufferIfNeeded ( bufferSize ) ) 
      return false;
  }

  // When we get to here we should have a good buffer.
  SL_ASSERT ( _bufferSize > 0 );
  SL_ASSERT ( _numValuesInBuffer == 0 );
  SL_ASSERT ( _buffer );

  // Call the base class's function.
  if ( false == SgGlRenderer::preRender ( scene ) )
    return false;

  // Set the feedback buffer.
  ::glFeedbackBuffer ( _bufferSize, GL_3D_COLOR, _buffer );

  // Switch to feedback mode. Should return zero indicating that we 
  // were in rendering mode.
  SL_VERIFY ( 0 == ::glRenderMode ( GL_FEEDBACK ) );

  // It worked!
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Render the node.
//
///////////////////////////////////////////////////////////////////////////////

bool SgGlFeedback::render ( SgNode &scene )
{
  SL_PRINT ( "SgGlFeedback::render()" );
  SL_ASSERT ( this );
  SL_ASSERT ( _bufferSize > 0 );
  SL_ASSERT ( _numValuesInBuffer == 0 );
  SL_ASSERT ( _buffer );

  // Call the base class's function to render the scene.
  if ( false == SgGlRenderer::render ( scene ) )
  {
    // Return to rendering mode now because postRender() will not be called.
    ::glRenderMode ( GL_RENDER );

    // It didn't work.
    return false;
  }

  // It worked!
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Finalize the rendering.
//
///////////////////////////////////////////////////////////////////////////////

bool SgGlFeedback::postRender ( SgNode &scene )
{
  SL_PRINT ( "SgGlFeedback::postRender()" );
  SL_ASSERT ( this );
  SL_ASSERT ( _bufferSize > 0 );
  SL_ASSERT ( _numValuesInBuffer == 0 );
  SL_ASSERT ( _buffer );

  // Draw everything right now.
  ::glFlush();

  // Return to rendering mode. This will return the number of values in 
  // the feedback buffer.
  _numValuesInBuffer = ::glRenderMode ( GL_RENDER );
  SL_TRACE2 ( "_numValuesInBuffer = %d\n", _numValuesInBuffer );

  // Should be true.
  SL_ASSERT ( _numValuesInBuffer <= _bufferSize );

  // Zero is ok (just means that the window is blank. Negative means that 
  // the buffer was too small.
  if ( _numValuesInBuffer < 0 )
    return false;

  // Prepare the array of primitive if there are any.
  return ( _numValuesInBuffer > 0 ) ? this->_processFeedbackBuffer() : true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the feedback buffer was just filled and the primitives need 
//  to be extracted from it.
//
///////////////////////////////////////////////////////////////////////////////

bool SgGlFeedback::_processFeedbackBuffer()
{
  SL_PRINT ( "SgGlFeedback::_processFeedbackBuffer()" );
  SL_ASSERT ( this );
  SL_ASSERT ( _bufferSize > 0 );
  SL_ASSERT ( _numValuesInBuffer >= 0 && _numValuesInBuffer <= _bufferSize );
  SL_ASSERT ( _buffer );

  // Count the primitives and check for errors.
  GLint numPrimitives ( 0 );
  if ( false == this->_countPrimitives ( numPrimitives ) )
    return false;

  // Allocate the primitive array if needed.
  if ( false == this->_reallocatePrimitivesIfNeeded ( numPrimitives ) ) 
    return false;

  // Should be true.
  SL_ASSERT ( _primitive );
  SL_ASSERT ( _numPrimitives == numPrimitives );

  // Calculate the average debth of each primitive.
  // This will also assign the primitive pointers into the buffer.
  this->_calculatePrimitiveDebths();

  // If we are supposed to sort...
  if ( sortDirection == BACK_TO_FRONT ) 
    ::qsort ( _primitive, _numPrimitives, sizeof ( Primitive ), &SgGlFeedback::_compareBackToFront );
  else if ( sortDirection == FRONT_TO_BACK ) 
    ::qsort ( _primitive, _numPrimitives, sizeof ( Primitive ), &SgGlFeedback::_compareFrontToBack );

  // Note: if we don't sort then the primitve array is good as is.

  // It worked!
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Calculate the debth of each primitve. This also assigns the pointers in 
//  the primitve array to the data in the feedback buffer.
//
///////////////////////////////////////////////////////////////////////////////

void SgGlFeedback::_calculatePrimitiveDebths()
{
  SL_PRINT ( "SgGlFeedback::_calculatePrimitiveDebths()" );
  SL_ASSERT ( this );
  SL_ASSERT ( _bufferSize > 0 );
  SL_ASSERT ( _numValuesInBuffer >= 0 && _numValuesInBuffer <= _bufferSize );
  SL_ASSERT ( _buffer );
  SL_ASSERT ( _primitive );
  SL_ASSERT ( _numPrimitives >= 0 && _numPrimitives < _numValuesInBuffer );

  // Declare and initialize.
  GLint index ( 0 );
  GLint token, numVertices;
  GLfloat *buffer = _buffer;
  GLfloat *endOfBuffer = _buffer + _numValuesInBuffer;
  GL3DColorFeedback *vertex;
  SlVec3<GLfloat> v01, v12, normal;

  // While we're not at the end of the buffer...
  while ( buffer < endOfBuffer )
  {
    // Save this primitive's location.
    _primitive[index].buffer = buffer;

    // Get the token and increment the buffer pointer.
    token = static_cast<GLint> ( *buffer );
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
      case AVERAGE:  _primitive[index].depth = ( vertex[0].z + vertex[1].z ) * 0.5f; break;
      case MIN_Z:    _primitive[index].depth = std::min ( vertex[0].z, vertex[1].z );  break;
      case MAX_Z:    _primitive[index].depth = std::max ( vertex[0].z, vertex[1].z );  break;
      default: SL_ASSERT ( 0 );
      }

      buffer += 14; // Offset.
      ++index;      // Increment the primitive counter.
      break;

    case GL_POLYGON_TOKEN:

      numVertices = (GLint) (*buffer);       // Get the number of vertices.
      ++buffer;                              // Increment the buffer pointer.
      vertex = (GL3DColorFeedback *) buffer; // Grab the first vertex.

      // First time this isn't true I will know that I need to use a for-loop.
      SL_ASSERT ( numVertices == 3 );

      // Calculate the "debth".
      switch ( sortOption )
      {
      case AVERAGE: _primitive[index].depth = ( vertex[0].z + vertex[1].z + vertex[2].z ) / numVertices; break;
      case MIN_Z:   _primitive[index].depth = CadKit::min ( vertex[0].z, vertex[1].z, vertex[2].z );          break;
      case MAX_Z:   _primitive[index].depth = CadKit::max ( vertex[0].z, vertex[1].z, vertex[2].z );          break;
      default: SL_ASSERT ( 0 );
      }

      // If the flag says to then determine if the triangle is facing the view.
      if ( this->hasFlags ( CULL_BACK_FACES ) )
      {
        // These are the vectors from vertex 0 to vertex 1, and vertex 1 to vertex 2.
        v01.setValue ( vertex[1].x - vertex[0].x, vertex[1].y - vertex[0].y, vertex[1].z - vertex[0].z );
        v12.setValue ( vertex[2].x - vertex[1].x, vertex[2].y - vertex[1].y, vertex[2].z - vertex[1].z );
        
        // Calculate the normal for the triangle from the vectors.
        normal = v12.cross ( v01 );

        // We use this triangle iff the normal is facing the view (i.e., if the z-value is positive).
        if ( normal[2] > 0.0 ) 
          ++index;
        else 
          --_numPrimitives;
      }

      // If we are not culling the back faces then we always use this triangle.
      else 
        ++index;

      // Offset to the next primitive.
      buffer += ( 7 * numVertices );
      break;

    case GL_POINT_TOKEN:

      vertex = (GL3DColorFeedback *) buffer;  // Get the buffer.
      _primitive[index].depth = vertex[0].z;  // Assign the debth.
      buffer += 7;                            // Offset.
      ++index;                                // Increment the primitive counter.
      break;

    case GL_PASS_THROUGH_TOKEN:

      ++buffer; // Ignore it.
      break;

    default:

      // What token is this?
      SL_ASSERT ( 0 );
    }
  }

  // Should be true.
  SL_ASSERT ( _numPrimitives >= 0 ); // Could have back-face culled them all away.
  SL_ASSERT ( index == _numPrimitives );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Reallocate the feedback buffer if needed.
//
///////////////////////////////////////////////////////////////////////////////

bool SgGlFeedback::_reallocatePrimitivesIfNeeded ( GLint &numPrimitives )
{
  SL_PRINT ( "SgGlFeedback::_reallocatePrimitivesIfNeeded()" );
  SL_ASSERT ( this );
  SL_ASSERT ( false == this->hasFlags ( _CLIENT_OWNS_BUFFER ) );
  SL_ASSERT ( numPrimitives >= 0 );
  SL_ASSERT ( numPrimitives < _numValuesInBuffer );

  // If the size is different...
  if ( numPrimitives != _numPrimitives )
  {
    // Delete the existing buffer.
    this->_deletePrimitives();

    // Allocate the buffer. If the new size is zero then we just set to NULL.
    Primitive *primitive = ( numPrimitives > 0 ) ? new Primitive[numPrimitives] : NULL;

    // Check allocation.
    if ( numPrimitives > 0 && primitive == NULL )
    {
      SL_ASSERT ( 0 ); // Too many primitives?
      return false;
    }

    // Set these.
    _numPrimitives = numPrimitives;
    _primitive = primitive;
  }

  // If we get here then we did good.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Delete the primitive array.
//
///////////////////////////////////////////////////////////////////////////////

void SgGlFeedback::_deletePrimitives()
{
  SL_PRINT ( "SgGlFeedback::_deletePrimitives()" );
  SL_ASSERT ( this );

  // Delete the array if it's not NULL.
  if ( _primitive ) 
    delete [] _primitive;

  // Reset these.
  _primitive = NULL;
  _numPrimitives = 0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Callback for qsort() to sort the array of primitives.
//
///////////////////////////////////////////////////////////////////////////////

int SgGlFeedback::_compareBackToFront ( const void *a, const void *b )
{
  SL_ASSERT ( a && b );

  // Get the primitives.
  Primitive *primA = (Primitive *) a;
  Primitive *primB = (Primitive *) b;

  // Calculate the difference in their debths.
  GLfloat diff = primB->depth - primA->depth;

  // Return the result.
  if ( diff > 0.0 ) 
    return 1;
  else if ( diff < 0.0 ) 
    return -1;
  else 
    return 0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Callback for qsort() to sort the array of primitives.
//
///////////////////////////////////////////////////////////////////////////////

int SgGlFeedback::_compareFrontToBack ( const void *a, const void *b )
{
  SL_ASSERT ( a && b );

  // Get the primitives.
  Primitive *primA = (Primitive *) a;
  Primitive *primB = (Primitive *) b;

  // Calculate the difference in their debths.
  GLfloat diff = primB->depth - primA->depth;

  // Return the result.
  if ( diff > 0.0 ) 
    return -1;
  else if ( diff < 0.0 ) 
    return 1;
  else 
    return 0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Count the primitives in the feedback buffer. Note: a zero count is ok.
//
///////////////////////////////////////////////////////////////////////////////

bool SgGlFeedback::_countPrimitives ( GLint &numPrimitives ) const
{
  SL_PRINT ( "SgGlFeedback::_countPrimitives()" );
  SL_ASSERT ( this );
  SL_ASSERT ( _bufferSize > 0 );
  SL_ASSERT ( _numValuesInBuffer >= 0 && _numValuesInBuffer <= _bufferSize );
  SL_ASSERT ( _buffer );

  // Initialize.
  numPrimitives = 0;
  GLint numVertices, token;
  GLfloat *buffer = _buffer;
  GLfloat *endOfBuffer = _buffer + _numValuesInBuffer;

  // While we're not at the end of the buffer...
  while ( buffer < endOfBuffer )
  {
    // Get the token and increment the buffer pointer.
    token = static_cast<GLint>(*buffer);
    ++buffer;

    // Switch off the token.
    switch ( token )
    {
    case GL_LINE_TOKEN:
    case GL_LINE_RESET_TOKEN:

      buffer += 14;    // Offset.
      ++numPrimitives; // Incerment count.
      break;

    case GL_POLYGON_TOKEN:

      numVertices = static_cast<GLint>(*buffer); // Vertex count in polygon.
      ++buffer;                                  // Offset for the number of vertices.
      buffer += ( 7 * numVertices );             // Offset for the actual vertices.
      ++numPrimitives;                           // Increment count.
      break;

    case GL_POINT_TOKEN:

      buffer += 7;     // Offset.
      ++numPrimitives; // Incerment count.
      break;

    case GL_PASS_THROUGH_TOKEN:

      ++buffer; // Ignore it.
      break;

    default:

      SL_ASSERT ( 0 ); // What token is this?
      return false;
    }
  }

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Implement all the visitor functions.
//
///////////////////////////////////////////////////////////////////////////////

SG_GL_IMPLEMENT_FEEDBACK_VISITOR_FUNCTION ( SgBackground );
SG_GL_IMPLEMENT_FEEDBACK_VISITOR_FUNCTION ( SgBaseColor );
SG_GL_IMPLEMENT_FEEDBACK_VISITOR_FUNCTION ( SgBillboard );
SG_GL_IMPLEMENT_FEEDBACK_VISITOR_FUNCTION ( SgCamera );
SG_GL_IMPLEMENT_FEEDBACK_VISITOR_FUNCTION ( SgCircle );
SG_GL_IMPLEMENT_FEEDBACK_VISITOR_FUNCTION ( SgColorWheel );
SG_GL_IMPLEMENT_FEEDBACK_VISITOR_FUNCTION ( SgCone );
SG_GL_IMPLEMENT_FEEDBACK_VISITOR_FUNCTION ( SgCoordinate3 );
SG_GL_IMPLEMENT_FEEDBACK_VISITOR_FUNCTION ( SgCoordinate4 );
SG_GL_IMPLEMENT_FEEDBACK_VISITOR_FUNCTION ( SgCube );
SG_GL_IMPLEMENT_FEEDBACK_VISITOR_FUNCTION ( SgCylinder );
SG_GL_IMPLEMENT_FEEDBACK_VISITOR_FUNCTION ( SgDirectionalLight );
SG_GL_IMPLEMENT_FEEDBACK_VISITOR_FUNCTION ( SgDrawStyle );
SG_GL_IMPLEMENT_FEEDBACK_VISITOR_FUNCTION ( SgEllipse );
SG_GL_IMPLEMENT_FEEDBACK_VISITOR_FUNCTION ( SgGroup );
SG_GL_IMPLEMENT_FEEDBACK_VISITOR_FUNCTION ( SgIdentity );
SG_GL_IMPLEMENT_FEEDBACK_VISITOR_FUNCTION ( SgLineSet );
SG_GL_IMPLEMENT_FEEDBACK_VISITOR_FUNCTION ( SgMaterial );
SG_GL_IMPLEMENT_FEEDBACK_VISITOR_FUNCTION ( SgNormal );
SG_GL_IMPLEMENT_FEEDBACK_VISITOR_FUNCTION ( SgOrthographicCamera );
SG_GL_IMPLEMENT_FEEDBACK_VISITOR_FUNCTION ( SgPerspectiveCamera );
SG_GL_IMPLEMENT_FEEDBACK_VISITOR_FUNCTION ( SgPointSet );
SG_GL_IMPLEMENT_FEEDBACK_VISITOR_FUNCTION ( SgQuadMesh );
SG_GL_IMPLEMENT_FEEDBACK_VISITOR_FUNCTION ( SgRectangle );
SG_GL_IMPLEMENT_FEEDBACK_VISITOR_FUNCTION ( SgRotation );
SG_GL_IMPLEMENT_FEEDBACK_VISITOR_FUNCTION ( SgScale );
SG_GL_IMPLEMENT_FEEDBACK_VISITOR_FUNCTION ( SgSelection );
SG_GL_IMPLEMENT_FEEDBACK_VISITOR_FUNCTION ( SgSeparator );
SG_GL_IMPLEMENT_FEEDBACK_VISITOR_FUNCTION ( SgShading );
SG_GL_IMPLEMENT_FEEDBACK_VISITOR_FUNCTION ( SgSphere );
SG_GL_IMPLEMENT_FEEDBACK_VISITOR_FUNCTION ( SgSquare );
SG_GL_IMPLEMENT_FEEDBACK_VISITOR_FUNCTION ( SgSwitch );
SG_GL_IMPLEMENT_FEEDBACK_VISITOR_FUNCTION ( SgTransform );
SG_GL_IMPLEMENT_FEEDBACK_VISITOR_FUNCTION ( SgTranslation );
SG_GL_IMPLEMENT_FEEDBACK_VISITOR_FUNCTION ( SgTriangle );
SG_GL_IMPLEMENT_FEEDBACK_VISITOR_FUNCTION ( SgTriStrip );
SG_GL_IMPLEMENT_FEEDBACK_VISITOR_FUNCTION ( SgUnScale );
SG_GL_IMPLEMENT_FEEDBACK_VISITOR_FUNCTION ( SgText );


///////////////////////////////////////////////////////////////////////////////
//
//  These are all the visit functions that we don't use. They should never 
//  get called. However, the base class requires that we overload them.
//
///////////////////////////////////////////////////////////////////////////////

bool SgGlFeedback::visit ( SgNode & )       { SL_ASSERT ( 0 ); return true; }
bool SgGlFeedback::visit ( SgShape & )      { SL_ASSERT ( 0 ); return true; }
bool SgGlFeedback::visit ( SgCoordinate & ) { SL_ASSERT ( 0 ); return true; }
bool SgGlFeedback::visit ( SgLight & )      { SL_ASSERT ( 0 ); return true; }
bool SgGlFeedback::visit ( SgVertexSet & )  { SL_ASSERT ( 0 ); return true; }
bool SgGlFeedback::visit ( SgPrimitive & )  { SL_ASSERT ( 0 ); return true; }
