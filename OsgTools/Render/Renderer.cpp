
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L. Miller IV and Adam Kubach
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
#include "OsgTools/Render/ClampProjection.h"
#include "OsgTools/Builders/GradientBackground.h"

#include "Usul/Errors/Checker.h"
#include "Usul/Bits/Bits.h"
#include "Usul/Errors/Stack.h"
#include "Usul/Interfaces/GUI/IProgressBar.h"
#include "Usul/Interfaces/GUI/IStatusBar.h"
#include "Usul/Interfaces/GUI/ICancelButton.h"
#include "Usul/Policies/Update.h"
#include "Usul/Registry/Constants.h"
#include "Usul/Resources/ProgressBar.h"
#include "Usul/Resources/StatusBar.h"
#include "Usul/Resources/CancelButton.h"
#include "Usul/Resources/TextWindow.h"
#include "Usul/Shared/Preferences.h"
#include "Usul/System/Clock.h"
#include "Usul/Headers/OpenGL.h"

#include "osg/Texture2D"
#include "osg/Version"
#include "osg/MatrixTransform"
#include "osg/Geometry"
#include "osg/Geode"

#ifndef USE_DEPRECATED_API
#define USE_DEPRECATED_API
#endif

#include "osg/CameraNode"

#include "osg/GLU"

#include "osgUtil/UpdateVisitor"

#include "osgDB/WriteFile"

#include <iomanip>
#include <list>
#include <fstream>

using namespace OsgTools::Render;


namespace Detail { const unsigned int NUM_CHANNELS ( 3 ); }


namespace Detail
{
  // Defined in Viewer.cpp
  void checkForErrors( unsigned int id );
}

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

Renderer::Renderer() : BaseClass(),
  _sceneView ( new osgUtil::SceneView ),
  _framestamp ( new osg::FrameStamp ),
  _times(),
  _timer(),
  _start_tick ( 0 ),
  _numPasses ( 1 ),
  _contextId ( 0 ),
  _hasAccumBuffer ( false )
{
  // Set the start tick.
  _start_tick = _timer.tick();

  _sceneView->setFrameStamp ( _framestamp.get() );

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
  // Set defaults for the global state set.
  this->getGlobalStateSet()->setGlobalDefaults();

  // Find out how many accumulation bits we have.
  GLint red ( 0 ), green ( 0 ), blue ( 0 ), alpha ( 0 );
  ::glGetIntegerv ( GL_ACCUM_RED_BITS,   &red   );
  ::glGetIntegerv ( GL_ACCUM_GREEN_BITS, &green );
  ::glGetIntegerv ( GL_ACCUM_BLUE_BITS,  &blue  );
  ::glGetIntegerv ( GL_ACCUM_ALPHA_BITS, &alpha );
  const bool hasAccum ( red && green && blue && alpha );
  _hasAccumBuffer = hasAccum;

  // This is a work-around for the fact that some geometries have a 
  // calculated near or far distance of zero. SceneViewer::cull() does not 
  // handle this case, and the projection matrix ends up with NANs.
  osgUtil::CullVisitor *cv ( this->viewer()->getCullVisitor() );
  cv->setClampProjectionMatrixCallback ( new OsgTools::Render::ClampProjection ( *cv, OsgTools::Render::Defaults::CAMERA_Z_NEAR, OsgTools::Render::Defaults::CAMERA_Z_FAR ) );

  // Related to above, and new with 0.9.8-2. osgUtil::SceneView and 
  // osgUtil::CullVisitor both inherit from osg::CullSettings. SceneView 
  // passes "*this" to it's cull visitor's CullSettings::inheritCullSettings, 
  // resetting the clamp-projection callback to null.
  cv->setInheritanceMask ( Usul::Bits::remove ( cv->getInheritanceMask(), osg::CullSettings::CLAMP_PROJECTION_MATRIX_CALLBACK ) );

  cv->setCullingMode ( Usul::Bits::remove ( cv->getCullingMode(), osg::CullingSet::SMALL_FEATURE_CULLING ) );

  this->viewer()->setCullingMode ( cv->getCullingMode() );
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
  return _hasAccumBuffer;
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
  if ( false == _sceneView.valid() )
    return;

  // If there is no scene then clear the screen. 
  // Otherwise, weird artifacts may show up.
  if ( 0x0 == _sceneView->getSceneData() )
  {
    const osg::Vec4 &color = this->backgroundColor();
    ::glClearColor ( color[0], color[1], color[2], color[3] );
    ::glClear ( _sceneView->getRenderStage()->getClearMask() );
  }

  // See if we are supposed to use multiple passes.
  if ( this->numRenderPasses() > 1 )
    this->_multiPassRender();
  else
    this->_singlePassRender();

  // Check for errors.
  Detail::checkForErrors( 2764743220 );
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
    Detail::checkForErrors( 2576774470 );

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
      Detail::checkForErrors( 2681461970 );

      // Accumulate the pixels from the frame buffer.
      float value ( 1.0f / static_cast < float > ( this->numRenderPasses() ) );
      ::glAccum ( GL_ACCUM, value );

      // Check for errors.
      Detail::checkForErrors( 3558773016 );
    }

    // Transfer the accumulation buffer into the frame buffer.
    ::glAccum ( GL_RETURN, 1.0f );

    // Check for errors.
    Detail::checkForErrors( 2459899470 );
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
  if ( 0x0 != this->viewer() )
    this->viewer()->setClearColor ( color );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the background color.
