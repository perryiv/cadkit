
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
//  WriteEPS: The EPS writer class.
//
/////////////////////////////////////////////////////////////////////////////

#include "OsgTools/IO/WriteEPS.h"
#include "OsgTools/Render/Viewer.h"

#include "Usul/Errors/Assert.h"
#include "Usul/Math/Absolute.h"
#include "Usul/Math/MinMax.h"

#include <limits>

using namespace OsgTools::IO;


/////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
/////////////////////////////////////////////////////////////////////////////

WriteEPS::WriteEPS( const std::string& filename, const std::string& creator ) : GLFeedback(), 
	_filename ( filename ),
	_creator ( creator ),
	_showpage ( false ),
	_gouraudThreshold ( 0.1 ), // Default value from original example.
	_lineSmoothingFactor ( 0.06 ), // Default value from original example.
	_currentMaxBufSize ( 0 )
{
}


/////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
/////////////////////////////////////////////////////////////////////////////

WriteEPS::~WriteEPS()
{
}


/////////////////////////////////////////////////////////////////////////////
//
//  Write the EPS file.
//
/////////////////////////////////////////////////////////////////////////////

bool WriteEPS::write ( OsgTools::Render::Viewer& viewer )
{
	// Initialize.
	_currentMaxBufSize = 1024;

	// We loop until we successfully write the file. Each time it will 
	// increase the feedback buffer size.
	while ( true )
	{
		// Call preRender() to initialize the feedback buffer. If it fails 
		// then we quit (because it probably means that we are out of memory).
		if ( this->preRender() != NO_RENDER_ERROR ) return false;

		// Call the render callback to fill the feedback buffer.
		viewer.render();

		// Call postRender() to process the feedback buffer.
		Result result = this->postRender();

		// If there was no error...
		if ( result == NO_RENDER_ERROR )
		{
			// If we get to here then we can write the file.
			return this->_writeFile();
		}

		// If the buffer size is too small...
		else if ( result == BUFFER_TOO_SMALL )
		{
			// Double the size.
			_currentMaxBufSize *= 2;

			// Make sure we don't overflow the buffer. Out member 
			// _currentMaxBufSize is an SlUint64 while the actual 
			// buffer in SgGLFeedback is an SlInt32 (because that 
			// is what openGL wants). So we can safely see if we 
			// are going to overflow.
      if ( _currentMaxBufSize >= std::numeric_limits< unsigned int >::max() ) return false;
		}

		// Otherwise we have to stop.
		else return false;
	}

	// Should never get here.
	USUL_ASSERT ( 0 );
	return false;
}


/////////////////////////////////////////////////////////////////////////////
//
//  This gets called by preRender(). We overload it so that we can do a 
//	better job of sizing the buffer.
//
/////////////////////////////////////////////////////////////////////////////

bool WriteEPS::_calculateBufferSize ( int &size ) const
{
  USUL_ASSERT ( _currentMaxBufSize < std::numeric_limits < unsigned int >::max() );

	// Set the size.
	size = (int) _currentMaxBufSize;

	// It worked.
	return true;
}


/////////////////////////////////////////////////////////////////////////////
//
//  Write the existing feedback buffer to an EPS file. If the filename is
//	NULL then dump to stdout. If the creator is NULL then that field is 
//	skipped.
//
/////////////////////////////////////////////////////////////////////////////

