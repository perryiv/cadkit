
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  Copyright (c) 2005, Perry L. Miller IV and Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

// Disable deprecated warning in Visual Studio 8 
#if defined ( _MSC_VER ) && _MSC_VER == 1400
#pragma warning ( disable : 4996 )
#endif

#include "OsgTools/Render/Viewer.h"
#include "OsgTools/Render/Defaults.h"
#include "OsgTools/Render/LightCallback.h"

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
#include "OsgTools/Callbacks/SortBackToFront.h"
#include "OsgTools/Utilities/DirtyBounds.h"
#include "OsgTools/Utilities/ReferenceFrame.h"
#include "OsgTools/Utilities/Transparency.h"
#include "OsgTools/Utilities/Intersect.h"
#include "OsgTools/Builders/Arrow.h"
#include "OsgTools/Widgets/Axes.h"
#include "OsgTools/Widgets/ClipPlane.h"
#include "OsgTools/IO/WriteEPS.h"
#include "OsgTools/Callbacks/HiddenLines.h"
#include "OsgTools/Font.h"

#include "OsgTools/Draggers/Trackball.h"

#include "Usul/Errors/Checker.h"
#include "Usul/Components/Manager.h"
#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Math/Constants.h"
#include "Usul/Math/UMath.h"
#include "Usul/Bits/Bits.h"
#include "Usul/Registry/Constants.h"

#include "Usul/Interfaces/GUI/IUpdateTreeControls.h"
#include "Usul/Interfaces/GUI/IStatusBar.h"
#include "Usul/Interfaces/GUI/IReportErrors.h"
#include "Usul/Interfaces/GUI/IMaterialEditor.h"
#include "Usul/Interfaces/ISendMessage.h"
#include "Usul/Interfaces/ICleanUp.h"
#include "Usul/Interfaces/IGetBoundingBox.h"
#include "Usul/Interfaces/IHandleMessage.h"
#include "Usul/Interfaces/IAnimate.h"
#include "Usul/Interfaces/IMatrixManipulator.h"
#include "Usul/Interfaces/IDatabasePager.h"
#include "Usul/Interfaces/ISceneUpdate.h"
#include "Usul/Interfaces/IToolLifeTime.h"

#include "Usul/Resources/StatusBar.h"
#include "Usul/Resources/ReportErrors.h"
#include "Usul/Resources/TextWindow.h"
#include "Usul/Shared/Preferences.h"
#include "Usul/System/Clock.h"
#include "Usul/Predicates/Tolerance.h"
#include "Usul/Functors/Delete.h"
#include "Usul/Scope/Reset.h"
#include "Usul/Cast/Cast.h"
#include "Usul/Strings/Case.h"
#include "Usul/File/Path.h"
#include "Usul/CommandLine/Arguments.h"
#include "Usul/Math/Absolute.h"

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

#include "osg/GL"
#include "osg/GLU"

#include "osgUtil/UpdateVisitor"
#include "osgUtil/IntersectVisitor"

#include "osgDB/WriteFile"
#include "osgDB/ReadFile"

#include <limits>

