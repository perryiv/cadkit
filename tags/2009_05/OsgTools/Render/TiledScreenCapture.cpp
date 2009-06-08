
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
#include "OsgTools/ScopedProjection.h"
#include "OsgTools/Group.h"

// For debugging...
#include "Usul/File/Temp.h"

#include "Usul/Bits/Bits.h"
#include "Usul/Math/Interpolate.h"

#include "osg/Geode"
#include "osg/Image"

#include "osgDB/WriteFile"

#include <vector>
#include <string>
#include <sstream>
#include <iostream>

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
	_numSamples ( 4 ),
	_scale ( 1.0f ),
	_background()
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
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create the image using Super-sampled, tiled rendering...
//
///////////////////////////////////////////////////////////////////////////////

void TiledScreenCapture::operator () ( osg::Image& image, osgUtil::SceneView& sceneView, const osg::Matrix& projection )
{
  // Scope the viewport.
  OsgTools::ScopedViewport sv ( sceneView.getViewport () );

	// Scope the projection.
	OsgTools::ScopedProjection sp ( sceneView );

	// Get the cull visitor.
	osgUtil::CullVisitor *cv ( sceneView.getCullVisitor() );

	// Turn off computing of near far.
	osg::CullSettings::ComputeNearFarMode cullingMode ( cv->getComputeNearFarMode() );
	cv->setComputeNearFarMode ( osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR );

	// Turn off any camp projection callback.
	osg::ref_ptr<osg::CullSettings::ClampProjectionMatrixCallback> cb ( cv->getClampProjectionMatrixCallback() );
	cv->setClampProjectionMatrixCallback ( 0x0 );

	// Get the inheritance mask.
	const unsigned int mask ( cv->getInheritanceMask() );

	cv->setInheritanceMask ( Usul::Bits::add ( cv->getInheritanceMask(), osg::CullSettings::CLAMP_PROJECTION_MATRIX_CALLBACK ) );

  osg::ref_ptr < ProcessScene > ps ( new ProcessScene ( this->scale () ) );
  sceneView.getSceneData()->accept ( *ps );

  osg::ref_ptr < osg::Node > scene ( sceneView.getSceneData() );

  try
  {
    this->_capturePixels ( image, sceneView, projection );
  }
  catch ( ... )
  {
		// Restore state.
		cv->setComputeNearFarMode ( cullingMode );
		cv->setClampProjectionMatrixCallback ( cb.get() );
		cv->setInheritanceMask ( mask );

    ps->restoreScene ();
    sceneView.setSceneData( scene.get() );

    std::cout << "Exception caught while trying to capture screen." << std::endl;
  }

	// Restore state.
	cv->setComputeNearFarMode ( cullingMode );
	cv->setClampProjectionMatrixCallback ( cb.get() );
	cv->setInheritanceMask ( mask );

  ps->restoreScene ();
  sceneView.setSceneData( scene.get() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create the image using Super-sampled, tiled rendering...
//
///////////////////////////////////////////////////////////////////////////////

void TiledScreenCapture::_capturePixels ( osg::Image& image, osgUtil::SceneView& sceneView, const osg::Matrix& projection )
{
  // Width and Height.
  unsigned int width  ( _size [ 0 ] );
  unsigned int height ( _size [ 1 ] );

  // Filter width and height.
  const float filterWidth ( 2.0f ), filterHeight ( 2.0f );

  // Tile width and height without padding.
  const unsigned int tileX ( 128 ), tileY ( 128 );

  // Number of samples in x and y direction.
  //const unsigned int samplesX ( this->numSamples () ), samplesY ( this->numSamples () );

  // Amount to pad the tiles.
  //unsigned int xPadding ( static_cast < unsigned int > ( ::ceil ( filterWidth  / ( 2.0 + 0.5 ) ) ) );
  //unsigned int yPadding ( static_cast < unsigned int > ( ::ceil ( filterHeight / ( 2.0 + 0.5 ) ) ) );

  // Tile width and height including padding.
  /*unsigned int tileWidth  ( tileX + ( 2 * xPadding ) );
  unsigned int tileHeight ( tileY + ( 2 * yPadding ) );*/
  unsigned int tileWidth  ( tileX );
  unsigned int tileHeight ( tileY );

  // Set the tile size.
  _background.update ( tileX, tileY );

  // Set up the scene for the clear node.
  osg::ref_ptr < osg::Group > group ( new osg::Group );
  group->addChild ( sceneView.getSceneData() );
  
  osg::ref_ptr < osg::ClearNode > clear ( new osg::ClearNode );
  clear->setClearColor ( this->clearColor() );
  clear->addChild ( _background.root() );
  
  group->addChild ( clear.get() );
  
  sceneView.setSceneData ( group.get() );

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

  // Set the viewport for the tile.
  //sceneView.setViewport ( 0, 0, tileX * samplesX, tileY * samplesY );
  sceneView.setViewport ( 0, 0, tileX, tileY );

  // Downsampler.
  OsgTools::Images::DownSample downSample;
  downSample.size ( tileX, tileY );
  downSample.numSamples ( this->numSamples( ) );
  downSample.filterSize ( filterWidth, filterHeight );

  //// Create a temp file.
  //std::string filename ( Usul::File::Temp::file() );

  //unsigned int i ( 0 );

  double v ( 0.0 );

  const double du ( static_cast < double > ( tileWidth ) / width );
  const double dv ( static_cast < double > ( tileHeight ) / height );

  typedef OsgTools::Builders::GradientBackground::Corners Corners;

  osg::Vec4 ll ( _background.color ( Corners::BOTTOM_LEFT ) );
  osg::Vec4 lr ( _background.color ( Corners::BOTTOM_RIGHT ) );
  osg::Vec4 ul ( _background.color ( Corners::TOP_LEFT ) );
  osg::Vec4 ur ( _background.color ( Corners::TOP_RIGHT ) );
  
  // Figure out how many tiles will be made.
  const unsigned int tiles ( static_cast<unsigned int> ( ::ceil ( static_cast<double> ( width ) / tileWidth ) ) * 
                             static_cast<unsigned int> ( ::ceil ( static_cast<double> ( height ) / tileHeight ) ) );

  // Number of tiles created.
  unsigned int tile ( 0 );
  
  // Create the tiles.
  for ( int y = -firstY; y < static_cast < int > ( height ) + firstY; y += tileY )
  {
    //unsigned int j ( 0 );
    double u ( 0.0 );

    for ( int x = -firstX; x < static_cast < int > ( width ) + firstX; x += tileX )
    {
      // Feedback.
      std::cout << "Processing tile " << ++tile << " of " << tiles << std::endl;
      
      // Interpolate colors for background.
      typedef Usul::Math::Interpolate<osg::Vec4> Interpolate;
      _background.color ( Interpolate::bilinear ( u,      v,      ll, lr, ur, ul ), Corners::BOTTOM_LEFT );
      _background.color ( Interpolate::bilinear ( u + du, v,      ll, lr, ur, ul ), Corners::BOTTOM_RIGHT );
      _background.color ( Interpolate::bilinear ( u,      v + dv, ll, lr, ur, ul ), Corners::TOP_LEFT );
      _background.color ( Interpolate::bilinear ( u + du, v + dv, ll, lr, ur, ul ), Corners::TOP_RIGHT );
      u += du;

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
     v += dv;
    //++i;
  }

  // Restore background
  _background.color ( ll, Corners::BOTTOM_LEFT );
  _background.color ( lr, Corners::BOTTOM_RIGHT );
  _background.color ( ul, Corners::TOP_LEFT );
  _background.color ( ur, Corners::TOP_RIGHT );
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
  image->allocateImage ( _size [ 0 ], _size [ 1 ], 1, GL_RGB, GL_UNSIGNED_BYTE );

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
        //*result++ = *pixels++;
      }
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the scale.
//
///////////////////////////////////////////////////////////////////////////////

void TiledScreenCapture::scale ( float scale )
{
  _scale = scale;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the scale.
//
///////////////////////////////////////////////////////////////////////////////

float TiledScreenCapture::scale () const
{
  return _scale;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

TiledScreenCapture::ProcessScene::ProcessScene ( float scale ) : BaseClass ( osg::NodeVisitor::TRAVERSE_ALL_CHILDREN ),
_scale ( scale ),
_autoTransforms (),
_projections (),
_texts ()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

TiledScreenCapture::ProcessScene::~ProcessScene ()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Restore the scene to it's orginal state.
//
///////////////////////////////////////////////////////////////////////////////

void TiledScreenCapture::ProcessScene::restoreScene ( )
{
  // Restore auto transform's scale.
  for ( AutoTransforms::iterator iter = _autoTransforms.begin(); iter != _autoTransforms.end(); ++iter )
    iter->first->setAutoScaleToScreen ( iter->second  );

  // Restore projection node masks
  for ( Projections::iterator iter = _projections.begin(); iter != _projections.end(); ++iter )
    iter->first->setNodeMask ( iter->second );

  // Restore osgText::Text.
  for ( Texts::iterator iter = _texts.begin(); iter != _texts.end(); ++iter )
    iter->first->setCharacterSize ( iter->second );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit the projection node.
//
///////////////////////////////////////////////////////////////////////////////

void TiledScreenCapture::ProcessScene::apply( osg::Projection& node )
{
  _projections.insert ( Projections::value_type ( &node, node.getNodeMask () ) );
  node.setNodeMask ( 0x0 );

  node.traverse ( *this );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit the transform node.
//
///////////////////////////////////////////////////////////////////////////////

void TiledScreenCapture::ProcessScene::apply( osg::Transform& node )
{
  if ( osg::AutoTransform *at = dynamic_cast < osg::AutoTransform * > ( &node ) )
  {
    _autoTransforms.insert ( AutoTransforms::value_type ( at, at->getAutoScaleToScreen() ) );
    at->setAutoScaleToScreen ( false );
  }

  node.traverse ( *this );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit the geode node.
//
///////////////////////////////////////////////////////////////////////////////

void TiledScreenCapture::ProcessScene::apply( osg::Geode& node )
{
  unsigned int numDrawables ( node.getNumDrawables () );
  for ( unsigned int i = 0; i < numDrawables; ++i )
  {
    if ( osgText::Text* text = dynamic_cast < osgText::Text* > ( node.getDrawable ( i ) ) )
    {
      _texts.insert ( Texts::value_type ( text, text->getCharacterHeight () ) );
      text->setCharacterSize ( text->getCharacterHeight() * _scale );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set background properties.
//
///////////////////////////////////////////////////////////////////////////////

void TiledScreenCapture::background ( const OsgTools::Builders::GradientBackground& background )
{
  _background = background;
}