//
///////////////////////////////////////////////////////////////////////////////

osg::Vec4 Renderer::backgroundColor() const
{
  return ( 0x0 == this->viewer() ) ? osg::Vec4 ( 0, 0, 0, 1 ) : this->viewer()->getClearColor();
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

void Renderer::viewer( SceneView* viewer )
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
  return _sceneView->getSceneData();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the scene.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node * Renderer::scene()
{
  return _sceneView->getSceneData();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the scene.
//
///////////////////////////////////////////////////////////////////////////////

void Renderer::update()
{
  // Handle particles and osg-animations.
  _framestamp->setFrameNumber ( _framestamp->getFrameNumber() + 1 );
  _framestamp->setReferenceTime ( _timer.delta_s( _start_tick, _timer.tick() ) );

  _sceneView->getUpdateVisitor()->setTraversalNumber(_framestamp->getFrameNumber());

  typedef RecordTime< Renderer > RecordTime;
  {
    RecordTime ut ( *this, "update" );  
    _sceneView->update();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make a temporary file and fill it with zeros. These will be the "pixels".
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  std::string makeFile ( unsigned int width, unsigned int height )
  {
    Usul::File::Temp file ( Usul::File::Temp::BINARY );

    typedef std::vector < unsigned int > Pixels;
    Pixels row ( width * Detail::NUM_CHANNELS, 0 );
    const unsigned int bytesPerRow ( row.size() * sizeof ( Pixels::value_type ) );

    // Write the dimensions.
    file.stream().write ( reinterpret_cast < const char * > ( &width ), sizeof ( width ) );
    file.stream().write ( reinterpret_cast < const char * > ( &height ), sizeof ( height ) );

    // Fill the file with zeros, row by row.
    for ( unsigned int i = 0; i < height; ++ i )
      file.stream().write ( reinterpret_cast < const char * > ( &row[0] ), bytesPerRow );

    // Return name.
    file.release();
    return file.name();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add the pixels from the temp file to the answer file.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  template < class InputIterator, class OutputIterator >
  void addPixels ( InputIterator begin, InputIterator end, OutputIterator out )
  {
    for ( InputIterator iter = begin; iter != end; ++iter )
    {
      *out += *iter;
      out++;
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add the pixels from the temp file to the answer file.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  template < class OutputContainer >
  void addPixels ( const std::string &image, OutputContainer& out, unsigned int width, unsigned int height )
  {
    // Open image for reading.
    std::ifstream in ( image.c_str(), std::ios::binary );
    if ( false == in.is_open() )
      throw std::runtime_error ( "Error 4068209531: failed to open file: " + image );

    // Read the size.
    unsigned int w ( 0 ), h ( 0 );
    in.read ( reinterpret_cast < char * > ( &w ), sizeof ( w ) );
    in.read ( reinterpret_cast < char * > ( &h ), sizeof ( h ) );
    if ( w != width || h != height )
      throw std::runtime_error ( "Error 2085849966: Inconsistant image sizes" );

    // Pixels in temporary intermediate files are unsigned chars.
    std::vector < unsigned char > row ( width * Detail::NUM_CHANNELS, 0 );

    typedef typename OutputContainer::iterator OutputIterator;
    typedef typename OutputIterator::value_type PixelComponent;

    // Loop through the rows of the image.
    for ( unsigned int i = 0; i < height; ++i )
    {
      // Read a single row from temp file.
      in.read ( reinterpret_cast < std::ifstream::char_type * > ( &row[0] ), row.size() );

      // Position to seek to.
      const unsigned int position ( i * width * Detail::NUM_CHANNELS );

      // Add this row to the answer file.
      Detail::addPixels ( row.begin(), row.end(), out.begin() + position );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Accumulate an image list into a single image.
//
///////////////////////////////////////////////////////////////////////////////

osg::Image* Renderer::_accumulate ( ImageList &images, unsigned int width, unsigned int height, GLenum pixelFormat, GLenum dataType ) const
{
  // Handle empty list.
  if ( true == images.empty() )
    return 0x0;

  Usul::Interfaces::IProgressBar::UpdateProgressBar progress ( 0, 1, Usul::Resources::progressBar() );
  Usul::Interfaces::IStatusBar::UpdateStatusBar status ( Usul::Resources::statusBar() );
  Usul::Policies::TimeBased elapsed ( 1000 );

  const unsigned int total ( images.size() );
  unsigned int count ( 0 );

  // Make a temporary vector and fill it with zeros. These will be the "pixels".
  std::vector < unsigned short > answer ( width * height * Detail::NUM_CHANNELS, 0 );

  // Loop through the image files.
  for ( ImageList::iterator iter = images.begin(); iter != images.end(); ++iter )
  {
    // Add the pixels from the temp file.
    Detail::addPixels ( *iter, answer, width, height );

    // Feedback
    if ( elapsed() )
    {
      progress ( count, total );
      std::ostringstream out;
      out << "Accumulating image " << std::distance ( images.begin(), iter ) + 1 << " of " << images.size();
      status ( out.str(), true );
    }
    ++count;
  }

  // Used to average.
  const float mult ( 1.0f / (float) images.size() );

  // Allocate the image.
  osg::ref_ptr < osg::Image > image ( new osg::Image );
  image->allocateImage ( width, height, 1, pixelFormat, dataType );

  // Image buffer
  unsigned char *buffer = image->data ( );

  // Read each row.
  for ( unsigned int r = 0; r < answer.size(); ++r )
  {
    buffer[r] = static_cast < unsigned char > ( static_cast < float > ( answer[r] ) * mult );
  }

  // All done.
  return image.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Small class to remove files in destructor.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  struct RemoveFiles
  {
    RemoveFiles ( Renderer::ImageList &files ) : _files ( files )
    {
    }
    ~RemoveFiles()
    {
      for ( Renderer::ImageList::iterator i = _files.begin(); i != _files.end(); ++i )
      {
        const std::string &file ( *i );
        try
        {
          Usul::File::Temp::remove ( file, true );
        }
        catch ( const std::exception & )
        {
          std::cout << "Error 2377855880: Failed to remove file '" << file << std::endl;
        }
        catch ( ... )
        {
          std::cout << "Error 2276766138: Failed to remove file '" << file << std::endl;
        }
      }
    }
    Renderer::ImageList &_files;
  };
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the image to file as raw data.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  void writeImage ( const osg::Image &image, const std::string &file )
  {
    std::ofstream out ( file.c_str(), std::ios::binary );
    if ( false == out.is_open() )
      throw std::runtime_error ( "Error 2581366847: Failed to open file for writing: " + file );
    if( image.s() < 0 )
      throw std::runtime_error ( "Error 1241366246: width is less than zero." );
    if( image.t() < 0 )
      throw std::runtime_error ( "Error 3243622229: height is less than zero." );
    const unsigned int width  ( static_cast < unsigned int > ( image.s() ) );
    const unsigned int height ( static_cast < unsigned int > ( image.t() ) );
    out.write ( reinterpret_cast < const char * > ( &width ),  sizeof ( width  ) );
    out.write ( reinterpret_cast < const char * > ( &height ), sizeof ( height ) );
    out.write ( reinterpret_cast < const char * > ( image.data() ), image.getImageSizeInBytes() );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Screen capture at the given view with the given height and width
//
///////////////////////////////////////////////////////////////////////////////

osg::Image* Renderer::screenCapture ( const osg::Matrix& matrix, unsigned int width, unsigned int height )
{
  osg::ref_ptr< osg::Image > answer ( new osg::Image() );
  osg::Matrix oldViewMatrix ( _sceneView->getViewMatrix() );

  // Initialize progress bar.
  Usul::Interfaces::IProgressBar::ShowHide showHide ( Usul::Resources::progressBar() );
  Usul::Interfaces::IProgressBar::UpdateProgressBar progress ( 0, 1, Usul::Resources::progressBar() );
  Usul::Interfaces::IStatusBar::UpdateStatusBar status ( Usul::Resources::statusBar() );
  Usul::Interfaces::ICancelButton::ShowHide cancel ( Usul::Resources::cancelButton() );

  try
  {
    _sceneView->setViewMatrix ( matrix );

    if ( this->numRenderPasses() > 1 )
    {
      // Save original projection matrix.
      OsgTools::ScopedProjection sp ( _sceneView.get() );

      // Needed in the loop.
      osg::Matrixd pMatrix;
      osg::ref_ptr<osg::Viewport> vp ( this->viewport() );
      const osg::Matrixd &proj = _sceneView->getProjectionMatrix();

      // Vector to store the images.
      ImageList images;
      images.reserve ( this->numRenderPasses() );

      // For cleaning up temporary files.
      Detail::RemoveFiles removeFiles ( images );

      // Loop through the passes...
      for ( unsigned int i = 0; i < this->numRenderPasses(); ++i )
      {
        // Set the proper projection matrix.
        OsgTools::Jitter::instance().perspective ( _numPasses, i, *vp, proj, pMatrix );
        _sceneView->setProjectionMatrix ( pMatrix );

        // Make a new image.
        osg::ref_ptr < osg::Image > tempImage ( new osg::Image );

        // Capture the image.
        this->_screenCapture ( *tempImage, pMatrix, width, height );

        // Write the image to file.
        images.push_back ( Usul::File::Temp::file() );
        Detail::writeImage ( *tempImage, images.back() );

        // Feedback
        {
          progress ( i, this->numRenderPasses() );
          std::ostringstream out;
          out << "Capturing render pass " << i;
          status ( out.str(), true );
        }
      }

      answer = this->_accumulate ( images, width, height, GL_RGB, GL_UNSIGNED_BYTE );
    }
    else
    {
      // Fill in the image.
      this->_screenCapture ( *answer, width, height );
    }
  }

  // If there is an exception, set the old view matrix and re-throw.
  catch ( ... )
  {
    _sceneView->setViewMatrix( oldViewMatrix );
    throw;
  }

  _sceneView->setViewMatrix( oldViewMatrix );

  return answer.release();
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
#if OSG_VERSION_MAJOR == 1 && OSG_VERSION_MINOR >= 1
  bool useFBO ( osg::FBOExtensions::instance( _contextId, true )->isSupported() && height <= 4096 && width <= 4096 );
#elif OSG_VERSION_MAJOR == 1 && OSG_VERSION_MINOR == 0
  bool useFBO ( osg::FBOExtensions::instance( _contextId )->isSupported() && height <= 4096 && width <= 4096 );
#else
  bool useFBO ( false );
#endif

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
//  Set the scatter scale.
//
///////////////////////////////////////////////////////////////////////////////

void Renderer::scatterScale ( double scale )
{
  OsgTools::Jitter::instance().scatterScale ( scale );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the scatter scale.
//
///////////////////////////////////////////////////////////////////////////////

double Renderer::scatterScale() const
{
  return OsgTools::Jitter::instance().scatterScale();
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

    // Get the current tile height.  Accounts for tiles at end that are not complete
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


///////////////////////////////////////////////////////////////////////////////
//
//  Set the projection matrix as a frustum.
//
///////////////////////////////////////////////////////////////////////////////

void Renderer::setFrustum ( double left, double right, double bottom, double top, double n, double f )
{
  _sceneView->setProjectionMatrixAsFrustum ( left, right, bottom, top, n, f );
}

///////////////////////////////////////////////////////////////////////////////
//
//  Get the projection matrix as a frustum.
//
///////////////////////////////////////////////////////////////////////////////

bool Renderer::getFrustum ( double &left, double &right, double &bottom, double &top, double &n, double& f ) const
{
  return _sceneView->getProjectionMatrixAsFrustum( left, right, bottom, top, n, f );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the view matrix.
//
///////////////////////////////////////////////////////////////////////////////

void Renderer::viewMatrix ( const osg::Matrixd& matrix )
{
  _sceneView->setViewMatrix ( matrix );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the view matrix.
//
///////////////////////////////////////////////////////////////////////////////

const osg::Matrixf Renderer::viewMatrix ( ) const
{
  return _sceneView->getViewMatrix();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the frame stamp.
//
///////////////////////////////////////////////////////////////////////////////

osg::FrameStamp* Renderer::framestamp()
{
  return _framestamp.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the frame stamp.
//
///////////////////////////////////////////////////////////////////////////////

const osg::FrameStamp* Renderer::framestamp() const
{
  return _framestamp.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the global state set.
//
///////////////////////////////////////////////////////////////////////////////

osg::StateSet* Renderer::getGlobalStateSet()
{
  return _sceneView->getGlobalStateSet();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the global state set.
//
///////////////////////////////////////////////////////////////////////////////

const osg::StateSet*  Renderer::getGlobalStateSet() const
{
  return _sceneView->getGlobalStateSet();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the light.
//
///////////////////////////////////////////////////////////////////////////////

osg::Light *Renderer::light()
{
  return ( 0x0 == _sceneView.get() ) ? 0x0 : _sceneView->getLight();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the light.
//
///////////////////////////////////////////////////////////////////////////////

const osg::Light *Renderer::light() const
{
  return ( 0x0 == _sceneView.get() ) ? 0x0 : _sceneView->getLight();
}