using namespace OsgTools::Render;


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to check for errors.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  void checkForErrors( unsigned int id )
  {
    GLenum error = ::glGetError();
    if ( GL_NO_ERROR != error )
    {
      std::cout << "Error " << id << ": OpenGL error detected." << std::endl
        << "Message: " << ::gluErrorString( error ) << std::endl;
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Static data member(s).
//
///////////////////////////////////////////////////////////////////////////////

Viewer::CameraBuffer Viewer::_cameraCopyBuffer ( false, osg::Matrixd::identity() );
Viewer::MatrixManipPtr Viewer::_navManipCopyBuffer ( 0x0 );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Viewer::Viewer ( Document *doc, IUnknown* context, IUnknown *caller ) :
  _context         ( context ),
  _renderer        ( new Renderer ),
  _sceneManager    ( new SceneManager ),
  _setCursor       ( caller ),
  _timeoutSpin     ( caller ),
  _caller          ( caller ),
  _lods            (),
  _document        ( doc ),
  _frameDump       (),
  _refCount        ( 0 ),
  _flags           ( _UPDATE_TIMES | _SHOW_AXES ),
  _animation       (),
  _navManip        ( 0x0 ),
  _currentTool     (),
  _lastTool        (),
  _currentMode     ( NAVIGATION ),
  _lightEditors    (),
  _contextId       ( 0 ),
  _gradient        (),
  _corners         ( Corners::ALL ),
  _useDisplayList  ( false, true ),
  _databasePager   ( 0x0 )
{
  // Add the document
  this->document( doc );

  // Light so that other lights don't alter geometry under the projection node.
  // Note...
  // With the introduction of the gradient background (2006-Dec-07) this line 
  // of code causes lighting problems in cow.osg and color_by_domain.ive.
#if 0
  osg::ref_ptr< osg::Light > light ( new osg::Light );
  light->setLightNum ( 1 );
  light->setDiffuse ( osg::Vec4 ( 0.8, 0.8, 0.8, 1.0 ) );
  osg::ref_ptr< osg::StateSet > ss ( _sceneManager->projection()->getOrCreateStateSet () );
  ss->setAttribute ( light.get(), osg::StateAttribute::ON | osg::StateAttribute::PROTECTED );
#endif

  // Initialize the clock.
  Usul::System::Clock::milliseconds();

  // Add the manipulator
  this->navManip( new Trackball );

  // Add axes to the scene.  
  // Make sure that this is called after the manipulator has been created.
  this->_addAxes();

  // Set the default fov
  Usul::Shared::Preferences::instance().setDouble( Usul::Registry::Keys::FOV, OsgTools::Render::Defaults::CAMERA_FOV_Y );

  // Unique context id to uniquely identify this viewer in OSG.
  static unsigned int count ( 0 );
  _contextId = ++count;

  _renderer->scene( _sceneManager->scene() );

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
  USUL_ASSERT ( 0 == _refCount );
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

  _renderer->clear();

  _context = static_cast < Usul::Interfaces::IUnknown* > ( 0x0 );
  _setCursor = static_cast < Usul::Interfaces::ISetCursorType* > ( 0x0 );
  _timeoutSpin = static_cast < Usul::Interfaces::ITimeoutSpin* > ( 0x0 );
  _caller = static_cast < Usul::Interfaces::IUnknown* > ( 0x0 );

  if( _databasePager.valid() )
  {
    _databasePager->setAcceptNewDatabaseRequests(false);
    //_databasePager->clear();
    //_databasePager->setDatabasePagerThreadPause(true);
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the default background.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::defaultBackground()
{
  this->backgroundColor ( OsgTools::Render::Defaults::CLEAR_COLOR );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the scene.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::updateScene()
{
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

  // Update the scene.
  Usul::Interfaces::ISceneUpdate::QueryPtr sceneUpdate ( _document );
  if ( sceneUpdate.valid() )
    sceneUpdate->sceneUpdate( this->queryInterface( Usul::Interfaces::IUnknown::IID ) );

  // Make this context current.
  if ( _context.valid() )
    _context->makeCurrent();

  // Update display lists.
  this->updateDisplayListUse();

  // Initialize the error.
  ::glGetError();

  // Check for errors.
  Detail::checkForErrors( 1491085606 );

  if( _databasePager.valid() )
  {
    // tell the DatabasePager the frame number of that the scene graph is being actively used to render a frame
    _databasePager->signalBeginFrame( _renderer->framestamp() );

    // syncronize changes required by the DatabasePager thread to the scene graph
    _databasePager->updateSceneGraph( _renderer->framestamp()->getReferenceTime() );
  }

  // Set the axes.
  this->_setAxes();

  // Set the lights
  this->_setLights();

  // Set the center of rotation
  this->_setCenterOfRotation();

  // Set high lod callbacks if we should
  if ( Usul::Shared::Preferences::instance().getBool( Usul::Registry::Keys::HIGH_LODS ) )
    this->_setLodCullCallback ( new OsgTools::Render::HighLodCallback );

  // Check for errors.
  Detail::checkForErrors( 774712060 );

  // If we are doing hidden-line rendering...
  if ( this->hasHiddenLines() && ( 0x0 != _sceneManager->model() ) )
  {
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
  else
  {
    // Draw.
    this->_render();
  }

  // Check for errors.
  Detail::checkForErrors( 840649560 );

  if( _databasePager.valid() )
  {
    // tell the DatabasePager that the frame is complete and that scene graph is no longer be activity traversed.
    _databasePager->signalEndFrame();
  }

  // Swap the buffers.
  if ( _context.valid() )
    _context->swapBuffers();

  if( _databasePager.valid() )
  {
    // clean up  and compile gl objects with a specified limit       
    double availableTime = 0.0025; // 2.5 ms

    // compile any GL objects that are required.
    _databasePager->compileGLObjects(*(this->viewer()->getState()),availableTime);

    // flush deleted GL objects.
    this->viewer()->flushDeletedGLObjects(availableTime);
  }

  // Check for errors.
  Detail::checkForErrors( 896118310 );

  // Dump the current frame.
  this->_dumpFrame();

  // Check for errors.
  Detail::checkForErrors( 952680810 );

  // Update the status-bar. Do not put this in onPaint() because you want 
  // it called every time the window is redrawn.
  if ( this->updateTimes() )
    this->updateStatusBar();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Render the scene.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::_render()
{
  // Update.
  _renderer->update();

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
//  View everything.
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
  osg::Vec3 T1 ( M1.getTrans() );
  osg::Quat R1; M1.get ( R1 );

  // Make the adapters.
  EventAdapter::Ptr ea ( new EventAdapter );
  ea->setWindowSize ( Usul::Math::Vec2ui ( this->width(), this->height() ) );
  ActionAdapter aa ( this->queryInterface( Usul::Interfaces::IUnknown::IID ) );

  // See if we have a trackball...
  Trackball *trackball = dynamic_cast < Trackball * > ( this->navManip ( ) );
  
  // Try to save the distance.
  std::pair<bool,float> dist ( false, 0 );
  if ( trackball )
  {
    dist.first = true;
    dist.second = trackball->distance();
  }

  // Tell the manipulator to go home.
  this->navManip()->home ( *ea, aa );

  typedef Usul::Interfaces::ICamera Camera;

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

  // Use low lods until this function returns.
  LowLods ll ( this );

  // Get the translation and rotation.
  osg::Vec3 T2 ( M2.getTrans() );
  osg::Quat R2; M2.get ( R2 );

  // Animate.
  //this->animate ( T1, R1, T2, R2 );
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
  double fovy  ( Usul::Shared::Preferences::instance().getDouble ( Usul::Registry::Keys::FOV ) );
  double zNear ( OsgTools::Render::Defaults::CAMERA_Z_NEAR );
  double zFar  ( OsgTools::Render::Defaults::CAMERA_Z_FAR );
  double width ( w ), height ( h );
  double aspect ( width / height );
  if ( this->viewer() )
    this->viewer()->setProjectionMatrixAsPerspective ( fovy, aspect, zNear, zFar );

  // Update the projection node.
  _sceneManager->projection()->setMatrix( osg::Matrix::ortho2D ( 0, w ,0, h ) );

  // Update the gradient background vertices.
  _gradient.update ( w, h );
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

void Viewer::twoSidedLighting ( bool twoSided )
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

bool Viewer::twoSidedLighting() const
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

  if( on )
    ss->setMode ( GL_LIGHTING, osg::StateAttribute::ON );
  else
    ss->setMode ( GL_LIGHTING, osg::StateAttribute::OFF );
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
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the background color.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::backgroundColor ( const osg::Vec4 &color )
{
  // Always remove the existing branch.
  OsgTools::Group::removeAllOccurances ( _gradient.root(), _sceneManager->projection() );

  // Always update the gradient object.
  _gradient.color ( color, this->backgroundCorners() );

  // Use regular background if all corners are the same.
  if ( Corners::ALL == this->backgroundCorners() )
  {
    _renderer->backgroundColor ( color );
  }

  // Otherwise, add the branch to render the gradient.
  else
  {
    _sceneManager->projection()->addChild ( _gradient.root() );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the background color.
//
///////////////////////////////////////////////////////////////////////////////

osg::Vec4 Viewer::backgroundColor() const
{
  return _gradient.color ( this->backgroundCorners() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the background corners.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::backgroundCorners ( unsigned int corners )
{
  _corners = corners;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the background corners.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Viewer::backgroundCorners() const
{
  return _corners;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Rotate the scene.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::rotate ( const osg::Vec3 &axis, float radians )
{
  // Return if no viewer.
  if ( !this->viewer() )
    return;

  // Get the trackball.
  Trackball *tb = dynamic_cast < Trackball* > ( this->navManip( ) );

  if( !tb )
    return;

  // Get the current state.
  osg::Quat R1 ( tb->rotation() );

  // Get the final state.
  osg::Quat dR, R2;
  dR.makeRotate ( radians, axis );
  R2 = dR * R1;

  // Use low lods until this function returns.
  Viewer::LowLods ll ( this );

  // Animate for this many seconds.
  const double duration ( this->_animationTime() );

  // Get the starting and stopping times.
  const double start ( Usul::System::Clock::milliseconds() );
  const double stop ( start + duration );

  // Loop until out of range.
  while ( Usul::System::Clock::milliseconds() < stop )
  {
    // The independent variable.
    const double current ( Usul::System::Clock::milliseconds() );
    const double u ( ( current - start ) / duration );

    // Linearly interpolate.
    osg::Quat R;
    R.slerp ( u, R1, R2 );

    // Set the new rotation.
    tb->rotation ( R );

    // Set the viewer's matrix and redraw.
    this->viewer()->setViewMatrix ( tb->getInverseMatrix() );
    this->render();
  }

  // Ensure that we end up where we are supposed to be. If we don't do this, 
  // then depending on how long things take in the above loop, the object may 
  // not end up in the final position.
  tb->rotation ( R2 );
  this->viewer()->setViewMatrix ( tb->getInverseMatrix() );

  // Render the scene
  this->render();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the lod-callback.
//
///////////////////////////////////////////////////////////////////////////////

Viewer::LowLods::LowLods ( Viewer *c ) : _c ( c )
{
  USUL_ERROR_CHECKER ( 0x0 != _c );

  if ( Usul::Shared::Preferences::instance().getBool( Usul::Registry::Keys::LOW_LODS ) )
    OsgTools::Render::setLodCullCallback ( new OsgTools::Render::LowLodCallback, _c->scene(), _c->_lods );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Restore the lod-callback.
//
///////////////////////////////////////////////////////////////////////////////

Viewer::LowLods::~LowLods()
{
  USUL_ERROR_CHECKER ( 0x0 != _c );

  OsgTools::Render::setLodCullCallback ( static_cast<OsgTools::Render::LowLodCallback *> ( 0x0 ), _c->scene(), _c->_lods );
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
//  Copy the current camera.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::copyCamera() const
{
  _cameraCopyBuffer.first  = true; 
  _cameraCopyBuffer.second = this->viewer()->getViewMatrix();

  if ( this->navManip() )
    _navManipCopyBuffer = dynamic_cast < MatrixManip * > ( this->navManip()->clone ( osg::CopyOp::DEEP_COPY_ALL ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Paste the camera.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::pasteCamera()
{
  if ( this->canPasteCamera() )
  {
    if ( this->navManip() && this->viewer() && _navManipCopyBuffer.valid() )
    {
      _navManip = dynamic_cast < MatrixManip * > ( this->navManip()->clone ( osg::CopyOp::DEEP_COPY_ALL ) );
    }
    this->viewer()->setViewMatrix ( _cameraCopyBuffer.second );
  } 
}


///////////////////////////////////////////////////////////////////////////////
//
//  Can we paste the camera?
//
///////////////////////////////////////////////////////////////////////////////

bool Viewer::canPasteCamera() const
{
  return _cameraCopyBuffer.first;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Draw a square representing the selection box.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::_drawSelectionBox ( const osg::Vec3& v1, const osg::Vec3& v2 )
{
  osg::Vec3 v3 ( v2[0], v1[1], 0.0 );
  osg::Vec3 v4 ( v1[0], v2[1], 0.0 );

  // Get the group for selection box, if it exists.
  osg::ref_ptr<osg::Group> group ( _sceneManager->projectionGroupGet ( OsgTools::Render::Constants::SELECTION_BOX ) );
  
  // Has the selection box been created?
  if ( !group.valid() )
  {
    osg::MatrixTransform* modelview_abs = new osg::MatrixTransform;
    modelview_abs->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
    modelview_abs->setMatrix(osg::Matrix::identity());

    // Create a geode for the geometry
    osg::ref_ptr <osg::Geode> box ( new osg::Geode );
    box->setName ( "Selection Box" );

    modelview_abs->addChild ( box.get() );

    group->addChild ( modelview_abs );

    // Get or create the state set
    osg::ref_ptr<osg::StateSet> ss ( group->getOrCreateStateSet() );

    // Set the line width
    OsgTools::State::StateSet::setLineWidth ( group.get(), 2.0 );

    // Turn off lighting
    OsgTools::State::StateSet::setLighting ( group.get(), false );

    // Make sure it's drawn last ( on top )
    ss->setRenderBinDetails ( 100, "RenderBin" );
	  ss->setMode ( GL_DEPTH_TEST, osg::StateAttribute::OFF );

    // Create the geometry for box
    osg::ref_ptr < osg::Geometry > geometry ( new osg::Geometry );

    // Build the vertex list
    osg::ref_ptr < osg::Vec3Array > vertices ( new osg::Vec3Array ( 4 ) );
    vertices->at( 0 ) = v1;
    vertices->at( 1 ) = v3;
    vertices->at( 2 ) = v2;
    vertices->at( 3 ) = v4;

    // Color line loop red.
    osg::ref_ptr< osg::Vec4Array > color ( new osg::Vec4Array ( 1 ) );
    color->at( 0 ) = osg::Vec4 ( 1.0, 0.0, 0.0, 1.0 );

    geometry->setVertexArray ( vertices.get() );
    geometry->setColorArray  ( color.get() );
    geometry->setColorBinding ( osg::Geometry::BIND_OVERALL );
    geometry->addPrimitiveSet ( new osg::DrawArrays ( osg::PrimitiveSet::LINE_LOOP, 0, vertices->size() ) );
    box->addDrawable ( geometry.get() );
  }

  osg::ref_ptr < osg::Group > mt ( group->getChild( 0 )->asGroup() );
  osg::ref_ptr < osg::Geode > box ( static_cast < osg::Geode * > ( mt->getChild( 0 ) ) );
  osg::ref_ptr < osg::Geometry > geometry ( box->getDrawable( 0 )->asGeometry() );

  // Build the vertex array
  osg::ref_ptr < osg::Vec3Array > vertices ( reinterpret_cast < osg::Vec3Array*> ( geometry->getVertexArray() ) );
  vertices->at( 0 ) = v1;
  vertices->at( 1 ) = v3;
  vertices->at( 2 ) = v2;
  vertices->at( 3 ) = v4;

  // Set the vertex array.  This is needed.
  geometry->setVertexArray ( vertices.get() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove the selection box.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::_removeSelectionBox()
{
  _sceneManager->groupRemove( OsgTools::Render::Constants::SELECTION_BOX );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get animation time
//
///////////////////////////////////////////////////////////////////////////////

double Viewer::_animationTime()
{ 
  double animation ( Usul::Shared::Preferences::instance().getDouble ( Usul::Registry::Keys::ANIMATION_TIME ) );
  return animation * OsgTools::Render::Defaults::TO_MILLISECONDS; 
}


///////////////////////////////////////////////////////////////////////////////
//
//  Dump the current frame to file.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::_dumpFrame()
{
  if ( _frameDump.dump() )
    this->writeImageFile ( _frameDump.file() );
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
  // Force all detail to render.
  Viewer *me ( const_cast < Viewer * > ( this ) );
  me->forceDetail();

  // Get scale of window size.
  const float scale ( this->frameDump().scale() );

  // Is the size of the image the same as the window?
  if ( 1.0f != scale )
  {
    // Calculate size.
    const unsigned int width  ( static_cast<unsigned int> ( scale * static_cast<float> ( this->width()  ) ) );
    const unsigned int height ( static_cast<unsigned int> ( scale * static_cast<float> ( this->height() ) ) );

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
//  Write the current frame to an image file.
//  TODO: This supports legacy code. Depreciated.
//
///////////////////////////////////////////////////////////////////////////////

bool Viewer::writeImageFile ( const std::string &filename, unsigned int width, unsigned int height ) const
{
  const float scale ( this->frameDump().scale() );
  bool result ( false );
  Viewer *me ( const_cast < Viewer * > ( this ) );
  FrameDump &fd ( me->frameDump() );

  try
  {
    fd.scale ( static_cast<float> ( width ) / static_cast<float> ( this->width() ) );
    result = this->writeImageFile ( filename );
    fd.scale ( scale );
  }

  catch ( ... )
  {
    fd.scale ( scale );
    throw;
  }

  return result;
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

  // Read the screen buffer.
  image->readPixels ( this->x(), this->y(), static_cast<int> ( this->width() ), static_cast<int> ( this->height() ), GL_RGB, GL_UNSIGNED_BYTE );

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
  // Get non const pointer to this
  Viewer *me ( const_cast < Viewer * > ( this ) );

  // Make this context current.
  if ( _context.valid() )
    me->_context->makeCurrent();

  // Capture image.
  osg::ref_ptr<osg::Image> image ( me->_renderer->screenCapture ( this->getViewMatrix(), width, height ) );

  // Write the image to file.
  return osgDB::writeImageFile ( *image, filename );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create image of current using current view matrix.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::takePicture ( const std::string& filename, float frameSizeScale, unsigned int numSamples )
{
  // Save the axes state.
  bool axes ( this->axes() );

  // Turn off the axes.
  this->axes( false );

  // Make this context current.
  if ( _context.valid() )
    _context->makeCurrent();

  osg::ref_ptr < osg::Image > image ( _renderer->screenCapture ( frameSizeScale, numSamples ) );

  // Write the image to file.
  osgDB::writeImageFile ( *image, filename );

  // Restore the state.
  this->axes ( axes );
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

    /*osg::StateSet *ss ( group->getOrCreateStateSet () );

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

      mt->addChild ( g.get () );
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

void Viewer::document( Document *document )
{
  _document = document;

  // Add this view to the document.
  if( _document )
    _document->addView ( this );

  Usul::Interfaces::IMatrixManipulator::QueryPtr mm ( _document );

  if( mm.valid() )
    this->navManip( mm->getMatrixManipulator() );

  Usul::Interfaces::IDatabasePager::QueryPtr dp ( _document );

  if( dp.valid() )
    this->databasePager( dp->getDatabasePager() );
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
  _renderer->numRenderPasses( num );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the number of rendering passes.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Viewer::numRenderPasses ( ) const
{
  return _renderer->numRenderPasses();
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

void Viewer::addPlane ( )
{
  // Get the bounding box
  osg::BoundingBox bb; 
  bb.expandBy ( _sceneManager->clipNode()->getBound() );

  // Intial points for the plane
  osg::Vec3 top_left     ( bb.corner( 1 ) );
  osg::Vec3 bottom_left  ( bb.corner( 0 ) );
  osg::Vec3 top_right    ( bb.corner( 7 ) );

    // Define the plane on the diagonal of the bounding box
  osg::Plane plane ( top_left, bottom_left, top_right );
  this->addPlane ( plane );
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
    osg::BoundingBox bb; 
    bb.expandBy ( this->model()->getBound() );

    osg::ref_ptr < OsgTools::Widgets::ClipPlane > widget ( new OsgTools::Widgets::ClipPlane ( bb, clipPlane.get() ) );

    osg::ref_ptr< osg::Group > group ( _sceneManager->groupGet ( OsgTools::Render::Constants::CLIPPING_PLANES ) );
    group->addChild ( widget.get() );
  }

  // Add the plane
  _sceneManager->clipNode()->addClipPlane ( clipPlane.get() );

  this->changedScene();
  this->render();

  return clipPlane.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a clipping plane to the scene.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::addClipBox ( const osg::BoundingBox& bb )
{
  _sceneManager->clipNode()->createClipBox ( bb );
  OsgTools::GlassBoundingBox gbb ( bb );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove a clipping plane from the scene
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::removePlane ( unsigned int index )
{
  _sceneManager->clipNode()->removeClipPlane ( index );

  osg::ref_ptr< osg::Group > group ( _sceneManager->groupGet ( OsgTools::Render::Constants::CLIPPING_PLANES ) );

  group->removeChild ( index );

  this->changedScene();
  this->render();
}

///////////////////////////////////////////////////////////////////////////////
//
//  Remove a clipping plane from the scene
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::removePlane ( osg::ClipPlane *plane )
{
  _sceneManager->clipNode()->removeClipPlane ( plane );
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
    _sceneManager->clipNode()->removeClipPlane ( i->get() );

  osg::ref_ptr< osg::Group > group ( _sceneManager->groupGet ( OsgTools::Render::Constants::CLIPPING_PLANES ) );

  group->removeChild( 0, group->getNumChildren() );
  
  this->changedScene();
  this->render();
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
  case Usul::Interfaces::IAxes::IID:
    return static_cast < Usul::Interfaces::IAxes* > ( this );
  case Usul::Interfaces::IExport::IID:
    return static_cast < Usul::Interfaces::IExport* > ( this );
  case Usul::Interfaces::IBoundingBox::IID:
    return static_cast < Usul::Interfaces::IBoundingBox* > ( this );
  case Usul::Interfaces::IBoundingSphere::IID:
    return static_cast < Usul::Interfaces::IBoundingSphere* > ( this );
  case Usul::Interfaces::IOpenGLState::IID:
    return static_cast < Usul::Interfaces::IOpenGLState* > ( this );
  case Usul::Interfaces::IOpenSceneGraph::IID:
    return static_cast<Usul::Interfaces::IOpenSceneGraph*>(this);
  case Usul::Interfaces::ISelectionBox::IID:
    return static_cast<Usul::Interfaces::ISelectionBox*>(this);
  case Usul::Interfaces::IStereo::IID:
    return static_cast < Usul::Interfaces::IStereo* > ( this );
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
  case Usul::Interfaces::IViewer::IID:
    return static_cast < Usul::Interfaces::IViewer* > ( this );
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
  case Usul::Interfaces::IMode::IID:
    return static_cast < Usul::Interfaces::IMode* > ( this );
  case Usul::Interfaces::ISetTool::IID:
    return static_cast<Usul::Interfaces::ISetTool*>(this);
  case Usul::Interfaces::ISpin::IID:
    return static_cast < Usul::Interfaces::ISpin* > ( this );
  case Usul::Interfaces::IBackground::IID:
    return static_cast < Usul::Interfaces::IBackground* > ( this );
  case Usul::Interfaces::IHeliosView::IID:
    return static_cast < Usul::Interfaces::IHeliosView* > ( this );
  case Usul::Interfaces::ILights::IID:
    return static_cast < Usul::Interfaces::ILights* > ( this );
  case Usul::Interfaces::ISceneStage::IID:
    return static_cast < Usul::Interfaces::ISceneStage * > ( this );
  case Usul::Interfaces::ICenterOfRotation::IID:
    return static_cast < Usul::Interfaces::ICenterOfRotation * > ( this );
  case Usul::Interfaces::IScreenCapture::IID:
    return static_cast < Usul::Interfaces::IScreenCapture * > ( this );
  case Usul::Interfaces::ISnapShot::IID:
    return static_cast < Usul::Interfaces::ISnapShot* > ( this );
  case Usul::Interfaces::IView::IID:
    return static_cast < Usul::Interfaces::IView * > ( this );
  default:
    return 0x0;
  }
  
}

///////////////////////////////////////////////////////////////////////////////
//
//  Reference this instance.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::ref()
{
  // Keep track for debugging purposes.
  ++_refCount;

  BaseClass::ref();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Unreference this instance.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::unref ( bool allowDeletion )
{
  // Keep track for debugging purposes.
  --_refCount;

  BaseClass::unref(allowDeletion);
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

void Viewer::_addAxes ()
{
  // Get the group.  It will be under a projection node that is set which each resize, so the geometry under it
  // won't get distorted by screen resizes.
  osg::ref_ptr< osg::Group > group ( _sceneManager->projectionGroupGet ( OsgTools::Render::Constants::AXES ) );

  // Axes Dragger
  osg::ref_ptr< OsgTools::Widgets::Axes > dragger ( new OsgTools::Widgets::Axes );
  dragger->setMatrix( osg::Matrix::translate ( osg::Vec3( 50, 50, -100 ) ) );
  dragger->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
  dragger->useWindowCoords ( true );

  group->addChild( dragger.get() );
  
  // The scene has changed.
  this->changedScene();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove the axes.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::_removeAxes ()
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

void Viewer::_setAxes ()
{
  // Return now if no axes
  if( !this->axes() )
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

void Viewer::axes( bool b )
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
//  Get the show axes state.
//
///////////////////////////////////////////////////////////////////////////////

bool Viewer::axes() const
{
  return Usul::Bits::has < unsigned int, unsigned int > ( _flags, _SHOW_AXES );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the view matrix.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::setViewMatrix ( const osg::Matrixf& matrix )
{
  this->viewer()->setViewMatrix ( matrix );
}

///////////////////////////////////////////////////////////////////////////////
//
//  Set the view matrix.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::setViewMatrix ( const osg::Matrixd& matrix )
{
  this->viewer()->setViewMatrix ( matrix );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the view matrix.
//
///////////////////////////////////////////////////////////////////////////////

const osg::Matrixd& Viewer::getViewMatrix ( ) const
{
  return this->viewer()->getViewMatrix ( );
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
//  Set the state of showing axes. Usul::Interfaces::IAxes.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::setAxes ( bool b )
{
  this->axes( b );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the show axes state.  Usul::Interfaces::IAxes.
//
///////////////////////////////////////////////////////////////////////////////

bool Viewer::hasAxes ( ) const
{
  return this->axes();
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
//  Draw selection box with given vertices
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::drawSelectionBox ( const osg::Vec3 &v1, const osg::Vec3 &v2 )
{
  this->_drawSelectionBox( v1, v2 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove the selection box from the scene
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::removeSelectionBox()
{
  this->_removeSelectionBox();
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
  const bool dump ( this->dumpFrames() );
  FrameDump fd ( dump, dir, base, ext, start, digits );
  this->frameDump ( fd );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Turn on/off frame-dumping.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::dumpFrames ( bool state )
{
  this->frameDump().dump ( state );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Are we dumping frames?
//
///////////////////////////////////////////////////////////////////////////////

bool Viewer::dumpFrames() const
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
//  Get group with given key.  Creates one if doesn't exist
//
///////////////////////////////////////////////////////////////////////////////

osg::Group*  Viewer::getGroup    ( const std::string& key )
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

bool Viewer::hasGroup    ( const std::string& key )
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
  // Return the scene's bounding box.
  osg::BoundingBox bb;
  bb.expandBy ( this->scene()->getBound() );
  return bb;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear the viewer
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::clearScene()
{
  this->scene( 0x0 );
}
 

///////////////////////////////////////////////////////////////////////////////
//
//  Get the location of the viewport.
//
///////////////////////////////////////////////////////////////////////////////

int Viewer::x() const
{
  return this->viewer()->getViewport()->x();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the location of the viewport.
//
///////////////////////////////////////////////////////////////////////////////

int Viewer::y() const
{
  return this->viewer()->getViewport()->y();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get height
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Viewer::height() const
{
  return static_cast < unsigned int > ( Usul::Math::absolute ( this->viewer()->getViewport()->height() ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get width
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Viewer::width() const
{
  return static_cast < unsigned int > ( Usul::Math::absolute ( this->viewer()->getViewport()->width() ) );
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

bool Viewer::_lineSegment ( float mouseX, float mouseY, osg::Vec3 &pt0, osg::Vec3 &pt1, bool useWindowCoords )
{
  // Handle no viewer.
  if ( !this->viewer() )
    return false;

  // Declare the event adapter.
  EventAdapter::Ptr ea ( new EventAdapter );
  ea->setWindowSize ( Usul::Math::Vec2ui( this->width(), this->height() ) );
  ea->setMouse ( Usul::Math::Vec2f( mouseX, mouseY ) );

  // Get the necessary coordinates.
  float x ( ea->getXnormalized() );
  float y ( ea->getYnormalized() );

  // Use window coordinates.  No projection.
  if ( useWindowCoords )
  {
    // Set the two points for our line-segment.
    pt0 = osg::Vec3 ( x, y, -1 ) * osg::Matrix::inverse( _sceneManager->projection()->getMatrix() );
    pt1 = osg::Vec3 ( x, y,  1 ) * osg::Matrix::inverse( _sceneManager->projection()->getMatrix() );
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
    pt0 = osg::Vec3 ( x, y, -1 ) * IMP;
    pt1 = osg::Vec3 ( x, y,  1 ) * IMP;
  }

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Intersect the scene.
//
///////////////////////////////////////////////////////////////////////////////

bool Viewer::_intersect ( float x, float y, osg::Node *scene, osgUtil::Hit &hit, bool useWindowCoords )
{
  // Handle no scene or viewer.
  if ( !scene || !this->viewer() )
    return false;

  // Calculate the two points for our line-segment.
  osg::Vec3 pt0, pt1;
  if ( !this->_lineSegment ( x, y, pt0, pt1, useWindowCoords ) )
    return false;

  // Declare the pick-visitor.
  typedef osgUtil::IntersectVisitor Visitor;
  osg::ref_ptr<Visitor> visitor ( new Visitor );

  // Make the line segment.
  osg::ref_ptr<osg::LineSegment> ls ( new osg::LineSegment ( pt0, pt1 ) );
  visitor->addLineSegment ( ls.get() );

#if 0
  osg::Group *group = scene->asGroup();
  OsgTools::Ray ray;
  ray.start ( pt0 );
  ray.end ( pt1 );
  ray.thickness ( 3 );
  static osg::ref_ptr<osg::Node> node;
  if ( group )
    group->removeChild ( node.get() );
  node = ray();
  std::ostringstream out;
  out << "pt0 = " << pt0[0] << ' ' << pt0[1] << ' ' << pt0[2] << ", "
      << "pt1 = " << pt1[0] << ' ' << pt1[1] << ' ' << pt1[2] << '\n';
  ::OutputDebugString ( out.str().c_str() );
#endif

  // Intersect the scene.
  typedef osgUtil::IntersectVisitor::HitList HitList;
  scene->accept ( *visitor );

#if 0
  if ( group )
    group->addChild ( node.get() );
#endif

  // See if there was an intersection.
  if ( !visitor->hits() )
    return false;

  // Get the hit-list for our line-segment.
  const HitList &hits = visitor->getHitList ( ls.get() );
  if ( hits.empty() )
    return false;

  // Set the hit.
  hit = hits.front();

#if 0
  std::ostringstream message;
  message << "hit._primitiveIndex = " << hit._primitiveIndex << std::endl;
  for ( unsigned int i = 0; i < hit._vecIndexList.size(); ++i )
    message << "hit._vecIndexList[" << i << "] = " << hit._vecIndexList[i] << std::endl;
  FX::FXMessageBox::information ( this, FX::MBOX_OK, "Intersection", message.str().c_str() );
#endif

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  See if event intersects the current scene
//
///////////////////////////////////////////////////////////////////////////////

bool Viewer::intersect ( float x, float y, osgUtil::Hit &hit ) 
{ 
  return _intersect( x, y, this->model(), hit ); 
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the trackball's center.
//
///////////////////////////////////////////////////////////////////////////////

osg::Vec3 Viewer::getCenter()
{
  return this->_trackball()->center();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the trackball's center.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::setCenter( const osg::Vec3 &c )
{
  this->setTrackball( c, this->getDistance(), this->getRotation(), true, true );
}

///////////////////////////////////////////////////////////////////////////////
//
//  Get the trackball's distance.
//
///////////////////////////////////////////////////////////////////////////////

float Viewer::getDistance()
{
  return this->_trackball()->distance(); 
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the trackball's distance.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::setDistance( float d )
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

void Viewer::setTrackball ( const osg::Vec3& center, float distance , const osg::Quat& rot, bool makeTrackball, bool setViewerToo )
{
  if ( !this->viewer() )
    return;

  // Make sure we have a trackball.
  Trackball::Ptr trackball ( dynamic_cast < Trackball * > ( this->navManip() ) );
  if ( !trackball.valid() && makeTrackball )
  {
    trackball = new Trackball;
    this->navManip ( trackball.get() );
  }

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
  if( trackball.valid() ) 
    return trackball.get(); 
  throw std::runtime_error ( "Error 3608089520: Trackball is not valid." );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Edit Material above hit, if it exists
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::_editMaterial  ( osgUtil::Hit &hit )
{
  //First check the drawable's state set
  osg::ref_ptr < osg::StateSet > stateset ( hit._drawable->getOrCreateStateSet() );
  osg::ref_ptr < osg::Material > mat ( reinterpret_cast < osg::Material* > ( stateset->getAttribute( osg::StateAttribute::MATERIAL ) ) );

  // If no material...
  if ( !mat.valid() )
  {
    osg::NodePath path ( hit._nodePath );

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

void Viewer::_findDragger ( const osgUtil::Hit &hit )
{
  // Initialize.
  Dragger *dragger = 0x0;
  osg::NodePath path ( hit._nodePath );

  // Loop through the nodes.
  for ( osg::NodePath::reverse_iterator i = path.rbegin(); i != path.rend(); ++i )
  {
    // See if it is a dragger.
    dragger = dynamic_cast < Dragger * > ( *i );
    if ( dragger )
      break;
  }

  // If we did not find a dragger...
  if ( 0x0 == dragger )
  {
    Dragger::dragging ( 0x0, osg::Matrixd::identity() );
    return;
  }

  // We found a dragger. Loop again from the root of the path.
  osg::Matrixd cumulative ( osg::Matrixd::identity() );
  for ( osg::NodePath::const_iterator i = path.begin(); i != path.end(); ++i )
  {
    // Stop at our dragger.
    if ( ( *i ) == static_cast < osg::Node * > ( dragger ) )
      break;

    // If it is a transform then accumulate.
    osg::MatrixTransform *mt = dynamic_cast < osg::MatrixTransform * > ( *i );
    if ( mt )
      cumulative = mt->getMatrix() * cumulative;
  }

  // Set the dragging data.
  Dragger::dragging ( dragger, cumulative );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the current mode
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::setMode ( ViewMode mode ) 
{ 
  _currentMode = mode;
  this->doneTool();
  this->updateCursor(); 
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the current tool
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::setTool ( Usul::Interfaces::ITool *tool )
{
  // Clean up existing tool if we can.
  Usul::Interfaces::ICleanUp::QueryPtr cleanUp ( _currentTool );
  if ( cleanUp.valid() )
    cleanUp->cleanUp ( this->queryInterface ( Usul::Interfaces::IUnknown::IID ) );

  // Set new tool.
  _currentTool = tool;

  // Update the cursor.
  this->updateCursor();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the current tool
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::ITool *Viewer::getTool()
{
  return _currentTool.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Release the current tool.  Save it in last tool.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::doneTool()
{
  Usul::Interfaces::ICleanUp::QueryPtr cleanUp ( _currentTool );
  if ( cleanUp.valid() )
    cleanUp->cleanUp ( this->queryInterface ( Usul::Interfaces::IUnknown::IID ) );

  if ( _currentTool.valid() )
  {
    _lastTool = _currentTool;
    _currentTool = static_cast < ITool * > ( 0x0 );
    this->updateCursor();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Load the last tool
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::loadLastTool()
{
  if ( _lastTool.valid() )
  {
    _currentTool = _lastTool;
    this->updateCursor();
  }
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
    break;
  }
  this->updateCursor();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the cursor. This matches the behavior of
//  osgGA::TrackballManipulator::calcMovement().
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::_setNavCursor ( bool left, bool middle, bool right, EventAdapter::EventType type )
{
  if ( !_setCursor.valid() )
    return;

  if ( EventAdapter::PUSH == type || EventAdapter::DRAG == type )
  {
    if( left && right || middle )
      _setCursor->setCursorTrans();
    else if ( left )
      _setCursor->setCursorRot();
    else if ( right )
      _setCursor->setCursorScale();
  }

  else if ( EventAdapter::RELEASE == type )
  {
    _setCursor->setCursorNav();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the cursor.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::updateCursor()
{
  if ( _setCursor.valid() )
    _setCursor->updateCursor();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when a mouse button is pressed.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::buttonPress ( float x, float y, bool left, bool middle, bool right )
{
  // Stop any spin.
  this->spin ( false );

  // Set the lod-callbacks if we are suppose to.
  if ( Usul::Shared::Preferences::instance().getBool( Usul::Registry::Keys::LOW_LODS ) )
    this->_setLodCullCallback ( new OsgTools::Render::LowLodCallback );

  // Handle the navigation event.
  this->handleNavigation ( x, y, left, middle, right, EventAdapter::PUSH );

  // Handle any tool events.
  this->handleTool ( left, middle, right, false, x, y, 0.0 );

}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when a mouse button is released.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::buttonRelease ( float x, float y, bool left, bool middle, bool right )
{
  // Set the lod-callbacks.
  this->_setLodCullCallback ( 0x0 );
  //this->_setLodCullCallback ( new Detail::HighLodCallback );

  // Handle the event.
  this->handleNavigation ( x, y, left, middle, right, EventAdapter::RELEASE );

  // Handle any tool events.
  this->handleTool ( left, middle, right, false, x, y, 0.0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Handle the navigation event.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::handleNavigation ( float x, float y, bool left, bool middle, bool right, EventAdapter::EventType type )
{
  // Make sure we are in navigation mode.
  if ( !this->navigating() || this->tool())
    return;

  // Handle no manipulator, scene, or viewer.
  if ( !this->navManip() || !this->scene() || !this->viewer() )
    return;

  // Declare the adapters.
  EventAdapter::Ptr ea ( new EventAdapter );
  ActionAdapter aa ( this->queryInterface( Usul::Interfaces::IUnknown::IID ) );

  // Set the event-adapter. Order is important.
  ea->setSeconds ( Usul::System::Clock::seconds() );
  ea->setWindowSize ( Usul::Math::Vec2ui( this->width(), this->height() ) );
  ea->setMouse ( Usul::Math::Vec2f ( x, y ) );

  ea->setButton ( left, middle, right );
  ea->setButtonMask ( left, middle, right );
  ea->setEventType ( type );

  // Ask the manipulator to handle the event.
  if ( this->navManip()->handle ( *ea, aa ) )
  {
    // Set the navigation cursor.
    this->_setNavCursor ( left, middle, right, type );

    // Set the scene's matrix.
    this->viewer()->setViewMatrix ( this->navManip()->getInverseMatrix() );

    // Render the scene unless this is the first button press.
    if ( EventAdapter::PUSH != type )
    {
      // If button release, render using the proper number of passes
      if ( EventAdapter::RELEASE == type )
        this->render();

      // If it's anything else, render using 1 render pass.  Reset to proper value.
      else
      {
        unsigned int oldValue ( this->numRenderPasses() );
        
        try
        {
          this->numRenderPasses ( 1 );
          this->render();
        }

        // Catch any exceptions and reset the number of rendering passes.
        catch ( ... )
        {
          this->numRenderPasses ( oldValue );
          throw;
        }

        this->numRenderPasses ( oldValue );
      }
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Handle the picking event.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::handlePicking ( float x, float y, bool left, unsigned int numClicks )
{
  // Make sure we are in picking mode.
  if ( !this->picking() )
    return;

  // Stop any spinning.
  this->spin ( false );
 
  // Intersect the scene.
  osgUtil::Hit hit;
  if ( !this->_intersect ( x, y, this->scene(), hit, false ) )
  {
    // If we didn't intersect with the scene, try intersecting with the screen projection matrix
    // using just the screen coordniates.
    osg::ref_ptr< osg::MatrixTransform > root ( new osg::MatrixTransform );
    root->setMatrix ( osg::Matrix::identity() );
    root->addChild( _sceneManager->projection() );

    // List of transforms that had the reference frame changed from absolute to relative.
    typedef std::vector< osg::ref_ptr< osg::Transform > > TList;
    TList tList;

    // Change all matrix transforms from absolute reference frame to relative.
    OsgTools::Utilities::setReferenceFrame( root.get(), osg::Transform::ABSOLUTE_RF, osg::Transform::RELATIVE_RF, tList );

    // Change all dirty bounds flags to true.
    OsgTools::Utilities::setDirtyBounds( root.get() );

    bool result ( this->_intersect ( x, y, root.get(), hit, true ) );

    // Change all matrix transforms that were changed before back to absolute.
    for( TList::iterator iter = tList.begin(); iter != tList.end(); ++iter )
      (*iter)->setReferenceFrame( osg::Transform::ABSOLUTE_RF );

    // Clear the list
    tList.clear();

    // Change all dirty bounds flags to true.
    OsgTools::Utilities::setDirtyBounds( root.get() );

    // Left mouse button was pressed...
    if ( left )
    {
      // Set the dragger being dragged, if there is one.
      this->_findDragger ( hit );

      // Edit the light, if any.
      if ( 2 == numClicks )
        this->_editLight ( hit );
    }

    // Un ref the matrix transform
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

    //Was this a double left click?
    if ( 2 == numClicks )
      this->_editMaterial ( hit );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Handle dragging.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::handleDragging ( float x, float y, Dragger::Command command )
{
  {
    // Lock the dragger.
    Dragger::Guard guard ( Dragger::dragging().mutex() );

    // Make sure we are in picking mode, and there is a dragger.
    if ( false == this->picking() || 0x0 == Dragger::dragging().value().dragger.get() )
      return;

    // Calculate the two points for our line-segment.
    osg::Vec3 pt0, pt1;
    if ( !this->_lineSegment ( x, y, pt0, pt1, Dragger::dragging().value().dragger->useWindowCoords() ) )
      return;

    // Have the dragger execute the command.
    Dragger::dragging().value().dragger->execute ( command, pt0, pt1 );
  }

  // Redraw.
  this->render();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Handle tool event.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::handleTool ( bool left, bool middle, bool right, bool motion, float x, float y, float z )
{
  // Check to see if we are in tool-mode.
  if ( !this->tool() )
    return;

  // If we have a tool command...
  if ( _currentTool.valid() )
  {
    // Execute the tool command.
    if ( _currentTool->execute ( this->queryInterface ( Usul::Interfaces::IUnknown::IID ), left, middle, right, motion, x, y, z ) )
    {
      // If the control-key is not down...
      //if ( !( event.state & FX::CONTROLMASK ) )
      {
        // We are done with this tool.
        Usul::Interfaces::IToolLifeTime::QueryPtr toolLifeTime ( _currentTool );
        if( toolLifeTime.valid() && toolLifeTime->temporary() )        
        {
          this->doneTool();
        }
      }
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Handle seek event.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::handleSeek ( float x, float y, bool left )
{
  // Retrun now if we aren't in seek mode.
  if( _currentMode != SEEK || false == left )
    return;

  //Find interface to animate, if one exists
  Usul::Interfaces::IAnimate::QueryPtr animate ( Usul::Components::Manager::instance().getInterface( Usul::Interfaces::IAnimate::IID ) );

  osgUtil::Hit hit;

  //Return if the click didn't intersect the scene
  if ( !this->intersect ( x, y, hit ) )
    return;

  //Make copy of trackball's current rotation
  const osg::Quat rot ( this->getRotation() );
  const osg::Vec3 center ( this->getCenter() );
  const float distance ( this->getDistance() );

  osg::Matrix m ( osg::Matrixd::translate(0.0,0.0,distance)*
                osg::Matrixd::rotate(rot)*
                osg::Matrixd::translate(center) );

  osg::Vec3 eye, c, up;

  // Get the eye position.
  m.inverse( m ).getLookAt ( eye, c, up );

  // Get the new center and distance.
  const osg::Vec3 c2 ( hit.getWorldIntersectPoint() );

  osg::Vec3 axis2 ( c2 - eye );

  const float d2 ( axis2.length() );

  //Use the animation interface if we found a valid one
  if( animate.valid() )
  {
    animate->animate ( c2, d2, rot );
  }

  //If no animation interface exists, just set the trackball
  else
  {
    this->setTrackball( c2, d2, rot, true, true );

    this->render();
  }
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
    if ( Usul::Shared::Preferences::instance().getBool ( Usul::Registry::Keys::ALLOW_SPIN ) )
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
  if ( false == Usul::Shared::Preferences::instance().getBool ( Usul::Registry::Keys::ALLOW_SPIN ) )
    return;

  // Handle bad cases.
  if ( !this->navManip() || !this->navigating() || !this->viewer() )
    return;

  // Make the adapters.
  EventAdapter::Ptr ea ( new EventAdapter );
  ActionAdapter aa ( this->queryInterface( Usul::Interfaces::IUnknown::IID ) );
  ea->setWindowSize ( Usul::Math::Vec2ui ( this->width(), this->height() ) );
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
//  Show the lights in the scene.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::_showLights()
{
  osg::ref_ptr<osg::Group> group ( _sceneManager->projectionGroupGet ( OsgTools::Render::Constants::LIGHT ) );

  // Get the light from the scene view.
  osg::ref_ptr< osg::Light > light ( this->viewer()->getLight() );

#if 0
  // This is for positional lights...TODO
  // Get the bounding sphere.  The light will move along the bounding sphere.
  osg::BoundingSphere bs ( this->scene()->getBound() );

  osg::Vec4 position ( light->getPosition() );
  osg::Vec3 pos ( position.x(), position.y(), position.z() );

  osg::Vec3 v0 ( bs.center() + pos * bs.radius() );

  double length ( bs.radius() * 0.25 );

  osg::Vec3 v1 ( v0 + light->getDirection() * length );
#endif

  osg::ref_ptr< osg::Geode > geode ( new osg::Geode );

  // Radius and height for cones.
  const double radius ( 5.0 );
  const double height ( 20.0 );

  // Length of the cylinder
  const double length ( 50.0 );

  osg::Vec3 x  ( length, 0.0, 0.0 );

  osg::Quat quat ( osg::inDegrees(90.0), osg::Vec3( 0.0, 1.0, 0.0 )  );

  {
    osg::Vec3 center  ( length / 2.0, 0.0, 0.0 );

    osg::ref_ptr < osg::Cylinder > cylinder ( new osg::Cylinder ( center, radius / 2.0, length ) );
    cylinder->setRotation( quat );
    osg::ref_ptr< osg::ShapeDrawable > sd ( new osg::ShapeDrawable( cylinder.get() ) );

    geode->addDrawable ( sd.get() );
  }

  {
    osg::ref_ptr < osg::Cone > cone ( new osg::Cone( x, radius, height ) );
    cone->setRotation( quat );
    osg::ref_ptr< osg::ShapeDrawable > sd ( new osg::ShapeDrawable( cone.get() ) );

    geode->addDrawable ( sd.get() );
  }

  // Set the material
  osg::ref_ptr< osg::Material > mat ( new osg::Material );
  mat->setDiffuse ( osg::Material::FRONT, light->getDiffuse() );

  osg::ref_ptr< osg::StateSet > ss ( geode->getOrCreateStateSet () );
  ss->setAttribute ( mat.get(), osg::StateAttribute::ON );

  // Add the geode to the container of light editors.
  _lightEditors.insert ( LightEditors::value_type ( geode.get(), light.get() ) );

  osg::Vec3 lightDirection ( light->getDirection() );
  lightDirection.normalize();

  // Make a dragger
  osg::ref_ptr< Dragger > dragger ( new OsgTools::Draggers::Trackball ( this->queryInterface( Usul::Interfaces::IUnknown::IID ) ) );

  // Set the geometrys
  dragger->geometry( Dragger::NORMAL, geode.get() );
  dragger->geometry( Dragger::DRAGGING, geode.get() );

  osg::ref_ptr< LightCallback > cb ( new LightCallback ( light.get() ) );
  dragger->callback ( Dragger::MOVE, cb.get() );
  dragger->callback ( Dragger::FINISH, cb.get() );
  dragger->useWindowCoords ( true );

  osg::ref_ptr < osg::MatrixTransform > mt = new osg::MatrixTransform;
  mt->setMatrix( osg::Matrix::rotate( x, lightDirection ) );
  mt->addChild( dragger.get() );

  // Create a matrix transform.
  osg::ref_ptr < osg::MatrixTransform > modelview_abs ( new osg::MatrixTransform );
  modelview_abs->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
  modelview_abs->setMatrix( osg::Matrix::translate ( osg::Vec3( this->width() - 50, 50, -100 ) ) );

  modelview_abs->addChild ( mt.get() );

  group->addChild( modelview_abs.get() );

  // The scene has changed.
  this->changedScene();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove the lights in the scene.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::_removeLights()
{
  _sceneManager->projectionGroupRemove ( OsgTools::Render::Constants::LIGHT );

  // The scene has changed.
  this->changedScene();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the light positions in the scene.  This is for directional lights 
//  (i.e. lights that aren't changed by the model view matrix).
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::_setLights()
{

}


///////////////////////////////////////////////////////////////////////////////
//
//  Edit the light that was clicked on
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::_editLight ( osgUtil::Hit &hit )
{
  LightEditors::iterator iter = _lightEditors.find( hit.getGeode() );

  if( iter != _lightEditors.end() )
  {
#if 0
    // Get the light.
    osg::ref_ptr< osg::Light > light ( iter->second );

    LightEditor le ( this, light.get(), this->queryInterface( Usul::Interfaces::IUnknown::IID ) );
    le.show();

    osg::ref_ptr< osg::Material > mat ( new osg::Material );
    mat->setAmbient ( osg::Material::FRONT, light->getAmbient() );
    mat->setDiffuse ( osg::Material::FRONT, light->getDiffuse() );
    mat->setSpecular ( osg::Material::FRONT, light->getSpecular() );

    osg::ref_ptr< osg::StateSet > ss ( iter->first->getOrCreateStateSet () );
    ss->setAttribute ( mat.get(), osg::StateAttribute::ON );
#endif
    std::cout << "Warning: Not implemented." << std::endl;
  }

}


///////////////////////////////////////////////////////////////////////////////
//
//  Add default clipping plane.
//  Usul::Interfaces::IClippingPlanes
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::addClippingPlane ( )
{
  this->addPlane();
}

///////////////////////////////////////////////////////////////////////////////
//
//  Add clipping plane defined by given plane.
//  Usul::Interfaces::IClippingPlanes
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::addClippingPlane ( const osg::Plane& plane )
{
  this->addPlane ( plane );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add 6 clipping planes defined by the given bounding box.
//  Usul::Interfaces::IClippingPlanes
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::addClippingBox   ( const osg::BoundingBox& bb )
{
  this->addClipBox( bb );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove clipping plane with given index.
//  Usul::Interfaces::IClippingPlanes
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::removeClippingPlane ( unsigned int index )
{
  this->removePlane( index );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove all clipping planes.
//  Usul::Interfaces::IClippingPlanes
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::removeClippingPlanes (  )
{
  this->removePlanes();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Are the lights drawn in the scene?
//
///////////////////////////////////////////////////////////////////////////////

bool Viewer::lights() const
{
  return Usul::Bits::has < unsigned int, unsigned int > ( _flags, _SHOW_LIGHTS );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Hide/Show the lights in the scene.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::lights( bool b )
{
  if ( b )
  {
    _flags = Usul::Bits::add < unsigned int, unsigned int > ( _flags, _SHOW_LIGHTS );
    this->_showLights();
  }
  else
  {
    _flags = Usul::Bits::remove < unsigned int, unsigned int > ( _flags, _SHOW_LIGHTS );
    this->_removeLights();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Hide/Show the lights in the scene
//  Usul::Interfaces::ILights
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::setLights ( bool b )
{
  this->lights( b );
}

///////////////////////////////////////////////////////////////////////////////
//
//  Are the lights drawn in the scene?
//  Usul::Interfaces::ILights
//
///////////////////////////////////////////////////////////////////////////////

bool Viewer::hasLights ( ) const
{
  return this->lights();
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
//  Set the FOV.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::fovSet ( double fov )
{
  Usul::Shared::Preferences::instance().setDouble( Usul::Registry::Keys::FOV, fov );

  unsigned int width  ( this->width() );
  unsigned int height ( this->height() );
  this->resize ( width, height );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the fov.
//
///////////////////////////////////////////////////////////////////////////////

double Viewer::fovGet () const
{
  return Usul::Shared::Preferences::instance().getDouble( Usul::Registry::Keys::FOV );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Need interface for adding chore, for now just render.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::addChoreRedraw()
{
  this->render();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Need interface for handling a message.  For now do nothing.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::handleMessage ( unsigned short message )
{
  Usul::Interfaces::IHandleMessage::QueryPtr hm ( _caller );

  if ( hm.valid() )
    hm->handleMessage ( message );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Edit the background.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::editBackground()
{
  Usul::Interfaces::IBackground::QueryPtr bg ( _caller );

  if( bg.valid() )
    bg->editBackground();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Edit the background.
//
///////////////////////////////////////////////////////////////////////////////

bool Viewer::timeoutAnimate ()
{
  // Get the matrix and see if we should continue.
  osg::Matrixd mat;
  bool animate ( _animation.matrix ( mat ) ) ;

  // Set the viewer's matrix.
  this->viewer()->setViewMatrix ( mat );
  this->render();

  return animate;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Zoom by given amount
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::zoom ( double delta )
{
  if ( delta == 0.0 ) {
    return;
  }
  
  float distance ( this->getDistance() );

  osg::BoundingBox bb ( this->getBoundingBox() );

  distance += ( delta * ( .1 * bb.radius() ) ) / Usul::Math::absolute ( delta );

  this->setDistance( distance );

  this->render();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is the scene stage on?
//
///////////////////////////////////////////////////////////////////////////////

bool Viewer::sceneStage() const
{
  return Usul::Bits::has < unsigned int, unsigned int > ( _flags, _SHOW_STAGE );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the show scene stage flag.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::sceneStage( bool b )
{
  if ( b )
  {
    _flags = Usul::Bits::add < unsigned int, unsigned int > ( _flags, _SHOW_STAGE );
    this->_addSceneStage();
  }
  else
  {
    _flags = Usul::Bits::remove < unsigned int, unsigned int > ( _flags, _SHOW_STAGE );
    this->_removeSceneStage();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add scene stage.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::_addSceneStage()
{
  if ( 0x0 == this->model() )
    return;

  GroupPtr group ( _sceneManager->groupGet ( OsgTools::Render::Constants::STAGE ) );

  osg::BoundingSphere bs ( this->scene()->getBound() );
  
  bs._radius = bs._radius *  1.25;

  osg::BoundingBox bb;
  bb.expandBy ( bs );

  float width_factor = 1.5;
  //float height_factor = 0.3;
  
  float xMin = bs.center().x()-bs.radius()*width_factor;
  float xMax = bs.center().x()+bs.radius()*width_factor;
  float yMin = bs.center().y()-bs.radius()*width_factor;
  float yMax = bs.center().y()+bs.radius()*width_factor;
  
  float z = bs.center().z()-bs.radius(); /**height_factor;*/

  if ( bb.valid() )
  {
    osg::ref_ptr < osg::Geometry > geometry ( new osg::Geometry );

    osg::Vec3Array* coords = new osg::Vec3Array(4);
    (*coords)[0].set(xMin,yMax,z);
    (*coords)[1].set(xMin,yMin,z);
    (*coords)[2].set(xMax,yMin,z);
    (*coords)[3].set(xMax,yMax,z);
    geometry->setVertexArray(coords);

    osg::ref_ptr < osg::Vec3Array > normals ( new osg::Vec3Array ( 1 ) );
    normals->at ( 0 ).set ( 0.0, 0.0, 1.0 );

    geometry->setNormalArray ( normals.get() );
    geometry->setNormalBinding ( osg::Geometry::BIND_PER_PRIMITIVE );

    osg::ref_ptr < osg::Vec4Array > colors ( new osg::Vec4Array ( 1 ) );
    colors->at( 0 ).set ( 1.0, 1.0, 1.0, 1.0 );

    geometry->setColorArray ( colors.get() );
    geometry->setColorBinding ( osg::Geometry::BIND_PER_PRIMITIVE );

    osg::ref_ptr < osg::Vec2Array > tcoords ( new osg::Vec2Array(4) );
    tcoords->at(0).set( 0.0f, 1.0f );
    tcoords->at(1).set( 0.0f, 0.0f );
    tcoords->at(2).set( 1.0f, 0.0f );
    tcoords->at(3).set( 1.0f, 1.0f );
    geometry->setTexCoordArray( 0,tcoords.get() );

    geometry->addPrimitiveSet ( new osg::DrawArrays ( osg::PrimitiveSet::QUADS, 0, coords->size() ) );

    osg::MatrixTransform* rootNode = new osg::MatrixTransform;
    //rootNode->setMatrix(osg::Matrix::rotate(osg::inDegrees(45.0f),1.0f,0.0f,0.0f));
    group->addChild ( rootNode );

    // make sure that the global color mask exists.
    osg::ColorMask* rootColorMask = new osg::ColorMask;
    rootColorMask->setMask(true,true,true,true);        
    
    // set up depth to be inherited by the rest of the scene unless
    // overrideen. this is overridden in bin 3.
    osg::Depth* rootDepth = new osg::Depth;
    rootDepth->setFunction(osg::Depth::LESS);
    rootDepth->setRange(0.0,1.0);

    osg::StateSet* rootStateSet = this->scene()->getOrCreateStateSet();
    rootStateSet->setAttribute(rootColorMask);
    rootStateSet->setAttribute(rootDepth);


    // bin1  - set up the stencil values and depth for mirror.
    {
    
      // set up the stencil ops so that the stencil buffer get set at
      // the mirror plane 
      osg::Stencil* stencil = new osg::Stencil;
      stencil->setFunction(osg::Stencil::ALWAYS,1,~0u);
      stencil->setOperation(osg::Stencil::KEEP, osg::Stencil::KEEP, osg::Stencil::REPLACE);
      
      // switch off the writing to the color bit planes.
      osg::ColorMask* colorMask = new osg::ColorMask;
      colorMask->setMask(false,false,false,false);
      
      osg::StateSet* statesetBin1 = new osg::StateSet();        
      statesetBin1->setRenderBinDetails(1,"RenderBin");
      statesetBin1->setMode(GL_CULL_FACE,osg::StateAttribute::OFF);
      statesetBin1->setAttributeAndModes(stencil,osg::StateAttribute::ON);
      statesetBin1->setAttribute(colorMask);
      
      // set up the mirror geode.
      osg::ref_ptr < osg::Geode > geode ( new osg::Geode );
      geode->addDrawable ( geometry.get() );
      geode->setStateSet(statesetBin1);
      
      rootNode->addChild( geode.get() );
        
    }

    // bin one - draw scene without mirror or reflection, unset 
    // stencil values where scene is infront of mirror and hence
    // occludes the mirror. 
    {        
        osg::Stencil* stencil = new osg::Stencil;
        stencil->setFunction(osg::Stencil::ALWAYS,0,~0u);
        stencil->setOperation(osg::Stencil::KEEP, osg::Stencil::KEEP, osg::Stencil::REPLACE);

        osg::StateSet* statesetBin2 = this->model()->getOrCreateStateSet();
        statesetBin2->setRenderBinDetails(2,"RenderBin");
        statesetBin2->setAttributeAndModes(stencil,osg::StateAttribute::ON);
        
    }
        
    // bin3  - set up the depth to the furthest depth value
    {
    
      // set up the stencil ops so that only operator on this mirrors stencil value.
      osg::Stencil* stencil = new osg::Stencil;
      stencil->setFunction(osg::Stencil::EQUAL,1,~0u);
      stencil->setOperation(osg::Stencil::KEEP, osg::Stencil::KEEP, osg::Stencil::KEEP);
      
      // switch off the writing to the color bit planes.
      osg::ColorMask* colorMask = new osg::ColorMask;
      colorMask->setMask(false,false,false,false);

      // set up depth so all writing to depth goes to maximum depth.
      osg::Depth* depth = new osg::Depth;
      depth->setFunction(osg::Depth::ALWAYS);
      depth->setRange(1.0,1.0);

      osg::StateSet* statesetBin3 = new osg::StateSet();
      statesetBin3->setRenderBinDetails(3,"RenderBin");
      statesetBin3->setMode(GL_CULL_FACE,osg::StateAttribute::OFF);
      statesetBin3->setAttributeAndModes(stencil,osg::StateAttribute::ON);
      statesetBin3->setAttribute(colorMask);
      statesetBin3->setAttribute(depth);
      
      // set up the mirror geode.
      osg::Geode* geode = new osg::Geode;
      geode->addDrawable( geometry.get() );
      geode->setStateSet(statesetBin3);
      
      rootNode->addChild(geode);
        
    }

    // bin4  - draw the reflection.
    {
    
      // now create the 'reflection' of the loaded model by applying
      // create a Transform which flips the loaded model about the z axis
      // relative to the mirror node, the loadedModel is added to the
      // Transform so now appears twice in the scene, but is shared so there
      // is negligable memory overhead.  Also use an osg::StateSet 
      // attached to the Transform to override the face culling on the subgraph
      // to prevert an 'inside' out view of the reflected model.
      // set up the stencil ops so that only operator on this mirrors stencil value.



      // this clip plane removes any of the scene which when mirror would
      // poke through the mirror.  However, this clip plane should really
      // flip sides once the eye point goes to the back of the mirror...
      //osg::ClipPlane* clipplane = new osg::ClipPlane;
      //clipplane->setClipPlane(0.0,0.0,-1.0,z);
      //clipplane->setClipPlaneNum(0);

      osg::ClipNode* clipNode = new osg::ClipNode;
      osg::BoundingBox bbox;
      bbox.expandBy ( this->model()->getBound() );
      //clipNode->createClipBox ( bbox );
      //clipNode->addClipPlane(clipplane);


      osg::StateSet* dstate = clipNode->getOrCreateStateSet();
      dstate->setRenderBinDetails(4,"RenderBin");
      dstate->setMode(GL_CULL_FACE,osg::StateAttribute::OVERRIDE|osg::StateAttribute::OFF);

      osg::Stencil* stencil = new osg::Stencil;
      stencil->setFunction(osg::Stencil::EQUAL,1,~0u);
      stencil->setOperation(osg::Stencil::KEEP, osg::Stencil::KEEP, osg::Stencil::KEEP);
      dstate->setAttributeAndModes(stencil,osg::StateAttribute::ON);

      osg::MatrixTransform* reverseMatrix = new osg::MatrixTransform;
      reverseMatrix->setStateSet(dstate);
      reverseMatrix->preMult(osg::Matrix::translate(0.0f,0.0f,-z)*
                   osg::Matrix::scale(1.0f,1.0f,-1.0f)*
                   osg::Matrix::translate(0.0f,0.0f,z));

      reverseMatrix->addChild( this->model() );

      clipNode->addChild(reverseMatrix);

      rootNode->addChild(clipNode);
    
    }


    // bin5  - draw the textured mirror and blend it with the reflection.
    {
    
      // set up depth so all writing to depth goes to maximum depth.
      osg::Depth* depth = new osg::Depth;
      depth->setFunction(osg::Depth::ALWAYS);

      osg::Stencil* stencil = new osg::Stencil;
      stencil->setFunction(osg::Stencil::EQUAL,1,~0u);
      stencil->setOperation(osg::Stencil::KEEP, osg::Stencil::KEEP, osg::Stencil::ZERO);

      // set up additive blending.
      osg::BlendFunc* trans = new osg::BlendFunc;
      trans->setFunction(osg::BlendFunc::ONE,osg::BlendFunc::ONE);

      osg::StateSet* statesetBin5 = new osg::StateSet;
      statesetBin5->setMode(GL_CULL_FACE,osg::StateAttribute::OFF|osg::StateAttribute::PROTECTED);

      osg::ref_ptr < osg::Image > image ( osgDB::readImageFile( Usul::CommandLine::Arguments::instance().directory() +  "\\icons\\marble-3.jpg" ) );
      if ( 0x0 != image.get() )
      {
        osg::ref_ptr < osg::Texture2D > texture ( new osg::Texture2D );
        texture->setImage( image.get() );
        statesetBin5->setTextureAttributeAndModes( 0, texture.get(), osg::StateAttribute::ON|osg::StateAttribute::PROTECTED );
      }
      statesetBin5->setRenderBinDetails(5,"RenderBin");
      statesetBin5->setMode(GL_CULL_FACE,osg::StateAttribute::OFF);
      statesetBin5->setAttributeAndModes(stencil,osg::StateAttribute::ON);
      statesetBin5->setAttributeAndModes(trans,osg::StateAttribute::ON);
      statesetBin5->setAttribute(depth);
      
      // set up the mirror geode.
      osg::Geode* geode = new osg::Geode;
      geode->addDrawable( geometry.get() );
      geode->setStateSet(statesetBin5);
      
      rootNode->addChild(geode);

    }


    //group->addChild ( geode.get() );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove scene stage.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::_removeSceneStage()
{
  _sceneManager->groupRemove ( OsgTools::Render::Constants::STAGE );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Can we export this?
//
///////////////////////////////////////////////////////////////////////////////

bool Viewer::canExport ( const std::string &filename )
{
  const std::string ext ( Usul::Strings::lowerCase ( Usul::File::extension ( filename ) ) );
  return ( "ive" == ext || "osg" == ext || "jpg" == ext || "png" == ext || "bmp" == ext || "rgba" == ext || "eps" == ext || "flt" == ext );
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
  return filters;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the filters that we can export.
//
///////////////////////////////////////////////////////////////////////////////

Viewer::Filters Viewer::filtersExport() const
{
  Filters filters ( this->filtersWriteImage() );
  filters.push_back ( Filter ( "OpenSceneGraph Binary (*.ive)", "*.ive" ) );
  filters.push_back ( Filter ( "OpenSceneGraph ASCII (*.osg)",  "*.osg" ) );
  //filters.push_back ( Filter ( "OpenFlight (*.flt)",  "*.flt" ) );
  filters.push_back ( Filter ( "Encapsulated PostScript (*.eps)", "*.eps" ) );
  return filters;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the file.
//
//  TODO: This should search the export-filter's extensions.
//
///////////////////////////////////////////////////////////////////////////////

bool Viewer::exportFile ( const std::string& filename )
{
  const std::string ext ( Usul::Strings::lowerCase ( Usul::File::extension ( filename ) ) );

  if ( "ive" == ext || "osg" == ext || "flt" == ext )
  {
    return this->writeSceneFile ( filename );
  }
  else if ( "jpg" == ext || "png" == ext || "bmp" == ext || "rgba" == ext )
  {
    return this->writeImageFile ( filename );
  }
  else if ( "eps" == ext )
  {
    OsgTools::IO::WriteEPS writer ( filename );
    return writer.write ( *this );
  }

  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Hide/Show the center of rotation.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::showCenterOfRotation ( bool b )
{
  if ( b )
  {
    _flags = Usul::Bits::add < unsigned int, unsigned int > ( _flags, _SHOW_COR );
  }
  else
  {
    _flags = Usul::Bits::remove < unsigned int, unsigned int > ( _flags, _SHOW_COR );
  }
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
  osg::ref_ptr< osg::Group > group ( this->getGroup ( Usul::Registry::Keys::COR_NAME ) );

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

osg::Image* Viewer::screenCapture ( const osg::Vec3f& center, float distance, const osg::Quat& rotation, unsigned int height, unsigned int width ) const
{
  // Get non const pointer to this
  Viewer *me ( const_cast < Viewer * > ( this ) );

  osg::Matrix m ( osg::Matrixd::translate(0.0,0.0,distance)*
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
  return this->viewer()->getViewMatrix ( );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get text at the (x,y) on the screen.
//
///////////////////////////////////////////////////////////////////////////////

osgText::Text* Viewer::getText ( unsigned int x, unsigned int y )
{
  return _sceneManager->getText( x, y );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set text value.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::setText ( unsigned int x, unsigned int y, const std::string& text )
{
  _sceneManager->setText( x, y, text );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove text.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::removeText  ( unsigned int x, unsigned int y )
{
  _sceneManager->removeText( x, y );
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
//  Set the database pager.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::databasePager( osgDB::DatabasePager* dbPager )
{
  _databasePager = dbPager;
  //osgDB::Registry::instance()->setDatabasePager( dbPager );

  _databasePager->registerPagedLODs( this->scene() ); 

  _databasePager->setAcceptNewDatabaseRequests(true);
  _databasePager->setDatabasePagerThreadPause(false);
  
  _databasePager->setUseFrameBlock( false );

  this->viewer()->getCullVisitor()->setDatabaseRequestHandler( dbPager );
  this->viewer()->getUpdateVisitor()->setDatabaseRequestHandler( dbPager );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the database pager.
//
///////////////////////////////////////////////////////////////////////////////

osgDB::DatabasePager* Viewer::databasePager()
{
  return _databasePager.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the database pager.
//
///////////////////////////////////////////////////////////////////////////////

const osgDB::DatabasePager* Viewer::databasePager() const
{
  return _databasePager.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set compute near far mode.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::computeNearFar( bool b )
{
  if( b )
  {
    osgUtil::CullVisitor *cv ( this->viewer()->getCullVisitor() );
    cv->setClampProjectionMatrixCallback ( new OsgTools::Render::ClampProjection ( *cv, OsgTools::Render::Defaults::CAMERA_Z_NEAR, OsgTools::Render::Defaults::CAMERA_Z_FAR ) );
    cv->setInheritanceMask ( Usul::Bits::remove ( cv->getInheritanceMask(), osg::CullSettings::CLAMP_PROJECTION_MATRIX_CALLBACK ) );
    cv->setComputeNearFarMode( osg::CullSettings::COMPUTE_NEAR_FAR_USING_BOUNDING_VOLUMES );
  }
  else
  {
    osgUtil::CullVisitor *cv ( this->viewer()->getCullVisitor() );
    cv->setClampProjectionMatrixCallback ( 0x0 );
    //cv->setInheritanceMask ( Usul::Bits::add ( cv->getInheritanceMask(), osg::CullSettings::CLAMP_PROJECTION_MATRIX_CALLBACK ) );
    cv->setComputeNearFarMode( osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Force all detail to render. This makes sense if the scene uses a 
//  database pager.
//  See: http://openscenegraph.net/pipermail/osg-users/2006-April/063593.html
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::forceDetail()
{
  const osgDB::DatabasePager *db ( this->databasePager() );
  if ( 0x0 == db )
    return;

  const unsigned int maxNum ( 10000 );
  unsigned int count ( 0 );

#if OSG_VERSION_MAJOR == 1 && OSG_VERSION_MINOR == 2
  while ( count < maxNum )
  {	
    const unsigned int requests  ( db->getFileRequestListSize()   );
    const unsigned int toCompile ( db->getDataToCompileListSize() );


    #ifdef _DEBUG
    std::cout << "         count = " << count << std::endl;
    std::cout << "      requests = " << requests << std::endl;
    std::cout << "     toCompile = " << toCompile << std::endl;
    #endif

    if ( 0 == requests && 0 == toCompile )
    {
      #ifdef _DEBUG
      std::cout << "Note 3611460148: all detail is paged in" << std::endl;
      #endif
      break;
    }

    else
    {
      this->render();
    }

    if ( maxNum == count )
    {
      #ifdef _DEBUG
      std::cout << "Warning 3611460148: max iterations reached" << std::endl;
      #endif
      break;
    }

    ++count;
	
  }
  #endif
}


/// Add/Remove group from projection node
osg::Group*           Viewer::projectionGroupGet    ( const std::string& key )
{
  return _sceneManager->projectionGroupGet ( key );
}
