
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  Copyright (c) 2005, Perry L. Miller IV and Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  OSG viewer class.
//
///////////////////////////////////////////////////////////////////////////////

// Disable deprecated warning in Visual Studio 8 
#if defined ( _MSC_VER ) && _MSC_VER == 1400
#pragma warning ( disable : 4996 )
#endif

#include "OsgTools/Render/Viewer.h"
#include "OsgTools/Render/Defaults.h"
#include "OsgTools/Group.h"
#include "OsgTools/ScopedOptions.h"
#include "OsgTools/Box.h"
#include "OsgTools/Sphere.h"
#include "OsgTools/GlassBoundingBox.h"
#include "OsgTools/ShapeFactory.h"
#include "OsgTools/State/PolygonMode.h"
#include "OsgTools/State/ShadeModel.h"
#include "OsgTools/State/StateSet.h"
#include "OsgTools/Render/LodCallbacks.h"
#include "OsgTools/Render/Trackball.h"
#include "OsgTools/Render/ActionAdapter.h"
#include "OsgTools/Render/EventAdapter.h"
#include "OsgTools/Render/Constants.h"
#include "OsgTools/Render/ClampProjection.h"
#include "OsgTools/Render/FBOScreenCapture.h"
#include "OsgTools/Callbacks/SortBackToFront.h"
#include "OsgTools/Utilities/DirtyBounds.h"
#include "OsgTools/Utilities/ReferenceFrame.h"
#include "OsgTools/Utilities/Transparency.h"
#include "OsgTools/Utilities/Intersect.h"
#include "OsgTools/Builders/Arrow.h"
#include "OsgTools/Widgets/Axes.h"
#include "OsgTools/Font.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Errors/Checker.h"
#include "Usul/Components/Manager.h"
#include "Usul/Convert/Convert.h"
#include "Usul/Convert/Vector2.h"
#include "Usul/Helper/AnimationNotify.h"
#include "Usul/Math/Constants.h"
#include "Usul/Math/Functions.h"
#include "Usul/Math/NaN.h"
#include "Usul/Bits/Bits.h"
#include "Usul/Registry/Constants.h"

#include "Usul/Interfaces/IAnimatePath.h"
#include "Usul/Interfaces/IBusyState.h"
#include "Usul/Interfaces/IGetBoundingBox.h"
#include "Usul/Interfaces/IHandleSeek.h"
#include "Usul/Interfaces/GUI/IUpdateTreeControls.h"
#include "Usul/Interfaces/GUI/IStatusBar.h"
#include "Usul/Interfaces/GUI/IMaterialEditor.h"
#include "Usul/Interfaces/IMatrixManipulator.h"
#include "Usul/Interfaces/IRenderListener.h"

#include "Usul/Registry/Constants.h"
#include "Usul/Registry/Database.h"
#include "Usul/Resources/StatusBar.h"
#include "Usul/Resources/ReportErrors.h"
#include "Usul/Resources/TextWindow.h"
#include "Usul/System/Clock.h"
#include "Usul/Predicates/Tolerance.h"
#include "Usul/Scope/Caller.h"
#include "Usul/Cast/Cast.h"
#include "Usul/Strings/Case.h"
#include "Usul/File/Path.h"
#include "Usul/CommandLine/Arguments.h"
#include "Usul/Math/Absolute.h"
#include "Usul/Threads/Named.h"
#include "Usul/Trace/Trace.h"

#include "osgUtil/CullVisitor"
#include "osgUtil/UpdateVisitor"

#include "osgDB/WriteFile"
#include "osgDB/ReadFile"

#include "osg/MatrixTransform"
#include "osg/NodeVisitor"
#include "osg/Material"
#include "osg/LineWidth"
#include "osg/BoundingBox"
#include "osg/Geode"
#include "osg/LightModel"
#include "osg/PolygonOffset"
#include "osg/ClipNode"
#include "osg/ClipPlane"
#include "osg/ShapeDrawable"
#include "osg/LightSource"
#include "osg/FrameBufferObject"
#include "osg/Notify"
#include "osg/Stencil"
#include "osg/AutoTransform"
#include "osg/Version"
#include "osg/GL"
#include "osg/GLU"
#include "osg/GLObjects"

#include <limits>

