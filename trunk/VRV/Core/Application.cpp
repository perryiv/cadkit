
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "VRV/Core/Application.h"
#include "VRV/Core/JugglerFunctors.h"
#include "VRV/Core/Exceptions.h"
#include "VRV/Jobs/LoadModel.h"

#include "Usul/App/Application.h"
#include "Usul/Errors/Assert.h"
#include "Usul/Trace/Trace.h"
#include "Usul/Jobs/Manager.h"
#include "Usul/System/Host.h"
#include "Usul/System/Directory.h"

#include "OsgTools/State/StateSet.h"

#include "osg/MatrixTransform"

#include "osgDB/WriteFile"

#include "vrj/Kernel/Kernel.h"
#include "vrj/Draw/OGL/GlWindow.h"

#include "boost/filesystem/operations.hpp"

#include <stdexcept>

using namespace VRV::Core;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Application::Application() : vrj::GlApp( vrj::Kernel::instance() ),
  _mutex           (),
  _root            ( new osg::Group ),
  _navBranch       ( new osg::MatrixTransform ),
  _models          ( new osg::MatrixTransform ),
  _timer           (),
  _framestamp      ( 0x0 ),
  _viewport        ( 0x0 ),
  _backgroundColor ( 0, 0, 0, 1 ),
  _dirty           ( false ),
  _initialTime     ( static_cast < osg::Timer_t > ( 0.0 ) ),
  _frameStart      ( static_cast < osg::Timer_t > ( 0.0 ) ),
  _sharedFrameTime (),
  _frameTime       ( 1 ),
  _renderer        (),
  _renderers       (),
  _sceneManager    ( new OsgTools::Render::SceneManager ),
  _progressBars    ( new ProgressBars ),
  _clipDist        ( 0, 0 ),
  _exportImage     ( false ),
  _preferences     ( new Preferences ),
  _buttons         ( new VRV::Devices::ButtonGroup ),
  _tracker         ( new VRV::Devices::TrackerDevice ( "VJWand" ) ),
  _joystick        ( new VRV::Devices::JoystickDevice ( "VJAnalog0", "VJAnalog1" ) ),
  _analogTrim      ( 0, 0 ),
  _wandOffset      ( 0, 0, 0 ), // feet (used to be z=-4)
  _refCount        ( 0 )
{
  USUL_TRACE_SCOPE;
  this->_construct();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Construct.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_construct()
{
  USUL_TRACE_SCOPE;

  // Crank up the max number of contexts. When you figure out a good way to 
  // get the real number, then use it here instead of this hard-coded one. 
  // Note: this has to be done early. Waiting for init() or contextInit() is 
  // too late.
  osg::DisplaySettings::instance()->setMaxNumberOfGraphicsContexts ( 20 );

  // Hook up the branches
  _root->addChild      ( _navBranch.get()    );
  _navBranch->addChild ( _models.get()       );

  // Name the branches.
  _root->setName         ( "_root"         );
  _navBranch->setName    ( "_navBranch"    );
  _models->setName       ( "_models"       );

  osg::Matrix m;
  m.makeRotate ( -osg::PI / 2, osg::Vec3 ( 1, 0, 0 ) );

  _models->setMatrix( m );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Application::~Application()
{
  USUL_TRACE_SCOPE;

  // Make sure we don't have any references hanging around.
  USUL_ASSERT ( 0 == _refCount );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown* Application::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case VRV::Interfaces::IModelAdd::IID:
    return static_cast < VRV::Interfaces::IModelAdd* > ( this );
  case VRV::Interfaces::IClippingDistanceFloat::IID:
    return static_cast < VRV::Interfaces::IClippingDistanceFloat* > ( this );
  case VRV::Interfaces::IFrameInfo::IID:
    return static_cast < VRV::Interfaces::IFrameInfo* > ( this );
  case VRV::Interfaces::IWorldInfo::IID:
    return static_cast < VRV::Interfaces::IWorldInfo* > ( this );
  case VRV::Interfaces::IModelsScene::IID:
    return static_cast < VRV::Interfaces::IModelsScene * > ( this );
  case VRV::Interfaces::INavigationScene::IID:
    return static_cast < VRV::Interfaces::INavigationScene* > ( this );
  case VRV::Interfaces::IMatrixMultiplyFloat::IID:
    return static_cast < VRV::Interfaces::IMatrixMultiplyFloat * > ( this );
  case VRV::Interfaces::IJoystickFloat::IID:
    return static_cast< VRV::Interfaces::IJoystickFloat * > ( this );
  case VRV::Interfaces::IWandStateFloat::IID:
    return static_cast< VRV::Interfaces::IWandStateFloat * > ( this );
  case Usul::Interfaces::IProgressBarFactory::IID:
    return static_cast < Usul::Interfaces::IProgressBarFactory* > ( this );
  default:
    return 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Run the application.
//
///////////////////////////////////////////////////////////////////////////////

void Application::run()
{
  USUL_TRACE_SCOPE;

  vrj::Kernel* kernel ( vrj::Kernel::instance() );
  kernel->setApplication( this );
  kernel->start();
  kernel->waitForKernelStop();
}


///////////////////////////////////////////////////////////////////////////////
//
//  A method to autocenter, places the node in front of viewer
//
///////////////////////////////////////////////////////////////////////////////

void Application::viewAll ( osg::Node* node, osg::Matrix::value_type zScale )
{
  USUL_TRACE_SCOPE;

  if( osg::MatrixTransform *mt = dynamic_cast < osg::MatrixTransform* > ( node ) )
  {
    // Get the bounding sphere of the group.
    osg::BoundingSphere bs ( node->getBound() );
    osg::Vec3 c ( bs.center() );

    // Push it back so we can see it.
    osg::Matrix matrix;
    osg::Matrix::value_type z ( zScale * bs.radius() + c[2] );
    matrix.makeTranslate( -c[0], -c[1], -z );
    mt->postMult( matrix );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called after the context is initalized.
//
///////////////////////////////////////////////////////////////////////////////

void Application::contextInit()
{
  USUL_TRACE_SCOPE;

  // Make a new Renderer.
  RendererPtr renderer ( new Renderer );

  // Initialize the renderer.
  renderer->init();

  unsigned int uniqueContextId ( vrj::GlDrawManager::instance()->getCurrentContext() );

  renderer->uniqueID( uniqueContextId );

  // Turn off the default light.
  renderer->viewer()->setLightingMode ( osgUtil::SceneView::NO_SCENEVIEW_LIGHT );

  // Turn off computing of the near and far plane.
  renderer->viewer()->setComputeNearFarMode ( osgUtil::CullVisitor::DO_NOT_COMPUTE_NEAR_FAR );

  // Needed for stereo to work.
  renderer->viewer()->setDrawBufferValue( GL_NONE );

  // Turn on back face culling.
  renderer->getGlobalStateSet()->setMode( GL_CULL_FACE, osg::StateAttribute::ON );

  // Keep handle to framestamp.
  _framestamp = renderer->framestamp();

  // Keep handle to viewport.
  _viewport = renderer->viewport();

  // Set the scene.
  renderer->scene ( _sceneManager->scene() );

  // Set the background color.
  renderer->backgroundColor ( _backgroundColor );

  (*_renderer) = renderer.get();

  // Get the viewport. 
  GLint vp[4];
  ::glGetIntegerv ( GL_VIEWPORT, vp );

  _viewport->setViewport ( vp[0], vp[1], vp[2], vp[3] );

  // Set the projection.
  _sceneManager->projection()->setMatrix ( osg::Matrix::ortho2D ( _viewport->x(), _viewport->width(), _viewport->y(), _viewport->height() ) );

  // Add this renderer to our list.
  _renderers.push_back ( renderer );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called before the context is closed.
//
///////////////////////////////////////////////////////////////////////////////

void Application::contextClose()
{
  RendererPtr renderer ( *_renderer );

  // Clean up context specific data.
  (*_renderer) = 0x0;

  // Remove the renderer from our list.
  _renderers.erase ( std::find_if ( _renderers.begin(), _renderers.end(), RendererPtr::IsEqual ( renderer ) ) );

  // Clear the local ref pointer.
  renderer = 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called after the context is made current, but before the frame draws.
//
///////////////////////////////////////////////////////////////////////////////

void Application::contextPreDraw()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Small class that pushes the state in constructor and pops it in destructor.
//  This is for exception safety.
//
///////////////////////////////////////////////////////////////////////////////

namespace VRV {
namespace Core {
namespace Detail 
{
  struct OpenGlStackPushPop
  {
    OpenGlStackPushPop()
    {
      #if 0
      // Clear the errors.
      while ( GL_ERROR_NO != ::glGetError() ){}
      #endif

      glPushAttrib(GL_ALL_ATTRIB_BITS);

      glMatrixMode(GL_MODELVIEW);
      glPushMatrix();

      glMatrixMode(GL_PROJECTION);
      glPushMatrix();

      //glMatrixMode(GL_TEXTURE);
      //glPushMatrix();
    }
    ~OpenGlStackPushPop()
    {
      // Check for errors.
      #if 0
      assert ( GL_ERROR_NO == ::glGetError() );
      #endif

      //glMatrixMode(GL_TEXTURE);
      //glPopMatrix();

      glMatrixMode(GL_PROJECTION);
      glPopMatrix();

      glMatrixMode(GL_MODELVIEW);
      glPopMatrix();

      glPopAttrib();
    }
  };
}
}
}


///////////////////////////////////////////////////////////////////////////////
//
//  Draw.
//
///////////////////////////////////////////////////////////////////////////////

void Application::draw()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  // For exception safety. Pushes attributes in constructor, pops them in destructor.
  Detail::OpenGlStackPushPop pushPop;

  // Get the renderer for this context.
  Renderer* renderer ( *(_renderer) );

  // Drawing is about to happen.
  this->_preDraw ( renderer );

  // Draw.
  this->_draw ( renderer );

  // Drawing has finished.
  this->_postDraw ( renderer );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Draw.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_draw ( OsgTools::Render::Renderer *renderer )
{
  USUL_TRACE_SCOPE;

  vrj::GlDrawManager* mgr ( vrj::GlDrawManager::instance() );
  USUL_ASSERT ( 0x0 != mgr );

  // ? constantly adjust the viewport ?
  osg::Viewport* vp ( renderer->viewport() );
  this->_setViewport ( vp, mgr );

  // constantly update the projection matrix
  vrj::GlUserData* userData    ( mgr->currentUserData() );
  vrj::Projection* projection  ( userData->getProjection() );
  vrj::Frustum frustum         ( projection->getFrustum() );
  
  // We need to set this every frame to account for stereo.
  renderer->setFrustum ( frustum[vrj::Frustum::VJ_LEFT],
				                 frustum[vrj::Frustum::VJ_RIGHT],
				                 frustum[vrj::Frustum::VJ_BOTTOM],
				                 frustum[vrj::Frustum::VJ_TOP],
				                 frustum[vrj::Frustum::VJ_NEAR],
				                 frustum[vrj::Frustum::VJ_FAR] );

  // Constantly update the view matrix.  
  // Since the navigation is done on a osg::MatrixTransform, is this needed?
  gmtl::Matrix44f proj ( projection->getViewMatrix() );
  renderer->viewMatrix( osg::Matrix ( proj.mData ) );

  // Update.  This needs to be moved into the Preframe.
  renderer->update();

  // Do the drawing.
  renderer->render();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called before draw happens.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_preDraw( OsgTools::Render::Renderer *renderer )
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write a image.
//
///////////////////////////////////////////////////////////////////////////////

class ImageWriteJob : public Usul::Jobs::Job
{
public:
  typedef Usul::Jobs::Job BaseClass;

  ImageWriteJob ( const std::string& filename, osg::Image *image ) : 
    BaseClass (),
    _filename ( filename ),
    _image ( image )
  {
  }

  virtual ~ImageWriteJob ()
  {
  }

protected:

  virtual void _started()
  {
    std::cout << " Writing image file: " << _filename << std::endl;

    // Write the image to file.
    osgDB::writeImageFile ( *_image, _filename );
  }

private:
  std::string _filename;
  osg::ref_ptr < osg::Image > _image;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Called before after happens.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_postDraw( OsgTools::Render::Renderer *renderer )
{
  USUL_TRACE_SCOPE;

  if( true == _exportImage )
  {
    // Get frame scale from the preferences.
    const double multiplier ( this->preferences()->frameScale() );

    // Scale the width and height.
    unsigned int width  ( static_cast < unsigned int > ( _viewport->width()  * multiplier ) );
    unsigned int height ( static_cast < unsigned int > ( _viewport->height() * multiplier ) );

    // Capture image.
    osg::ref_ptr<osg::Image> image ( renderer->screenCapture ( multiplier, 1 ) );

    // How many images have we exported.
    static unsigned int count ( 0 );

    // Get the extension from the preferences.
    const std::string ext ( this->preferences()->imageExportExtension() );

    // Construct the filename.
    std::ostringstream filename;
    filename << "/array/cluster/data/screen_shots/" << count++ << "_" << Usul::System::Host::name() << ext;

    Usul::Jobs::Manager::instance().add ( new ImageWriteJob ( filename.str(), image.get() ) );

    // Don't export next time.
    _exportImage = false;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the scene data.
//
///////////////////////////////////////////////////////////////////////////////

void Application::setSceneData( osg::Node* node )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  _sceneManager->model( node );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the scene data.
//
///////////////////////////////////////////////////////////////////////////////

const osg::Node* Application::getSceneData() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  return _sceneManager->model();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the scene data.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* Application::getSceneData()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  return _sceneManager->model();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the time elapsed since the start of the application.
//
///////////////////////////////////////////////////////////////////////////////

double Application::getTimeSinceStart()
{
  USUL_TRACE_SCOPE;
  return _timer.delta_s( _initialTime, _timer.tick() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize the application.
//
///////////////////////////////////////////////////////////////////////////////

void Application::init()
{
  USUL_TRACE_SCOPE;

  // Set the scene-viewer's scene.
  this->setSceneData ( _root.get() );

  _initialTime = _timer.tick();

  // Initialize the shared frame time data.
  vpr::GUID guid ( "8297080d-c22c-41a6-91c1-188a331fabe5" );
  _sharedFrameTime.init ( guid, "viz0" );

  // Add the progress bars to the scene.
  osg::ref_ptr < osg::Group > group ( _sceneManager->groupGet ( "ProgressBarGroup" ) );
  group->addChild ( _progressBars->buildScene() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called before the frame.
//
///////////////////////////////////////////////////////////////////////////////

void Application::preFrame()
{
  USUL_TRACE_SCOPE;

  // Write the frame time if we've suppose to.
  if( _sharedFrameTime.isLocal() )
  {
    // Capture the frame time.
    _sharedFrameTime->data = _frameTime;
  }

  // TODO: Write FrameStamp data.

  // Mark the start of the frame.
  _frameStart = _timer.tick();

  // Update the progress bars.
  _progressBars->buildScene();

  // Purge.
  Usul::Jobs::Manager::instance().purge();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called after preFrame, but before the frame.
//
///////////////////////////////////////////////////////////////////////////////

void Application::latePreFrame()
{
  USUL_TRACE_SCOPE;

  // Get the frame time.
  _frameTime = _sharedFrameTime->data;

  // Update these input devices.
  _buttons->update();
  _tracker->update();
  _joystick->update();

  // Send any notifications.
  _joystick->notify();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the frame is done.
//
///////////////////////////////////////////////////////////////////////////////

void Application::postFrame()
{
  USUL_TRACE_SCOPE;

  // Capture the frame time.
  _frameTime = _timer.delta_s( _frameStart, _timer.tick() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the viewport.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_setViewport( osg::Viewport* vp, vrj::GlDrawManager* mgr )
{
  USUL_TRACE_SCOPE;

  float vpX,vpY,vpW,vpH;
  int winX,winY,winW,winH;

  mgr->currentUserData()->getViewport()->getOriginAndSize ( vpX, vpY, vpW, vpH );
  mgr->currentUserData()->getGlWindow()->getOriginSize ( winX, winY, winW, winH );

  unsigned int x ( static_cast < unsigned int > ( vpX * static_cast < float > ( winW ) ) );
  unsigned int y ( static_cast < unsigned int > ( vpY * static_cast < float > ( winH ) ) );
  unsigned int w ( static_cast < unsigned int > ( vpW * static_cast < float > ( winW ) ) );
  unsigned int h ( static_cast < unsigned int > ( vpH * static_cast < float > ( winH ) ) );

  vp->setViewport ( x, y, w, h );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Quit the application.
//
///////////////////////////////////////////////////////////////////////////////

void Application::quit()
{
  USUL_TRACE_SCOPE;
  vrj::Kernel::instance()->stop();
}


///////////////////////////////////////////////////////////////////////////////
//
// Set the normalization state.
//
///////////////////////////////////////////////////////////////////////////////

void Application::normalize ( bool on )
{
  USUL_TRACE_SCOPE;
  OsgTools::State::StateSet::setNormalize ( this->getSceneData(), on );
}


///////////////////////////////////////////////////////////////////////////////
//
// Return the normalization state.
//
///////////////////////////////////////////////////////////////////////////////

bool Application::normalize() const
{
  USUL_TRACE_SCOPE;
  return OsgTools::State::StateSet::getNormalize ( this->getSceneData() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the frame stamp.
//
///////////////////////////////////////////////////////////////////////////////

osg::FrameStamp* Application::getFrameStamp()
{
  USUL_TRACE_SCOPE;
  return _framestamp.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the frame stamp.
//
///////////////////////////////////////////////////////////////////////////////

const osg::FrameStamp* Application::getFrameStamp() const
{
  USUL_TRACE_SCOPE;
  return _framestamp.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the duration of the last frame in seconds.
//
///////////////////////////////////////////////////////////////////////////////

double Application::frameTime() const
{
  USUL_TRACE_SCOPE;
  return _frameTime;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a light.
//
///////////////////////////////////////////////////////////////////////////////

void Application::addLight ( osg::Light* light )
{
  USUL_TRACE_SCOPE;
  _sceneManager->globalLight ( light );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Reference.  Keep track for debugging.
//
///////////////////////////////////////////////////////////////////////////////

void Application::ref()
{
  ++_refCount;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Unreference.  Keep track for debugging..
//
///////////////////////////////////////////////////////////////////////////////

void Application::unref ( bool )
{
  --_refCount;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Load all the config files.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_loadConfigFiles ( const std::vector < std::string > &configs )
{
  USUL_TRACE_SCOPE;

  // See if there is at least one config file. Do not use the ErrorChecker.
  if ( configs.empty() )
  {
    std::cout << "No VRJuggler config-files specified."
              << "\n\tAttempting to use a sim-mode configuration."
              << std::endl;
    this->_loadSimConfigs();
    return;
  }

  // Load the config files.
  std::for_each ( configs.begin(), configs.end(), VRV::Core::Detail::LoadConfigFile() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Load the default config files.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_loadSimConfigs()
{
  USUL_TRACE_SCOPE;

  std::cout << "Looking for environment variable VJ_CONFIG_DIR" << std::endl;

  // Try this environment variable.
  const char *dir = ::getenv ( "VJ_CONFIG_DIR" );
  if ( dir )
  {
    // Load default config files from this directory.
    this->_loadSimConfigs ( dir );
    return;
  }

  std::cout << "Environment variable VJ_CONFIG_DIR not found." << std::endl;
  std::cout << "Looking for environment variable VJ_BASE_DIR" << std::endl;

  // Try this environment variable.
  dir = ::getenv ( "VJ_BASE_DIR" );
  if ( dir )
  {
    // Make sure there is a slash.
    std::string d ( dir );
    std::string::size_type last ( d.size() - 1 );
    if ( '/' != d[last] || '\\' != d[last] )
      d += '/';

    // Add the sub-directory.
    d += "share/vrjuggler/data/configFiles";

    // Load default config files from this directory.
    this->_loadSimConfigs ( d );
    return;
  }

  std::cout << "Environment variable VJ_BASE_DIR not found." << std::endl;

  // If we get this far then we failed.
  Usul::Exceptions::Thrower < VRV::Core::Exceptions::UserInput > 
    ( "No default VRJuggler config-files found." );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Load the default config files from the given directory.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_loadSimConfigs ( const std::string& d )
{
  USUL_TRACE_SCOPE;

  // Return if string is empty.
  if ( d.empty() )
    return;

  // Make a copy.
  std::string dir ( d );

  // Make sure there is a slash.
  std::string::size_type last ( dir.size() - 1 );
  if ( '/' != dir[last] || '\\' != dir[last] )
    dir += '/';

  // The config-file loader.
  VRV::Core::Detail::LoadConfigFile loader;

  // Load the config files.
  loader ( dir + "sim.base.jconf" );
  loader ( dir + "sim.wand.mixin.jconf" );
  loader ( dir + "sim.analog.mixin.jconf" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a model.
//
///////////////////////////////////////////////////////////////////////////////

void Application::addModel ( osg::Node *model, const std::string& filename )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  // Set its name to the filename, minus the pathing portion, if there is no name.
  if ( model->getName().empty() )
  {
    unsigned int loc = filename.find_last_of ( "/\\" );
    std::string name = filename.substr ( loc + 1 );
    model->setName ( name );
  }

  // Hook things up.
  _models->addChild ( model );

  // Based on the scene size, set the near and far clipping plane distances.
  this->_setNearAndFarClippingPlanes();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Load the models.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_loadModelFiles  ( const Filenames& filenames )
{
  USUL_TRACE_SCOPE;

  // Create a job.
  VRV::Jobs::LoadModel::RefPtr job ( new VRV::Jobs::LoadModel ( filenames, this->queryInterface ( Usul::Interfaces::IUnknown::IID ) ) );

  // Add the job to the manager.
  Usul::Jobs::Manager::instance().add ( job.get() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the models node.
//
///////////////////////////////////////////////////////////////////////////////

osg::MatrixTransform* Application::models()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  return _models.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the models node.
//
///////////////////////////////////////////////////////////////////////////////

const osg::MatrixTransform* Application::models() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  return _models.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the near and far clipping planes based on the scene.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_setNearAndFarClippingPlanes()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  // Get the bounding sphere.
  const osg::BoundingSphere &sphere = _sceneManager->scene()->getBound();
  float radius = sphere.radius();

  // Handle zero-sized bounding spheres.
  if ( radius <= 1e-6 )
    radius = 1;

  // Set both distances.
  _clipDist[0] = this->preferences()->nearClippingDistance();
  _clipDist[1] = radius * this->preferences()->farClippingPlaneMultiplier();
  
  // Set the clipping planes
  vrj::Projection::setNearFar ( _clipDist[0], _clipDist[1] );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the clipping planes
//
///////////////////////////////////////////////////////////////////////////////

void Application::getClippingDistances ( float &nearDist, float &farDist ) const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  nearDist = _clipDist[0];
  farDist  = _clipDist[1];
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the clipping planes.
//
///////////////////////////////////////////////////////////////////////////////

void Application::setClippingDistances ( float nearDist, float farDist )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  _clipDist[0] = nearDist;
  _clipDist[1] = farDist;
  vrj::Projection::setNearFar ( nearDist, farDist );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the maximum world size.
//
///////////////////////////////////////////////////////////////////////////////

double Application::worldRadius() const
{
  Guard guard ( this->mutex() );

  // Use the whole scene or just the navBranch?
  return _navBranch->getBound().radius();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the navigation scene.
//
///////////////////////////////////////////////////////////////////////////////

const osg::Group *Application::navigationScene() const
{
  Guard guard ( this->mutex() );
  return _navBranch.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the navigation scene.
//
///////////////////////////////////////////////////////////////////////////////

osg::Group *Application::navigationScene()
{
  Guard guard ( this->mutex() );
  return _navBranch.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the models scene.
//
///////////////////////////////////////////////////////////////////////////////

const osg::Group *Application::modelsScene() const
{
  Guard guard ( this->mutex() );
  return _models.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the models scene.
//
///////////////////////////////////////////////////////////////////////////////

osg::Group *Application::modelsScene()
{
  Guard guard ( this->mutex() );
  return _models.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Multiply the navigation matrix.
//
///////////////////////////////////////////////////////////////////////////////

void Application::postMultiply ( const Matrix44f &m )
{
  Guard guard ( this->mutex() );
  _navBranch->postMult ( osg::Matrixf ( m.get() ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Multiply the navigation matrix.
//
///////////////////////////////////////////////////////////////////////////////

void Application::preMultiply ( const Matrix44f &m )
{
  Guard guard ( this->mutex() );
  _navBranch->preMult ( osg::Matrixf ( m.get() ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the navigation matrix.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_navigationMatrix ( const osg::Matrixd& m )
{
  Guard guard ( this->mutex() );
  _navBranch->setMatrix ( m );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the navigation matrix.
//
///////////////////////////////////////////////////////////////////////////////

const osg::Matrixd& Application::_navigationMatrix ( ) const
{
  Guard guard ( this->mutex() );
  return _navBranch->getMatrix ( );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the scene root.
//
///////////////////////////////////////////////////////////////////////////////

osg::Group* Application::_sceneRoot()
{
  Guard guard ( this->mutex() );
  return _root.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the scene root.
//
///////////////////////////////////////////////////////////////////////////////

const osg::Group* Application::_sceneRoot() const
{
  Guard guard ( this->mutex() );
  return _root.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create a progress bar.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown* Application::createProgressBar()
{
  Guard guard ( this->mutex() );
  return _progressBars->append();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Export the next image.
//
///////////////////////////////////////////////////////////////////////////////

void Application::exportNextFrame()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  _exportImage = true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the backgroup color.
//
///////////////////////////////////////////////////////////////////////////////

void Application::setBackgroundColor( const osg::Vec4& color )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  _backgroundColor = color;

  for( Renderers::iterator iter = _renderers.begin(); iter != _renderers.end(); ++iter )
  {
    (*iter)->backgroundColor ( _backgroundColor );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the backgroup color.
//
///////////////////////////////////////////////////////////////////////////////

const osg::Vec4& Application::getBackgroundColor() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  return _backgroundColor;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the number of rendering passes.
//
///////////////////////////////////////////////////////////////////////////////

void Application::numRenderPasses ( unsigned int num )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  for( Renderers::iterator iter = _renderers.begin(); iter != _renderers.end(); ++iter )
  {
    (*iter)->numRenderPasses ( num );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the preferences.
//
///////////////////////////////////////////////////////////////////////////////

Application::Preferences * Application::preferences ()
{
  USUL_TRACE_SCOPE;
  return _preferences.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the preferences.
//
///////////////////////////////////////////////////////////////////////////////

const Application::Preferences *  Application::preferences () const
{
  USUL_TRACE_SCOPE;
  return _preferences.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the user's preferences.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_readUserPreferences()
{
  USUL_TRACE_SCOPE;

  // If the file is missing then it throws.
  try
  {
    // Find the path to the preference file.
    std::string filename ( Usul::App::Application::instance().configFile ( "preferences" ) );

    // Read the preferences.
    _preferences->read ( filename );
  }

  // Catch expected exceptions.
  catch ( const std::exception &e )
  {
    std::ostringstream message;
    message << "Warning 1083877168, exception generated when attempting "
            << "to read user-preferences file."
            << "\n\t" << e.what();
    //this->_update ( *_msgText, message.str() );
    std::cout << message << std::endl;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Increase speed.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_increaseTranslateSpeed ( double amount )
{
  USUL_TRACE_SCOPE;
  this->preferences()->translationSpeed ( this->preferences()->translationSpeed() * amount );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Decrease speed.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_decreaseTranslateSpeed ( double amount )
{
  USUL_TRACE_SCOPE;
  this->preferences()->translationSpeed ( this->preferences()->translationSpeed() / amount );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the buttons.
//
///////////////////////////////////////////////////////////////////////////////

VRV::Devices::ButtonGroup * Application::buttons ()
{
  USUL_TRACE_SCOPE;
  return _buttons.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the buttons.
//
///////////////////////////////////////////////////////////////////////////////

const VRV::Devices::ButtonGroup * Application::buttons () const
{
  USUL_TRACE_SCOPE;
  return _buttons.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the tracker.
//
///////////////////////////////////////////////////////////////////////////////

VRV::Devices::TrackerDevice * Application::tracker ()
{
  USUL_TRACE_SCOPE;
  return _tracker.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the tracker.
//
///////////////////////////////////////////////////////////////////////////////

const VRV::Devices::TrackerDevice * Application::tracker () const
{
  USUL_TRACE_SCOPE;
  return _tracker.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the joystick.
//
///////////////////////////////////////////////////////////////////////////////

VRV::Devices::JoystickDevice * Application::joystick ()
{
  USUL_TRACE_SCOPE;
  return _joystick.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the joystick.
//
///////////////////////////////////////////////////////////////////////////////

const VRV::Devices::JoystickDevice *  Application::joystick () const
{
  USUL_TRACE_SCOPE;
  return _joystick.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the analog input in the range [-1,1].
//
///////////////////////////////////////////////////////////////////////////////

float Application::joystickHorizontal() const
{
  USUL_TRACE_SCOPE;
  return 2.0f * ( this->joystick()->horizontal() + _analogTrim[0] ) - 1.0f;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the analog input in the range [-1,1].
//
///////////////////////////////////////////////////////////////////////////////

float Application::joystickVertical() const
{
  USUL_TRACE_SCOPE;
  return 2.0f * ( this->joystick()->vertical() + _analogTrim[1] ) - 1.0f;
}

///////////////////////////////////////////////////////////////////////////////
//
//  Get the wand's position.
//
///////////////////////////////////////////////////////////////////////////////

void Application::wandPosition ( Usul::Math::Vec3f &p ) const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  // Get the wand's offset.
  Usul::Math::Vec3f offset;
  this->wandOffset ( offset );

  // Set the vector from the wand's position plus the offset.
  p[0] = this->tracker()->x() + offset[0];
  p[1] = this->tracker()->y() + offset[1];
  p[2] = this->tracker()->z() + offset[2];
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the wand's matrix.
//
///////////////////////////////////////////////////////////////////////////////

void Application::wandMatrix ( Matrix44f &W ) const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  // Set the given matrix from the wand's matrix.
  W.set ( this->tracker()->matrix().getData() );

  // Get the wand's offset.
  Usul::Math::Vec3f offset;
  this->wandOffset ( offset );

  // Translate by our wand's offset.
  Matrix44f T;
  T.makeTranslation ( offset );
  W = T * W;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the wand's offset.
//
///////////////////////////////////////////////////////////////////////////////

void Application::wandOffset ( Usul::Math::Vec3f &v ) const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  v = _wandOffset;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the wand's offset.
//
///////////////////////////////////////////////////////////////////////////////

void Application::wandOffset ( const Usul::Math::Vec3f &v )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  _wandOffset = v;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the rotation portion of the wand's matrix.
//
///////////////////////////////////////////////////////////////////////////////

void Application::wandRotation ( Matrix44f &W ) const
{
  USUL_TRACE_SCOPE;

  // Get the wand's matrix.
  this->wandMatrix ( W );

  // Zero-out the translations.
  W.setTranslation ( Usul::Math::Vec3f ( 0, 0, 0 ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the analog trim.
//
///////////////////////////////////////////////////////////////////////////////

const Usul::Math::Vec2f& Application::analogTrim () const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _analogTrim;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the analog trim.
//
///////////////////////////////////////////////////////////////////////////////

void Application::analogTrim ( float x, float y )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  _analogTrim.set ( x, y );
}
