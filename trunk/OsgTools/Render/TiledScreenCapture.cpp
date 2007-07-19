
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "OsgTools/Render/TiledScreenCapture.h"
#include "OsgTools/Render/FBOScreenCapture.h"
#include "OsgTools/Images/DownSample.h"
#include "OsgTools/ScopedViewport.h"

// For debugging...
#include "Usul/File/Temp.h"
#include "Usul/File/Remove.h"

#include "osgDB/WriteFile"

#include <vector>
#include <string>
#include <sstream>

#include "osg/Image"

using namespace OsgTools::Render;

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

TiledScreenCapture::TiledScreenCapture () :
_size (),
_color (),
_viewMatrix (),
_numSamples ( 4 )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

TiledScreenCapture::~TiledScreenCapture ()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the size.
//
///////////////////////////////////////////////////////////////////////////////

void TiledScreenCapture::size ( unsigned int width, unsigned int height )
{
  _size.set ( width, height );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the size.
//
///////////////////////////////////////////////////////////////////////////////

void TiledScreenCapture::size ( const Usul::Math::Vec2ui& size )
{
  _size = size;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the size.
//
///////////////////////////////////////////////////////////////////////////////

const Usul::Math::Vec2ui& TiledScreenCapture::size () const
{
  return _size;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the clear color.
//
///////////////////////////////////////////////////////////////////////////////

void TiledScreenCapture::clearColor ( const osg::Vec4& color )
{
  _color = color;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the clear color.
//
///////////////////////////////////////////////////////////////////////////////

const osg::Vec4& TiledScreenCapture::clearColor () const
{
  return _color;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the view matrix.
//
///////////////////////////////////////////////////////////////////////////////

void TiledScreenCapture::viewMatrix ( const osg::Matrix& matrix )
{
  _viewMatrix = matrix;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the view matrix.
//
///////////////////////////////////////////////////////////////////////////////

const osg::Matrix& TiledScreenCapture::viewMatrix () const
{
  return _viewMatrix;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the number of samples.
//
///////////////////////////////////////////////////////////////////////////////

void TiledScreenCapture::numSamples( unsigned int samples )
{
  _numSamples = samples;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the number of samples.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int TiledScreenCapture::numSamples() const
{
  return _numSamples;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Compute the projection matrix for a tile.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  osg::Matrix computeTileProjection ( const osg::Matrix& proj, 
                                      unsigned int answerWidth, 
                                      unsigned int answerHeight, 
                                      int x, 
                                      int y, 
                                      unsigned int tileX, 
                                      unsigned int tileY )
  {
    double zNear   ( 0.0 );
    double zFar    ( 0.0 );
    double top     ( 0.0 );
    double bottom  ( 0.0 );
    double left    ( 0.0 );
    double right   ( 0.0 );

    proj.getFrustum( left, right, bottom, top, zNear, zFar );

    // compute projection parameters
    const double currentLeft   ( left          + ( right - left ) *  x / answerWidth );
    const double currentRight  ( currentLeft   + ( right - left ) *  tileX / answerWidth );
    const double currentBottom ( bottom        + ( top - bottom ) *  y / answerHeight );
    const double currentTop    ( currentBottom + ( top - bottom ) *  tileY / answerHeight );

    // Construct tile matrix.
    osg::Matrix m;
    m.makeFrustum ( currentLeft, currentRight, currentBottom, currentTop, zNear, zFar );
    return m;

    //osg::Matrix ras2ndc (
    //    1.0f / answerWidth,           0,          0,   0,
    //    0,                  -1.0f / answerHeight, 0, -1.0f,
    //    0,                            0,          1,   0,
    //    0,                            0,          0,   1 );

    //// Find the lower left corner of this tile in Normalized Device Coordinates ( ndc ).
    //float xndc ( static_cast < float > ( x ) / answerWidth  );
    //float yndc ( static_cast < float > ( y ) / answerHeight );

    //// Find the inverse width and height of this tile in ndc coordinates.
    //float inv_wndc ( static_cast < float > ( answerWidth  / tileX ) );
    //float inv_hndc ( static_cast < float > ( answerHeight / tileY ) );

    //// Construct a matrix to zoom this tile's ndc region to [-1,1]x[-1,1].
    //osg::Matrix T0 ( osg::Matrix::translate ( -xndc, -yndc, 0 ) );
    //osg::Matrix S  ( osg::Matrix::scale     ( 2 * inv_wndc, 2 * inv_hndc, 1 ) );
    //osg::Matrix T1 ( osg::Matrix::translate ( -1, -1, 0 ) );
    //osg::Matrix ndc2tile ( T0 * S * T1 );

    //// Construct tile matrix.
    //osg::Matrix m ( proj * ras2ndc * ndc2tile );
    //return m;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create the image using Super-sampled, tiled rendering...
//
///////////////////////////////////////////////////////////////////////////////

void TiledScreenCapture::operator () ( osg::Image& image, osgUtil::SceneView& sceneView, const osg::Matrix& projection )
{
  // Width and Height.
  unsigned int width  ( _size [ 0 ] );
  unsigned int height ( _size [ 1 ] );

  // Filter width and height.
  const float filterWidth ( 2.0f ), filterHeight ( 2.0f );

  // Tile width and height without padding.
  const unsigned int tileX ( 128 ), tileY ( 128 );

  // Number of samples in x and y direction.
  const unsigned int samplesX ( this->numSamples () ), samplesY ( this->numSamples () );

  // Amount to pad the tiles.
  unsigned int xPadding ( static_cast < unsigned int > ( ::ceil ( filterWidth  / ( 2.0 + 0.5 ) ) ) );
  unsigned int yPadding ( static_cast < unsigned int > ( ::ceil ( filterHeight / ( 2.0 + 0.5 ) ) ) );

  // Tile width and height including padding.
  /*unsigned int tileWidth  ( tileX + ( 2 * xPadding ) );
  unsigned int tileHeight ( tileY + ( 2 * yPadding ) );*/
  unsigned int tileWidth  ( tileX );
  unsigned int tileHeight ( tileY );

  // Starting x and y value for the loop.
  // This is pad the final image to ensure edge pixels are correct.
  int firstX ( static_cast < int > ( ::ceil ( filterWidth  / 2.0 - 0.5 ) ) );
  int firstY ( static_cast < int > ( ::ceil ( filterHeight / 2.0 - 0.5 ) ) );

  // Frame Buffer Object to render the tiles to.
  FBOScreenCapture fbo;
  //fbo.size ( tileWidth * samplesX, tileHeight * samplesY );
  fbo.size ( tileWidth, tileHeight );
  fbo.viewMatrix ( this->viewMatrix() );
  fbo.clearColor ( this->clearColor() );

  // Scope the viewport.
  OsgTools::ScopedViewport sv ( &sceneView );

  // Set the viewport for the tile.
  //sceneView.setViewport ( 0, 0, tileX * samplesX, tileY * samplesY );
  sceneView.setViewport ( 0, 0, tileX, tileY );

  // Downsampler.
  OsgTools::Images::DownSample downSample;
  downSample.size ( tileX, tileY );
  downSample.numSamples ( this->numSamples( ) );
  downSample.filterSize ( filterWidth, filterHeight );

  //typedef std::vector < std::string > Files;
  //Files files;

  //// Create a temp file.
  //std::string filename ( Usul::File::Temp::file() );
  //files.push_back ( filename );

  unsigned int i ( 0 );

  // Create the tiles.
  for ( int y = -firstY; y < static_cast < int > ( height ) + firstY; y += tileY )
  {
    unsigned int j ( 0 );

    for ( int x = -firstX; x < static_cast < int > ( width ) + firstX; x += tileX )
    {
      // Get the matrix.
      osg::Matrix m ( Detail::computeTileProjection ( projection, width, height, x, y, tileX, tileY ) );

      // Create the tile.
      osg::ref_ptr < osg::Image > tile ( fbo ( sceneView, m ) );

      // Downsample.
      //osg::ref_ptr < osg::Image > downTile ( downSample ( tile.get() ) );

      /*std::ostringstream os;
      os << filename << "_" << i << "_" << j++ << ".bmp";
      files.push_back ( os.str() );

      osgDB::writeImageFile ( *tile, os.str () );*/

      // Add tile to answer image.
      this->_accumulate ( image, *tile, static_cast < unsigned int > ( x + firstX ), static_cast < unsigned int > ( y + firstY ) );
    }

    ++i;
  }

  /*for ( Files::iterator iter = files.begin(); iter != files.end(); ++iter )
    Usul::File::remove ( *iter );*/
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create the image.
//
///////////////////////////////////////////////////////////////////////////////

osg::Image* TiledScreenCapture::operator () ( osgUtil::SceneView& sceneView, const osg::Matrix& projection )
{
  osg::ref_ptr < osg::Image > image ( new osg::Image );

  // Make enough space
  image->allocateImage ( _size [ 0 ], _size [ 1 ], 1, GL_RGBA, GL_UNSIGNED_BYTE );

  (*this ) ( *image, sceneView, projection );

  return image.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Accumulate the tile into the final image.
//
///////////////////////////////////////////////////////////////////////////////

void TiledScreenCapture::_accumulate ( osg::Image& image, const osg::Image& tile, unsigned int x, unsigned int y )
{
  for ( int i = 0; i < tile.s(); ++ i )
  {
    for ( int j = 0; j < tile.t(); ++ j )
    {
      int x0 ( x + i );
      int y0 ( y + j );

      if ( x0 >= image.s() || y0 >= image.t() )
        continue;
      
      {
        unsigned char* result ( image.data ( x0, y0 ) );
        const unsigned char* pixels ( tile.data ( i, j ) );
        
        *result++ = *pixels++;
        *result++ = *pixels++;
        *result++ = *pixels++;
        *result++ = *pixels++;
      }
    }
  }
}