bool WriteEPS::_writeFile() const
{
	USUL_ASSERT ( _maxBufferSize > 0 );
	USUL_ASSERT ( _actualBufferSize >= 0 && _actualBufferSize <= _maxBufferSize );
	USUL_ASSERT ( _buffer );
	USUL_ASSERT ( _gouraudThreshold > 0.0 );
	USUL_ASSERT ( _lineSmoothingFactor > 0.0 );

	// Open the file or direct to stdout if the filename is NULL.
  FILE *out = ( _filename == "" ) ? stdout : ::fopen ( _filename.c_str(), "w" );

	// Check the file pointer.
	if ( !out )
	{
		USUL_ASSERT ( 0 );
		return false;
	}

	// Need these below.
	GLfloat clearColor[4], viewport[4];
	GLfloat lineWidth, pointSize;

	// Get these properties from OpenGL.
	::glGetFloatv ( GL_VIEWPORT, viewport );
	::glGetFloatv ( GL_COLOR_CLEAR_VALUE, clearColor );
	::glGetFloatv ( GL_LINE_WIDTH, &lineWidth );
	::glGetFloatv ( GL_POINT_SIZE, &pointSize );

	// Write the EPS header.
	// Note: we have to use %% to get a % in the file.
	::fprintf ( out, "%%!PS-Adobe-2.0 EPSF-2.0\n" );

	// If there is a creator then write it.
	if ( _creator != "" ) ::fprintf ( out, "%%%%Creator: %s\n", _creator.c_str() );

	// Finish the header.
	::fprintf ( out, "%%%%BoundingBox: %g %g %g %g\n", viewport[0], viewport[1], viewport[2], viewport[3]);
	::fprintf ( out, "%%%%EndComments\n" );
	::fprintf ( out, "\ngsave\n\n" );

	// Output Frederic Delhoume's "gouraudtriangle" PostScript fragment.
	::fprintf ( out, "%% The gouraudtriangle PostScript fragement below is free,\n" );
	::fprintf ( out, "%% written by Frederic Delhoume (delhoume@ilog.fr).\n" );
	::fprintf ( out, "/threshold %g def\n", _gouraudThreshold );

	// This is the Gouraud triangle PostScript fragement written by Frederic 
	// Delhoume (delhoume@ilog.fr).
	char *gouraudTriangle[] =
	{
	  "/bd{bind def}bind def /triangle { aload pop   setrgbcolor  aload pop 5 3",
	  "roll 4 2 roll 3 2 roll exch moveto lineto lineto closepath fill } bd",
	  "/computediff1 { 2 copy sub abs threshold ge {pop pop pop true} { exch 2",
	  "index sub abs threshold ge { pop pop true} { sub abs threshold ge } ifelse",
	  "} ifelse } bd /computediff3 { 3 copy 0 get 3 1 roll 0 get 3 1 roll 0 get",
	  "computediff1 {true} { 3 copy 1 get 3 1 roll 1 get 3 1 roll 1 get",
	  "computediff1 {true} { 3 copy 2 get 3 1 roll  2 get 3 1 roll 2 get",
	  "computediff1 } ifelse } ifelse } bd /middlecolor { aload pop 4 -1 roll",
	  "aload pop 4 -1 roll add 2 div 5 1 roll 3 -1 roll add 2 div 3 1 roll add 2",
	  "div 3 1 roll exch 3 array astore } bd /gouraudtriangle { computediff3 { 4",
	  "-1 roll aload 7 1 roll 6 -1 roll pop 3 -1 roll pop add 2 div 3 1 roll add",
	  "2 div exch 3 -1 roll aload 7 1 roll exch pop 4 -1 roll pop add 2 div 3 1",
	  "roll add 2 div exch 3 -1 roll aload 7 1 roll pop 3 -1 roll pop add 2 div 3",
	  "1 roll add 2 div exch 7 3 roll 10 -3 roll dup 3 index middlecolor 4 1 roll",
	  "2 copy middlecolor 4 1 roll 3 copy pop middlecolor 4 1 roll 13 -1 roll",
	  "aload pop 17 index 6 index 15 index 19 index 6 index 17 index 6 array",
	  "astore 10 index 10 index 14 index gouraudtriangle 17 index 5 index 17",
	  "index 19 index 5 index 19 index 6 array astore 10 index 9 index 13 index",
	  "gouraudtriangle 13 index 16 index 5 index 15 index 18 index 5 index 6",
	  "array astore 12 index 12 index 9 index gouraudtriangle 17 index 16 index",
	  "15 index 19 index 18 index 17 index 6 array astore 10 index 12 index 14",
	  "index gouraudtriangle 18 {pop} repeat } { aload pop 5 3 roll aload pop 7 3",
	  "roll aload pop 9 3 roll 4 index 6 index 4 index add add 3 div 10 1 roll 7",
	  "index 5 index 3 index add add 3 div 10 1 roll 6 index 4 index 2 index add",
	  "add 3 div 10 1 roll 9 {pop} repeat 3 array astore triangle } ifelse } bd",
	  0x0
	};

	// This will loop through the gouraudTriangle array until it gets to the last 
	// element in the array, which is zero. Each time the i'th element, gouraudTriangle[i], 
	// is a pointer (some number other than zero), so it keeps looping.
	for ( int i = 0; gouraudTriangle[i]; ++i ) ::fprintf ( out, "%s\n", gouraudTriangle[i] );

	// Print the line width.
	::fprintf ( out, "\n%g setlinewidth\n", lineWidth );

	// Clear the background the same color as OpenGL.
	::fprintf ( out, "%g %g %g setrgbcolor\n",	clearColor[0], clearColor[1], clearColor[2] );
	::fprintf ( out, "%g %g %g %g rectfill\n\n", viewport[0], viewport[1], viewport[2], viewport[3] );

	// Now write the actual buffer to file.
	if ( !this->_writeBuffer ( pointSize, out ) ) return false;

	// Write the trailer.
	::fprintf ( out, "grestore\n\n" );

	// Write the "showpage" if we are supposed to. 
	// This makes it possible to send the EPS file to a printer.
	if ( _showpage )	::fprintf ( out, "showpage\n\n" );

	// Close the file if it is not stdout.
	if ( ( out != stdout ) && ( ::fclose ( out ) != 0 ) )
	{
		USUL_ASSERT ( 0 );
		return false;
	}

	// It worked!
	return true;
}