using namespace OsgTools::Render;
namespace Sections = Usul::Registry::Sections;
namespace Keys = Usul::Registry::Keys;
typedef Usul::Registry::Database Reg;

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( Viewer, Viewer::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
//  NaN convienence function.
//
///////////////////////////////////////////////////////////////////////////////

USUL_DECLARE_NAN_VEC3 ( osg::Vec3f );
USUL_DECLARE_NAN_VEC3 ( osg::Vec3d );

///////////////////////////////////////////////////////////////////////////////
//
//  Converters for registry.
//
///////////////////////////////////////////////////////////////////////////////

USUL_CONVERT_DEFINE_STRING_CONVERTER_VECTOR_4 ( osg::Vec4f );
USUL_CONVERT_DEFINE_STRING_CONVERTER_VECTOR_4 ( osg::Vec4d );
USUL_CONVERT_DEFINE_STRING_CONVERTER_VECTOR_3 ( osg::Vec3f );
USUL_CONVERT_DEFINE_STRING_CONVERTER_VECTOR_3 ( osg::Vec3d );
USUL_CONVERT_DEFINE_STRING_CONVERTER_VECTOR_2 ( osg::Vec2f );
USUL_CONVERT_DEFINE_STRING_CONVERTER_VECTOR_2 ( osg::Vec2d );
USUL_CONVERT_DEFINE_STRING_CONVERTER_VECTOR_4 ( osg::Quat );


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to check for errors.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  void checkForErrors ( unsigned int id )
  {
    GLenum error = ::glGetError();
    if ( GL_NO_ERROR != error )
    {
      std::cout << "Error " << id << ": OpenGL error detected." << std::endl
        << "Message: " << ::gluErrorString ( error ) << std::endl;
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Viewer::Viewer ( Document *doc, IUnknown* context, IUnknown *caller ) :
  BaseClass            (),
  _context             ( context ),
  _renderer            ( new Renderer ),
  _sceneManager        ( new SceneManager ),
  _timeoutSpin         ( caller ),
  _caller              ( caller ),
  _lods                (),
  _document            ( doc ),
  _frameDump           (),
  _flags               ( _UPDATE_TIMES | _SHOW_AXES | _SHOW_TEXT | _USE_LOW_LODS ),
  _animation           (),
  _navManip            ( 0x0 ),
  _currentMode         ( NAVIGATION ),
  _contextId           ( 0 ),
  _useDisplayList      ( false, true ),
  _renderListeners     (),
  _intersectListeners  (),
  _updateListeners     (),
  _mouseEventListeners (),
  _activeDragger       ( 0x0 ),
  _pointerInfo         (),
  _clipPlaneWidgets    (),
  _clampProjectionMatrixCallback ( 0x0 )
{
  // Initialize the clock.
  Usul::System::Clock::milliseconds();

  // Add the manipulator.
  osg::ref_ptr<Trackball> trackball ( new Trackball );
  trackball->minimumZoomScale ( std::numeric_limits<float>::min() );
  this->navManip ( trackball.get() );

  // Add axes to the scene.  
  // Make sure that this is called after the manipulator has been created.
  this->_addAxes();

  // Add the document
  this->document ( doc );

  // Light so that other lights don't alter geometry under the projection node.
  // Note...
  // With the introduction of the gradient background (2006-Dec-07) this line 
  // of code causes lighting problems in cow.osg and color_by_domain.ive.
#if 0
  osg::ref_ptr< osg::Light > light ( new osg::Light );
  light->setLightNum ( 1 );
  light->setDiffuse ( osg::Vec4 ( 0.8, 0.8, 0.8, 1.0 ) );
  osg::ref_ptr< osg::StateSet > ss ( _sceneManager->projection()->getOrCreateStateSet() );
  ss->setAttribute ( light.get(), osg::StateAttribute::ON | osg::StateAttribute::PROTECTED );
#endif

  // Unique context id to identify this viewer in OSG.
  static unsigned int count ( 0 );
  _contextId = ++count;

  // Set the scene.
  _renderer->scene ( _sceneManager->scene() );
  
  // Make the background the default.
  this->defaultBackground();

#ifdef _DEBUG
  //osg::setNotifyLevel ( osg::INFO );
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Viewer::~Viewer()
{
  // Better be zero
  USUL_ASSERT ( 0 == this->refCount() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::create()
{
  // If we have a valid context...
  if ( _context.valid() )
  {
    // Make this context current.
    _context->makeCurrent();
  }

  // Initialize renderer.
  _renderer->init();

  // Set default stereo modes.
  this->stereoEyeDistance ( 0.01f );
  GLboolean hasStereo ( GL_FALSE );
  ::glGetBooleanv ( GL_STEREO, &hasStereo );
  if ( GL_TRUE == hasStereo )
    this->stereoMode ( osg::DisplaySettings::QUAD_BUFFER );

  // Counter for display-list id. OSG will handle using the correct display 
  // list for this context.
  _renderer->uniqueID ( _contextId );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear the viewer.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::clear()
{
  // If we have an active timer then remove it.
  this->spin ( false );

  // Remove this view from the document.
  if ( this->document() )
  {
    this->document()->removeView ( this );
  }

  // Delete all display-lists associated with our context id.
  if ( _context.valid() )
  {
    _context->makeCurrent();
  }
  
  // Get the state for the viewer.
  osg::ref_ptr<osg::State> state ( this->viewer()->getRenderInfo().getState() );
  
  // Have the scene release all it's gl objects.
  _sceneManager->scene()->releaseGLObjects ( state.get() );
  
  // Unref the scene manager.
  _sceneManager = 0x0;
  
  // Have the viewer release all it's gl objects.
  this->viewer()->releaseAllGLObjects();
  
  // Reset the state.
  if ( true == state.valid() ) 
    state->reset();

  // Clear the renderer.
  _renderer->clear();
  
  // Unset the scene.
  this->viewer()->setSceneData ( 0x0 );
  
  // Flush all delete objects for this context.
  osg::flushAllDeletedGLObjects ( _contextId );

  _context = static_cast < Usul::Interfaces::IUnknown* > ( 0x0 );
  _timeoutSpin = static_cast < Usul::Interfaces::ITimeoutSpin* > ( 0x0 );
  _caller = static_cast < Usul::Interfaces::IUnknown* > ( 0x0 );

  // Remove listeners.
  this->clearRenderListeners();
  this->clearIntersectListeners();
  this->clearMouseEventListeners();
  this->_clearUpdateListeners();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the default background.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::defaultBackground()
{
  this->backgroundColor ( OsgTools::Render::Defaults::CLEAR_COLOR, Corners::TOP );
  this->backgroundColor ( osg::Vec4 ( 0.0, 0.0, 0.0, 1.0 ), Corners::BOTTOM );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the scene.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::update()
{
  // Notify our listeners to update.
  {
    Guard guard ( this->mutex() );
    Usul::Interfaces::IUnknown::QueryPtr unknown ( this->queryInterface ( Usul::Interfaces::IUnknown::IID ) );
    std::for_each ( _updateListeners.begin(), _updateListeners.end(), std::bind2nd ( std::mem_fun ( &IUpdateListener::updateNotify ), unknown.get() ) );
  }

  // Have the renderer update.
  _renderer->update();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Render the scene.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::render()
{
  // Handle no viewer or scene.
  if ( !this->viewer() || !this->viewer()->getSceneData() || false == _context.valid() )
    return;

  // Return if it's not the correct thread.
  if ( false == _context->isContextThread() )
    return;

  // Make this context current.
  if ( _context.valid() )
    _context->makeCurrent();

  // Update display lists.
  this->updateDisplayListUse();

  // Initialize the error.
  ::glGetError();
  
  // Set the multisample state.
#ifdef __APPLE__ // Not compiling on windows.
  this->useMultisampleGet() ? ::glEnable ( GL_MULTISAMPLE_ARB ) : ::glDisable ( GL_MULTISAMPLE_ARB );
#endif

  // Check for errors.
  Detail::checkForErrors ( 1491085606 );

  // Update.
  this->update();
  
  // Need local scope.
  {
    Usul::Scope::Caller::RefPtr preAndPostCall ( Usul::Scope::makeCaller ( 
      Usul::Adaptors::memberFunction ( this, &Viewer::_preRenderNotify ), 
      Usul::Adaptors::memberFunction ( this, &Viewer::_postRenderNotify ) ) );

    // Set the axes.
    this->_setAxes();

    // Set the center of rotation
    this->_setCenterOfRotation();

    // Set high lod callbacks if we should
    if ( this->useHighLodsGet() )
      this->_setLodCullCallback ( new OsgTools::Render::HighLodCallback );

    // Check for errors.
    Detail::checkForErrors ( 774712060 );

    // If we are doing hidden-line rendering...
    if ( this->hasHiddenLines() && ( 0x0 != _sceneManager->model() ) )
    {
      // Special path for hidden lines.
      this->_hiddenLineRender();
    }
    else
    {
      // Regular rendering.
      this->_render();
    }

    // Check for errors.
    Detail::checkForErrors ( 840649560 );

#if 0
#ifdef _DEBUG
    std::cout << "Just rendered frame: " << _renderer->viewer()->getFrameStamp()->getFrameNumber() << std::endl;
#endif
#endif
  }

  // Swap the buffers.
  if ( _context.valid() )
    _context->swapBuffers();

  // Flush deleted GL objects.
  this->viewer()->flushAllDeletedGLObjects();
  
  double maxtime ( std::numeric_limits<double>::max() );
  osg::Texture::flushDeletedTextureObjects ( _contextId, Usul::System::Clock::milliseconds(), maxtime );

  // Check for errors.
  Detail::checkForErrors ( 896118310 );

  // Dump the current frame.
  this->_dumpFrame();

  // Check for errors.
  Detail::checkForErrors ( 952680810 );

  // Update the status-bar. Do not put this in onPaint() because you want 
  // it called every time the window is redrawn.
  if ( this->updateTimes() )
    this->updateStatusBar();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Render hidden-line mode.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::_hiddenLineRender()
{
  // If we are doing hidden-line rendering...
  if ( ( false == this->hasHiddenLines() ) || ( 0x0 == _sceneManager->model() ) )
    return;

  // Temporarily re-structure the scene. Better to do/undo this than keep 
  // it altered. An altered scene may mess up intersections.
  osg::ref_ptr<osg::Node> model ( _sceneManager->model() );
  osg::ref_ptr<osg::Group> root   ( new osg::Group );
  osg::ref_ptr<osg::Group> normal ( new osg::Group );
  osg::ref_ptr<osg::Group> hidden ( new osg::Group );
  root->addChild ( normal.get() );
  root->addChild ( hidden.get() );
  normal->addChild ( model.get() );
  hidden->addChild ( model.get() );

  // Safely...
  try
  {
    // Set the new scene.
    _sceneManager->model ( root.get() );

    // Set the state-sets for the branches.
    OsgTools::State::StateSet::hiddenLines ( this->backgroundColor(), normal->getOrCreateStateSet(), hidden->getOrCreateStateSet() );

    // Draw.
    this->_render();
  }

  // Catch all exceptions.
  catch ( ... )
  {
    // Restore the scene and re-throw.
    _sceneManager->model ( model.get() );
    throw;
  }

  // Restore the scene.
  _sceneManager->model ( model.get() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Render the scene.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::_render()
{
  // Draw.
  _renderer->render();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the viewer.
//
///////////////////////////////////////////////////////////////////////////////

Viewer::SceneView *Viewer::viewer()
{
  return _renderer->viewer();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the viewer.
//
///////////////////////////////////////////////////////////////////////////////

const Viewer::SceneView *Viewer::viewer() const
{
  return _renderer->viewer();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Go to given camera.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::camera ( CameraOption option )
{
  // For convenience.
  SceneView *viewer = this->viewer();

  // If we don't have a viewer...
  if ( !viewer )
    return;

  // Get the current state. Note: do not use _navManip->getInverseMatrix().
  osg::Matrixd M1 ( viewer->getViewMatrix() );
  osg::Vec3d T1 ( M1.getTrans() );
  osg::Quat R1; M1.get ( R1 );

  // Make the adapters.
  EventAdapter::Ptr ea ( new EventAdapter );
  unsigned int width ( static_cast < unsigned int > ( this->width() ) );
  unsigned int height ( static_cast < unsigned int > ( this->height() ) );
  ea->setWindowSize ( Usul::Math::Vec2ui ( width, height ) );
  ActionAdapter aa ( this->queryInterface( Usul::Interfaces::IUnknown::IID ) );

  // See if we have a trackball...
  Trackball *trackball = dynamic_cast < Trackball * > ( this->navManip() );

  // Try to save the distance.
  std::pair<bool,float> dist ( false, 0 );
  if ( trackball )
  {
    dist.first = true;
    dist.second = trackball->distance();
  }

  typedef Usul::Interfaces::ICamera Camera;

  // Is the request with respect to the current matrix?
  if ( Camera::ROTATE_Y_N45 != option && Camera::ROTATE_Y_P45 != option &&
       Camera::ROTATE_X_N45 != option && Camera::ROTATE_X_P45 != option )
  {
    // Tell the manipulator to go home.
    this->navManip()->home ( *ea, aa );
  }

  // If we are not resetting and we have a trackball...
  if ( Camera::RESET != option && trackball )
  {
    osg::Quat adjust;
    switch ( option )
    {
      case Camera::FIT:
        trackball->rotation ( R1.inverse() );
        break;
      case Camera::BACK:
        adjust.makeRotate (  osg::PI,   osg::Vec3 ( 0, 1, 0 ) );
        trackball->rotation ( trackball->rotation() * adjust );
        break;
      case Camera::LEFT:
        adjust.makeRotate ( -osg::PI_2, osg::Vec3 ( 0, 1, 0 ) );
        trackball->rotation ( trackball->rotation() * adjust );
        break;
      case Camera::RIGHT:
        adjust.makeRotate (  osg::PI_2, osg::Vec3 ( 0, 1, 0 ) );
        trackball->rotation ( trackball->rotation() * adjust );
        break;
      case Camera::TOP:
        adjust.makeRotate ( -osg::PI_2, osg::Vec3 ( 1, 0, 0 ) );
        trackball->rotation ( trackball->rotation() * adjust );
        break;
      case Camera::BOTTOM:
        adjust.makeRotate (  osg::PI_2, osg::Vec3 ( 1, 0, 0 ) );
        trackball->rotation ( trackball->rotation() * adjust );
      case Camera::ROTATE_Y_N45:
        adjust.makeRotate ( -osg::PI_4, osg::Vec3 ( 0, 1, 0 ) );
        trackball->rotation ( adjust * trackball->rotation() );
        break;
      case Camera::ROTATE_Y_P45:
        adjust.makeRotate (  osg::PI_4, osg::Vec3 ( 0, 1, 0 ) );
        trackball->rotation ( adjust * trackball->rotation() );
        break;
      case Camera::ROTATE_X_N45:
        adjust.makeRotate ( -osg::PI_4, osg::Vec3 ( 1, 0, 0 ) );
        trackball->rotation ( adjust * trackball->rotation() );
        break;
      case Camera::ROTATE_X_P45:
        adjust.makeRotate (  osg::PI_4, osg::Vec3 ( 1, 0, 0 ) );
        trackball->rotation ( adjust * trackball->rotation() );
        break;
    }
  }

  // Unless we are fitting, reset the distance.
  if ( Camera::FIT != option && trackball && true == dist.first )
    trackball->distance ( dist.second );

  // Get the final matrix.
  osg::Matrixd M2 ( this->navManip()->getInverseMatrix() );

  // See if the new matrix is the same as the old.
  Usul::Predicates::Tolerance<double> tolerance ( 0.001 );
  const bool same ( std::equal ( M1.ptr(), M1.ptr() + 16, M2.ptr(), tolerance ) );

  // If we are not supposed to animate, or if the matrices are the same...
  // Note, if the matrices are the same then animating makes no sense.
  if ( same )
  {
    // Set the viewer's matrix.
    viewer->setViewMatrix ( M2 );
    return;
  }

  // Use low lods, if we are suppose to.
  if ( this->useLowLodsGet() )
    this->_setLodCullCallback ( new OsgTools::Render::LowLodCallback );

  // Get the translation and rotation.
  osg::Vec3d T2 ( M2.getTrans() );
  osg::Quat R2; M2.get ( R2 );

  // Animate.
  Usul::Interfaces::ITimeoutAnimate::QueryPtr animate ( _caller );
  if ( animate.valid() )
  {
    _animation.init ( T1, R1, T2, R2, _ANIMATION_DURATION_MILLISECONDS );
    animate->startAnimation ( _ANIMATION_TIMER_MILLISECONDS );
  }
  else
  {
    viewer->setViewMatrix ( M2 );
    this->render();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Access to the scene.
//
///////////////////////////////////////////////////////////////////////////////

const osg::Node *Viewer::scene() const
{
  return _sceneManager->scene();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Access to the scene.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node *Viewer::scene()
{
  return _sceneManager->scene();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the scene.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::scene ( osg::Node *node )
{
  // Handle same scene.
  if ( 0x0 != node && node == _sceneManager->model() )
    return;

  _sceneManager->model( node );

  // Give the scene to the viewer.
  _renderer->scene ( _sceneManager->scene() );

  // The scene changed.
  this->changedScene(); 

  // Display list use is dirty.
  _useDisplayList.second = true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Resize the window.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::resize ( unsigned int w, unsigned int h )
{
  USUL_ERROR_CHECKER ( w < (unsigned int) std::numeric_limits<int>::max() );
  USUL_ERROR_CHECKER ( h < (unsigned int) std::numeric_limits<int>::max() );

  // Ignore initial values sent; they make a poor viewport.
  if ( w <= 1 || h <= 1 )
    return;

  // Set the viewport.
  this->viewport ( 0, 0, (int) w, (int) h );

  // Set the viewer's projection matrix.
  const double fovy  ( OsgTools::Render::Defaults::CAMERA_FOV_Y );
  double zNear ( OsgTools::Render::Defaults::CAMERA_Z_NEAR );
  double zFar  ( OsgTools::Render::Defaults::CAMERA_Z_FAR );
  double width ( w ), height ( h );
  double aspect ( width / height );
  if ( this->viewer() )
    this->viewer()->setProjectionMatrixAsPerspective ( fovy, aspect, zNear, zFar );

  // Update the projection node.
  _sceneManager->resize ( 0, w ,0, h );

  // Update the renderer.
  _renderer->resize ( w, h );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the viewport.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::viewport ( osg::Viewport *vp )
{
  _renderer->viewport( vp );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the viewport.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::viewport ( int x, int y, unsigned int w, unsigned int h )
{
  _renderer->viewport ( x, y, w, h );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the viewport.
//
///////////////////////////////////////////////////////////////////////////////

const osg::Viewport *Viewer::viewport() const
{
  return _renderer->viewport();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the viewport.
//
///////////////////////////////////////////////////////////////////////////////

osg::Viewport *Viewer::viewport()
{
  return _renderer->viewport();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Toggle the polygon mode.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::togglePolygonMode ( osg::PolygonMode::Face face, osg::PolygonMode::Mode mode )
{
  if ( this->hasPolygonMode ( face, mode ) )
    this->removePolygonMode();
  else
    this->setPolygonMode ( face, mode );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the polygon mode.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::setPolygonMode ( osg::PolygonMode::Face face, osg::PolygonMode::Mode mode )
{
  // Handle no viewer.
  if ( !this->viewer() )
    return;

  // Get the state set, or make one.
  osg::ref_ptr<osg::StateSet> ss ( this->viewer()->getGlobalStateSet() );
  if ( !ss.valid() )
    return;

  // Make a polygon-mode.
  osg::ref_ptr<osg::PolygonMode> pm ( new osg::PolygonMode );
  pm->setMode ( face, mode );

  // Set the state. Make it override any other similar states.
  typedef osg::StateAttribute Attribute;
  ss->setAttributeAndModes ( pm.get(), Attribute::OVERRIDE | Attribute::ON );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Toggle the shade model.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::toggleShadeModel ( osg::ShadeModel::Mode mode )
{
  if ( this->hasShadeModel ( mode ) )
    this->removeShadeModel();
  else
    this->setShadeModel ( mode );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the shade model.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::setShadeModel ( osg::ShadeModel::Mode mode )
{
  // Handle no viewer.
  if ( !this->viewer() )
    return;

  // Get the state set, or make one.
  osg::ref_ptr<osg::StateSet> ss ( this->viewer()->getGlobalStateSet() );
  if ( !ss.valid() )
    return;

  // Make a shade-model.
  osg::ref_ptr<osg::ShadeModel> sm ( new osg::ShadeModel );
  sm->setMode ( mode );

  // Set the state. Make it override any other similar states.
  typedef osg::StateAttribute Attribute;
  ss->setAttributeAndModes ( sm.get(), Attribute::OVERRIDE | Attribute::ON );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query the polygon mode.
//
///////////////////////////////////////////////////////////////////////////////

bool Viewer::hasPolygonMode ( osg::PolygonMode::Face face, osg::PolygonMode::Mode mode ) const
{
  // Handle no scene.
  if ( !this->scene() )
    return false;

  // Get the state set.
  const osg::StateSet *ss = this->viewer()->getGlobalStateSet();
  if ( !ss )
    return false;

  // Get the polygon-mode attribute, if any.
  const osg::StateAttribute *sa = ss->getAttribute ( osg::StateAttribute::POLYGONMODE );
  if ( !sa )
    return false;

  // Should be true.
  const osg::PolygonMode *pm = dynamic_cast < const osg::PolygonMode * > ( sa );
  if ( !pm )
    return false;

  // Is the mode the same?
  return ( pm->getMode ( face ) == mode );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query the polygon mode.
//
///////////////////////////////////////////////////////////////////////////////

bool Viewer::hasPolygonMode ( osg::PolygonMode::Face face ) const
{
  // See if there is any polygon-mode for the given face.
  const bool filled ( this->hasPolygonMode ( face, osg::PolygonMode::FILL  ) );
  const bool lines  ( this->hasPolygonMode ( face, osg::PolygonMode::LINE  ) );
  const bool points ( this->hasPolygonMode ( face, osg::PolygonMode::POINT ) );
  return ( filled || lines || points );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query the polygon mode.
//
///////////////////////////////////////////////////////////////////////////////

bool Viewer::hasPolygonMode() const
{
  // See if there is any polygon-mode at all.
  const bool front ( this->hasPolygonMode ( osg::PolygonMode::FRONT ) );
  const bool back  ( this->hasPolygonMode ( osg::PolygonMode::BACK ) );
  const bool both  ( this->hasPolygonMode ( osg::PolygonMode::FRONT_AND_BACK ) );
  return ( front || back || both );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query the shading model.
//
///////////////////////////////////////////////////////////////////////////////

bool Viewer::hasShadeModel ( osg::ShadeModel::Mode mode ) const
{
  // Get the state set.
  const osg::StateSet *ss = _renderer->getGlobalStateSet();

  // Get the attribute, if any.
  const osg::StateAttribute *sa = ss->getAttribute ( osg::StateAttribute::SHADEMODEL );
  if ( !sa )
    return false;

  // Should be true.
  const osg::ShadeModel *sm = dynamic_cast < const osg::ShadeModel * > ( sa );
  if ( !sm )
    return false;

  // Is the mode the same?
  return ( sm->getMode() == mode );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query the shading model.
//
///////////////////////////////////////////////////////////////////////////////

bool Viewer::hasShadeModel() const
{
  // See if there is any polygon-mode for the given face.
  const bool smooth ( this->hasShadeModel ( osg::ShadeModel::SMOOTH ) );
  const bool flat   ( this->hasShadeModel ( osg::ShadeModel::FLAT   ) );
  return ( smooth || flat );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove the polygon mode.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::removePolygonMode()
{
  // Get the state set.
  osg::StateSet *ss = _renderer->getGlobalStateSet();

  // Have the state-set inherit the attribute. This will delete any 
  // existing attribute in the state-set.
  ss->removeAttribute ( osg::StateAttribute::POLYGONMODE );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove the shading model.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::removeShadeModel()
{
  // Get the state set.
  osg::StateSet *ss = _renderer->getGlobalStateSet();

  // Have the state-set inherit the attribute. This will delete any 
  // existing attribute in the state-set.
  ss->removeAttribute ( osg::StateAttribute::SHADEMODEL );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the texture environment.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::setTextureEnvironment ( TexEnv::Mode mode )
{
  // Handle no viewer.
  if ( !this->viewer() )
    return;

  // Get the state set, or make one.
  osg::ref_ptr<osg::StateSet> ss ( this->viewer()->getGlobalStateSet() );
  if ( !ss.valid() )
    return;

  // Make a texture environment.
  osg::ref_ptr<osg::TexEnv> te ( new osg::TexEnv );
  te->setMode ( mode );

  // Set the state. Make it override any other similar states.
  typedef osg::StateAttribute Attribute;
  ss->setAttributeAndModes ( te.get(), Attribute::OVERRIDE | Attribute::ON );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Do we have the texture environment?
//
///////////////////////////////////////////////////////////////////////////////

bool Viewer::hasTextureEnvironment ( TexEnv::Mode mode ) const
{
  // Get the state set.
  const osg::StateSet *ss = _renderer->getGlobalStateSet();

  // Get the attribute, if any.
  const osg::StateAttribute *sa = ss->getAttribute ( osg::StateAttribute::TEXENV );
  if ( !sa )
    return false;

  // Should be true.
  const osg::TexEnv *te = dynamic_cast < const osg::TexEnv * > ( sa );
  if ( !te )
    return false;

  // Is the mode the same?
  return ( te->getMode() == mode );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Do we have any texture environment?
//
///////////////////////////////////////////////////////////////////////////////

bool Viewer::hasTextureEnvironment() const
{
  // See if there is any polygon-mode for the given face.
  const bool decal    ( this->hasTextureEnvironment ( osg::TexEnv::DECAL ) );
  const bool modulate ( this->hasTextureEnvironment ( osg::TexEnv::MODULATE ) );
  const bool blend    ( this->hasTextureEnvironment ( osg::TexEnv::BLEND ) );
  const bool replace  ( this->hasTextureEnvironment ( osg::TexEnv::REPLACE ) );
  const bool add      ( this->hasTextureEnvironment ( osg::TexEnv::ADD ) );
  return ( decal || modulate || blend || replace || add );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove the texture environment.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::removeTextureEnvironment()
{
  osg::StateSet *ss = _renderer->getGlobalStateSet();
  ss->removeAttribute ( osg::StateAttribute::TEXENV );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the texture mode.
//
///////////////////////////////////////////////////////////////////////////////

bool Viewer::textureMode ( osg::StateAttribute::GLMode mode ) const
{
  const osg::StateSet *ss = _renderer->getGlobalStateSet();
  return Usul::Bits::has ( ss->getTextureMode ( 0, mode ), osg::StateAttribute::ON );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the texture mode.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::textureMode ( osg::StateAttribute::GLMode mode, bool state )
{
  osg::StateSet *ss = _renderer->getGlobalStateSet();
  return ss->setTextureMode ( 0, mode, ( ( state ) ? osg::StateAttribute::ON : osg::StateAttribute::OFF ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set two sided lighting
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::twoSidedLightingSet ( bool twoSided )
{
  // Get the state set, or make one.
  osg::ref_ptr<osg::StateSet> ss ( _renderer->getGlobalStateSet() );

  // Make a light-model.
  osg::ref_ptr<osg::LightModel> lm ( new osg::LightModel );
  lm->setTwoSided( twoSided );

  // Set the state. Make it override any other similar states.
  typedef osg::StateAttribute Attribute;
  ss->setAttributeAndModes ( lm.get(), Attribute::OVERRIDE | Attribute::ON );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get two sided lighting
//
///////////////////////////////////////////////////////////////////////////////

bool Viewer::twoSidedLightingGet() const
{
  // Get the state set.
  const osg::StateSet *ss = _renderer->getGlobalStateSet();

  // Get the attribute, if any.
  const osg::StateAttribute *sa = ss->getAttribute ( osg::StateAttribute::LIGHTMODEL );
  if ( !sa )
    return false;

  // Should be true.
  const osg::LightModel *lm = dynamic_cast < const osg::LightModel * > ( sa );
  if ( !lm )
    return false;

  //Is two sided light on?
  return ( lm->getTwoSided() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set lighting
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::lighting ( bool on )
{
  // Get the state set, or make one.
  osg::ref_ptr<osg::StateSet> ss ( _renderer->getGlobalStateSet() );
  ss->setMode ( GL_LIGHTING, on ? osg::StateAttribute::ON : osg::StateAttribute::OFF );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get lighting
//
///////////////////////////////////////////////////////////////////////////////

bool Viewer::lighting() const
{
  // Get the state set.
  const osg::StateSet *ss = _renderer->getGlobalStateSet();

  return Usul::Bits::has ( ss->getMode( GL_LIGHTING ), osg::StateAttribute::ON );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the lod-callbacks.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::_setLodCullCallback ( osg::NodeCallback *cb )
{
  OsgTools::Render::setLodCullCallback ( cb, this->scene(), _lods );
  
  // Since the document may change the scene in an update callback, we cannot cache the lods we found.
  // Should the document let the viewer know that it has been updated an to clear any cache?
  _lods.second.clear();
  _lods.first = false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the background color.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::backgroundColor ( const osg::Vec4 &color, unsigned int corners )
{
  _renderer->backgroundColor ( color, corners );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the background color.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::backgroundColor ( const osg::Vec4 &color )
{
  this->backgroundColor ( color, this->backgroundCorners() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the background color.
//
///////////////////////////////////////////////////////////////////////////////

osg::Vec4 Viewer::backgroundColor() const
{
  return _renderer->backgroundColor();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the background color.
//
///////////////////////////////////////////////////////////////////////////////

osg::Vec4 Viewer::backgroundColor ( unsigned int corners ) const
{
  return _renderer->backgroundColor ( corners );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the background corners.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::backgroundCorners ( unsigned int corners )
{
  _renderer->backgroundCorners ( corners );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the background corners.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Viewer::backgroundCorners() const
{
  return _renderer->backgroundCorners();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Rotate the scene with no animation.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::rotate ( const osg::Vec3 &axis, float radians )
{
  // Return if no viewer.
  if ( !this->viewer() )
    return;

  // Get the trackball.
  Trackball *tb = dynamic_cast < Trackball* > ( this->navManip( ) );
  if ( 0x0 == tb )
    return;

  // Get the current state.
  osg::Quat R1 ( tb->rotation() );

  // Get the final state.
  osg::Quat dR, R2;
  dR.makeRotate ( radians, axis );
  R2 = dR * R1;

  // Set the rotation.
  tb->rotation ( R2 );
  this->viewer()->setViewMatrix ( tb->getInverseMatrix() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the lod-callback.
//
///////////////////////////////////////////////////////////////////////////////

Viewer::LowLods::LowLods ( Viewer &c ) : _c ( c )
{
  if ( _c.useLowLodsGet() )
    _c._setLodCullCallback ( new OsgTools::Render::LowLodCallback );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Restore the lod-callback.
//
///////////////////////////////////////////////////////////////////////////////

Viewer::LowLods::~LowLods()
{
  _c._setLodCullCallback ( static_cast<OsgTools::Render::LowLodCallback *> ( 0x0 ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the last time.
//
///////////////////////////////////////////////////////////////////////////////

double Viewer::timeLast ( const std::string &name ) const
{
  return _renderer->timeLast( name );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the average time.
//
///////////////////////////////////////////////////////////////////////////////

double Viewer::timeAverage ( const std::string &name ) const
{
  return _renderer->timeAverage( name );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Invalidate the various scene shortcuts.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::changedScene()
{
  // Reset the lod-callback cache.
  _lods.first = false;
  _lods.second.clear();

  // Invalidate the bounding spheres.
  if ( this->scene() )
    this->scene()->dirtyBound();

  // Update the trees.
  typedef Usul::Interfaces::IUpdateTreeControls IUpdateTreeControls;
  typedef Usul::Components::Manager             PluginManager;

  IUpdateTreeControls::QueryPtr update ( PluginManager::instance().getInterface( IUpdateTreeControls::IID ) );
  if( update.valid() )
    update->updateTreeControls ( this->scene() );

  // Update the manipulators.
  this->navManip()->setNode ( this->scene() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the status-bar.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::updateStatusBar()
{
    // Format the text.
    const unsigned int size ( 512 );
    char buf[size];
    double lu ( this->timeLast    ( "update" ) );
    double lc ( this->timeLast    ( "cull"   ) );
    double ld ( this->timeLast    ( "draw"   ) );
    double au ( this->timeAverage ( "update" ) );
    double ac ( this->timeAverage ( "cull"   ) );
    double ad ( this->timeAverage ( "draw"   ) );
    ::sprintf ( buf, "(Last:Avg)   Update: %0.6f:%0.6f   Cull: %0.6f:%0.6f   Draw: %0.6f:%0.6f", 
                lu, au, lc, ac, ld, ad );

    // Set the text.
    this->setStatusBarText ( buf, false );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the status bar text.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::setStatusBarText ( const std::string &text, bool force )
{
  Usul::Interfaces::IStatusBar::QueryPtr status ( Usul::Resources::statusBar() );
  if( status.valid() )
    status->setStatusBarText( text, force );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Dump the current frame to file.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::_dumpFrame()
{
  if ( Usul::Interfaces::IFrameDump::NEVER_DUMP == _frameDump.dump() )
  {
    return;
  }

  if ( Usul::Interfaces::IFrameDump::IF_DOCUMENT_NOT_BUSY == _frameDump.dump() )
  {
    Usul::Interfaces::IBusyState::QueryPtr busy ( this->document() );
    if ( true == busy->busyStateGet() )
    {
      return;
    }
  }

#if 1

  this->writeImageFile ( _frameDump.file() );

#else // Better for the long-term, but no anti-aliasing yet...

  this->_writeImageFile ( _frameDump.file(), 
                          static_cast<unsigned int> ( this->width() ), 
                          static_cast<unsigned int> ( this->height() ) );
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the current frame to an image file.
//
//  TODO, look into this:
//  http://www.codeproject.com/opengl/glexport.asp?df=100&forumid=1853&exp=0&select=826220
//  http://www.mesa3d.org/brianp/sig97/offscrn.htm <-- tiled rendering
//
///////////////////////////////////////////////////////////////////////////////

bool Viewer::writeImageFile ( const std::string &filename ) const
{
  // Get scale of window size.
  const float scale ( this->frameDump().scale() );

  // Is the size of the image the same as the window?
  if ( 1.0f != scale )
  {
    // Calculate size.
    const unsigned int width  ( static_cast<unsigned int> ( scale * this->width()  ) );
    const unsigned int height ( static_cast<unsigned int> ( scale * this->height() ) );

    // Write the image to file.
    return this->_writeImageFile ( filename, width, height );
  }

  // Otherwise...
  else
  {
    // Write the image to file.
    return this->_writeImageFile ( filename );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the current frame to the file.  No tile rendering.
//
///////////////////////////////////////////////////////////////////////////////

bool Viewer::_writeImageFile ( const std::string &filename ) const
{
  // Make the image
  osg::ref_ptr<osg::Image> image ( new osg::Image );

  int x ( static_cast < int > ( this->x() ) );
  int y ( static_cast < int > ( this->y() ) );

  // Read the screen buffer.
  image->readPixels ( x, y, static_cast<int> ( this->width() ), static_cast<int> ( this->height() ), GL_RGB, GL_UNSIGNED_BYTE );

  // Write the image.
  return osgDB::writeImageFile ( *image, filename );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the current frame to the file.  
//  Size will be the specified height and width
//
///////////////////////////////////////////////////////////////////////////////

bool Viewer::_writeImageFile ( const std::string &filename, unsigned int width, unsigned int height ) const
{
  if ( 0x0 == this->viewer() )
    return false;

  // Get non const pointer to this
  Viewer *me ( const_cast < Viewer * > ( this ) );

  // Make this context current.
  if ( _context.valid() )
    me->_context->makeCurrent();

  // Initialize the image.
  osg::ref_ptr<osg::Image> image ( 0x0 );

  // Get size of frame buffer object.
  const Usul::Math::Vec2ui maxSize 
    ( Reg::instance()[Sections::VIEWER_SETTINGS][Keys::FRAME_BUFFER_OBJECT_SIZE].get<Usul::Math::Vec2ui> 
      ( Usul::Math::Vec2ui ( 4096, 4096 ) ) );

  // Can we use the frame-buffer object?
  if ( ( width <= maxSize[0] ) && ( height <= maxSize[1] ) )
  {
    OsgTools::Render::FBOScreenCapture fbo;
    fbo.size ( width, height );
    fbo.viewMatrix ( this->getViewMatrix() );
    image = fbo ( *(me->viewer()), this->viewer()->getProjectionMatrix() );
  }

  // Otherwise, tile.
  else
  {
    image = me->_renderer->screenCapture ( this->getViewMatrix(), width, height );
  }

  // Write the image to file.
  return ( ( true == image.valid() ) ? osgDB::writeImageFile ( *image, filename ) : false );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create image of current using current view matrix.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::takePicture ( const std::string& filename, float frameSizeScale, unsigned int numSamples )
{
  // Make this context current.
  if ( _context.valid() )
    _context->makeCurrent();

  // Is the size of the image the same as the window?
  if ( 1.0f != frameSizeScale )
  {
    osg::ref_ptr < osg::Image > image ( _renderer->screenCapture ( frameSizeScale, numSamples ) );

    // Write the image to file.
    osgDB::writeImageFile ( *image, filename );
  }

  // Otherwise...
  else
  {
    // Write the image to file.
    this->_writeImageFile ( filename );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the current scene to file.
//
///////////////////////////////////////////////////////////////////////////////

bool Viewer::writeSceneFile ( const std::string &filename, const std::string &options ) const
{
  // Save and restore default options.
  OsgTools::ScopedOptions scoped ( options );

  // Write the scene to file.
  return osgDB::writeNodeFile ( *this->scene(), filename );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the current model to file.
//
///////////////////////////////////////////////////////////////////////////////

bool Viewer::writeModelFile ( const std::string &filename, const std::string &options ) const
{
  // Save and restore default options.
  OsgTools::ScopedOptions scoped ( options );

  // Write the scene to file.
  return osgDB::writeNodeFile ( *this->model(), filename );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Show/hide the bounding box.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::boundingBox ( bool state )
{
    // Remove the bounding box if it exists.
  if ( _sceneManager->groupHas( OsgTools::Render::Constants::BOUNDING_BOX ) )
  {
    _sceneManager->groupRemove( OsgTools::Render::Constants::BOUNDING_BOX );
  }

  // If we are supposed to show the bounding box...
  if ( state )
  {
    // Get the existing bounding box, if it exists.
    osg::ref_ptr<osg::Group> group ( _sceneManager->groupGet ( OsgTools::Render::Constants::BOUNDING_BOX ) );

    // Get the model's bounding box.
    osg::BoundingBox bb;

    // If the document supports the get bounding box interface, use that for the bounding box.
    // If not use the model node in the scene.
    Usul::Interfaces::IGetBoundingBox::QueryPtr getBoundingBox ( this->document() );
    if( getBoundingBox.valid() )
      bb.expandBy ( getBoundingBox->getBoundingBox() );
    else
      bb.expandBy ( this->model()->getBound() );

    /*osg::StateSet *ss ( group->getOrCreateStateSet() );

    osg::Material *mat ( new osg::Material );
    mat->setDiffuse ( osg::Material::FRONT, osg::Vec4 ( 255.0 / 255.0, 164.5 / 255.0, 85.0 / 255.0, 1 ) );

    ss->setAttribute( mat, osg::StateAttribute::ON );

    osg::ref_ptr< OsgTools::ShapeFactory > sf ( new OsgTools::ShapeFactory );
    osg::Geode *geode ( new osg::Geode );
    geode->addDrawable ( sf->cylinder ( 1.0, 20, bb.corner( 4 ), bb.corner( 5 ) ) );
    geode->addDrawable ( sf->cylinder ( 1.0, 20, bb.corner( 5 ), bb.corner( 7 ) ) );
    geode->addDrawable ( sf->cylinder ( 1.0, 20, bb.corner( 7 ), bb.corner( 6 ) ) );
    geode->addDrawable ( sf->cylinder ( 1.0, 20, bb.corner( 6 ), bb.corner( 4 ) ) );
    geode->addDrawable ( sf->cylinder ( 1.0, 20, bb.corner( 3 ), bb.corner( 7 ) ) );
    geode->addDrawable ( sf->cylinder ( 1.0, 20, bb.corner( 1 ), bb.corner( 5 ) ) );
    geode->addDrawable ( sf->cylinder ( 1.0, 20, bb.corner( 3 ), bb.corner( 2 ) ) );
    geode->addDrawable ( sf->cylinder ( 1.0, 20, bb.corner( 1 ), bb.corner( 0 ) ) );
    geode->addDrawable ( sf->cylinder ( 1.0, 20, bb.corner( 3 ), bb.corner( 1 ) ) );
    geode->addDrawable ( sf->cylinder ( 1.0, 20, bb.corner( 2 ), bb.corner( 0 ) ) );
    geode->addDrawable ( sf->cylinder ( 1.0, 20, bb.corner( 2 ), bb.corner( 6 ) ) );
    geode->addDrawable ( sf->cylinder ( 1.0, 20, bb.corner( 4 ), bb.corner( 0 ) ) );

    group->addChild ( geode );

    for ( unsigned int i = 0; i < 8; ++i )
    {
      osg::ref_ptr < osg::Geode > g ( new osg::Geode );
      osg::ref_ptr < osg::MatrixTransform > mt ( new osg::MatrixTransform );
      mt->setMatrix ( osg::Matrix::translate ( bb.corner( i ) ) );

      mt->addChild ( g.get() );
      group->addChild ( mt.get() );

      OsgTools::ShapeFactory::MeshSize size ( 20, 20 );
      g->addDrawable ( sf->sphere ( 1.1 , size ) );
    }*/
    

    // Make a new blue box.
    OsgTools::ColorBox box ( bb );
    box.color_policy().color ( osg::Vec4 ( 0, 0, 1, 1 ) );
    //box.color_policy().color ( osg::Vec4 ( 255.0 / 255.0, 164.5 / 255.0, 85.0 / 255.0, 1 ) );

    // Position it.
    osg::ref_ptr<osg::MatrixTransform> mt ( new osg::MatrixTransform );
    mt->setMatrix ( osg::Matrix::translate ( bb.center() ) );
    mt->addChild ( box() );

    // Wire-frame.
    OsgTools::State::StateSet::setPolygonsLines ( mt.get(), true );
    OsgTools::State::StateSet::setLighting ( mt.get(), false );

    group->addChild ( mt.get() );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is the bounding box showing?
//
///////////////////////////////////////////////////////////////////////////////

bool Viewer::boundingBox() const
{
  return _sceneManager->groupHas ( OsgTools::Render::Constants::BOUNDING_BOX );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Show/hide the bounding sphere.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::boundingSphere ( bool state )
{
  // Remove the bounding sphere if it exists.
  if ( _sceneManager->groupHas ( OsgTools::Render::Constants::BOUNDING_SPHERE ) )
  {
    _sceneManager->groupRemove ( OsgTools::Render::Constants::BOUNDING_SPHERE );
  }

  // If we are supposed to show the bounding sphere...
  if ( state )
  {
    // Get the group for the bounding sphere
    osg::ref_ptr<osg::Group> group ( _sceneManager->groupGet ( OsgTools::Render::Constants::BOUNDING_SPHERE ) );

    // Get the scene's bounding sphere.
    const osg::BoundingSphere &bound = _sceneManager->model()->getBound();

    // Make a new red sphere.
    OsgTools::ColorSphere sphere ( bound );
    sphere.color_policy().color ( osg::Vec4 ( 1, 0, 0, 1 ) );

    // Position it.
    osg::ref_ptr<osg::MatrixTransform> mt ( new osg::MatrixTransform );
    mt->setMatrix ( osg::Matrix::translate ( bound.center() ) );
    mt->addChild ( sphere() );

    // Wire-frame.
    OsgTools::State::StateSet::setPolygonsLines ( mt.get(), true );
    OsgTools::State::StateSet::setLighting ( mt.get(), false );

    group->addChild ( mt.get() );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is the bounding sphere showing?
//
///////////////////////////////////////////////////////////////////////////////

bool Viewer::boundingSphere() const
{
  return _sceneManager->groupHas( OsgTools::Render::Constants::BOUNDING_SPHERE );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the document.
//
///////////////////////////////////////////////////////////////////////////////

const Viewer::Document *Viewer::document() const
{
  return _document.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the document.
//
///////////////////////////////////////////////////////////////////////////////

Viewer::Document *Viewer::document()
{
  return _document.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the document.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::document ( Document *document )
{
  _document = document;

  // Return now if we don't have a valid document.
  if ( false == _document.valid() )
    return;
  
  // Add this view to the document.
  _document->addView ( this );

  Usul::Interfaces::IMatrixManipulator::QueryPtr mm ( _document );
  if ( true == mm.valid() )
  {
    this->navManip ( mm->getMatrixManipulator() );
  }

  Usul::Interfaces::IUnknown::QueryPtr unknown ( _document );
  if ( true == unknown.valid() )
  {
    // Add the document as a render listener.
    this->addRenderListener ( unknown.get() );

    // Add the document as an update listener.
    this->addUpdateListener ( unknown.get() );

    // Add the document as an intersect listener.
    this->addIntersectListener ( unknown.get() );

    // Add the document as a mouse event listenter.
    this->addMouseEventListener ( unknown.get() );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the display-list state.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::updateDisplayListUse()
{
  // Return if not dirty.
  if ( false == _useDisplayList.second )
    return;

  // Handle no viewer or model.
  if ( !this->viewer() || !this->model() )
    return;

  // Declare the visitor.
  osg::ref_ptr<osg::NodeVisitor> visitor ( 
    OsgTools::MakeVisitor<osg::Geode>::make ( 
    Usul::Adaptors::memberFunction ( this, &Viewer::_setDisplayListsGeode ) ) );

  // Visit the scene.
  this->model()->accept ( *visitor );

  // If we are not using display lists, and we have been created...
  if ( false == _useDisplayList.first )
  {
    // Delete all display-lists associated with our context id.
    this->viewer()->releaseAllGLObjects();
    this->viewer()->flushAllDeletedGLObjects();
  }

  // No longer dirty.
  _useDisplayList.second = false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return if display lists are being used.
//
///////////////////////////////////////////////////////////////////////////////

bool Viewer::useDisplayLists() const
{
  return _useDisplayList.first;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return if display lists are being used.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::useDisplayLists ( bool state )
{
  _useDisplayList.first = state;
  _useDisplayList.second = true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set all the geode's drawables to the proper display-list state.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::_setDisplayListsGeode ( osg::Geode *geode )
{
  for ( unsigned int i = 0; i < geode->getNumDrawables(); ++i )
  {
    osg::Drawable *drawable = geode->getDrawable ( i );
    drawable->setUseDisplayList ( this->useDisplayLists() );
    drawable->dirtyDisplayList();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the number of rendering passes. Unavailable numbers have no effect.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::numRenderPasses ( unsigned int num )
{
  _renderer->setNumRenderPasses( num );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the number of rendering passes.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Viewer::numRenderPasses() const
{
  return _renderer->getNumRenderPasses();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the number of rendering passes. Unavailable numbers have no effect.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::renderingPasses ( unsigned int num )
{
  this->numRenderPasses( num );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the number of rendering passes.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Viewer::renderingPasses() const
{
  return this->numRenderPasses();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Toggle the hidden lines.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::toggleHiddenLines()
{
  if ( this->hasHiddenLines() )
    this->removeHiddenLines();
  else
    this->setHiddenLines();
}


///////////////////////////////////////////////////////////////////////////////
//
//  See if there are hidden lines.
//
///////////////////////////////////////////////////////////////////////////////

bool Viewer::hasHiddenLines() const
{
  return Usul::Bits::has < unsigned int, unsigned int > ( _flags, _HIDDEN_LINES );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove any hidden lines.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::removeHiddenLines()
{
  _flags = Usul::Bits::remove < unsigned int, unsigned int > ( _flags, _HIDDEN_LINES );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the state for hidden lines.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::setHiddenLines()
{
  // Make sure there is no polygon mode in the viewer's global-state-set.
  this->removePolygonMode();

  // Set the flag.
  _flags = Usul::Bits::add < unsigned int, unsigned int >( _flags, _HIDDEN_LINES );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a clipping plane to the scene.
//
///////////////////////////////////////////////////////////////////////////////

osg::ClipPlane* Viewer::addPlane()
{
  // Get the bounding box
  osg::BoundingBox bb ( this->getBoundingBox() );

  // Initial points for the plane.
  osg::Vec3 top_left     ( bb.corner( 1 ) );
  osg::Vec3 bottom_left  ( bb.corner( 0 ) );
  osg::Vec3 top_right    ( bb.corner( 7 ) );

  // Define the plane on the diagonal of the bounding box
  osg::Plane plane ( top_left, bottom_left, top_right );
  return this->addPlane ( plane );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a clipping plane to the scene.
//
///////////////////////////////////////////////////////////////////////////////

osg::ClipPlane* Viewer::addPlane ( const osg::Plane& plane, bool widget )
{
  //Make the clipping plane
  osg::ref_ptr< osg::ClipPlane > clipPlane ( new osg::ClipPlane( _sceneManager->clipNode()->getNumClipPlanes(), plane ) );

  if( widget )
  {
    // Get the bounding box
    osg::BoundingBox bb ( this->getBoundingBox() );

    osg::ref_ptr < OsgTools::Widgets::ClipPlane > widget ( new OsgTools::Widgets::ClipPlane ( bb, clipPlane.get() ) );

    osg::ref_ptr< osg::Group > group ( _sceneManager->groupGet ( OsgTools::Render::Constants::CLIPPING_PLANES ) );
    group->addChild ( widget.get() );
    _clipPlaneWidgets.insert ( ClipPlaneWidgets::value_type (  clipPlane->getClipPlaneNum(), widget.get() ) );
  }

  // Add the plane
  _sceneManager->clipNode()->addClipPlane ( clipPlane.get() );

  this->changedScene();

  return clipPlane.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove a clipping plane from the scene
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::removePlane ( unsigned int index )
{
  osg::ref_ptr<osg::ClipNode> clipNode ( ( true == _sceneManager.valid() ) ? _sceneManager->clipNode() : 0x0 );
  if ( true == clipNode.valid() )
  {
    osg::ref_ptr < osg::ClipPlane > clipPlane ( clipNode->getClipPlane ( index ) );
    if ( true == clipPlane.valid() )
    {
      this->removePlane ( clipPlane.get() );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove a clipping plane from the scene
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::removePlane ( osg::ClipPlane *plane )
{
  if ( 0x0 == plane )
    return;

  // Get the index.
  unsigned int index ( plane->getClipPlaneNum() );

  // Remove the plane from the clip node.
  _sceneManager->clipNode()->removeClipPlane ( plane );

  // Remove the widget.
  osg::ref_ptr< osg::Group > group ( _sceneManager->groupGet ( OsgTools::Render::Constants::CLIPPING_PLANES ) );

  ClipPlaneWidget::Ptr widget ( _clipPlaneWidgets[index] );
  _clipPlaneWidgets.erase ( index );
  group->removeChild ( widget.get() );

  this->changedScene();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove all clipping planes
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::removePlanes()
{
  typedef osg::ClipNode::ClipPlaneList ClipPlaneList;
  ClipPlaneList cliplist ( _sceneManager->clipNode()->getClipPlaneList() );
  for ( ClipPlaneList::iterator i = cliplist.begin(); i != cliplist.end(); ++i )
    this->removePlane ( i->get() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the number of clipping planes in the scene
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Viewer::planes() const
{
  return _sceneManager->clipNode()->getNumClipPlanes();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *Viewer::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::IViewMatrix::IID:
    return static_cast < Usul::Interfaces::IViewMatrix* > ( this );
  case Usul::Interfaces::IShadeModel::IID:
    return static_cast < Usul::Interfaces::IShadeModel* > ( this );
  case Usul::Interfaces::IPolygonMode::IID:
    return static_cast < Usul::Interfaces::IPolygonMode* > ( this );
  case Usul::Interfaces::IExportImage::IID:
    return static_cast < Usul::Interfaces::IExportImage* > ( this );
  case Usul::Interfaces::IExportModel::IID:
    return static_cast < Usul::Interfaces::IExportModel* > ( this );
  case Usul::Interfaces::IExportScene::IID:
    return static_cast < Usul::Interfaces::IExportScene* > ( this );
  case Usul::Interfaces::IOpenSceneGraph::IID:
    return static_cast<Usul::Interfaces::IOpenSceneGraph*>(this);
  case Usul::Interfaces::IFrameDump::IID:
    return static_cast<Usul::Interfaces::IFrameDump*>(this);
  case Usul::Interfaces::ITextMatrix::IID:
    return static_cast<Usul::Interfaces::ITextMatrix*>(this);
  case Usul::Interfaces::IGetDocument::IID:
    return static_cast<Usul::Interfaces::IGetDocument*>(this);
  case Usul::Interfaces::IGroup::IID:
    return static_cast < Usul::Interfaces::IGroup* > ( this );
  case Usul::Interfaces::IClippingPlanes::IID:
    return static_cast < Usul::Interfaces::IClippingPlanes* > ( this );
  case Usul::Interfaces::IGetBoundingBox::IID:
    return static_cast < Usul::Interfaces::IGetBoundingBox* > ( this );
  case Usul::Interfaces::ICamera::IID:
    return static_cast < Usul::Interfaces::ICamera* > ( this );
  case Usul::Interfaces::ISceneIntersect::IID:
    return static_cast<Usul::Interfaces::ISceneIntersect*>(this);
  case Usul::Interfaces::ITrackball::IID:
    return static_cast<Usul::Interfaces::ITrackball*>(this);
  case Usul::Interfaces::IRedraw::IID:
    return static_cast<Usul::Interfaces::IRedraw*>(this);
  case Usul::Interfaces::ISpin::IID:
    return static_cast < Usul::Interfaces::ISpin* > ( this );
  case Usul::Interfaces::IScreenCapture::IID:
    return static_cast < Usul::Interfaces::IScreenCapture * > ( this );
  case Usul::Interfaces::ISnapShot::IID:
    return static_cast < Usul::Interfaces::ISnapShot* > ( this );
  case Usul::Interfaces::IView::IID:
    return static_cast < Usul::Interfaces::IView * > ( this );
  case Usul::Interfaces::IRenderNotify::IID:
    return static_cast < Usul::Interfaces::IRenderNotify * > ( this );
  case Usul::Interfaces::IIntersectNotify::IID:
    return static_cast < Usul::Interfaces::IIntersectNotify * > ( this );
  case Usul::Interfaces::IFrameStamp::IID:
    return static_cast < Usul::Interfaces::IFrameStamp * > ( this );
  case Usul::Interfaces::IUpdateSceneVisitor::IID:
    return static_cast < Usul::Interfaces::IUpdateSceneVisitor * > ( this );
  case Usul::Interfaces::ICullSceneVisitor::IID:
    return static_cast < Usul::Interfaces::ICullSceneVisitor * > ( this );
  case Usul::Interfaces::IUpdateSubject::IID:
    return static_cast < Usul::Interfaces::IUpdateSubject * > ( this );
  case Usul::Interfaces::IClippingDistance::IID:
    return static_cast < Usul::Interfaces::IClippingDistance * > ( this );
  case Usul::Interfaces::IViewport::IID:
    return static_cast < Usul::Interfaces::IViewport * > ( this );
  case Usul::Interfaces::IRenderLoop::IID:
    return static_cast < Usul::Interfaces::IRenderLoop * > ( this );
  case Usul::Interfaces::IRenderingPasses::IID:
    return static_cast < Usul::Interfaces::IRenderingPasses * > ( this );
  case Usul::Interfaces::IAxes::IID:
    return static_cast < Usul::Interfaces::IAxes * > ( this );
  case Usul::Interfaces::IMouseEventSubject::IID:
    return static_cast < Usul::Interfaces::IMouseEventSubject* > ( this );
	case Usul::Interfaces::IModelsScene::IID:
		return static_cast< Usul::Interfaces::IModelsScene* > ( this );
  case Usul::Interfaces::IRenderInfoOSG::IID:
    return static_cast < Usul::Interfaces::IRenderInfoOSG* > ( this );
  case Usul::Interfaces::IProjectionMatrix::IID:
    return static_cast<Usul::Interfaces::IProjectionMatrix*> ( this );
  case Usul::Interfaces::IViewMode::IID:
    return static_cast<Usul::Interfaces::IViewMode*> ( this );
  default:
    return ( _caller.valid() ? _caller->queryInterface ( iid ) : 0x0 );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get sort back to front state.
//
///////////////////////////////////////////////////////////////////////////////

bool Viewer::sortBackToFront() const
{
  return Usul::Bits::has < unsigned int, unsigned int > ( _flags, _SORT_BACK_TO_FRONT );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set sort back to front state.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::sortBackToFront ( bool b )
{
  if ( b )
  {
    _flags = Usul::Bits::add < unsigned int, unsigned int > ( _flags, _SORT_BACK_TO_FRONT );
    osg::ref_ptr< OsgTools::Callbacks::SetSortToFrontCallback > visitor ( new OsgTools::Callbacks::SetSortToFrontCallback );
    this->model()->accept ( *visitor );
  }
  else
  {
    _flags = Usul::Bits::remove < unsigned int, unsigned int > ( _flags, _SORT_BACK_TO_FRONT );
    osg::ref_ptr< OsgTools::Callbacks::UnSetSortToFrontCallback > visitor ( new OsgTools::Callbacks::UnSetSortToFrontCallback );
    this->model()->accept ( *visitor );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the model.
//
///////////////////////////////////////////////////////////////////////////////

const osg::Node* Viewer::model() const
{
  return _sceneManager->model();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the model.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* Viewer::model()
{
  return _sceneManager->model();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add axes to the scene.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::_addAxes()
{
  // Get the group.  It will be under a projection node that is set which each resize, so the geometry under it
  // won't get distorted by screen resizes.
  osg::ref_ptr< osg::Group > group ( _sceneManager->projectionGroupGet ( OsgTools::Render::Constants::AXES ) );

  // Axes Dragger
  osg::ref_ptr < OsgTools::Widgets::Axes > axes ( new OsgTools::Widgets::Axes );
  axes->setMatrix ( osg::Matrix::translate ( osg::Vec3 ( 50, 50, -100 ) ) );
  axes->setReferenceFrame ( osg::Transform::ABSOLUTE_RF );

  group->addChild ( axes.get() );
  
  // The scene has changed.
  this->changedScene();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove the axes.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::_removeAxes()
{
  _sceneManager->projectionGroupRemove ( OsgTools::Render::Constants::AXES );

  // The scene has changed.
  this->changedScene();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the rotation of the axes.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::_setAxes()
{
  // Return now if no axes
  if( !this->isAxesShown() )
    return;

  osg::ref_ptr< osg::Group > group ( _sceneManager->projectionGroupGet ( OsgTools::Render::Constants::AXES ) );
  osg::ref_ptr< osg::MatrixTransform > mt ( dynamic_cast < osg::MatrixTransform* > (  group->getChild( 0 ) ) );

  // Return if we don't have a valid matrix transform.
  if( !mt.valid() )
    return;

  // Get the translation of the matrix transform.
  osg::Matrix trans;
  trans.setTrans ( mt->getMatrix().getTrans() );

  // Matrix to capture the rotation of the view matrix.
  osg::Matrix mat;
  mat.makeIdentity();

  // Get the view matrix.
  osg::Matrix view ( this->viewer()->getViewMatrix() );

  osg::Matrix::value_type *ptr = mat.ptr();
  osg::Matrix::value_type *vptr = view.ptr();

  // Get the view matrix's rotation.  Assuming no scaling.
  ptr[0] = vptr[0];
  ptr[1] = vptr[1];
  ptr[2] = vptr[2];

  ptr[4] = vptr[4];
  ptr[5] = vptr[5];
  ptr[6] = vptr[6];

  ptr[8] = vptr[8];
  ptr[9] = vptr[9];
  ptr[10] = vptr[10];

  mt->setMatrix( mat * trans );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the state of showing axes
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::axesShown ( bool b )
{
  if ( b )
  {
    _flags = Usul::Bits::add < unsigned int, unsigned int > ( _flags, _SHOW_AXES );
    this->_addAxes();
  }
  else
  {
    _flags = Usul::Bits::remove < unsigned int, unsigned int > ( _flags, _SHOW_AXES );
    this->_removeAxes();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the state of showing text
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::textShown ( bool b )
{
  if ( b )
  {
    _flags = Usul::Bits::add < unsigned int, unsigned int > ( _flags, _SHOW_TEXT );
    _sceneManager->projectionGroupGet ( OsgTools::Render::Constants::TEXT_MATRIX )->setNodeMask ( 0xFFFFFFFF );
  }
  else
  {
    _flags = Usul::Bits::remove < unsigned int, unsigned int > ( _flags, _SHOW_TEXT );
    _sceneManager->projectionGroupGet ( OsgTools::Render::Constants::TEXT_MATRIX )->setNodeMask ( 0 );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the show axes state.
//
///////////////////////////////////////////////////////////////////////////////

bool Viewer::isAxesShown() const
{
  return Usul::Bits::has < unsigned int, unsigned int > ( _flags, _SHOW_AXES );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the show text state.
//
///////////////////////////////////////////////////////////////////////////////

bool Viewer::isTextShown() const
{
  return Usul::Bits::has < unsigned int, unsigned int > ( _flags, _SHOW_TEXT );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the axes label (IAxes).
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::axesLabels ( const std::string& x, const std::string& y, const std::string& z )
{
  osg::ref_ptr< osg::Group > group ( _sceneManager->projectionGroupGet ( OsgTools::Render::Constants::AXES ) );

  // See if it's an Axes (it should be).
  if( OsgTools::Widgets::Axes *axes = dynamic_cast < OsgTools::Widgets::Axes* > ( group->getChild( 0 ) ) )
  {
    axes->xText ( x );
    axes->yText ( y );
    axes->zText ( z );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the view matrix.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::setViewMatrix ( const osg::Matrixd& matrix )
{
  this->viewer()->setViewMatrix ( matrix );
  this->navManip()->setByInverseMatrix ( this->getViewMatrix() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the view matrix.
//
///////////////////////////////////////////////////////////////////////////////

osg::Matrixd Viewer::getViewMatrix() const
{
  return this->viewer()->getViewMatrix();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the shade model.  Usul::Interfaces::IShadeModel
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::shadeModel ( IShadeModel::Mode mode )
{
  if( mode == IShadeModel::NONE )
    this->removeShadeModel();
  else
  {
    this->toggleShadeModel ( OsgTools::State::ShadeModel::mode( mode ) );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the shade model.  Usul::Interfaces::IShadeModel
//
///////////////////////////////////////////////////////////////////////////////


Viewer::IShadeModel::Mode Viewer::shadeModel() const
{
  if ( this->hasShadeModel ( osg::ShadeModel::FLAT ) )
    return IShadeModel::FLAT;
  
  if ( this->hasShadeModel ( osg::ShadeModel::SMOOTH ) )
    return IShadeModel::SMOOTH;

  return IShadeModel::NONE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the Polygon Mode.  Usul::Interfaces::IPolygonMode
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::polygonMode ( IPolygonMode::Mode mode )
{
  if( this->hasHiddenLines() )
      this->removeHiddenLines();

  if( IPolygonMode::NONE == mode )
  {
    this->removePolygonMode();
  }
  else if ( IPolygonMode::HIDDEN_LINES == mode )
  {
    this->setHiddenLines();
  }
  else
  {
    this->setPolygonMode ( osg::PolygonMode::FRONT_AND_BACK, OsgTools::State::PolygonMode::mode( mode ) );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the Polygon Mode.  Usul::Interfaces::IPolygonMode
//
///////////////////////////////////////////////////////////////////////////////

Viewer::IPolygonMode::Mode Viewer::polygonMode() const
{
  if( this->hasHiddenLines() )
    return IPolygonMode::HIDDEN_LINES;

  if ( this->hasPolygonMode ( osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::POINT ) )
    return IPolygonMode::POINTS;

  if ( this->hasPolygonMode ( osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE ) )
    return IPolygonMode::WIRE_FRAME;

  if ( this->hasPolygonMode ( osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::FILL ) )
    return IPolygonMode::FILLED;

  return IPolygonMode::NONE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the scene as a referenced.
//
///////////////////////////////////////////////////////////////////////////////

osg::Referenced *Viewer::osgReferenced()
{
  return this->scene();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the scene as a referenced.
//
///////////////////////////////////////////////////////////////////////////////

const osg::Referenced *Viewer::osgReferenced() const
{
  return this->scene();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is stereo turned on?
//
///////////////////////////////////////////////////////////////////////////////

bool Viewer::stereo() const
{
  return this->viewer()->getDisplaySettings()->getStereo();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Turn on or off stereo
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::stereo( bool b )
{
  this->viewer()->getDisplaySettings()->setStereo( b );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Change the stereo mode
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::stereoMode ( unsigned int m )
{
  this->viewer()->getDisplaySettings()->setStereoMode ( osg::DisplaySettings::StereoMode ( m ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the stereo mode
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Viewer::stereoMode() const
{
  return this->viewer()->getDisplaySettings()->getStereoMode();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Change the eye distance
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::stereoEyeDistance ( float s)
{
  this->viewer()->getDisplaySettings()->setEyeSeparation( s ) ;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the eye distance
//
///////////////////////////////////////////////////////////////////////////////

float Viewer::stereoEyeDistance ( ) const
{
  return this->viewer()->getDisplaySettings()->getEyeSeparation( ) ;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set properties for dumping frames.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::frameDumpProperties ( const std::string &dir, 
                                   const std::string &base, 
                                   const std::string &ext, 
                                   unsigned int start, 
                                   unsigned int digits )
{
  const Usul::Interfaces::IFrameDump::DumpState dump ( this->getFrameDumpState() );
  FrameDump fd ( dump, dir, base, ext, start, digits );
  this->frameDump ( fd );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Turn on/off frame-dumping.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::setFrameDumpState ( Usul::Interfaces::IFrameDump::DumpState state )
{
  this->frameDump().dump ( state );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Are we dumping frames?
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IFrameDump::DumpState Viewer::getFrameDumpState() const
{
  return this->frameDump().dump();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Reset the file name counter.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::resetFrameDumpCounter()
{
  this->frameDump().reset();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the current file number
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Viewer::currentFile() const
{
  return this->frameDump().current();
}



///////////////////////////////////////////////////////////////////////////////
//
//  Get the filenames that were written out.
//
///////////////////////////////////////////////////////////////////////////////

Viewer::Filenames Viewer::filenames() const
{
  Filenames names;
  if ( 0x0 != this->frameDump().collection() )
    names = this->frameDump().collection()->value();
  return names;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Should the filenames be saved?
//
///////////////////////////////////////////////////////////////////////////////

bool Viewer::saveNames() const
{
  return ( 0x0 != this->frameDump().collection() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Should the filenames be saved?
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::saveNames ( bool state )
{
  if ( true == state )
  {
    FrameDump::Collection::RefPtr collection ( this->frameDump().collection() );
    if ( false == collection.valid() )
      this->frameDump().collection ( new FrameDump::Collection() );
  }
  else
  {
    this->frameDump().collection ( 0x0 );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Usul::Interfaces::IGetDocument::getDocument()
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *Viewer::getDocument()
{
  return ( this->document() ) ? this->document()->queryInterface ( Usul::Interfaces::IUnknown::IID ) : 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get group with given key. Creates one if doesn't exist
//
///////////////////////////////////////////////////////////////////////////////

osg::Group* Viewer::getGroup ( const std::string& key )
{
  return _sceneManager->groupGet ( key );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove group with given key
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::removeGroup ( const std::string& key )
{
  _sceneManager->groupRemove ( key );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is the group created?
//
///////////////////////////////////////////////////////////////////////////////

bool Viewer::hasGroup ( const std::string& key )
{
  return _sceneManager->groupHas ( key );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the bounding box.
//
///////////////////////////////////////////////////////////////////////////////

osg::BoundingBox Viewer::getBoundingBox() const
{
  // See if the document returns a bounding box.
  Usul::Interfaces::IGetBoundingBox::QueryPtr getBB ( const_cast<Document*> ( this->document() ) );

  // Get the scene's bounding box.
  osg::BoundingBox bb;
  bb.expandBy ( this->model()->getBound() );

  // Return the right one.
  return getBB.valid() ? getBB->getBoundingBox() : bb;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the location of the viewport.
//
///////////////////////////////////////////////////////////////////////////////

double Viewer::x() const
{
  return this->viewer()->getViewport()->x();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the location of the viewport.
//
///////////////////////////////////////////////////////////////////////////////

double Viewer::y() const
{
  return this->viewer()->getViewport()->y();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get height
//
///////////////////////////////////////////////////////////////////////////////

double Viewer::height() const
{
  return Usul::Math::absolute ( this->viewer()->getViewport()->height() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get width
//
///////////////////////////////////////////////////////////////////////////////

double Viewer::width() const
{
  return Usul::Math::absolute ( this->viewer()->getViewport()->width() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the matrix manipulator.
//
///////////////////////////////////////////////////////////////////////////////

const Viewer::MatrixManip * Viewer::navManip() const 
{ 
  return _navManip.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the matrix manipulator.
//
///////////////////////////////////////////////////////////////////////////////

Viewer::MatrixManip * Viewer::navManip()      
{ 
  return _navManip.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the matrix manipulator.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::navManip ( MatrixManip *manip  )
{
  _navManip = manip;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the line segment into the scene that goes through the 2D coordinate.
//
///////////////////////////////////////////////////////////////////////////////

bool Viewer::_lineSegment ( float mouseX, float mouseY, osg::Vec3d &pt0, osg::Vec3d &pt1, bool useWindowCoords )
{
  // Handle no viewer.
  if ( !this->viewer() )
    return false;

  // Declare the event adapter.
  EventAdapter::Ptr ea ( new EventAdapter );
  ea->setWindowSize ( Usul::Math::Vec2ui( static_cast < unsigned int > ( this->width() ), static_cast < unsigned int > ( this->height() ) ) );
  ea->setMouse ( Usul::Math::Vec2f( mouseX, mouseY ) );

  // Get the necessary coordinates.
  const double x ( ea->getXnormalized() );
  const double y ( ea->getYnormalized() );

  // Use window coordinates.  No projection.
  if ( useWindowCoords )
  {
    // Set the two points for our line-segment.
    pt0 = osg::Vec3d ( x, y, -1 ) * osg::Matrix::inverse( _sceneManager->camera()->getProjectionMatrix() );
    pt1 = osg::Vec3d ( x, y,  1 ) * osg::Matrix::inverse( _sceneManager->camera()->getProjectionMatrix() );
  }

  // Project into the scene.
  else
  {
    // Get the matrix.
    osg::Matrix M ( this->viewer()->getViewMatrix() );
    osg::Matrix P ( this->viewer()->getProjectionMatrix() );
    osg::Matrix MP ( M * P );
    osg::Matrix IMP ( osg::Matrix::inverse ( MP ) );

    // Calculate the two points for our line-segment.
    pt0 = osg::Vec3d ( x, y, -1 ) * IMP;
    pt1 = osg::Vec3d ( x, y,  1 ) * IMP;
  }

  // Are the numbers valid?
  const bool valid ( ( false == Usul::Math::nan ( pt0 ) ) && ( false == Usul::Math::nan ( pt1 ) ) );

  // It worked if the values are valid.
  return valid;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Intersect the scene.
//
///////////////////////////////////////////////////////////////////////////////

bool Viewer::_intersect ( float x, float y, osg::Node *scene, osgUtil::LineSegmentIntersector::Intersection &hit, bool useWindowCoords )
{
  _pointerInfo.reset();

  // Get the hit-list for our line-segment.
  typedef osgUtil::LineSegmentIntersector::Intersections Intersections;
  Intersections hits;
  this->_intersect ( x, y, scene, hits, useWindowCoords );
  if ( hits.empty() )
    return false;

  // Set the hit.
  hit = *hits.begin();

  for ( Intersections::const_iterator iter = hits.begin(); iter != hits.end(); ++iter )
    _pointerInfo.addIntersection ( iter->nodePath, iter->getLocalIntersectPoint() );

  // Not sure what this is for...
  _pointerInfo._hitIter = _pointerInfo._hitList.begin();

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Intersect the scene.
//
///////////////////////////////////////////////////////////////////////////////

bool Viewer::_intersect ( float x, float y, osg::Node *scene, osgUtil::LineSegmentIntersector::Intersections &intersections, bool useWindowCoords )
{
  // Handle no scene or viewer.
  if ( !scene || !this->viewer() )
    return false;
  
  // Calculate the two points for our line-segment.
  osg::Vec3d pt0, pt1;
  if ( !this->_lineSegment ( x, y, pt0, pt1, useWindowCoords ) )
    return false;
  
  // Make the intersector.
  osg::ref_ptr<osgUtil::LineSegmentIntersector> intersector ( new osgUtil::LineSegmentIntersector ( pt0, pt1 ) );
  
  // Declare the pick-visitor.
  typedef osgUtil::IntersectionVisitor Visitor;
  osg::ref_ptr<Visitor> visitor ( new Visitor );
  visitor->setIntersector ( intersector.get() );
  
  // Intersect the scene.
  scene->accept ( *visitor );
  
  // Get the hit-list for our line-segment.
  intersections = intersector->getIntersections();
  if ( intersections.empty() )
    return false;
  
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  See if event intersects the current scene
//
///////////////////////////////////////////////////////////////////////////////

bool Viewer::intersect ( float x, float y, osgUtil::LineSegmentIntersector::Intersection &hit ) 
{ 
  return this->_intersect ( x, y, this->model(), hit ); 
}


///////////////////////////////////////////////////////////////////////////////
//
//  See if event intersects the current scene
//
///////////////////////////////////////////////////////////////////////////////

bool Viewer::intersect ( float x, float y, osgUtil::LineSegmentIntersector::Intersections &intersections )
{
  return this->_intersect ( x, y, this->model(), intersections );
}


///////////////////////////////////////////////////////////////////////////////
//
//  See if event intersects the current scene
//
///////////////////////////////////////////////////////////////////////////////

bool Viewer::intersect ( double x, double y, osgUtil::PolytopeIntersector::Intersection &intersection, double w, double h, unsigned int dimensionMask )
{
  typedef osgUtil::PolytopeIntersector::Intersections Intersections;

  // Get all the intersections.
  Intersections intersections;
  this->intersect ( x, y, intersections, w, h, dimensionMask );

  // Return now if no intersections.
  if ( intersections.empty() )
    return false;

  // Set the intersection.
  intersection = *intersections.begin();
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  See if event intersects the current scene.
//  Note: Could not get this to work from MinervaDocument::_intersectScene().
//
///////////////////////////////////////////////////////////////////////////////

bool Viewer::intersect ( double x, double y, osgUtil::PolytopeIntersector::Intersections &intersections, double w, double h, unsigned int dimensionMask )
{
  // Get the scene and viewer.
  osg::ref_ptr<osg::Node> scene ( 0x0 );
  osg::ref_ptr<osgUtil::SceneView> viewer ( 0x0 );
  {
    Guard guard ( this );
    scene = this->scene();
    viewer = this->viewer();
  }

  // Handle no scene or viewer.
  if ( ( false == scene.valid() ) || ( false == viewer.valid() ) )
    return false;

  // Declare the event adapter.
  EventAdapter::Ptr ea ( new EventAdapter );
  ea->setWindowSize ( Usul::Math::Vec2ui ( static_cast < unsigned int > ( this->width() ), static_cast < unsigned int > ( this->height() ) ) );
  ea->setMouse ( Usul::Math::Vec2f ( static_cast < float > ( x ), static_cast < float > ( y ) ) );

  // Adjust the coordinates.
  x = ea->getXnormalized();
  y = ea->getYnormalized();

  // Make the intersector.
  osg::ref_ptr<osgUtil::PolytopeIntersector> intersector 
    ( new osgUtil::PolytopeIntersector ( osgUtil::Intersector::PROJECTION, x - w, y - h, x + w, y + h ) );

  // Intersect with everything.
  intersector->setDimensionMask ( dimensionMask );

  // Declare the visitor.
  osgUtil::IntersectionVisitor visitor ( intersector.get() );
  
  // Intersect the scene.
  scene->accept ( visitor );
  
  // Get the hit-list for our line-segment.
  intersections = intersector->getIntersections();
  if ( intersections.empty() )
    return false;
  
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the trackball's center.
//
///////////////////////////////////////////////////////////////////////////////

osg::Vec3d Viewer::getCenter()
{
  return this->_trackball()->center();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the trackball's center.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::setCenter( const osg::Vec3d &c )
{
  this->setTrackball( c, this->getDistance(), this->getRotation(), true, true );
}

///////////////////////////////////////////////////////////////////////////////
//
//  Get the trackball's distance.
//
///////////////////////////////////////////////////////////////////////////////

double Viewer::getDistance()
{
  return this->_trackball()->distance(); 
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the trackball's distance.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::setDistance( double d )
{
  this->setTrackball( this->getCenter(), d, this->getRotation(), true, true );
}

///////////////////////////////////////////////////////////////////////////////
//
//  Get the trackball's rotation
//
///////////////////////////////////////////////////////////////////////////////

osg::Quat Viewer::getRotation()
{
  return this->_trackball()->rotation(); 
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the trackball's rotation.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::setRotation( const osg::Quat &r )
{
  this->setTrackball( this->getCenter(), this->getDistance(), r, true, true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the trackball with the given values
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::setTrackball ( const osg::Vec3d& center, double distance, const osg::Quat& rot, bool makeTrackball, bool setViewerToo )
{
  if ( !this->viewer() )
    return;

  // Only make a trackball if we don't have a manipulator of any kind.
  if ( 0x0 == this->navManip() && makeTrackball )
  {
    this->navManip ( new Trackball );
  }

  // Make sure we have a trackball.
  Trackball::Ptr trackball ( dynamic_cast < Trackball * > ( this->navManip() ) );

  if ( false == trackball.valid() )
    return;

  // Set the trackball.
  trackball->center ( center );
  trackball->distance ( distance );
  trackball->rotation ( rot );

  // Set the viewer's matrix.
  if ( setViewerToo )
    this->viewer()->setViewMatrix ( trackball->getInverseMatrix() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the trackball pointer
//
///////////////////////////////////////////////////////////////////////////////

OsgTools::Render::Trackball* Viewer::_trackball()
{
  Trackball::Ptr trackball ( dynamic_cast < Trackball * > ( this->navManip( ) ) );
  if ( trackball.valid() ) 
    return trackball.get(); 
  throw std::runtime_error ( "Error 3608089520: Trackball is not valid." );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Edit Material above hit, if it exists
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::_editMaterial ( osgUtil::LineSegmentIntersector::Intersection &hit )
{
  //First check the drawable's state set
  osg::ref_ptr < osg::StateSet > stateset ( hit.drawable->getOrCreateStateSet() );
  osg::ref_ptr < osg::Material > mat ( reinterpret_cast < osg::Material* > ( stateset->getAttribute( osg::StateAttribute::MATERIAL ) ) );

  // If no material...
  if ( !mat.valid() )
  {
    osg::NodePath path ( hit.nodePath );

    // Search up the node list
    for ( osg::NodePath::reverse_iterator i = path.rbegin(); i != path.rend(); ++i )
    {
      osg::ref_ptr < osg::StateSet > ss ( (*i)->getOrCreateStateSet() );
      mat = ( reinterpret_cast < osg::Material* > ( ss->getAttribute( osg::StateAttribute::MATERIAL ) ) );
      if( mat.valid() )
      {
        stateset = ss;
        break;
      }
    }
  }

  // Still if no material, make one
  if( !mat.valid() )
  {
    mat = new osg::Material;
    stateset->setAttribute( mat.get() );
  }

  typedef Usul::Interfaces::IMaterialEditor Editor;
  typedef Usul::Components::Manager Manager;
  Editor::QueryPtr editor ( Manager::instance().getInterface ( Editor::IID ) );

  if( editor.valid() )
  {
    editor->setCurrentMaterial ( mat.get() );
    editor->runModalDialog();

    // Set the proper attributes for transparency or opaque
    if ( mat->getAmbient( osg::Material::FRONT )[3] < 255.0 || 
         mat->getDiffuse( osg::Material::FRONT )[3] < 255.0 || 
         mat->getSpecular( osg::Material::FRONT )[3] < 255.0 || 
         mat->getEmission( osg::Material::FRONT )[3] < 255.0 )
    {
      stateset->setMode ( GL_BLEND,      osg::StateAttribute::ON  );
      stateset->setMode ( GL_DEPTH_TEST, osg::StateAttribute::ON );
      stateset->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );
    }
    else
    {
      stateset->setMode ( GL_BLEND,      osg::StateAttribute::OFF );
      stateset->setMode ( GL_DEPTH_TEST, osg::StateAttribute::ON  );
      stateset->setRenderingHint( osg::StateSet::DEFAULT_BIN );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Usul::Interfaces::IRedraw::redraw()
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::redraw()
{
  this->render();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Usul::Interfaces::IRedraw::setStatsDisplay()
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::setStatsDisplay ( bool b )
{
  this->updateTimes ( b );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Find the deepest dragger if there is one (they may be nested) and set 
//  it as the dragger being dragged.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::_findDragger ( const osgUtil::LineSegmentIntersector::Intersection &hit )
{
  // Initialize.
  Dragger *dragger = 0x0;
  osg::NodePath path ( hit.nodePath );

  // Loop through the nodes.
  for ( osg::NodePath::reverse_iterator i = path.rbegin(); i != path.rend(); ++i )
  {
    // See if it is a dragger.
    dragger = dynamic_cast < Dragger * > ( *i );
    if ( dragger )
      break;
  }

  _activeDragger = dragger;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the current mode
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::setViewMode ( ViewMode mode ) 
{ 
  _currentMode = mode;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the current mode
//
///////////////////////////////////////////////////////////////////////////////

Viewer::ViewMode Viewer::getViewMode () const
{
  return _currentMode;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is the mode equal to our current mode.
//
///////////////////////////////////////////////////////////////////////////////

bool Viewer::isViewModeCurrent ( ViewMode mode ) const
{
  return this->getViewMode() == mode;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Cycle through the modes
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::cycleMode()
{
  switch ( _currentMode )
  {
  case NAVIGATION:
    _currentMode = PICK;
    break;
  case PICK:
    _currentMode = NAVIGATION;
    break;
  default:
    _currentMode = NAVIGATION;
    break;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when a mouse button is pressed.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::buttonPress ( EventAdapter *ea )
{
  // Stop any spin.
  this->spin ( false );

  // Set the lod-callbacks if we are suppose to.
  if ( this->useLowLodsGet() )
    this->_setLodCullCallback ( new OsgTools::Render::LowLodCallback );

  // Handle the navigation event.
  this->_handleNavigation ( ea );

  // Does nothing unless in seek mode.
  this->_handleSeek ( ea );

  // Handle the events. Make sure you pick before you drag.
  this->_handlePicking  ( ea );
  this->_handleDragging ( ea );

  // Notify of the mouse event.
  this->_mouseEventNotify ( ea );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when a mouse button is released.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::buttonRelease ( EventAdapter *ea )
{
  // Set the lod-callbacks.
  this->_setLodCullCallback ( 0x0 );
  //this->_setLodCullCallback ( new Detail::HighLodCallback );

  // Handle the event.
  this->_handleNavigation ( ea );

  // Handle the events. Make sure you pick before you drag.
  this->_handlePicking  ( ea );
  this->_handleDragging ( ea );

  // Notify of the mouse event.
  this->_mouseEventNotify ( ea );
}


///////////////////////////////////////////////////////////////////////////////
//
//  The mouse has moved.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::mouseMove ( EventAdapter *ea )
{
  // Handle the events. Make sure you pick before you drag.
  this->_handleNavigation ( ea );
  this->_handlePicking    ( ea );
  this->_handleDragging   ( ea );
  this->_handleIntersect  ( ea );

  // Notify of the mouse event.
  this->_mouseEventNotify ( ea );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create an event adaptor.
//
///////////////////////////////////////////////////////////////////////////////

EventAdapter * Viewer:: eventAdaptor ( float x, float y, bool left, bool middle, bool right, EventAdapter::EventType type )
{
  EventAdapter::Ptr ea ( new EventAdapter );

  // Set the event-adapter. Order is important.
  ea->setSeconds ( Usul::System::Clock::seconds() );
  ea->setWindowSize ( Usul::Math::Vec2ui( static_cast < unsigned int > ( this->width() ), static_cast < unsigned int > ( this->height() ) ) );
  ea->setMouse ( Usul::Math::Vec2f ( x, y ) );

  ea->setButton ( left, middle, right );
  ea->setButtonMask ( left, middle, right );
  ea->setEventType ( type );

  return ea.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Handle the navigation event.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::_handleNavigation ( EventAdapter *ea )
{
  // Make sure we are in navigation mode.
  if ( !this->navigating() )
    return;

  // Handle no manipulator, scene, or viewer.
  if ( !this->navManip() || !this->scene() || !this->viewer() || 0x0 == ea )
    return;

  // Declare the adapters.
  ActionAdapter aa ( this->queryInterface( Usul::Interfaces::IUnknown::IID ) );

  EventAdapter::EventType type ( ea->getEventType() );

  // Ask the manipulator to handle the event.
  if ( this->navManip()->handle ( *ea, aa ) )
  {
    // Set the scene's matrix.
    this->viewer()->setViewMatrix ( this->navManip()->getInverseMatrix() );

    // Render the scene unless this is the first button press.
    if ( EventAdapter::PUSH != type )
    {
      // If button release, render using the proper number of passes
      if ( EventAdapter::RELEASE == type )
      {
        this->render();
      }

      // If it's anything else, render using 1 render pass.  Reset to proper value.
      else
      {
        const unsigned int passes ( this->numRenderPasses() );
        
        try
        {
          this->numRenderPasses ( 1 );
          this->render();
        }

        // Catch any exceptions and reset the number of rendering passes.
        catch ( ... )
        {
          this->numRenderPasses ( passes );
          throw;
        }

        this->numRenderPasses ( passes );
      }
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Handle the picking event.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::_handlePicking ( EventAdapter *ea )
{
  // Make sure we are in picking mode.
  if ( !this->picking() || 0x0 == ea )
    return;

  // Stop any spinning.
  this->spin ( false );

  const float x ( ea->getX() );
  const float y ( ea->getY() );
  const bool left ( Usul::Bits::has ( ea->getButton(), EventAdapter::LEFT_MOUSE_BUTTON ) );

  // Intersect the scene.
  osgUtil::LineSegmentIntersector::Intersection hit;
  if ( !this->_intersect ( x, y, this->scene(), hit, false ) )
  {
    // If we didn't intersect with the scene, try intersecting with the screen projection matrix
    // using just the screen coordniates.
    osg::ref_ptr < osg::MatrixTransform > root ( new osg::MatrixTransform );
    root->setMatrix ( osg::Matrix::identity() );
    root->addChild ( _sceneManager->camera() );

    // List of transforms that had the reference frame changed from absolute to relative.
    typedef std::vector < osg::ref_ptr < osg::Transform > > TList;
    TList tList;

    // Change all matrix transforms from absolute reference frame to relative.
    OsgTools::Utilities::setReferenceFrame ( root.get(), osg::Transform::ABSOLUTE_RF, osg::Transform::RELATIVE_RF, tList );

    // Change all dirty bounds flags to true.
    OsgTools::Utilities::setDirtyBounds ( root.get() );

    this->_intersect ( x, y, root.get(), hit, true );

    // Change all matrix transforms that were changed before back to absolute.
    for ( TList::iterator iter = tList.begin(); iter != tList.end(); ++iter )
      (*iter)->setReferenceFrame ( osg::Transform::ABSOLUTE_RF );

    // Clear the list
    tList.clear();

    // Change all dirty bounds flags to true.
    OsgTools::Utilities::setDirtyBounds ( root.get() );

    // Left mouse button was pressed...
    if ( left )
    {
      // Set the dragger being dragged, if there is one.
      this->_findDragger ( hit );
    }

    // Unref the matrix transform
    root = 0x0;

    // Regardless of the intersect test, return.
    // The hit is now invalid at this point scene the temp root has been deleted.
    return;
  }

  // Left mouse button was pressed...
  if ( left )
  {
    // Set the dragger being dragged, if there is one.
    this->_findDragger ( hit );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Handle dragging.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::_handleDragging ( EventAdapter *ea )
{
  // Make sure we are in picking mode, and there is a dragger.
  if ( false == this->picking() || 0x0 == _activeDragger.get() || 0x0 == ea)
    return;

  // Declare the adapters.
  ActionAdapter aa ( this->queryInterface( Usul::Interfaces::IUnknown::IID ) );

  _pointerInfo.setCamera ( this->viewer()->getCamera() );
  _pointerInfo.setMousePosition ( ea->getX(), ea->getY() );

  _activeDragger->handle ( _pointerInfo, *ea, aa );

  // Redraw.
  this->render();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Handle seek event.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::_handleSeek ( EventAdapter *ea )
{
  // Handle bad input.
  if ( 0x0 == ea )
    return;

  const bool left ( Usul::Bits::has ( ea->getButton(), EventAdapter::LEFT_MOUSE_BUTTON ) );

  // Retrun now if we aren't in seek mode.
  if ( ( _currentMode != SEEK ) || ( false == left ) )
    return;

  // Return if the click didn't intersect the scene
  // Note: switched to use scene so that could seek to camera path points -- 2008-02-21, Perry.
  osgUtil::LineSegmentIntersector::Intersection hit;
#if 0
  if ( !this->intersect ( ea->getX(), ea->getY(), hit ) )
#else
  if ( !this->_intersect ( ea->getX(), ea->getY(), this->scene(), hit, false ) )
#endif
    return;

  Usul::Interfaces::IHandleSeek::QueryPtr hs ( this->document() );
  if ( hs.valid() )
  {
    osg::Vec3d point ( hit.getWorldIntersectPoint() );
    hs->handleSeek ( Usul::Math::Vec3d ( point[0], point[1], point[2] ) );
  }
  else if ( 0x0 != dynamic_cast < Trackball * > ( this->navManip() ) )
  {
    // Make copy of trackball's current rotation
    const osg::Quat r1 ( this->getRotation() );
    const osg::Vec3d c1 ( this->getCenter() );
    const double d1 ( this->getDistance() );

    // Get the eye position.
    osg::Vec3d eye, c, up;
    osg::Matrix m ( osg::Matrixd::translate ( 0.0, 0.0, d1 ) * osg::Matrixd::rotate ( r1 ) * osg::Matrixd::translate ( c1 ) );
    m.inverse( m ).getLookAt ( eye, c, up );

    // Get the new center and distance.
    const osg::Vec3d c2 ( hit.getWorldIntersectPoint() );
    osg::Vec3d axis2 ( c2 - eye );
    const double d2 ( axis2.length() );

    // Find interfaces to animate, if they exists.
    typedef Usul::Interfaces::IAnimatePath IAnimatePath;
    IAnimatePath::QueryPtr animate ( Usul::Components::Manager::instance().getInterface ( IAnimatePath::IID ) );

    // Use the animation interface if we found a valid one.
    if ( true == animate.valid() )
    {
      // Prepare path.
      IAnimatePath::PackedMatrices matrices;
      const osg::Matrixd m1 ( this->getViewMatrix() );
      const osg::Matrixd m2 ( Trackball::matrix ( c2, r1, d2 ) );
      matrices.push_back ( IAnimatePath::PackedMatrix ( m1.ptr(), m1.ptr() + 16 ) );
      matrices.push_back ( IAnimatePath::PackedMatrix ( m2.ptr(), m2.ptr() + 16 ) );

      // Get step size.
      const unsigned int steps ( Reg::instance()[Sections::VIEWER_SETTINGS][Keys::SEEK_NUM_STEPS].get<unsigned int> ( 10, true ) );

      // We do this also so that the trackball behaves well.
      typedef Usul::Helper::AnimationNotify AnimationNotify;
      Usul::Interfaces::IUnknown::QueryPtr me ( this );
      Usul::Interfaces::IUnknown::QueryPtr notify ( new AnimationNotify ( me.get(), c2, d2, r1, true, true ) );

      // Animate through the path.
      animate->animatePath ( matrices, steps, notify.get() );
    }

    // If no animation interface exists, just set the trackball
    else
    {
      this->setTrackball ( c2, d2, r1, true, true );
      this->render();
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Handle intersection event.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::_handleIntersect ( EventAdapter *ea )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  if ( 0x0 == ea )
    return;

  // Don't bother intersecting if there any no listeners.
  if ( _intersectListeners.empty() )
    return;

  const float x ( ea->getX() );
  const float y ( ea->getY() );

  // Intersect the model. Return if no intersection.
  osgUtil::LineSegmentIntersector::Intersection hit;
  this->_intersect ( x, y, this->scene(), hit, false );

  // Notify the listeners (even if there is no intersection).
  this->_intersectNotify ( x, y, hit );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Start/stop the spin.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::spin ( bool state )
{
  if ( false == _timeoutSpin.valid() )
    return;

  // If we are supposed to start it.
  if ( true == state )
  {
    // We can't spin if there is no viewer.
    if ( 0x0 == this->viewer() )
      return;

    // First, stop any spin there is.
    this->spin ( false );

    // Set a new timeout event if we are allowed.
    //if ( Usul::Shared::Preferences::instance().getBool ( Keys::ALLOW_SPIN ) )
    _timeoutSpin->startSpin ( _ANIMATION_TIMER_MILLISECONDS );
  }

  // Otherwise, stop it.
  else
  {
    _timeoutSpin->stopSpin();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called by the timer.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::timeoutSpin()
{
  // Punt if we should.
  //if ( false == Usul::Shared::Preferences::instance().getBool ( Keys::ALLOW_SPIN ) )
  //  return;

  // Handle bad cases.
  if ( !this->navManip() || !this->navigating() || !this->viewer() )
    return;

  // Make the adapters.
  EventAdapter::Ptr ea ( new EventAdapter );
  ActionAdapter aa ( this->queryInterface( Usul::Interfaces::IUnknown::IID ) );
  ea->setWindowSize ( Usul::Math::Vec2ui ( static_cast < unsigned int > ( this->width() ), static_cast < unsigned int > ( this->height() ) ) );
  ea->setEventType ( EventAdapter::FRAME );

  // Ask the manipulator to handle the event.
  this->navManip()->handle ( *ea, aa );
  this->viewer()->setViewMatrix ( this->navManip()->getInverseMatrix() );

  this->render();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the background color.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::setBackground ( const osg::Vec4 &color )
{
  this->backgroundColor ( color );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add default clipping plane (IClippingPlanes).
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Viewer::addClippingPlane()
{
  osg::ref_ptr < osg::ClipPlane > clipPlane ( this->addPlane() );
  return ( clipPlane.valid() ? clipPlane->getClipPlaneNum() : 0 );
}

///////////////////////////////////////////////////////////////////////////////
//
//  Add clipping plane defined by given plane (IClippingPlanes).
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Viewer::addClippingPlane ( const osg::Plane& plane )
{
  osg::ref_ptr < osg::ClipPlane > clipPlane ( this->addPlane( plane ) );
  return ( clipPlane.valid() ? clipPlane->getClipPlaneNum() : 0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove clipping plane with given index IClippingPlanes.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::removeClippingPlane ( unsigned int index )
{
  this->removePlane ( index );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove all clipping planes (IClippingPlanes).
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::removeClippingPlanes()
{
  this->removePlanes();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the clipping plane (IClippingPlanes).
//
///////////////////////////////////////////////////////////////////////////////

osg::Plane Viewer::getClippingPlane ( unsigned int index )
{
  osg::ref_ptr < osg::ClipPlane > clipPlane ( _sceneManager->clipNode()->getClipPlane ( index ) );
  return ( clipPlane.valid() ? osg::Plane ( clipPlane->getClipPlane() ) : osg::Plane() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the clipping plane (IClippingPlanes).
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::setClippingPlane ( unsigned int index, const osg::Plane& plane )
{
  osg::ref_ptr < osg::ClipPlane > clipPlane ( _sceneManager->clipNode()->getClipPlane ( index ) );
  if ( clipPlane.valid() )
    clipPlane->setClipPlane ( plane );

  ClipPlaneWidget::Ptr widget ( _clipPlaneWidgets[index] );
  if ( widget.valid() )
    widget->update();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Go through the scene and set all materials transparency value to alpha.
//  Change the appropriate attributes for transparency.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::setAllMaterialsAlpha( float alpha )
{
  OsgTools::Utilities::setTransparency ( this->model(), alpha );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Edit the background.
//
///////////////////////////////////////////////////////////////////////////////

bool Viewer::timeoutAnimate()
{
  // Get the matrix and see if we should continue.
  osg::Matrixd mat;
  const bool animate ( _animation.matrix ( mat ) ) ;

  // Set the viewer's matrix.
  this->viewer()->setViewMatrix ( mat );
  this->render();
  
  if ( false == animate )
    this->_setLodCullCallback ( 0x0 );

  return animate;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Zoom by given amount
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::zoom ( double delta )
{
  if ( delta == 0.0 )
    return;
  
  float distance ( this->getDistance() );
  osg::BoundingBox bb ( this->getBoundingBox() );
  distance += ( delta * ( .1 * bb.radius() ) ) / Usul::Math::absolute ( delta );

  this->setDistance ( distance );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the filters for the images that we can expoxt.
//
///////////////////////////////////////////////////////////////////////////////

Viewer::Filters Viewer::filtersWriteImage() const
{
  Filters filters;
  filters.push_back ( Filter ( "JPEG Image (*.jpg)", "*.jpg"   ) );
  filters.push_back ( Filter ( "PNG Image (*.png)", "*.png"    ) );
  filters.push_back ( Filter ( "BMP Image (*.bmp)", "*.bmp"    ) );
  filters.push_back ( Filter ( "RGBA Image (*.rgba)", "*.rgba" ) );
  //filters.push_back ( Filter ( "Encapsulated PostScript (*.eps)", "*.eps" ) );
  return filters;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the filters that we can export.
//
///////////////////////////////////////////////////////////////////////////////

Viewer::Filters Viewer::filtersWriteScene() const
{
  return this->filtersWriteModel();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the filters that we can export.
//
///////////////////////////////////////////////////////////////////////////////

Viewer::Filters Viewer::filtersWriteModel() const
{
  Filters filters;
  filters.push_back ( Filter ( "OpenSceneGraph Binary (*.ive)", "*.ive" ) );
  filters.push_back ( Filter ( "OpenSceneGraph ASCII (*.osg)",  "*.osg" ) );
  return filters;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Hide/Show the center of rotation.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::showCenterOfRotation ( bool b )
{
  _flags = Usul::Bits::set < unsigned int, unsigned int > ( _flags, _SHOW_COR, b );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is the center of rotation is shown?
//
///////////////////////////////////////////////////////////////////////////////

bool Viewer::showCenterOfRotation ( ) const
{
  return Usul::Bits::has < unsigned int, unsigned int > ( _flags, _SHOW_COR );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the center of rotation.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::_setCenterOfRotation()
{
  osg::ref_ptr< osg::Group > group ( this->getGroup ( Keys::COR_NAME ) );

  group->removeChild( 0, group->getNumChildren() );

  if ( ! this->showCenterOfRotation() )
    return;

  osg::Vec3 center ( this->getCenter() );

  osg::ref_ptr < OsgTools::ShapeFactory > sf ( new OsgTools::ShapeFactory );

  osg::ref_ptr < osg::Geode > geode ( new osg::Geode );
  geode->addDrawable ( sf->sphere ( 15 ) );

  osg::ref_ptr < osg::Geometry > geom ( new osg::Geometry );
  osg::ref_ptr < osg::Vec3Array > vertices ( new osg::Vec3Array );

  vertices->push_back ( osg::Vec3 ( -30.0, 0.0,  0.0  ) );
  vertices->push_back ( osg::Vec3 (  30.0, 0.0,  0.0  ) );
  vertices->push_back ( osg::Vec3 (  0.0, -30.0, 0.0  ) );
  vertices->push_back ( osg::Vec3 (  0.0,  30.0, 0.0  ) );
  vertices->push_back ( osg::Vec3 (  0.0,  0.0, -30.0 ) );
  vertices->push_back ( osg::Vec3 (  0.0,  0.0,  30.0 ) );

  osg::ref_ptr< osg::Vec4Array > colors ( new osg::Vec4Array );
  colors->push_back ( osg::Vec4 ( 0.0, 0.0, 0.0, 1.0 ) );

  geom->setVertexArray ( vertices.get() );
  geom->setColorArray ( colors.get() );
  geom->setColorBinding ( osg::Geometry::BIND_OVERALL );
  geom->addPrimitiveSet ( new osg::DrawArrays ( osg::PrimitiveSet::LINES, 0, vertices->size() ) );

  OsgTools::State::StateSet::setLighting ( geom->getOrCreateStateSet(), false );

  geode->addDrawable ( geom.get( ) );

  osg::ref_ptr< osg::AutoTransform > transform ( new osg::AutoTransform ( ) );
  transform->setPosition ( center );
  transform->setAutoScaleToScreen ( true );

  // Set the normalize state to true, so when the sphere size changes it still looks correct.
  OsgTools::State::StateSet::setNormalize ( transform.get(), true );

  transform->addChild ( geode.get() );

  // Set the material.
  OsgTools::MaterialSetter ms;
  ms.diffuse ( osg::Vec4 ( 1.0, 1.0, 0.0, 1.0 ) );
  ms ( transform.get() );
  
  group->addChild ( transform.get() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Screen capture at the given view with the given height and width
//
///////////////////////////////////////////////////////////////////////////////

osg::Image* Viewer::screenCapture ( const osg::Vec3d& center, double distance, const osg::Quat& rotation, unsigned int height, unsigned int width ) const
{
  // Get non const pointer to this
  Viewer *me ( const_cast < Viewer * > ( this ) );

  osg::Matrixd m ( osg::Matrixd::translate(0.0,0.0,distance)*
                  osg::Matrixd::rotate(rotation)*
                  osg::Matrixd::translate(center) );

  // Make this context current.
  if( _context.valid() ) { me->_context->makeCurrent(); }

  return me->_renderer->screenCapture ( m.inverse( m ), width, height );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Screen capture with given height and width
//
///////////////////////////////////////////////////////////////////////////////

osg::Image* Viewer::screenCapture ( unsigned int height, unsigned int width ) const
{
  // Get non const pointer to this
  Viewer *me ( const_cast < Viewer * > ( this ) );

  // Make this context current.
  if( _context.valid() ) { me->_context->makeCurrent(); }

  return me->_renderer->screenCapture ( this->getViewMatrix(), width, height );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is there an accumulation buffer?
//
///////////////////////////////////////////////////////////////////////////////

bool Viewer::hasAccumBuffer() const
{
  return _renderer->hasAccumBuffer();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the scatter scale.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::scatterScale ( double scale )
{
  if ( _renderer.valid() )
    _renderer->scatterScale ( scale );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the scatter scale.
//
///////////////////////////////////////////////////////////////////////////////

double Viewer::scatterScale() const
{
  return ( _renderer.valid() ) ? _renderer->scatterScale() : 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the view matrix.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::viewMatrix ( const osg::Matrixf& mat )
{
  this->viewer()->setViewMatrix ( mat );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the view matrix.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::viewMatrix ( const osg::Matrixd& mat )
{
  this->viewer()->setViewMatrix ( mat );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the view matrix.
//
///////////////////////////////////////////////////////////////////////////////

const osg::Matrixd& Viewer::viewMatrix ( ) const
{
  return this->viewer()->getViewMatrix();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get text at the (x,y) on the screen.
//
///////////////////////////////////////////////////////////////////////////////

osgText::Text* Viewer::getText ( unsigned int x, unsigned int y )
{
  return _sceneManager->getText ( x, y );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set text value.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::setText ( unsigned int x, unsigned int y, const std::string& text, const osg::Vec4f& color, const osg::Vec4f& backDropColor )
{
  _sceneManager->setText ( x, y, text, color, backDropColor );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove text.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::removeText ( unsigned int x, unsigned int y )
{
  _sceneManager->removeText ( x, y );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set interface for the OpenGL context.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::context ( Usul::Interfaces::IUnknown* context )
{
  _context = context;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the light.
//
///////////////////////////////////////////////////////////////////////////////

osg::Light *Viewer::light()
{
  return ( 0x0 == _renderer ) ? 0x0 : _renderer->light();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the light.
//
///////////////////////////////////////////////////////////////////////////////

const osg::Light *Viewer::light() const
{
  return ( 0x0 == _renderer ) ? 0x0 : _renderer->light();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set compute near far mode.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::computeNearFarSet ( bool b )
{
  // Return now if state is the same.
  if ( this->computeNearFarGet() == b )
    return;
  
  // Return now if no viewer.
  if ( 0x0 == this->viewer() )
    return;
  
  osgUtil::CullVisitor *cv ( this->viewer()->getCullVisitor() );
  
  if ( 0x0 == cv )
    return;
  
  if ( b )
  {
    // Get the last callback.  Make a new one if null.
    osg::ref_ptr<osg::CullSettings::ClampProjectionMatrixCallback> cb ( 
      _clampProjectionMatrixCallback.valid() ? _clampProjectionMatrixCallback : 
        new OsgTools::Render::ClampProjection ( *cv, OsgTools::Render::Defaults::CAMERA_Z_NEAR, OsgTools::Render::Defaults::CAMERA_Z_FAR ) );
    
    cv->setClampProjectionMatrixCallback ( cb.get() );
    cv->setInheritanceMask ( Usul::Bits::remove ( cv->getInheritanceMask(), osg::CullSettings::CLAMP_PROJECTION_MATRIX_CALLBACK ) );
    cv->setComputeNearFarMode ( osg::CullSettings::COMPUTE_NEAR_FAR_USING_BOUNDING_VOLUMES );
    this->viewer()->setComputeNearFarMode ( osg::CullSettings::COMPUTE_NEAR_FAR_USING_BOUNDING_VOLUMES );
  }
  else
  {
    // Save the current callback.
    _clampProjectionMatrixCallback = cv->getClampProjectionMatrixCallback();
    
    cv->setClampProjectionMatrixCallback ( 0x0 );
    //cv->setInheritanceMask ( Usul::Bits::add ( cv->getInheritanceMask(), osg::CullSettings::CLAMP_PROJECTION_MATRIX_CALLBACK ) );
    cv->setComputeNearFarMode ( osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR );
    this->viewer()->setComputeNearFarMode ( osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get compute near far mode.
//
///////////////////////////////////////////////////////////////////////////////

bool Viewer::computeNearFarGet() const
{
  // Return now if no viewer.
  if ( 0x0 == this->viewer() )
    return false;
  
  const osgUtil::CullVisitor *cv ( this->viewer()->getCullVisitor() );
  
  if ( 0x0 == cv )
    return false;
  
  return osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR != cv->getComputeNearFarMode();
}


///////////////////////////////////////////////////////////////////////////////
//
// Add/Remove group from projection node.
//
///////////////////////////////////////////////////////////////////////////////

osg::Group* Viewer::projectionGroupGet ( const std::string& key )
{
  return _sceneManager->projectionGroupGet ( key );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to remove the listener.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  template < class Listeners, class MutexType > void removeListener ( Listeners &listeners, Usul::Interfaces::IUnknown::RefPtr caller, MutexType &mutex )
  {
    typedef typename Listeners::value_type::element_type InterfaceType;
    typedef Usul::Threads::Guard<MutexType> Guard;
    typedef typename Listeners::iterator Itr;

    USUL_TRACE_SCOPE_STATIC;

    typename InterfaceType::QueryPtr listener ( caller );
    if ( true == listener.valid() )
    {
      Guard guard ( mutex );
      typename InterfaceType::RefPtr value ( listener.get() );
      Itr end ( std::remove ( listeners.begin(), listeners.end(), value ) );
      listeners.erase ( end, listeners.end() );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to add the listener.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  template < class Listeners, class MutexType > void addListener ( Listeners &listeners, Usul::Interfaces::IUnknown::RefPtr caller, MutexType &mutex )
  {
    typedef typename Listeners::value_type::element_type InterfaceType;
    typedef Usul::Threads::Guard<MutexType> Guard;

    USUL_TRACE_SCOPE_STATIC;

    // Don't add twice.
    Helper::removeListener ( listeners, caller, mutex );

    // Check for necessary interface.
    typename InterfaceType::QueryPtr listener ( caller );
    if ( true == listener.valid() )
    {
      // Block while we add the listener.
      Guard guard ( mutex );
      listeners.push_back ( typename InterfaceType::RefPtr ( listener.get() ) );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add the listener.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::addRenderListener ( IUnknown *caller )
{
  USUL_TRACE_SCOPE;
  Helper::addListener ( _renderListeners, caller, this->mutex() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add the listener.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::addIntersectListener ( IUnknown *caller )
{
  USUL_TRACE_SCOPE;
  Helper::addListener ( _intersectListeners, caller, this->mutex() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove all render listeners.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::clearRenderListeners()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  _renderListeners.clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove all intersect listeners.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::clearIntersectListeners()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  _intersectListeners.clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove the listener.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::removeRenderListener ( IUnknown *caller )
{
  USUL_TRACE_SCOPE;
  Helper::removeListener ( _renderListeners, caller, this->mutex() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove the listener.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::removeIntersectListener ( IUnknown *caller )
{
  USUL_TRACE_SCOPE;
  Helper::removeListener ( _intersectListeners, caller, this->mutex() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add the listener.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::addMouseEventListener ( IUnknown *caller )
{
  Helper::addListener ( _mouseEventListeners, caller, this->mutex() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove all mouse event listeners.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::clearMouseEventListeners()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  _mouseEventListeners.clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove the listener.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::removeMouseEventListener ( IUnknown *caller )
{
  USUL_TRACE_SCOPE;
  Helper::removeListener ( _mouseEventListeners, caller, this->mutex() );
}

///////////////////////////////////////////////////////////////////////////////
//
//  Notify of rendering.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::_preRenderNotify()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  Usul::Interfaces::IUnknown::QueryPtr me ( this->queryInterface ( Usul::Interfaces::IUnknown::IID ) );
  std::for_each ( _renderListeners.begin(), _renderListeners.end(), std::bind2nd ( std::mem_fun ( &IRenderListener::preRenderNotify ), me.get() ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Notify of rendering.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::_postRenderNotify()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  Usul::Interfaces::IUnknown::QueryPtr me ( this->queryInterface ( Usul::Interfaces::IUnknown::IID ) );
  std::for_each ( _renderListeners.begin(), _renderListeners.end(), std::bind2nd ( std::mem_fun ( &IRenderListener::postRenderNotify ), me.get() ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Notify of intersection.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::_intersectNotify ( float x, float y, osgUtil::LineSegmentIntersector::Intersection &hit )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  Usul::Interfaces::IUnknown::QueryPtr me ( this->queryInterface ( Usul::Interfaces::IUnknown::IID ) );
  for ( IntersectListeners::iterator i = _intersectListeners.begin(); i != _intersectListeners.end(); ++i )
  {
    IIntersectListener::RefPtr listener ( *i );
    if ( true == listener.valid() )
    {
      listener->intersectNotify ( x, y, hit, me.get() );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Notify of mouse event.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::_mouseEventNotify ( EventAdapter *ea )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  Usul::Interfaces::IUnknown::QueryPtr me ( this->queryInterface ( Usul::Interfaces::IUnknown::IID ) );
  for ( MouseEventListeners::iterator i = _mouseEventListeners.begin(); i != _mouseEventListeners.end(); ++i )
  {
    IMouseEventListener::RefPtr listener ( *i );
    if ( true == listener.valid() && 0x0 != ea )
    {
      listener->mouseEventNotify ( *ea, me );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the flags that says to update the recorded times.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::updateTimes ( bool state )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  _flags = Usul::Bits::set < unsigned int, unsigned int > ( _flags, _UPDATE_TIMES, state );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the flags that says to update the recorded times.
//
///////////////////////////////////////////////////////////////////////////////

bool Viewer::updateTimes() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return Usul::Bits::has < unsigned int, unsigned int > ( _flags, _UPDATE_TIMES );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the frame stamp.
//
///////////////////////////////////////////////////////////////////////////////

osg::FrameStamp *Viewer::frameStamp()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return ( ( true == _renderer.valid() ) ? _renderer->framestamp() : 0x0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the frame stamp.
//
///////////////////////////////////////////////////////////////////////////////

const osg::FrameStamp *Viewer::frameStamp() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return ( ( true == _renderer.valid() ) ? _renderer->framestamp() : 0x0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the cull visitor.
//
///////////////////////////////////////////////////////////////////////////////

osgUtil::CullVisitor *Viewer::getCullSceneVisitor ( Usul::Interfaces::IUnknown * )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return ( ( 0x0 != this->viewer() ) ? this->viewer()->getCullVisitor() : 0x0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the update visitor.
//
///////////////////////////////////////////////////////////////////////////////

osg::NodeVisitor *Viewer::getUpdateSceneVisitor ( Usul::Interfaces::IUnknown * )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return ( ( 0x0 != this->viewer() ) ? this->viewer()->getUpdateVisitor() : 0x0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add the update listener.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::addUpdateListener ( IUnknown *caller )
{
  USUL_TRACE_SCOPE;

  // Don't add twice.
  this->removeUpdateListener ( caller );

  IUpdateListener::QueryPtr listener ( caller );
  if ( true == listener.valid() )
  {
    Guard guard ( this->mutex() );
    _updateListeners.push_back ( IUpdateListener::RefPtr ( listener.get() ) );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove the listener.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::removeUpdateListener ( IUnknown *caller )
{
  USUL_TRACE_SCOPE;

  IUpdateListener::QueryPtr listener ( caller );
  if ( true == listener.valid() )
  {
    Guard guard ( this->mutex() );
    IUpdateListener::RefPtr value ( listener.get() );
    UpdateListeners::iterator end ( std::remove ( _updateListeners.begin(), _updateListeners.end(), value ) );
    _updateListeners.erase ( end, _updateListeners.end() );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove all update listeners.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::_clearUpdateListeners()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  _updateListeners.clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the clipping distances.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::getClippingDistances ( double &nearDist, double &farDist ) const
{
  USUL_TRACE_SCOPE;
  
  double n ( 0.0 ), f ( 0.0 );
  
  // Get the near and far distances.
  {
    Guard guard ( this->mutex() );
    if ( _renderer.valid() )
      _renderer->nearFar ( n, f );
  }
  
  nearDist = n;
  farDist  = f;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the clipping distances.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::setClippingDistances ( double nearDist, double farDist )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  if ( 0x0 == this->viewer() )
    return;

  // No longer compute near far.
  this->computeNearFarSet ( false );
  
  const double fovy  ( OsgTools::Render::Defaults::CAMERA_FOV_Y );
  const double width ( this->width() ), height ( this->height() );
  const double aspect ( width / height );
  
  if ( this->viewer() )
    this->viewer()->setProjectionMatrixAsPerspective ( fovy, aspect, nearDist, farDist );

#if 0
  double left ( 0.0 ), right ( 0.0 ), top ( 0.0 ), bottom ( 0.0 ), zNear ( 0.0 ), zFar ( 0.0 );
  if ( this->viewer()->getProjectionMatrixAsFrustum ( left, right, bottom, top, zNear, zFar ) )
  {
    this->viewer()->setProjectionMatrixAsFrustum ( left, right, bottom, top, nearDist, farDist );
  }
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set render loop flag.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::renderLoop ( bool b )
{
  // Redirect to the caller.
  Usul::Interfaces::IRenderLoop::QueryPtr rl ( this->caller() );
  if ( rl.valid() )
    rl->renderLoop ( b );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get render loop flag.
//
///////////////////////////////////////////////////////////////////////////////

bool Viewer::renderLoop() const
{
  // Redirect to the caller.
  Usul::Interfaces::IRenderLoop::QueryPtr rl ( const_cast < IUnknown * > ( this->caller() ) );
  return rl.valid() ? rl->renderLoop ( ) : false ;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the caller.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown * Viewer::caller()
{
  Guard guard ( this->mutex() );
  return _caller.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the caller.
//
///////////////////////////////////////////////////////////////////////////////

const Usul::Interfaces::IUnknown * Viewer::caller() const
{
  Guard guard ( this->mutex() );
  return _caller.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Save the state.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::stateSave() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  // Get string key for document.
  std::string doc ( ( ( true == _document.valid() ) ? _document->registryTagName() : "" ) );
  if ( true == doc.empty() )
    return;

  // Get trackball.
  const Trackball *trackball ( dynamic_cast < const Trackball * > ( this->navManip() ) );
  if ( 0x0 != trackball )
  {
    // Save properties in registry.
    Usul::Registry::Node &reg ( Reg::instance()[Sections::VIEWER_SETTINGS][Keys::TRACKBALL][doc] );
    reg["center"]   = trackball->center();
    reg["rotation"] = trackball->rotation();
    reg["distance"] = trackball->distance();
  }
  {
    // Save properties in registry.
    Usul::Registry::Node &reg ( Reg::instance()[Sections::VIEWER_SETTINGS][Keys::BACKGROUND_COLOR][doc] );
    reg["top_left"]     = this->backgroundColor ( Corners::TOP_LEFT );
    reg["top_right"]    = this->backgroundColor ( Corners::TOP_RIGHT );
    reg["bottom_left"]  = this->backgroundColor ( Corners::BOTTOM_LEFT );
    reg["bottom_right"] = this->backgroundColor ( Corners::BOTTOM_RIGHT );
  }
  {
    // Save properties in registry.
    Usul::Registry::Node &reg ( Reg::instance()[Sections::VIEWER_SETTINGS][Keys::RENDER_LOOP][doc] );
    reg["state"] = this->renderLoop();
  }
  {
    Usul::Registry::Node &reg ( Reg::instance()[Sections::VIEWER_SETTINGS] );
    reg[Keys::LOW_LODS ][doc] = this->useLowLodsGet();
    reg[Keys::HIGH_LODS][doc] = this->useHighLodsGet();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Load the state.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::stateLoad()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  // Get string key for document.
  std::string doc ( ( ( true == _document.valid() ) ? _document->registryTagName() : "" ) );
  if ( true == doc.empty() )
    return;

  // Get trackball.
  Trackball *trackball ( dynamic_cast < Trackball * > ( this->navManip() ) );
  if ( 0x0 != trackball )
  {
    // Get properties from registry.
    Usul::Registry::Node &reg ( Reg::instance()[Sections::VIEWER_SETTINGS][Keys::TRACKBALL][doc] );
    const osg::Vec3d  center ( reg["center"].get<osg::Vec3d>  ( trackball->center()   ) );
    const osg::Quat rotation ( reg["rotation"].get<osg::Quat> ( trackball->rotation() ) );
    const double    distance ( reg["distance"].get<float>     ( trackball->distance() ) );

    // Set trackball's properties.
    this->setTrackball ( center, distance, rotation, false, true );
  }
  {
    // Get properties from registry.
    Usul::Registry::Node &reg ( Reg::instance()[Sections::VIEWER_SETTINGS][Keys::BACKGROUND_COLOR][doc] );
    this->backgroundColor ( reg["top_left"].get<osg::Vec4>     ( this->backgroundColor ( Corners::TOP_LEFT     ) ), Corners::TOP_LEFT     );
    this->backgroundColor ( reg["top_right"].get<osg::Vec4>    ( this->backgroundColor ( Corners::TOP_RIGHT    ) ), Corners::TOP_RIGHT    );
    this->backgroundColor ( reg["bottom_left"].get<osg::Vec4>  ( this->backgroundColor ( Corners::BOTTOM_LEFT  ) ), Corners::BOTTOM_LEFT  );
    this->backgroundColor ( reg["bottom_right"].get<osg::Vec4> ( this->backgroundColor ( Corners::BOTTOM_RIGHT ) ), Corners::BOTTOM_RIGHT );
  }
  {
    // Get properties from registry.
    Usul::Registry::Node &reg ( Reg::instance()[Sections::VIEWER_SETTINGS][Keys::RENDER_LOOP][doc] );
    this->renderLoop ( reg["state"].get<bool> ( this->renderLoop() ) );
  }
  {
    Usul::Registry::Node &reg ( Reg::instance()[Sections::VIEWER_SETTINGS] );
    this->useLowLodsSet  ( reg[Keys::LOW_LODS ][doc].get<bool> ( this->useLowLodsGet()  ) );
    this->useHighLodsSet ( reg[Keys::HIGH_LODS][doc].get<bool> ( this->useHighLodsGet() ) );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the back face culling state.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::showBackFaces ( bool b )
{
  USUL_TRACE_SCOPE;
  
  // Handle no viewer.
  if ( !this->viewer() )
    return;
  
  // Set the state.
  OsgTools::State::StateSet::setBackFaceCulling ( this->viewer()->getGlobalStateSet(), !b );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the back face culling state.
//
///////////////////////////////////////////////////////////////////////////////

bool Viewer::isBackFacesShowing() const
{
  USUL_TRACE_SCOPE;
  
  // Return the state.
  return ( ( 0x0 == this->viewer() ) ? false : !OsgTools::State::StateSet::getBackFaceCulling ( this->viewer()->getGlobalStateSet() ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the model's scene.
//
///////////////////////////////////////////////////////////////////////////////

const osg::Group * Viewer::modelsScene() const
{
	USUL_TRACE_SCOPE;
	return ( ( 0x0 == this->sceneManager() ) ? 0x0 : this->sceneManager()->clipNode() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the model's scene.
//
///////////////////////////////////////////////////////////////////////////////

osg::Group * Viewer::modelsScene()
{
	USUL_TRACE_SCOPE;
	return ( ( 0x0 == this->sceneManager() ) ? 0x0 : this->sceneManager()->clipNode() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the scene manager.
//
///////////////////////////////////////////////////////////////////////////////

const SceneManager * Viewer::sceneManager() const
{
	USUL_TRACE_SCOPE;
	Guard guard ( this->mutex() );
	return _sceneManager.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the scene manager.
//
///////////////////////////////////////////////////////////////////////////////

SceneManager * Viewer::sceneManager()
{
	USUL_TRACE_SCOPE;
	Guard guard ( this->mutex() );
	return _sceneManager.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the render info for this Viewer.
//
///////////////////////////////////////////////////////////////////////////////

osg::RenderInfo Viewer::getRenderInfo() const
{
  USUL_TRACE_SCOPE;
	Guard guard ( this->mutex() );
  return ( _renderer.valid() ? _renderer->getRenderInfo() : osg::RenderInfo() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the use low lods state.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::useLowLodsSet ( bool b )
{
  USUL_TRACE_SCOPE;
	Guard guard ( this->mutex() );

  // Set this state.
  this->_useLowLodsSet ( b );

  // Don't allow both to be on.
  if ( true == b )
    this->_useHighLodsSet ( false );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the use high lods state.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::useHighLodsSet ( bool b )
{
  USUL_TRACE_SCOPE;
	Guard guard ( this->mutex() );

  // Set this state.
  this->_useHighLodsSet ( b );

  // Don't allow both to be on.
  if ( true == b )
    this->_useLowLodsSet ( false );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the use low lods state.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::_useLowLodsSet ( bool b )
{
  USUL_TRACE_SCOPE;
	Guard guard ( this->mutex() );
  _flags = Usul::Bits::set < unsigned int, unsigned int > ( _flags, _USE_LOW_LODS, b );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the use high lods state.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::_useHighLodsSet ( bool b )
{
  USUL_TRACE_SCOPE;
	Guard guard ( this->mutex() );
  _flags = Usul::Bits::set < unsigned int, unsigned int > ( _flags, _USE_HIGH_LODS, b );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the use high lods state.
//
///////////////////////////////////////////////////////////////////////////////

bool Viewer::useHighLodsGet() const
{
  USUL_TRACE_SCOPE;
	Guard guard ( this->mutex() );
  return Usul::Bits::has < unsigned int, unsigned int > ( _flags, _USE_HIGH_LODS );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the use low lods state.
//
///////////////////////////////////////////////////////////////////////////////

bool Viewer::useLowLodsGet() const
{
  USUL_TRACE_SCOPE;
	Guard guard ( this->mutex() );
  return Usul::Bits::has < unsigned int, unsigned int > ( _flags, _USE_LOW_LODS );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the use multisamle state.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::useMultisampleSet ( bool b )
{
  USUL_TRACE_SCOPE;
	Guard guard ( this->mutex() );
  _flags = Usul::Bits::set < unsigned int, unsigned int > ( _flags, _USE_MULTISAMPLE, b );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the use multisamle state.
//
///////////////////////////////////////////////////////////////////////////////

bool Viewer::useMultisampleGet() const
{
  USUL_TRACE_SCOPE;
	Guard guard ( this->mutex() );
  return Usul::Bits::has < unsigned int, unsigned int > ( _flags, _USE_MULTISAMPLE );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the projection matrix (IProjectionMatrix).
//
///////////////////////////////////////////////////////////////////////////////

osg::Matrixd Viewer::getProjectionMatrix() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  const SceneView * viewer ( this->viewer() );
  return ( 0x0 != viewer ? viewer->getProjectionMatrix() : osg::Matrixd() );
}
