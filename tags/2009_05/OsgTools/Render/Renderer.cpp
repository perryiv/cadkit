
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L. Miller IV and Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  OSG renderer class.
//
///////////////////////////////////////////////////////////////////////////////

#include "OsgTools/Render/Renderer.h"

#include "OsgTools/Jitter.h"
#include "OsgTools/Render/RecordTime.h"
#include "OsgTools/ScopedProjection.h"
#include "OsgTools/ScopedNodeMask.h"
#include "OsgTools/Render/Defaults.h"
#include "OsgTools/Render/ClampProjection.h"
#include "OsgTools/Render/TiledScreenCapture.h"
#include "OsgTools/Builders/GradientBackground.h"
#include "OsgTools/Group.h"

#include "Usul/Errors/Checker.h"
#include "Usul/Bits/Bits.h"
#include "Usul/Errors/Stack.h"
#include "Usul/System/Clock.h"
#include "Usul/Headers/OpenGL.h"

#include "osgUtil/UpdateVisitor"

#include "osg/MatrixTransform"
#include "osg/Geometry"
#include "osg/Geode"
#include "osg/GLU"
#include "osg/Version"

using namespace OsgTools::Render;

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( Renderer, Renderer::BaseClass );

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
  _sceneView          ( new osgUtil::SceneView ),
  _framestamp         ( new osg::FrameStamp ),
  _timer              ( ),
  _start_tick         ( 0 ),
  _times              ( ),
  _numPasses          ( 1 ),
  _contextId          ( 0 ),
  _hasAccumBuffer     ( false ),
  _gradient           (),
  _corners            ( 0 ),
  _clearNode          ( new osg::ClearNode )
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

  // Turn this on for more debuging information.
#if 0
  _sceneView->getState()->setCheckForGLErrors( osg::State::ONCE_PER_ATTRIBUTE );
#endif
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
  // Initialize the SceneView.
  this->viewer()->init();

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

  cv->setFrameStamp ( _framestamp.get () );
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

  // Set the clear node.
#if OPENSCENEGRAPH_MAJOR_VERSION <= 2 && OPENSCENEGRAPH_MINOR_VERSION <= 2
  this->viewer()->getCullVisitor()->setClearNode ( _clearNode.get() );