/////////////////////////////////////////////////////////////////////////////
//
//  Write the buffer to the given file.
//
/////////////////////////////////////////////////////////////////////////////

bool WriteEPS::_writeBuffer ( const float &pointSize, FILE *out ) const
{
	USUL_ASSERT ( _maxBufferSize > 0 );
	USUL_ASSERT ( _actualBufferSize >= 0 && _actualBufferSize <= _maxBufferSize );
	USUL_ASSERT ( _buffer );
	USUL_ASSERT ( _gouraudThreshold > 0.0 );
	USUL_ASSERT ( _lineSmoothingFactor > 0.0 );
	USUL_ASSERT ( out );
	USUL_ASSERT ( ( _primitive != 0x0 && _numPrimitives > 0 ) || ( _primitive == 0x0 && _numPrimitives == 0 ) );

	// Loop through all the primitives (it's ok if there aren't any).
	for ( int i = 0; i < _numPrimitives; ++i )
	{
		// Write the primitive.
		if ( !this->_writePrimitive ( pointSize, i, out ) ) return false;
	}

	// It worked!
	return true;
}


/////////////////////////////////////////////////////////////////////////////
//
//  Write the primitive to the given file.
//
/////////////////////////////////////////////////////////////////////////////

bool WriteEPS::_writePrimitive ( const float &pointSize, const int &whichPrim, FILE *out ) const
{
	USUL_ASSERT ( _maxBufferSize > 0 );
	USUL_ASSERT ( _actualBufferSize >= 0 && _actualBufferSize <= _maxBufferSize );
	USUL_ASSERT ( _buffer );
	USUL_ASSERT ( _gouraudThreshold > 0.0 );
	USUL_ASSERT ( _lineSmoothingFactor > 0.0 );
	USUL_ASSERT ( whichPrim >= 0 );
	USUL_ASSERT ( whichPrim < _numPrimitives );
	USUL_ASSERT ( out );

	// We declare variables up here because we can't do it in the case statements.
	int token;			// The token to process in the switch statement.
	int numVertices;	// The number of vertices. Only used when it's a polygon token.
	int i;				// Counter for loop.
	bool smooth;			// Flag that says the polygon vertices are different colors.
	int steps;			// Number of steps to take to draw one line segment (for varying color).
	GLfloat r;		// Red.
	GLfloat g;		// Green.
	GLfloat b;		// Blue.
	GLfloat dx;		// Change in x.
	GLfloat dy;		// Change in y.
	GLfloat dr;		// Change in red.
	GLfloat dg;		// Change in green.
	GLfloat db;		// Change in blue.
	GLfloat absR;	// Absolute value of red.
	GLfloat absG;	// Absolute value of green.
	GLfloat absB;	// Absolute value of blue.
	GLfloat xstep;	// Step in x-direction to take for line segments with varying color.
	GLfloat ystep;	// Step in y-direction to take for line segments with varying color.
	GLfloat rstep;	// Step in red to take for line segments with varying color.
	GLfloat gstep;	// Step in green to take for line segments with varying color.
	GLfloat bstep;	// Step in blue to take for line segments with varying color.
	GLfloat xnext;	// Used in a loop below, the next x coordinate.
	GLfloat ynext;	// Used in a loop below, the next y coordinate.
	GLfloat rnext;	// Used in a loop below, the next red value.
	GLfloat gnext;	// Used in a loop below, the next green value.
	GLfloat bnext;	// Used in a loop below, the next blue value.
	GLfloat maxColor;	// Max color change in line segment.
	GLfloat distance;	// Line segment distance.
	GL3DColorFeedback *vertex;	// Pointer to vertex data in the feedback buffer.

	// Get a pointer to the buffer.
	const GLfloat *buffer = _primitive[whichPrim].buffer;

	// Initialize the token.
	token = static_cast < int > ( *buffer );

	// Since we grabbed the token, increment the pointer into the buffer.
	++buffer;

	// Note: we do not have to finish each case statement with "buffer += ..."

	// Switch off the token type.
	switch (token)
	{
	case GL_LINE_RESET_TOKEN:
	case GL_LINE_TOKEN:

		// Grab the vertex data.
		vertex = (GL3DColorFeedback *) buffer;

		// There are two vertices following a line token.
		// Calculate the change in color from the first vertex to the second.
		dr = vertex[1].r - vertex[0].r;
		dg = vertex[1].g - vertex[0].g;
		db = vertex[1].b - vertex[0].b;

		// If there is a change in color...
		if ( dr != 0 || dg != 0 || db != 0 )
		{
			// Calculate the change in the coordinates.
			dx = vertex[1].x - vertex[0].x;
			dy = vertex[1].y - vertex[0].y;

			// Calculate the distance of the line segment.
			distance = sqrt(dx * dx + dy * dy);

			// Get the absolute value of the change in color.
      absR = Usul::Math::absolute ( dr );
			absG = Usul::Math::absolute ( dg );
			absB = Usul::Math::absolute ( db );

			// Calculate the maximum color.
      maxColor = Usul::Math::maximum ( absR, Usul::Math::maximum ( absG, absB ) );

			// Calculate the number of steps to take for drawing the line.
			steps = static_cast < int > ( Usul::Math::maximum ( 1.0, maxColor * distance * _lineSmoothingFactor ) );

			// Calculate the distance in the x and y-direction for each step.
			xstep = dx / steps;
			ystep = dy / steps;

			// Calculate the change in color for each step.
			rstep = dr / steps;
			gstep = dg / steps;
			bstep = db / steps;

			// Initialize these for the loop below.
			xnext = vertex[0].x - xstep / 2.0;
			ynext = vertex[0].y - ystep / 2.0;
			rnext = vertex[0].r - rstep / 2.0;
			gnext = vertex[0].g - gstep / 2.0;
			bnext = vertex[0].b - bstep / 2.0;
		}

		// Otherwise, we have a single color line.
		else steps = 0;

		// Write the first vertex.
		::fprintf ( out, "%g %g %g setrgbcolor\n", vertex[0].r, vertex[0].g, vertex[0].b );
		::fprintf ( out, "%g %g moveto\n", vertex[0].x, vertex[0].y );

		// Loop through all the steps (if we have any). If we have steps then 
		// for this one line segment we are taking many steps (because of the 
		// variation in color between the vertex end points).
		for ( i = 0; i < steps; ++i )
		{
			// Increment to the next step.
			xnext += xstep;
			ynext += ystep;
			rnext += rstep;
			gnext += gstep;
			bnext += bstep;

			// Write this point.
			::fprintf ( out, "%g %g lineto stroke\n", xnext, ynext );
			::fprintf ( out, "%g %g %g setrgbcolor\n", rnext, gnext, bnext );
			::fprintf ( out, "%g %g moveto\n", xnext, ynext );
		}

		// Write the last vertex.
		::fprintf ( out, "%g %g lineto stroke\n", vertex[1].x, vertex[1].y);
		break;

	case GL_POLYGON_TOKEN:

		// Grab the number of vertices.
		numVertices = static_cast < int > ( *buffer );

		// Move to the next place in the buffer.
		++buffer;

		// Grab the vertex data.
		vertex = (GL3DColorFeedback *) buffer;

		// If we have any vertices...
		if ( numVertices > 0 )
		{
			// Assign the colors.
			r = vertex[0].r;
			g = vertex[0].g;
			b = vertex[0].b;

			// Initialize this flag.
			smooth = false;

			// Loop through all the rest of the vertices.
			for ( i = 1; i < numVertices; ++i )
			{
				// If we have a different color than the original...
				if ( r != vertex[i].r || g != vertex[i].g || b != vertex[i].b )
				{
					// Set the flag that says we have to do smooth shading and stop looping.
					smooth = true;
					break;
				}
			}

			// If we have to do smooth shading...
			if (smooth)
			{
				// Break polygon into "numVertices-2" triangle fans.
				int stop = numVertices - 2;

				// Loop through each triangle.
				for ( i = 0; i < stop; ++i )
				{
					::fprintf ( out, "[%g %g %g %g %g %g]", 
								vertex[0].x, vertex[i+1].x, vertex[i+2].x, 
								vertex[0].y, vertex[i+1].y, vertex[i+2].y );
					::fprintf ( out, " [%g %g %g] [%g %g %g] [%g %g %g] gouraudtriangle\n", 
								vertex[0].r,   vertex[0].g,   vertex[0].b, 
								vertex[i+1].r, vertex[i+1].g, vertex[i+1].b, 
								vertex[i+2].r, vertex[i+2].g, vertex[i+2].b );
				}
			}

			// Otherwise, we have a flat shaded polygon.
			else
			{
				// Write the color.
				fprintf ( out, "newpath\n");
				fprintf ( out, "%g %g %g setrgbcolor\n", r, g, b );

				// Draw a filled triangle.
				::fprintf ( out, "%g %g moveto\n", vertex[0].x, vertex[0].y );
				for ( i = 1; i < numVertices; ++i ) ::fprintf ( out, "%g %g lineto\n", vertex[i].x, vertex[i].y );
				::fprintf ( out, "closepath fill\n\n");
			}
		}

		break;
	
	case GL_POINT_TOKEN:
	
		// Get the vertex data.
		vertex = (GL3DColorFeedback *) buffer;

		// Write the vertex data.
		::fprintf ( out, "%g %g %g setrgbcolor\n", vertex[0].r, vertex[0].g, vertex[0].b );
		::fprintf ( out, "%g %g %g 0 360 arc fill\n\n", vertex[0].x, vertex[0].y, pointSize / 2.0 );
		break;

	case GL_PASS_THROUGH_TOKEN:

		// We should not call this function with GL_PASS_THROUGH_TOKEN as the token.

	default:

		USUL_ASSERT ( 0 ); // What token is this?
		return false;
	}


	// It worked!
	return true;
}


/////////////////////////////////////////////////////////////////////////////
//
//  Set the threshold for the Gouraud shading algorithm that gets written 
//	to the EPS file.
//
/////////////////////////////////////////////////////////////////////////////

void WriteEPS::setGouraudShadingThreshold ( const double &value )
{
	/*if ( value < SG_EPS_WRITE_MIN_GOURAUD_SHADING_THRESHOLD || value > SG_EPS_WRITE_MAX_GOURAUD_SHADING_THRESHOLD )
	{
		USUL_ASSERT ( 0 );
		return;
	}*/

	_gouraudThreshold = value;
}


/////////////////////////////////////////////////////////////////////////////
//
//  Set the line smoothing factor.
//
/////////////////////////////////////////////////////////////////////////////

void WriteEPS::setLineSmoothingFactor ( const double &value )
{
	/*if ( value < SG_EPS_WRITE_MIN_LINE_SMOOTHING_FACTOR || value > SG_EPS_WRITE_MAX_LINE_SMOOTHING_FACTOR )
	{
		USUL_ASSERT ( 0 );
		return;
	}*/

	_lineSmoothingFactor = value;
}

