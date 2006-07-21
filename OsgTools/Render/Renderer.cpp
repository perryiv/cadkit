
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  Copyright (c) 2005, Perry L. Miller IV and Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "Renderer.h"

#include "OsgTools/Jitter.h"
#include "OsgTools/Render/RecordTime.h"
#include "OsgTools/ScopedProjection.h"
#include "OsgTools/Render/Defaults.h"
#include "OsgTools/Images/Matrix.h"

#include "Usul/Errors/Checker.h"
#include "Usul/Bits/Bits.h"
#include "Usul/System/Clock.h"
#include "Usul/Errors/Stack.h"
#include "Usul/Shared/Preferences.h"
#include "Usul/Registry/Constants.h"

#include "osg/Texture2D"

#include "osgUtil/UpdateVisitor"

#include "osgDB/WriteFile"
#include <iomanip>

using namespace OsgTools::Render;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

Renderer::Renderer() : BaseClass(),
_sceneView ( new osgUtil::SceneView ),
_times(),
_numPasses ( 1 ),
_contextId ( 0 ),
_hasAccumulationBuffer ( false )
{
  // Set the update-visitor.
  _sceneView->setUpdateVisitor ( new osgUtil::UpdateVisitor );

  // Set the display settings.
  _sceneView->setDisplaySettings ( new osg::DisplaySettings() );

  // Set the viewer's defaults.
  _sceneView->setDefaults();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

Renderer::~Renderer()
{
  _sceneView = 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize.
//
///////////////////////////////////////////////////////////////////////////////

void Renderer::init()
{
  GLint red ( 0 ), green ( 0 ), blue ( 0 ), alpha ( 0 );
  ::glGetIntegerv ( GL_ACCUM_RED_BITS,   &red   );
  ::glGetIntegerv ( GL_ACCUM_GREEN_BITS, &green );
  ::glGetIntegerv ( GL_ACCUM_BLUE_BITS,  &blue  );
  ::glGetIntegerv ( GL_ACCUM_ALPHA_BITS, &alpha );
  const bool hasAccum ( red && green && blue && alpha );
  _hasAccumulationBuffer = hasAccum;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear.
//
///////////////////////////////////////////////////////////////////////////////

void Renderer::clear()
{
  _sceneView->releaseAllGLObjects();
  _sceneView->flushAllDeletedGLObjects();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is there an accumulation buffer?
//
///////////////////////////////////////////////////////////////////////////////

bool Renderer::hasAccumBuffer() const
{
  return _hasAccumulationBuffer;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the clipping plane distances.
//
///////////////////////////////////////////////////////////////////////////////

void Renderer::nearFar ( double &n, double &f ) const
{
  // If we have a valid viewer...
  if ( _sceneView.valid() )
  {
    // Get the projection matrix.
    osg::Matrix P ( _sceneView->getProjectionMatrix() );

    // Try to get perspective parameters.
    double left, right, bottom, top;
    if ( P.getFrustum ( left, right, bottom, top, n, f ) )
      return;

    // Try to get orthographic parameters.
    if ( P.getOrtho ( left, right, bottom, top, n, f ) )
      return;
  }

  // If we get to here then it did not work.
  throw ( std::runtime_error ( "Failed to calculate far clipping distance" ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Render the scene.
//
///////////////////////////////////////////////////////////////////////////////

void Renderer::render()
{
  // Handle no viewer.
  if ( !_sceneView.valid() )
    return;

  // If there is no scene then clear the screen. 
  // Otherwise, weird artifacts may show up.
  if ( !_sceneView->getSceneData() )
  {
    const osg::Vec4 &color = this->backgroundColor();
    ::glClearColor ( color[0], color[1], color[2], color[3] );
    ::glClear ( _sceneView->getRenderStage()->getClearMask() );
  }

  // Handle particles and osg-animations.
  osg::ref_ptr<osg::FrameStamp> fs ( new osg::FrameStamp );
  fs->setFrameNumber ( fs->getFrameNumber() + 1 );
  fs->setReferenceTime ( ::time ( 0x0 )  );
  _sceneView->setFrameStamp ( fs.get() );

  // Draw
  // See if we are supposed to use multiple passes.
  if ( this->numRenderPasses() > 1 )
    this->_multiPassRender();
  else
    this->_singlePassRender();

  // Check for errors.
  USUL_ERROR_CHECKER ( GL_NO_ERROR == ::glGetError() );

}



///////////////////////////////////////////////////////////////////////////////
//
//  Handle single-pass rendering.
//
///////////////////////////////////////////////////////////////////////////////

void Renderer::_singlePassRender()
{
  // Handle no viewer.
  if ( !_sceneView.valid() )
    return;

  // Cull and draw.
  this->_cullAndDraw();  
}


///////////////////////////////////////////////////////////////////////////////
//
//  Handle multi-pass rendering.
//
///////////////////////////////////////////////////////////////////////////////

void Renderer::_multiPassRender()
{
  // Handle no viewer.
  if ( !_sceneView.valid() )
    return;

  // Safely...
  try
  {
    // Save original projection matrix.
    OsgTools::ScopedProjection sp ( _sceneView.get() );

    // Clear the accumulation buffer.
    ::glClearAccum ( 0.0f, 0.0f, 0.0f, 0.0f );
    ::glClear ( GL_ACCUM_BUFFER_BIT );

    // Check for errors.
    USUL_ERROR_CHECKER ( GL_NO_ERROR == ::glGetError() );

    // Needed in the loop.
    osg::Matrixd matrix;
    osg::ref_ptr<osg::Viewport> vp ( this->viewport() );
    const osg::Matrixd &proj = _sceneView->getProjectionMatrix();

    // Loop through the passes...
    for ( unsigned int i = 0; i < this->numRenderPasses(); ++i )
    {
      // Set the proper projection matrix.
      OsgTools::Jitter::instance().perspective ( _numPasses, i, *vp, proj, matrix );
      _sceneView->setProjectionMatrix ( matrix );

      // Render a single pass.
      this->_singlePassRender();

      // Check for errors.
      USUL_ERROR_CHECKER ( GL_NO_ERROR == ::glGetError() );

      // Accumulate the pixels from the frame buffer.
      float value ( 1.0f / static_cast < float > ( this->numRenderPasses() ) );
      ::glAccum ( GL_ACCUM, value );

      // Check for errors.
      USUL_ERROR_CHECKER ( GL_NO_ERROR == ::glGetError() );
    }

    // Transfer the accumulation buffer into the frame buffer.
    ::glAccum ( GL_RETURN, 1.0f );

    // Check for errors.
    USUL_ERROR_CHECKER ( GL_NO_ERROR == ::glGetError() );
  }

  // Catch all exceptions and reset the number of passes. Otherwise, you 
  // can get stuck in a loop where the error dialog causes a render, and 
  // the render causes an error dialog, etc.

  // Standard exceptions.
  catch ( const std::exception &e )
  {
    Usul::Errors::Stack::instance().push ( e.what() );
    std::ostringstream message;
    message << "Error 2156758683: Standard exception caught when attempting to render with " << this->numRenderPasses() << " passes";
    this->numRenderPasses ( 1 );
    throw std::runtime_error ( message.str() );
  }

  // Unknown exceptions.
  catch ( ... )
  {
    std::ostringstream message;
    message << "Error 3880806891: Unknown exception caught when attempting to render with " << this->numRenderPasses() << " passes";
    this->numRenderPasses ( 1 );
    throw std::runtime_error ( message.str() );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the background color.
//
///////////////////////////////////////////////////////////////////////////////

void Renderer::backgroundColor ( const osg::Vec4 &color )
{
  // Set the color.
  if ( this->viewer() )
    this->viewer()->setClearColor ( color );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the background color.
//
///////////////////////////////////////////////////////////////////////////////

const osg::Vec4 &Renderer::backgroundColor() const
{
  if ( !this->viewer() )
    throw ( std::runtime_error ( "Cannot get the background color because there is no viewer" ) );
  return this->viewer()->getClearColor();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the viewport.
//
///////////////////////////////////////////////////////////////////////////////

void Renderer::viewport ( osg::Viewport *vp )
{
  if ( _sceneView.valid() )
    _sceneView->setViewport ( vp );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the viewport.
//
///////////////////////////////////////////////////////////////////////////////

void Renderer::viewport ( int x, int y, unsigned int w, unsigned int h )
{
  if ( _sceneView.valid() )
    _sceneView->setViewport ( x, y, (int) w, (int) h );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the viewport.
//
///////////////////////////////////////////////////////////////////////////////

const osg::Viewport *Renderer::viewport() const
{
  return ( _sceneView.valid() ) ? _sceneView->getViewport() : 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the viewport.
//
///////////////////////////////////////////////////////////////////////////////

osg::Viewport *Renderer::viewport()
{
  return ( _sceneView.valid() ) ? _sceneView->getViewport() : 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the viewer.
//
///////////////////////////////////////////////////////////////////////////////

Renderer::SceneView *Renderer::viewer()
{
  return _sceneView.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the viewer.
//
///////////////////////////////////////////////////////////////////////////////

const Renderer::SceneView *Renderer::viewer() const
{
  return _sceneView.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the viewer.
//
///////////////////////////////////////////////////////////////////////////////

void Renderer::viewer ( SceneView *viewer )
{
  _sceneView = viewer;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Handle the cull and draw.
//
///////////////////////////////////////////////////////////////////////////////

void Renderer::_cullAndDraw()
{
  // Handle no viewer.
  if ( false == _sceneView.valid() )
    return;

  typedef RecordTime< Renderer > RecordTime;

  // Cull.
  {
    RecordTime ct ( *this, "cull" );
    _sceneView->cull();
  }

  // Draw.
  {
    RecordTime dt ( *this, "draw" );
    _sceneView->draw();
  }
}

///////////////////////////////////////////////////////////////////////////////
//
//  Record the start-time.
//
///////////////////////////////////////////////////////////////////////////////

void Renderer::timeStart( const std::string& name )
{
  // Record the time.
  TimePair now ( (double) Usul::System::Clock::milliseconds() / (double) CLOCKS_PER_SEC, 0 );
  _times[name].push_back ( now );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Record the end-time.
//
///////////////////////////////////////////////////////////////////////////////

void Renderer::timeStop( const std::string &name )
{
  // Record the end-time.
  TimeHistory &h = _times[name];
  TimePair &p = h.back();
  p.second = (double) Usul::System::Clock::milliseconds() / (double) CLOCKS_PER_SEC;

  // Trim to a reasonable size.
  while ( h.size() > 100 )
    h.pop_front();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the last time.
//
///////////////////////////////////////////////////////////////////////////////

double Renderer::timeLast ( const std::string &name ) const
{
  // Try to get the time-history.
  TimeHistories::const_iterator i = _times.find ( name );
  if ( _times.end() == i )
  {
    return 0.0;
  }

  // Return the most recent time.
  const TimeHistory &h = i->second;
  const TimePair &p = h.back();
  return ( p.second - p.first );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the average time.
//
///////////////////////////////////////////////////////////////////////////////

double Renderer::timeAverage ( const std::string &name ) const
{
  // Try to get the time-history.
  TimeHistories::const_iterator i = _times.find ( name );
  if ( _times.end() == i )
  {
    return 0.0;
  }

  // Sum the durations.
  const TimeHistory &h = i->second;
  double sum ( 0 );
  for ( TimeHistory::const_iterator j = h.begin(); j != h.end(); ++j )
  {
    const TimePair &p = *j;
    sum += ( p.second - p.first );
  }

  // Return the average.
  return sum / h.size();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the number of rendering passes. Unavailable numbers have no effect.
//
///////////////////////////////////////////////////////////////////////////////

void Renderer::numRenderPasses ( unsigned int num )
{
  if ( 1 == num || OsgTools::Jitter::instance().available ( num ) )
    _numPasses = num;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set a unique ID for the viewer.
//
///////////////////////////////////////////////////////////////////////////////

void Renderer::uniqueID ( unsigned int id )
{
  // Counter for display-list id. OSG will handle using the correct display 
  // list for this context.
  _sceneView->getState()->setContextID ( id );

  _contextId = id;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the scene.
//
///////////////////////////////////////////////////////////////////////////////

void Renderer::scene ( osg::Node *node )
{
  _sceneView->setSceneData ( node );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the scene.
//
///////////////////////////////////////////////////////////////////////////////

const osg::Node * Renderer::scene() const
{
  return _sceneView->getSceneData ( );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the scene.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node * Renderer::scene()
{
  return _sceneView->getSceneData ( );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the scene.
//
///////////////////////////////////////////////////////////////////////////////

void Renderer::updateScene()
{
  if( !_sceneView.valid() )
    return;

  typedef RecordTime< Renderer > RecordTime;

  {
    RecordTime ut ( *this, "update" );
    _sceneView->update();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Accumulate an image list into a single image.
//
///////////////////////////////////////////////////////////////////////////////

osg::Image* Renderer::_accumulate( const ImageList &images ) const
{
  osg::ref_ptr < osg::Image > image ( new osg::Image );

  if ( images.size() > 1 )
  {
    osg::ref_ptr< const osg::Image > front ( images.front().get() );

    unsigned int width ( front->s() );
    unsigned int height ( front->t() );

    OsgTools::Images::Matrix< osg::Vec3f > accumulationBuffer ( width, height );

    float mult ( 1.0f / (float) images.size() );

    unsigned int size ( front->getImageSizeInBytes() );

    std::vector < float > buffer ( width * height * 3, 0.0 );

    for( ImageList::const_iterator iter = images.begin(); iter != images.end(); ++iter )
    {
      const unsigned char *pixels ( (*iter)->data() );

      for ( unsigned int i = 0; i < size; ++ i )
      {
        buffer.at( i ) += pixels[i] * mult;
      }
    }
    
    image->allocateImage ( width, height, 1, front->getPixelFormat(), front->getDataType() );

    unsigned char *pixels ( image->data() );

    for ( unsigned int i = 0; i < size; ++ i )
    {
      pixels[i] = buffer.at( i );
    }
#if 0

      /*std::ostringstream out;
      out << "E:/adam/models/image" << std::setw ( 3 ) << iter - images.begin() << ".jpg";
      std::string outname ( out.str() );
      std::replace ( outname.begin(), outname.end(), ' ', '0' );

      osgDB::writeImageFile ( *(*iter), outname );*/

      for( unsigned int t = 0; t < height; ++t )
      {
        for ( unsigned int s = 0; s < width; ++s )
        {
        
          const unsigned char *temp ( (*iter)->data( s, t ) );

          osg::Vec3f & data ( accumulationBuffer.at( s, t ) );

          unsigned char red ( temp[0] );
          unsigned char green ( temp[1] );
          unsigned char blue ( temp[2] );

          data[0] += red * mult;
          data[1] += green * mult;
          data[2] += blue * mult;
        }
      }
    }

    image->allocateImage ( width, height, 1, front->getPixelFormat(), front->getDataType() );

    for ( unsigned int s = 0; s < width; ++s )
    {
      for( unsigned int t = 0; t < height; ++t )
      {
        const osg::Vec3f & temp ( accumulationBuffer.at( s, t ) );

        unsigned char *data ( image->data( s, t ) );

        *data = (unsigned char) temp[0];
        *(data+1) = (unsigned char) temp[1];
        *(data+2) = (unsigned char) temp[2];
      }
    }
#endif
  }

  return image.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Screen capture at the given view with the given height and width
//
///////////////////////////////////////////////////////////////////////////////

osg::Image* Renderer::screenCapture ( const osg::Matrix& matrix, unsigned int width, unsigned int height )
{
  osg::ref_ptr< osg::Image > image ( new osg::Image() );

  osg::Matrix oldViewMatrix ( _sceneView->getViewMatrix() );

  try
  {
    _sceneView->setViewMatrix ( matrix );

    if ( this->numRenderPasses() > 1 )
    {
      // Save original projection matrix.
      OsgTools::ScopedProjection sp ( _sceneView.get() );

      // Needed in the loop.
      osg::Matrixd pMatrix;
      //osg::ref_ptr<osg::Viewport> vp ( new osg::Viewport( 0, 0, width, height ) /*this->viewport()*/ );
      osg::ref_ptr<osg::Viewport> vp ( this->viewport() );
      const osg::Matrixd &proj = _sceneView->getProjectionMatrix();

      // vector to store the images.
      ImageList images;

      // Loop through the passes...
      for ( unsigned int i = 0; i < this->numRenderPasses(); ++i )
      {
        // Set the proper projection matrix.
        OsgTools::Jitter::instance().perspective ( _numPasses, i, *vp, proj, pMatrix );
        _sceneView->setProjectionMatrix ( pMatrix );

        images.push_back( new osg::Image );
        
        // Fill in the image.
        this->_screenCapture ( *images.back(), pMatrix, width, height );
      }

      image = this->_accumulate( images );
    }
    else
    {
      // Fill in the image.
      this->_screenCapture ( *image, width, height );
    }
  }

  // If there is an exception, set the old view matrix and re throw.
  catch ( ... )
  {
    _sceneView->setViewMatrix( oldViewMatrix );
    throw;
  }

  _sceneView->setViewMatrix( oldViewMatrix );

  return image.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Capture the screen.
//
///////////////////////////////////////////////////////////////////////////////

void Renderer::_screenCapture ( osg::Image& image, unsigned int width, unsigned int height )
{
  // Set the projection matrix.
  double fovy  ( Usul::Shared::Preferences::instance().getDouble ( Usul::Registry::Keys::FOV ) );
  double zNear ( OsgTools::Render::Defaults::CAMERA_Z_NEAR );
  double zFar  ( OsgTools::Render::Defaults::CAMERA_Z_FAR );
  double w ( width ), h ( height );
  double aspect ( w / h );

  this->_screenCapture ( image, osg::Matrix::perspective( fovy, aspect, zNear, zFar ), width, height );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Capture the screen.
//
///////////////////////////////////////////////////////////////////////////////

void Renderer::_screenCapture ( osg::Image& image, const osg::Matrix& projection, unsigned int width, unsigned int height )
{
  // Should we use frame buffer objects?
  bool useFBO ( osg::FBOExtensions::instance( _contextId/*, true*/ )->isSupported() && height <= 4096 && width <= 4096 );

  // Make enough space
  image.allocateImage ( width, height, 1, GL_RGB, GL_UNSIGNED_BYTE );
      
  if ( useFBO )
    this->_fboScreenCapture ( image, projection, width, height );
  else
    this->_tiledScreenCapture( image, projection, width, height );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Capture the screen using a frame buffer object.
//
///////////////////////////////////////////////////////////////////////////////

void Renderer::_fboScreenCapture ( osg::Image& image, const osg::Matrix& projection, unsigned int width, unsigned int height )
{
  // Set up the texture.
  osg::ref_ptr< osg::Texture2D > tex ( new osg::Texture2D );
  tex->setTextureSize(width, height);
  tex->setInternalFormat(GL_RGBA);
  tex->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR);
  tex->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);
  tex->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
  tex->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE);
  
  // Make the fbo.
  osg::ref_ptr< osg::FrameBufferObject > fbo ( new osg::FrameBufferObject );
  fbo->setAttachment(GL_COLOR_ATTACHMENT0_EXT, osg::FrameBufferAttachment(tex.get()));
  fbo->setAttachment(GL_DEPTH_ATTACHMENT_EXT, osg::FrameBufferAttachment(new osg::RenderBuffer(width, height, GL_DEPTH_COMPONENT24)));

  // Make the camera buffer.
  osg::ref_ptr< osg::CameraNode > camera ( new osg::CameraNode );
  camera->setClearColor( this->viewer()->getClearColor() );
  camera->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  camera->setViewport(0, 0, width, height);

  // Set the camera to render before the main camera.
  camera->setRenderOrder(osg::CameraNode::PRE_RENDER);

  // Set the projection matrix.
  camera->setProjectionMatrix ( projection );

  // Set view.
  camera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
  camera->setViewMatrix ( this->viewer()->getViewMatrix() );

  // Tell the camera to use OpenGL frame buffer object where supported.
  camera->setRenderTargetImplementation( osg::CameraNode::FRAME_BUFFER_OBJECT );

  // Attach the texture and use it as the color buffer.
  camera->attach( osg::CameraNode::COLOR_BUFFER, &image );

  // Save the old root.
  osg::ref_ptr< osg::Node > node ( this->scene() );

  // Add the scene to the camera.
  camera->addChild ( this->scene() );

  // Make the camera file the scene data.
  this->viewer()->setSceneData ( camera.get() );

  // Render to the image.
  this->_singlePassRender();

  // Set the old root back to the scene data.
  this->viewer()->setSceneData ( node.get() );

  // Figure out how to avoid this last render.
  this->render();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Capture the screen using tiles.
//
///////////////////////////////////////////////////////////////////////////////

void Renderer::_tiledScreenCapture ( osg::Image& image, const osg::Matrix& projection, unsigned int width, unsigned int height )
{
  // Tile height and width
    const unsigned int tileWidth ( 256 );
    const unsigned int tileHeight ( 256 );

    // Calculate the number of rows and columns we will need
    const unsigned int numRows ( ( height + tileHeight - 1 ) / tileHeight );
    const unsigned int numCols ( ( width + tileWidth - 1 )   / tileWidth  );

    // Set the current tile
    unsigned int currentTile ( 0 );

    // Get the old viewport
    osg::ref_ptr<osg::Viewport> ovp ( static_cast < osg::Viewport* > ( this->viewport()->clone( osg::CopyOp::DEEP_COPY_ALL ) ) );

    osg::Matrix opm ( this->viewer()->getProjectionMatrix() );

    //double fovy  ( OsgTools::Render::Defaults::CAMERA_FOV_Y );
    double zNear ( 0.0 );
    double zFar  ( 0.0 );
    //double aspect ( width / height );

    double top     ( 0.0 );
    double bottom  ( 0.0 );
    double left    ( 0.0 );
    double right   ( 0.0 );

    projection.getFrustum( left, right, bottom, top, zNear, zFar);

    do
    {
      // Begin tile 
      const unsigned int currentRow ( currentTile / numCols );
      const unsigned int currentCol ( currentTile % numCols );
      
      // Current tile height and width
      unsigned int currentTileHeight ( 0 );
      unsigned int currentTileWidth  ( 0 );

      // Get the current tile height.  Accounts for tiles at end that are not comlete
      if ( currentRow < numRows - 1 )
        currentTileHeight = tileHeight;
      else
        currentTileHeight = height - ( ( numRows - 1 ) * tileHeight );

      // Get the current tile width.  Accounts for tiles at end that are not comlete
      if ( currentCol  < numCols - 1 )
        currentTileWidth = tileWidth;
      else
        currentTileWidth = width - ( ( numCols - 1 ) * tileWidth );    

      // Set the view port to the tile width and height
      this->viewer()->setViewport ( 0, 0, currentTileWidth, currentTileHeight );

      // compute projection parameters
      const double currentLeft   ( left          + ( right - left ) *  ( currentCol * tileWidth ) / width );
      const double currentRight  ( currentLeft   + ( right - left ) *            currentTileWidth / width );
      const double currentBottom ( bottom        + ( top - bottom ) * ( currentRow * tileHeight ) / height );
      const double currentTop    ( currentBottom + ( top - bottom ) *           currentTileHeight / height );

      // Set the new frustum
      this->viewer()->setProjectionMatrixAsFrustum ( currentLeft, currentRight, currentBottom, currentTop, zNear, zFar );
      
      // Draw
      this->_singlePassRender();

      // Previous values
      GLint prevRowLength, prevSkipRows, prevSkipPixels;

      // save current glPixelStore values
      ::glGetIntegerv(GL_PACK_ROW_LENGTH,  &prevRowLength);
      ::glGetIntegerv(GL_PACK_SKIP_ROWS,   &prevSkipRows);
      ::glGetIntegerv(GL_PACK_SKIP_PIXELS, &prevSkipPixels);

      // Calculate position in image buffer to write to
      GLint destX ( currentTileWidth  * currentCol );
      GLint destY ( currentTileHeight * currentRow );

      // setup pixel store for glReadPixels
      // This makes sure that the buffer is read into the correct spot in the image buffer
      ::glPixelStorei(GL_PACK_ROW_LENGTH,  width);
      ::glPixelStorei(GL_PACK_SKIP_ROWS,   destY);
      ::glPixelStorei(GL_PACK_SKIP_PIXELS, destX);

      // read the tile into the final image
      ::glReadPixels(0, 0, currentTileWidth, currentTileHeight, GL_RGB, GL_UNSIGNED_BYTE, image.data( ) );

      // restore previous glPixelStore values
      ::glPixelStorei(GL_PACK_ROW_LENGTH,  prevRowLength);
      ::glPixelStorei(GL_PACK_SKIP_ROWS,   prevSkipRows);
      ::glPixelStorei(GL_PACK_SKIP_PIXELS, prevSkipPixels);

      // Go the the next tile
      currentTile++;

      // Are we done?
      if ( currentTile >= numRows * numCols )
        break;

    } while ( 1 );

    // Restore old settings
    this->viewport ( ovp.get() );
    this->viewer()->setProjectionMatrix( opm );
    //this->resize ( ovp->width(), ovp->height() );
}