#endif

  // See if we are supposed to use multiple passes.
  //const unsigned int frameCount ( ( 0x0 != _sceneView->getFrameStamp() ) ? _sceneView->getFrameStamp()->getFrameNumber() : 0 );
  const unsigned int numPasses ( this->getNumRenderPasses() );
  if ( numPasses > 1 )
  {
    //std::cout << Usul::Strings::format ( "Rendering frame " << frameCount << " with " << numPasses << " passes" ) << std::endl;
    this->_multiPassRender();
  }
  else
  {
    //std::cout << Usul::Strings::format ( "Rendering frame " << frameCount << " with " << numPasses << " pass" ) << std::endl;
    this->_singlePassRender();
  }

  // Check for errors.
  Detail::checkForErrors( 2764743220u );
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
    OsgTools::ScopedProjection sp ( *_sceneView.get() );

    // Clear the accumulation buffer.
    ::glClearAccum ( 0.0f, 0.0f, 0.0f, 0.0f );
    ::glClear ( GL_ACCUM_BUFFER_BIT );

    // Check for errors.
    Detail::checkForErrors( 2576774470u );

    // Needed in the loop.
    osg::Matrixd matrix;
    osg::ref_ptr<osg::Viewport> vp ( this->viewport() );
    const osg::Matrixd &proj = _sceneView->getProjectionMatrix();

    // Loop through the passes...
    const unsigned int numPasses ( this->getNumRenderPasses() );
    for ( unsigned int i = 0; i < numPasses; ++i )
    {
      // Set the proper projection matrix.
      OsgTools::Jitter::instance().perspective ( numPasses, i, *vp, proj, matrix );
      _sceneView->setProjectionMatrix ( matrix );

      // Render a single pass.
      this->_singlePassRender();

      // Check for errors.
      Detail::checkForErrors( 2681461970u );

      // Accumulate the pixels from the frame buffer.
      float value ( 1.0f / static_cast < float > ( numPasses ) );
      ::glAccum ( GL_ACCUM, value );

      // Check for errors.
      Detail::checkForErrors( 3558773016u );
    }

    // Transfer the accumulation buffer into the frame buffer.
    ::glAccum ( GL_RETURN, 1.0f );

    // Check for errors.
    Detail::checkForErrors( 2459899470u );
  }

  // Catch all exceptions and reset the number of passes. Otherwise, you 
  // can get stuck in a loop where the error dialog causes a render, and 
  // the render causes an error dialog, etc.
  //
  // 12-Jun-2008 Note: Is the above comment still true?

  // Standard exceptions.
  catch ( const std::exception &e )
  {
    Usul::Errors::Stack::instance().push ( e.what() );
    std::ostringstream message;
    message << "Error 2156758683: Standard exception caught when attempting to render with " << this->getNumRenderPasses() << " passes";
    //this->setNumRenderPasses ( 1 ); // Seeing if this is no longer needed... 12-Jun-2008
    throw std::runtime_error ( message.str() );
  }

  // Unknown exceptions.
  catch ( ... )
  {
    std::ostringstream message;
    message << "Error 3880806891: Unknown exception caught when attempting to render with " << this->getNumRenderPasses() << " passes";
    //this->setNumRenderPasses ( 1 ); // Seeing if this is no longer needed... 12-Jun-2008
    throw std::runtime_error ( message.str() );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the background color.
//
///////////////////////////////////////////////////////////////////////////////

void Renderer::backgroundColor ( const osg::Vec4 &color, unsigned int corner )
{
  // Always remove the existing branch.
  OsgTools::Group::removeAllOccurances ( _gradient.root(), _clearNode.get() );

  // Always update the gradient object.
  _gradient.color ( color, corner );

  // Use regular background if all corners are the same.
  if ( Corners::ALL == corner )
  {
    _clearNode->setClearColor ( color );
  }

  // Otherwise, add the branch to render the gradient.
  else
  {
    _clearNode->addChild ( _gradient.root() );
  }
}

///////////////////////////////////////////////////////////////////////////////
//
//  Set the background color.
//
///////////////////////////////////////////////////////////////////////////////

void Renderer::backgroundColor ( const osg::Vec4 &color )
{
  this->backgroundColor ( color, this->backgroundCorners() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the background color.
//
///////////////////////////////////////////////////////////////////////////////

osg::Vec4 Renderer::backgroundColor ( unsigned int corners ) const
{
  return _gradient.color ( corners );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the background color.
//
///////////////////////////////////////////////////////////////////////////////

osg::Vec4 Renderer::backgroundColor() const
{
  return this->backgroundColor ( this->backgroundCorners() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the background corners.
//
///////////////////////////////////////////////////////////////////////////////

void Renderer::backgroundCorners ( unsigned int corners )
{
  _corners = corners;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the background corners.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Renderer::backgroundCorners() const
{
  return _corners;
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

void Renderer::setNumRenderPasses ( unsigned int num )
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
  osg::ref_ptr < osg::Group > root ( new osg::Group );
  root->addChild ( node );
  root->addChild ( _clearNode.get() );
  _sceneView->setSceneData ( root.get() );
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
  _framestamp->setSimulationTime ( _framestamp->getReferenceTime () );

  _sceneView->getUpdateVisitor()->setTraversalNumber(_framestamp->getFrameNumber());

  typedef RecordTime< Renderer > RecordTime;
  {
    RecordTime ut ( *this, "update" );  
    _sceneView->update();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Screen capture at the given view with the given height and width
//
///////////////////////////////////////////////////////////////////////////////

osg::Image* Renderer::screenCapture ( const osg::Matrix& matrix, unsigned int width, unsigned int height )
{
  TiledScreenCapture tiled;
  tiled.size ( width, height );
  tiled.clearColor ( this->backgroundColor() );
  tiled.viewMatrix ( matrix );
  tiled.numSamples ( 1 );
  tiled.scale ( 1.0f );
  tiled.background ( _gradient );
  
  // Turn off the node mask for the clear node.  The tiled screen capture uses it's own clear node.
  OsgTools::ScopedNodeMask nm ( *_clearNode, 0x0 );
  
  return tiled ( *this->viewer(), _sceneView->getProjectionMatrix() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Capture the screen using super-sampled, tiled rendering.
//
///////////////////////////////////////////////////////////////////////////////

osg::Image* Renderer::screenCapture ( float frameSizeScale, unsigned int numSamples )
{
  // Get our current width and height.
  unsigned int width  ( static_cast < unsigned int > ( this->viewport()->width ()  * frameSizeScale ) );
  unsigned int height ( static_cast < unsigned int > ( this->viewport()->height () * frameSizeScale ) );

  TiledScreenCapture tiled;
  tiled.size ( width, height );
  tiled.clearColor ( this->backgroundColor() );
  tiled.viewMatrix ( this->viewMatrix() );
  tiled.numSamples ( numSamples );
  tiled.scale ( frameSizeScale );
  tiled.background ( _gradient );
  
  // Turn off the node mask for the clear node.  The tiled screen capture uses it's own clear node.
  OsgTools::ScopedNodeMask nm ( *_clearNode, 0x0 );

  return tiled ( *this->viewer(), _sceneView->getProjectionMatrix() );
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

const osg::Matrixd Renderer::viewMatrix() const
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


///////////////////////////////////////////////////////////////////////////////
//
//  Resize the window.
//
///////////////////////////////////////////////////////////////////////////////

void Renderer::resize ( unsigned int w, unsigned int h )
{
  // Update the gradient background vertices.
  _gradient.update ( w, h );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the render info for this Renderer.
//
///////////////////////////////////////////////////////////////////////////////

osg::RenderInfo Renderer::getRenderInfo() const
{
  return ( 0x0 != _sceneView ? _sceneView->getRenderInfo() : osg::RenderInfo() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *Renderer::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::IRenderInfoOSG::IID:
    return static_cast<Usul::Interfaces::IRenderInfoOSG*> ( this );
  default:
    return 0x0;
  }
}
