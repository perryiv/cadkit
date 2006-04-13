
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
#include "OsgTools/ScopedProjection.h"
#include "OsgTools/Box.h"
#include "OsgTools/Sphere.h"
#include "OsgTools/Jitter.h"
#include "OsgTools/GlassBoundingBox.h"
#include "OsgTools/State/PolygonMode.h"
#include "OsgTools/State/ShadeModel.h"
#include "OsgTools/State/StateSet.h"
#include "OsgTools/Render/ClampProjection.h"
#include "OsgTools/Render/LodCallbacks.h"
#include "OsgTools/Render/Trackball.h"
#include "OsgTools/Render/ActionAdapter.h"
#include "OsgTools/Render/EventAdapter.h"
#include "OsgTools/Render/Constants.h"
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

#include "OsgTools/Draggers/Trackball.h"

#include "Usul/Errors/Checker.h"
#include "Usul/Components/Manager.h"
#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Math/Constants.h"
#include "Usul/Math/UMath.h"
#include "Usul/Bits/Bits.h"
#include "Usul/System/LastError.h"
#include "Usul/Registry/Constants.h"
#include "Usul/Interfaces/IUpdateTreeControls.h"
#include "Usul/Interfaces/IStatusBar.h"
#include "Usul/Interfaces/IReportErrors.h"
#include "Usul/Interfaces/ISendMessage.h"
#include "Usul/Interfaces/IMaterialEditor.h"
#include "Usul/Interfaces/ICleanUp.h"
#include "Usul/Interfaces/ISetCursor.h"
#include "Usul/Interfaces/IGetBoundingBox.h"
#include "Usul/Interfaces/IHandleMessage.h"
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

#include "osg/GL"

#include "osgUtil/UpdateVisitor"
#include "osgUtil/IntersectVisitor"

#include "osgDB/WriteFile"
#include "osgDB/ReadFile"

#include <limits>

using namespace OsgTools::Render;


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

Viewer::Viewer ( Document *doc, IContext* context, IUnknown *caller ) :
  _context         ( context ),
  _setCursor       ( caller ),
  _timeoutSpin     ( caller ),
  _caller          ( caller ),
  _sceneView       ( new SceneView ),
  _scene           ( new Group ),
  _clipNode        ( new osg::ClipNode ),
  _projectionNode  ( new osg::Projection ),
  _lods            (),
  _times           (),
  _document        ( doc ),
  _frameDump       (),
  _groupMap        (),
  _projectionMap   (),
  _textMap         (),
  _numPasses       ( 1 ),
  _refCount        ( 0 ),
  _flags           ( _UPDATE_TIMES | _SHOW_AXES ),
  _animation       (),
  _navManip        ( 0x0 ),
  _currentTool     (),
  _lastTool        (),
  _currentMode     ( NAVIGATION ),
  _lightEditors    (),
  _contextId       ( 0 )
{
  // Add this view to the document
  if( this->document() )
    this->document()->addView ( this );

  // Add the clip node to the scene.
  _scene->addChild ( _clipNode.get() );

  // Light so that other lights don't after geometry under the projection node.
  osg::ref_ptr< osg::Light > light ( new osg::Light );
  light->setLightNum ( 1 );
  light->setDiffuse ( osg::Vec4 ( 0.8, 0.8, 0.8, 1.0 ) );

  // Add the projection node to the scene
  _scene->addChild( _projectionNode.get() );
  osg::ref_ptr< osg::StateSet > ss ( _projectionNode->getOrCreateStateSet () );
  ss->setAttribute ( light.get(), osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE );

  // Set the update-visitor.
  _sceneView->setUpdateVisitor ( new osgUtil::UpdateVisitor );

  // Set the display settings.
  _sceneView->setDisplaySettings ( new osg::DisplaySettings() );

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

#ifdef _DEBUG
  osg::setNotifyLevel ( osg::INFO );
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
  // Set the viewer's defaults.
  _sceneView->setDefaults();

  // If we have a valid context...
  if ( _context.valid() )
  {

    // Make this context current.
    _context->makeCurrent();

    // See if there is an accumulation buffer.
    GLint red ( 0 ), green ( 0 ), blue ( 0 ), alpha ( 0 );
    ::glGetIntegerv ( GL_ACCUM_RED_BITS,   &red   );
    ::glGetIntegerv ( GL_ACCUM_GREEN_BITS, &green );
    ::glGetIntegerv ( GL_ACCUM_BLUE_BITS,  &blue  );
    ::glGetIntegerv ( GL_ACCUM_ALPHA_BITS, &alpha );
    const bool hasAccum ( red && green && blue && alpha );
    _flags = ( hasAccum ) ? Usul::Bits::add < unsigned int, unsigned int > ( _flags, _HAS_ACCUM_BUFFER ) 
      : Usul::Bits::remove < unsigned int, unsigned int > ( _flags, _HAS_ACCUM_BUFFER );

    // Set default stereo modes.
    this->stereoEyeDistance( 0.01f );
    GLboolean hasStereo ( GL_FALSE );
    ::glGetBooleanv ( GL_STEREO, &hasStereo );
    if( GL_TRUE == hasStereo )
      this->stereoMode( osg::DisplaySettings::QUAD_BUFFER );

  }

  // Counter for display-list id. OSG will handle using the correct display 
  // list for this context.
  _sceneView->getState()->setContextID ( _contextId );

  // Set the background color.
  //this->backgroundColor ( OsgFox::Registry::read ( Usul::Registry::Sections::OPEN_GL_CANVAS, Usul::Registry::Keys::CLEAR_COLOR, OsgFox::Defaults::CLEAR_COLOR ) );

  // This is a work-around for the fact that some geometries have a 
  // calculated near or far distance of zero. SceneViewer::cull() does not 
  // handle this case, and the projection matrix ends up with NANs.
  osgUtil::CullVisitor *cv ( _sceneView->getCullVisitor() );
  cv->setClampProjectionMatrixCallback ( new OsgTools::Render::ClampProjection ( *cv, OsgTools::Render::Defaults::CAMERA_Z_NEAR, OsgTools::Render::Defaults::CAMERA_Z_FAR ) );

  // Related to above, and new with 0.9.8-2. osgUtil::SceneView and 
  // osgUtil::CullVisitor both inherit from osg::CullSettings. SceneView 
  // passes "*this" to it's cull visitor's CullSettings::inheritCullSettings, 
  // resetting the clamp-projection callback to null.
  cv->setInheritanceMask ( Usul::Bits::remove ( cv->getInheritanceMask(), osg::CullSettings::CLAMP_PROJECTION_MATRIX_CALLBACK ) );

  // Set all display-list states.
  this->setDisplayLists();
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

    Usul::Interfaces::IViewer::ValidQueryPtr me ( this );

    // If we are the active view, set the active view to null.
    if( me.get() == this->document()->activeView() )
      this->document()->activeView( 0x0 );
  }

  // Delete all display-lists associated with our context id.
  if ( _context.valid() )
  {
    _context->makeCurrent();
    _sceneView->releaseAllGLObjects();
    _sceneView->flushAllDeletedGLObjects();
  }

  _context = 0x0;
  _setCursor = static_cast < Usul::Interfaces::ISetCursorType* > ( 0x0 );
  _timeoutSpin = static_cast < Usul::Interfaces::ITimeoutSpin* > ( 0x0 );
  _caller = static_cast < Usul::Interfaces::IUnknown* > ( 0x0 );
}

///////////////////////////////////////////////////////////////////////////////
//
//  Set the default background.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::defaultBackground()
{
  this->backgroundColor ( OsgTools::Render::Defaults::CLEAR_COLOR );
  this->render();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the scene.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::updateScene()
{
  // Update the scene.
  if ( _sceneView.valid() )
  {
    RecordTime ut ( this, "update" );
    _sceneView->update();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Render the scene.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::render()
{
  // Handle no viewer or scene.
  if ( !_sceneView.valid() || !_scene.valid() || !_sceneView->getSceneData() || !_context.valid() )
    return;

  // Handle particles and osg-animations.
  osg::ref_ptr<osg::FrameStamp> fs ( new osg::FrameStamp );
  fs->setFrameNumber ( fs->getFrameNumber() + 1 );
  fs->setReferenceTime ( ::time ( 0x0 )  );
  _sceneView->setFrameStamp ( fs.get() );

  // Initialize the error.
  ::glGetError();

  // Make this context current.
  _context->makeCurrent();

  // Check for errors.
  USUL_ERROR_CHECKER ( GL_NO_ERROR == ::glGetError() );

  // Set the axes.
  this->_setAxes();

  // Set the lights
  this->_setLights();

  // Set high lod callbacks if we should
  if ( Usul::Shared::Preferences::instance().getBool( Usul::Registry::Keys::HIGH_LODS ) )
    this->_setLodCullCallback ( new OsgTools::Render::HighLodCallback );

  // Check for errors.
  USUL_ERROR_CHECKER ( GL_NO_ERROR == ::glGetError() );

  // Draw
  this->_render();

  // Check for errors.
  USUL_ERROR_CHECKER ( GL_NO_ERROR == ::glGetError() );

  // Swap the buffers.
  _context->swapBuffers();

  // Check for errors.
  USUL_ERROR_CHECKER ( GL_NO_ERROR == ::glGetError() );

  // Dump the current frame.
  this->_dumpFrame();

  // Check for errors.
  USUL_ERROR_CHECKER ( GL_NO_ERROR == ::glGetError() );

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
  // If there is no scene then clear the screen. 
  // Otherwise, weird artifacts may show up.
  if ( !_scene.valid() || _clipNode->getNumChildren() == 0 )
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

}


///////////////////////////////////////////////////////////////////////////////
//
//  Handle single-pass rendering.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::_singlePassRender()
{
  // Handle no viewer.
  if ( !_sceneView.valid() )
    return;

  // If we are doing hidden-line rendering...
  if ( this->hasHiddenLines() && _clipNode->getNumChildren() > 0 )
  {
    // Temporarily re-structure the scene. Better to do/undo this than keep 
    // it altered. An altered scene may mess up intersections.
    osg::ref_ptr<osg::Node> model ( _clipNode->getChild ( 0 ) );
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
      this->scene ( root.get() );

      // Set the state-sets for the branches.
      OsgTools::State::StateSet::hiddenLines ( this->backgroundColor(), normal->getOrCreateStateSet(), hidden->getOrCreateStateSet() );

      /*osg::ref_ptr < OsgTools::Callbacks::SetHiddenCallback > v ( new OsgTools::Callbacks::SetHiddenCallback );

      model->accept( *v );*/

      // Cull and draw.
      this->_cullAndDraw();

      /*osg::ref_ptr < OsgTools::Callbacks::UnSetHiddenCallback > unset ( new OsgTools::Callbacks::UnSetHiddenCallback );

      model->accept( *unset );*/
    }

    // Catch all exceptions.
    catch ( ... )
    {
      // Restore the scene and re-throw.
      this->scene ( model.get() );
      throw;
    }

    // Restore the scene.
    this->scene ( model.get() );
  }
  else
  {
    // Cull and draw.
    this->_cullAndDraw();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Handle multi-pass rendering.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::_multiPassRender()
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
    for ( unsigned i = 0; i < this->numRenderPasses(); ++i )
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
//  Get the viewer.
//
///////////////////////////////////////////////////////////////////////////////

Viewer::SceneView *Viewer::viewer()
{
  return _sceneView.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the viewer.
//
///////////////////////////////////////////////////////////////////////////////

const Viewer::SceneView *Viewer::viewer() const
{
  return _sceneView.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the viewer.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::viewer ( Viewer::SceneView *viewer )
{
  _sceneView = viewer;
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
  trackball->home ( *ea, aa );

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
  osg::Matrixd M2 ( trackball->getInverseMatrix() );

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

const osg::Group *Viewer::scene() const
{
  return _scene.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Access to the scene.
//
///////////////////////////////////////////////////////////////////////////////

osg::Group *Viewer::scene()
{
  return _scene.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the scene.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::scene ( osg::Node *node )
{
  // Handle same scene.
  if ( 0x0 != node && node == _scene.get() )
    return;

  // Remove any thing that may be under the clip node
  _clipNode->removeChild ( 0 , _clipNode->getNumChildren() );

  // If we are given null...
  if ( 0x0 == node )
  {
    // Clear the scene
    _scene->removeChild ( 0 , _clipNode->getNumChildren() ); 

    // Re-add the clip node
    _scene->addChild ( _clipNode.get() );
  }

  // Otherwise...
  else
  {
    // Add the node
    _clipNode->addChild ( node );
  }

  // Give the scene to the viewer.
  if ( _sceneView.valid() )
    _sceneView->setSceneData ( _scene.get() );

  // The scene changed.
  this->changedScene(); 
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

  // Ignore initial values sent by FOX. Makes a poor viewport.
  if ( w <= 1 || h <= 1 )
    return;

  // Set the viewport.
  this->viewport ( 0, 0, (int) w, (int) h );

  // Set the projection matrix.
  double fovy  ( Usul::Shared::Preferences::instance().getDouble ( Usul::Registry::Keys::FOV ) );
  double zNear ( OsgTools::Render::Defaults::CAMERA_Z_NEAR );
  double zFar  ( OsgTools::Render::Defaults::CAMERA_Z_FAR );
  double width ( w ), height ( h );
  double aspect ( width / height );

  if ( _sceneView.valid() )
  {
    _sceneView->setProjectionMatrixAsPerspective ( fovy, aspect, zNear, zFar );
    //_sceneView->setProjectionMatrixAsOrtho ( 0, w, 0, h, -10000, 10000 );

    _projectionNode->setMatrix( osg::Matrix::ortho( 0, w ,0, h, -10000, 10000 ) );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the viewport.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::viewport ( osg::Viewport *vp )
{
  if ( _sceneView.valid() )
    _sceneView->setViewport ( vp );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the viewport.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::viewport ( int x, int y, unsigned int w, unsigned int h )
{
  if ( _sceneView.valid() )
    _sceneView->setViewport ( x, y, (int) w, (int) h );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the viewport.
//
///////////////////////////////////////////////////////////////////////////////

const osg::Viewport *Viewer::viewport() const
{
  return ( _sceneView.valid() ) ? _sceneView->getViewport() : 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the viewport.
//
///////////////////////////////////////////////////////////////////////////////

osg::Viewport *Viewer::viewport()
{
  return ( _sceneView.valid() ) ? _sceneView->getViewport() : 0x0;
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
  bool filled ( this->hasPolygonMode ( face, osg::PolygonMode::FILL  ) );
  bool lines  ( this->hasPolygonMode ( face, osg::PolygonMode::LINE  ) );
  bool points ( this->hasPolygonMode ( face, osg::PolygonMode::POINT ) );
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
  bool front ( this->hasPolygonMode ( osg::PolygonMode::FRONT ) );
  bool back  ( this->hasPolygonMode ( osg::PolygonMode::BACK ) );
  bool both  ( this->hasPolygonMode ( osg::PolygonMode::FRONT_AND_BACK ) );
  return ( front || back || both );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query the shading model.
//
///////////////////////////////////////////////////////////////////////////////

bool Viewer::hasShadeModel ( osg::ShadeModel::Mode mode ) const
{
  // Handle no scene.
  if ( !this->scene() )
    return false;

  // Get the state set.
  const osg::StateSet *ss = this->viewer()->getGlobalStateSet();
  if ( !ss )
    return false;

  // Get the shade-model attribute, if any.
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
  bool smooth ( this->hasShadeModel ( osg::ShadeModel::SMOOTH ) );
  bool flat   ( this->hasShadeModel ( osg::ShadeModel::FLAT   ) );
  return ( smooth || flat );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove the polygon mode.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::removePolygonMode()
{
  // Handle no scene.
  if ( !this->scene() )
    return;

  // Get the state set.
  osg::StateSet *ss = this->viewer()->getGlobalStateSet();
  if ( !ss )
    return;

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
  // Handle no scene.
  if ( !this->scene() )
    return;

  // Get the state set.
  osg::StateSet *ss = this->viewer()->getGlobalStateSet();
  if ( !ss )
    return;

  // Have the state-set inherit the attribute. This will delete any 
  // existing attribute in the state-set.
  ss->removeAttribute ( osg::StateAttribute::SHADEMODEL );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set two sided lighting
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::twoSidedLighting ( bool twoSided )
{
  // Handle no scene.
  if ( !this->scene() )
    return;

  // Get the state set, or make one.
  osg::ref_ptr<osg::StateSet> ss ( this->viewer()->getGlobalStateSet() );
  if ( !ss.valid() )
    return;

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
  // Handle no scene.
  if ( !this->scene() )
    return false;

  // Get the state set.
  const osg::StateSet *ss = this->viewer()->getGlobalStateSet();
  if ( !ss )
    return false;

  // Get the shade-model attribute, if any.
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
  // Handle no scene.
  if ( !this->scene() )
    return;

  // Get the state set, or make one.
  osg::ref_ptr<osg::StateSet> ss ( this->viewer()->getGlobalStateSet() );
  if ( !ss.valid() )
    return;

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
  // Handle no scene.
  if ( !this->scene() )
    return false;

  // Get the state set.
  const osg::StateSet *ss = this->viewer()->getGlobalStateSet();
  if ( !ss )
    return false;

  return Usul::Bits::has ( ss->getMode( GL_LIGHTING ), osg::StateAttribute::ON );
}

///////////////////////////////////////////////////////////////////////////////
//
//  Get the clipping plane distances.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::nearFar ( double &n, double &f ) const
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
  // Set the color.
  if ( this->viewer() )
    this->viewer()->setClearColor ( color );

  // Write it to the registry.
  // OsgFox::Registry::write ( Usul::Registry::Sections::OPEN_GL_CANVAS, Usul::Registry::Keys::CLEAR_COLOR, color ); // TODO, put this in preference class?
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the background color.
//
///////////////////////////////////////////////////////////////////////////////

const osg::Vec4 &Viewer::backgroundColor() const
{
  if ( !this->viewer() )
    throw ( std::runtime_error ( "Cannot get the background color because there is no viewer" ) );
  return this->viewer()->getClearColor();
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
//  Record the start-time.
//
///////////////////////////////////////////////////////////////////////////////

Viewer::RecordTime::RecordTime ( Viewer *c, const std::string &name ) : _c ( c ), _name ( name )
{
  USUL_ERROR_CHECKER ( 0x0 != _c );

  // Record the time.
  TimePair now ( (double) Usul::System::Clock::milliseconds() / (double) CLOCKS_PER_SEC, 0 );
  _c->_times[_name].push_back ( now );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Record the end-time.
//
///////////////////////////////////////////////////////////////////////////////

Viewer::RecordTime::~RecordTime()
{
  USUL_ERROR_CHECKER ( 0x0 != _c );

  // Record the end-time.
  TimeHistory &h = _c->_times[_name];
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

double Viewer::timeLast ( const std::string &name ) const
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

double Viewer::timeAverage ( const std::string &name ) const
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
  _cameraCopyBuffer.second = _sceneView->getViewMatrix();

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
    _sceneView->setViewMatrix ( _cameraCopyBuffer.second );
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
//  Report any errors.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::_reportErrors ( unsigned int options, bool clear )
{
  Usul::Interfaces::IReportErrors::QueryPtr report ( Usul::Resources::reportErrors() );
  if( report.valid() )
    report->reportErrors ( options, clear );
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
  osg::ref_ptr<osg::Group> group ( this->getGroupProjection( OsgTools::Render::Constants::SELECTION_BOX ) );
  
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

    // Add the selection box to the scene
    _scene->addChild ( group.get() );

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
  this->_removeGroup ( OsgTools::Render::Constants::SELECTION_BOX );
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
  {
    // Save and restore default options.
    OsgTools::ScopedOptions scoped ( "" );
    this->_writeImageFile ( _frameDump.filename() );
  }
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

bool Viewer::writeImageFile ( const std::string &filename, const std::string &options ) const
{
  // Save and restore default options.
  OsgTools::ScopedOptions scoped ( options );

  // Write the image to file.
  return this->_writeImageFile ( filename, 1.0 );
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
//  Show/hide the bounding box.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::boundingBox ( bool state )
{
  // Get the existing bounding box, if it exists.
  osg::ref_ptr<osg::Group> &group = _groupMap[OsgTools::Render::Constants::BOUNDING_BOX];

  // Remove the bounding box if it exists.
  if ( group.valid() )
  {
    _scene->removeChild ( group.get() );
    group = 0x0;
  }

  // If we are supposed to show the bounding box...
  if ( state )
  {
    // Get the model's bounding box.
    osg::BoundingBox bb;

    // If the document supports the get bounding box interface, use that for the bounding box.
    // If not use the model node in the scene.
    Usul::Interfaces::IGetBoundingBox::QueryPtr getBoundingBox ( this->document() );
    if( getBoundingBox.valid() )
      bb.expandBy ( getBoundingBox->getBoundingBox() );
    else
      bb.expandBy ( this->model()->getBound() );

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

    // Add it to the group-map and scene.
    group = mt.get();
    _scene->addChild ( group.get() ); 
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is the bounding box showing?
//
///////////////////////////////////////////////////////////////////////////////

bool Viewer::boundingBox() const
{
  GroupMap::const_iterator i = _groupMap.find ( OsgTools::Render::Constants::BOUNDING_BOX );
  return ( ( _groupMap.end() != i ) && ( i->second.valid() ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Show/hide the bounding sphere.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::boundingSphere ( bool state )
{
  // Get the existing bounding sphere, if it exists.
  osg::ref_ptr<osg::Group> &group = _groupMap[OsgTools::Render::Constants::BOUNDING_SPHERE];

  // Remove the bounding sphere if it exists.
  if ( group.valid() )
  {
    _scene->removeChild ( group.get() );
    group = 0x0;
  }

  // If we are supposed to show the bounding sphere...
  if ( state )
  {
    // Get the scene's bounding sphere.
    const osg::BoundingSphere &bound = _scene->getBound();

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

    // Add it to the group-map and scene.
    group = mt.get();
    _scene->addChild ( group.get() );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is the bounding sphere showing?
//
///////////////////////////////////////////////////////////////////////////////

bool Viewer::boundingSphere() const
{
  GroupMap::const_iterator i = _groupMap.find ( OsgTools::Render::Constants::BOUNDING_SPHERE );
  return ( ( _groupMap.end() != i ) && ( i->second.valid() ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add text to the given matrix in the row and column
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::text ( float x, float y, unsigned int row, unsigned int col, const std::string& string )
{
  // Get xPos and yPos of upper left most point of the matrix
  int xPos ( (int)(this->width()  * x) );
  int yPos ( (int)(this->height() * y) );

  XYPair key ( x, y );

  // Find the xy pair if it exists
  TextMap::iterator i = _textMap.find( key );
  if( i == _textMap.end() )
  {
    this->textCreateMatrix ( x, y, 5, 5, 20, 20 );
  }

  // Get the matrix and geode
  TextMatrix &matrix = i->second.first.first;
  GeodePtr   geode   ( i->second.first.second  );
  int rowHeight      ( i->second.second.first  );
  int columnWidth    ( i->second.second.second );

  // Do we have enough rows?  if not resize
  if ( matrix.size() <= row )
    matrix.resize( row + 1);

  // Do we have enough columns? if not resize
  if( matrix.at( row ).size() <= col )
    matrix.at( row ).resize( col + 1);

  // Get the osgText from matrix
  TextPtr &text = matrix[row][col];

  // If text is not valid, create a new one
  if( !text.valid() )
  {
    text = new osgText::Text;

    osg::Vec4 layoutColor(1.0f,0.0f,0.0f,1.0f);
    float layoutCharacterSize = 20.0f;    

    text->setColor(layoutColor);
    text->setCharacterSize(layoutCharacterSize);

    float x ( xPos + ( col * columnWidth ) );
    float y ( yPos - ( row * rowHeight ) );
    text->setPosition ( osg::Vec3( x, y, 0.0 ) );

    // Add text to geode
    geode->addDrawable( text.get() );
  }

  // Set the text string
  text->setText( string );

  text->update();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create a text matrix
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::textCreateMatrix ( float x, float y, unsigned int numRows, unsigned int numCols, int rowHeight, int columnWidth )
{
  // Get the group for text matrices, if it exists.
  osg::ref_ptr<osg::Group> &group = _groupMap[OsgTools::Render::Constants::TEXT_MATRIX];

  if ( !group.valid() )
  {
    osg::ref_ptr< osg::Projection > projection = new osg::Projection;
    projection->setMatrix( osg::Matrix::ortho2D( 0, this->width() ,0, this->height() ) );

    // Make sure it's drawn last ( on top )
    osg::ref_ptr<osg::StateSet> ss ( projection->getOrCreateStateSet() );
    ss->setRenderBinDetails ( 100, "RenderBin" );
	  ss->setMode ( GL_DEPTH_TEST, osg::StateAttribute::OFF );

    osg::MatrixTransform* modelview_abs = new osg::MatrixTransform;
    modelview_abs->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
    modelview_abs->setMatrix(osg::Matrix::identity());

    projection->addChild ( modelview_abs );
    group = projection.get();
    _scene->addChild( group.get() );
  }

  // This is the matrix transform under the projection matrix
  osg::ref_ptr< osg::Group > node = group->getChild( 0 )->asGroup();

  XYPair key ( x, y );

  // Find the xy pair if it exists
  TextMap::iterator i = _textMap.find( key );
  if( i == _textMap.end() )
  {
    // Create a new matrix and geode to hold osgText
    TextMatrix matrix;
    
    // Set matrix requested size
    {
      matrix.resize( numRows );
      for( unsigned int i = 0; i < numRows; ++i )
        matrix.at( i ).resize( numCols );
    }

    _textMap[key] = MatrixData ( MatrixPair ( matrix, new osg::Geode ), MatrixProperties ( rowHeight, columnWidth ) );
    i = _textMap.find( key );

    // Add the geode to the text group
    if( node.valid() )
      node->addChild( i->second.first.second.get() );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove a text matrix
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::textRemoveMatrix ( float x, float y )
{
  // Get the group for text matrices, if it exists.
  osg::ref_ptr<osg::Group> &group = _groupMap[OsgTools::Render::Constants::TEXT_MATRIX];

  if ( group.valid() )
  {
    XYPair key ( x, y );
  
    TextMap::iterator i = _textMap.find ( key );
    if ( i != _textMap.end() )
    {
      GeodePtr geode ( i->second.first.second );
      GroupPtr group ( geode->getParent ( 0 ) );

      group->removeChild ( geode.get() );
    }
  }
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
//  Set the display-list state.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::setDisplayLists()
{
  // Handle no viewer or scene.
  if ( !_sceneView.valid() || !_scene.valid() )
    return;

  // Declare the visitor.
  osg::ref_ptr<osg::NodeVisitor> visitor ( 
    OsgTools::MakeVisitor<osg::Geode>::make ( 
    Usul::Adaptors::memberFunction ( this, &Viewer::_setDisplayListsGeode ) ) );

  // Visit the scene.
  this->model()->accept ( *visitor );

  // If we are not using display lists, and we have been created...
  const bool use ( Usul::Shared::Preferences::instance().getBool ( Usul::Registry::Keys::DISPLAY_LISTS ) );
  if ( !use )
  {
    // Delete all display-lists associated with our context id.
    _sceneView->releaseAllGLObjects();
    _sceneView->flushAllDeletedGLObjects();
  }
}

void Viewer::setDisplayLists(bool on) 
{
  Usul::Shared::Preferences::instance().setBool ( Usul::Registry::Keys::DISPLAY_LISTS, on );
  // Handle no viewer or scene.
  if ( !_sceneView.valid() || !_scene.valid() )
    return;
  
  // Declare the visitor.
  osg::ref_ptr<osg::NodeVisitor> visitor ( 
             OsgTools::MakeVisitor<osg::Geode>::make ( 
             Usul::Adaptors::memberFunction ( this, &Viewer::_setDisplayListsGeode ) ) );
  
  // Visit the scene.
  this->model()->accept ( *visitor );
  
  // If we are not using display lists, and we have been created...
  const bool use ( Usul::Shared::Preferences::instance().getBool ( Usul::Registry::Keys::DISPLAY_LISTS ) );
  if ( !use )
  {
    // Delete all display-lists associated with our context id.
    _sceneView->releaseAllGLObjects();
    _sceneView->flushAllDeletedGLObjects();
  }
}

//Return if DisplayLists are being used
bool Viewer::displayLists() const
{
  return ( Usul::Shared::Preferences::instance().getBool ( Usul::Registry::Keys::DISPLAY_LISTS ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set all the geode's drawables to the proper display-list state.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::_setDisplayListsGeode ( osg::Geode *geode )
{
  const bool use ( Usul::Shared::Preferences::instance().getBool ( Usul::Registry::Keys::DISPLAY_LISTS ) );
  for ( unsigned int i = 0; i < geode->getNumDrawables(); ++i )
  {
    osg::Drawable *drawable = geode->getDrawable ( i );
    drawable->setUseDisplayList ( use );
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
  if ( 1 == num || OsgTools::Jitter::instance().available ( num ) )
    _numPasses = num;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get group with given key.  Creates one if doesn't exist
//
///////////////////////////////////////////////////////////////////////////////

osg::Group* Viewer::_getGroup ( const std::string &key )
{
  osg::ref_ptr<osg::Group> &group = _groupMap[ key ];

  // Has the group been created
  if ( !group.valid() )
  {
    // Make a new group
    group = new osg::Group;

    // Set the name
    group->setName( key );

    // Add the group to the scene
    _scene->addChild( group.get() );
  }

  return group.get();
}
  

///////////////////////////////////////////////////////////////////////////////
//
//  Remove group with given key
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::_removeGroup ( const std::string &key )
{
  osg::ref_ptr<osg::Group> &group = _groupMap[key];
  _scene->removeChild ( group.get() );
  group = 0x0;

  // Remove key from group map.
  _groupMap.erase ( key );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is the group created?
//
///////////////////////////////////////////////////////////////////////////////

bool Viewer::_hasGroup    ( const std::string& key )
{
  GroupMap::const_iterator i = _groupMap.find ( key );
  return i != _groupMap.end();
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
//  Handle the cull and draw.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::_cullAndDraw()
{
  // Handle no viewer.
  if ( false == _sceneView.valid() )
    return;

  // Cull.
  {
    RecordTime ct ( this, "cull" );
    _sceneView->cull();
  }

  // Draw.
  {
    RecordTime dt ( this, "draw" );
    _sceneView->draw();
  }
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
  bb.expandBy ( _clipNode->getBound() );

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
  osg::ref_ptr< osg::ClipPlane > clipPlane ( new osg::ClipPlane( _clipNode->getNumClipPlanes(), plane ) );

  if( widget )
  {
    // Get the bounding box
    osg::BoundingBox bb; 
    bb.expandBy ( this->model()->getBound() );

    osg::ref_ptr < OsgTools::Widgets::ClipPlane > widget ( new OsgTools::Widgets::ClipPlane ( bb, clipPlane.get() ) );

    osg::ref_ptr< osg::Group > group ( this->_getGroup ( OsgTools::Render::Constants::CLIPPING_PLANES ) );
    group->addChild ( widget.get() );
  }

  // Add the plane
  _clipNode->addClipPlane ( clipPlane.get() );

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
  _clipNode->createClipBox ( bb );
  OsgTools::GlassBoundingBox gbb ( bb );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove a clipping plane from the scene
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::removePlane ( unsigned int index )
{
  _clipNode->removeClipPlane ( index );

  osg::ref_ptr< osg::Group > group ( this->_getGroup ( OsgTools::Render::Constants::CLIPPING_PLANES ) );

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
  _clipNode->removeClipPlane ( plane );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove all clipping planes
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::removePlanes ()
{
  typedef osg::ClipNode::ClipPlaneList ClipPlaneList;
  ClipPlaneList cliplist ( _clipNode->getClipPlaneList() );
  for ( ClipPlaneList::iterator i = cliplist.begin(); i != cliplist.end(); ++i )
    _clipNode->removeClipPlane ( i->get() );

  osg::ref_ptr< osg::Group > group ( this->_getGroup ( OsgTools::Render::Constants::CLIPPING_PLANES ) );

  group->removeChild( 0, group->getNumChildren() );
  
  this->changedScene();
  this->render();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the number of clipping planes in the scene
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Viewer::planes ()
{
  return _clipNode->getNumClipPlanes();
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
  image->readPixels ( this->x(), this->y(), this->width(), this->height(), GL_RGB, GL_UNSIGNED_BYTE );

  // Write the image.
  return osgDB::writeImageFile ( *image, filename );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the current frame to the file.  
//  Size will be a percentage of the viewport.
//
///////////////////////////////////////////////////////////////////////////////

bool Viewer::_writeImageFile ( const std::string &filename, double percent ) const
{
  const int h ( this->height() );
  const int w ( this->width() );

  const unsigned int height ( (unsigned int)(h * percent) );
  const unsigned int width  ( (unsigned int)(w * percent) );

  // Hack until I get tiled rendering working
  if( percent == 1.0 )
    return _writeImageFile( filename );

  return _writeImageFile( filename, height, width );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the current frame to the file.  
//  Size will be the specified height and width
//
///////////////////////////////////////////////////////////////////////////////

bool Viewer::_writeImageFile ( const std::string &filename, unsigned int height, unsigned int width ) const
{
  // Get non const pointer to this
  Viewer *me ( const_cast < Viewer * > ( this ) );

  // Make this context current.
  me->_context->makeCurrent();

// Hack to make large pictures.
#if 1
  height = 4048;
  width = 4048;
#endif

  // Make the image
  osg::ref_ptr<osg::Image> image ( new osg::Image );

  // Make enough space
  image->allocateImage ( width, height, 1, GL_RGB, GL_UNSIGNED_BYTE ); 

  // What I think should happen here:
  // 1. Check for frame buffer object support.
  // 2. If it doesn't exist, fall back on current method.
  // 3. If it does exist, attach a texture for the color buffer to the camera in osgUtil::Viewer.  
  //    Depending on the size requested, should be able to make the texture the requested size and then render once.  No tiling needed.
  // 4. Set the viewport and projection as it is now.
  // 5. Read from the texture and add it to final image.

  if ( osg::FBOExtensions::instance( _contextId, true )->isSupported() )
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
    camera->setClearColor( _sceneView->getClearColor() );
    camera->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    camera->setViewport(0, 0, width, height);

    // Set the camera to render before the main camera.
    camera->setRenderOrder(osg::CameraNode::PRE_RENDER);

    // Set the projection matrix.
    double fovy  ( Usul::Shared::Preferences::instance().getDouble ( Usul::Registry::Keys::FOV ) );
    double zNear ( OsgTools::Render::Defaults::CAMERA_Z_NEAR );
    double zFar  ( OsgTools::Render::Defaults::CAMERA_Z_FAR );
    double w ( width ), h ( height );
    double aspect ( w / h );

    camera->setProjectionMatrixAsPerspective ( fovy, aspect, zNear, zFar );

    // Set view.
    camera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
    camera->setViewMatrix ( _sceneView->getViewMatrix() );

    // Tell the camera to use OpenGL frame buffer object where supported.
    camera->setRenderTargetImplementation( osg::CameraNode::FRAME_BUFFER_OBJECT );

    // Attach the texture and use it as the color buffer.
    camera->attach( osg::CameraNode::COLOR_BUFFER, image.get() );

    // Save the old root.
    GroupPtr group = _scene;

    // Add the scene to the camera.
    camera->addChild ( me->_scene.get() );

    // Make the camera file the scene data.
    me->_sceneView->setSceneData ( camera.get() );

    // Render to the image.
    me->render();

    // Set the old root back to the scene data.
    me->_sceneView->setSceneData ( group.get() );
  }
  else
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
    osg::ref_ptr<osg::Viewport> ovp ( me->viewport() );

    double fovy  ( OsgTools::Render::Defaults::CAMERA_FOV_Y );
    double zNear ( OsgTools::Render::Defaults::CAMERA_Z_NEAR );
    double zFar  ( OsgTools::Render::Defaults::CAMERA_Z_FAR );
    double aspect ( width / height );

    const double top     ( zNear * tan(fovy * osg::PI / 360.0) );
    const double bottom  ( -top );
    const double left    ( bottom * aspect );
    const double right   ( top * aspect );

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
      me->_sceneView->setViewport ( 0, 0, currentTileWidth, currentTileHeight );

      // compute projection parameters
      const double currentLeft   ( left          + ( right - left ) *  ( currentCol * tileWidth ) / width );
      const double currentRight  ( currentLeft   + ( right - left ) *            currentTileWidth / width );
      const double currentBottom ( bottom        + ( top - bottom ) * ( currentRow * tileHeight ) / height );
      const double currentTop    ( currentBottom + ( top - bottom ) *           currentTileHeight / height );

      // Set the new frustum
      me->_sceneView->setProjectionMatrixAsFrustum ( currentLeft, currentRight, currentBottom, currentTop, zNear, zFar );
      
      // Draw
      me->render();

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
      ::glReadPixels(0, 0, currentTileWidth, currentTileHeight, GL_RGB, GL_UNSIGNED_BYTE, image->data( ) );

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
    me->viewport ( ovp.get() );
    me->resize ( ovp->width(), ovp->height() );
    me->render();
  }
  //image->scaleImage( height, width, 1 );

  // Write the image to file.
  return osgDB::writeImageFile ( *image, filename );

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
  case Usul::Interfaces::IOsgFoxView::IID:
    return static_cast < Usul::Interfaces::IOsgFoxView* > ( this );
  case Usul::Interfaces::ILights::IID:
    return static_cast < Usul::Interfaces::ILights* > ( this );
  case Usul::Interfaces::ISceneStage::IID:
    return static_cast < Usul::Interfaces::ISceneStage * > ( this );
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
  return _clipNode.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the model.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* Viewer::model()
{
  return _clipNode.get();
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
  osg::ref_ptr< osg::Group > group ( this->getGroupProjection( OsgTools::Render::Constants::AXES ) );

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
  this->removeGroupProjection( OsgTools::Render::Constants::AXES );

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

  osg::ref_ptr< osg::Group > group ( this->getGroupProjection( OsgTools::Render::Constants::AXES ) );
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
  osg::Matrix view ( _sceneView->getViewMatrix() );

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

Viewer::IPolygonMode::Mode     Viewer::polygonMode() const
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

const Viewer::Filenames& Viewer::filenames() const
{
  return this->frameDump().filenames();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Should the filenames be saved.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::filenamesSave ( bool b )
{
  this->frameDump().filenamesSave( b );
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
  return this->_getGroup ( key );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove group with given key
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::removeGroup ( const std::string& key )
{
  this->_removeGroup ( key );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is the group created?
//
///////////////////////////////////////////////////////////////////////////////

bool Viewer::hasGroup    ( const std::string& key )
{
  return this->_hasGroup ( key );
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
//  Get X
//
///////////////////////////////////////////////////////////////////////////////

int Viewer::x()
{
  return _sceneView->getViewport()->x();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get Y
//
///////////////////////////////////////////////////////////////////////////////

int Viewer::y()
{
  return _sceneView->getViewport()->y();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get Height
//
///////////////////////////////////////////////////////////////////////////////

int Viewer::height()
{
  return _sceneView->getViewport()->height();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get width
//
///////////////////////////////////////////////////////////////////////////////

int Viewer::width()
{
  return _sceneView->getViewport()->width();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get X
//
///////////////////////////////////////////////////////////////////////////////

int Viewer::x() const
{
  return _sceneView->getViewport()->x();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get Y
//
///////////////////////////////////////////////////////////////////////////////

int Viewer::y() const
{
  return _sceneView->getViewport()->y();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get Height
//
///////////////////////////////////////////////////////////////////////////////

int Viewer::height() const
{
  return _sceneView->getViewport()->height();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get Width
//
///////////////////////////////////////////////////////////////////////////////

int Viewer::width() const
{
  return _sceneView->getViewport()->width();
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
    pt0 = osg::Vec3 ( x, y, -1 ) * osg::Matrix::inverse( _projectionNode->getMatrix() );
    pt1 = osg::Vec3 ( x, y,  1 ) * osg::Matrix::inverse( _projectionNode->getMatrix() );
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

void Viewer::setTool ( Usul::Interfaces::IFoxEvent *tool )
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

Usul::Interfaces::IFoxEvent *Viewer::getTool()
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
    _currentTool = static_cast < IFoxEvent * > ( 0x0 );
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
        Usul::Scope::Reset< unsigned int > reset ( _numPasses, 1, _numPasses );
        this->render();
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
    root->addChild( _projectionNode.get() );

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
        this->doneTool();
      }
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Start/stop the spin.
//
///////////////////////////////////////////////////////////////////////////////
void Viewer::spin ( bool state )
{
  if( !_timeoutSpin.valid() )
    return;

  // If we are supposed to start it.
  if ( state )
  {
    // We can't spin if there is no viewer.
    if ( !this->viewer() )
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

void Viewer::timeoutSpin ()
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
//  set the background color.  Returns old value
//
///////////////////////////////////////////////////////////////////////////////
void Viewer::setBackground ( const osg::Vec4 &color)
{
  this->backgroundColor ( color );
  this->render();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Show the lights in the scene.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::_showLights()
{
  osg::ref_ptr<osg::Group> group ( this->getGroupProjection( OsgTools::Render::Constants::LIGHT ) );

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
  this->removeGroupProjection( OsgTools::Render::Constants::LIGHT );

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
//  Get group under the projection node with given key.  Creates one if doesn't exist
//
///////////////////////////////////////////////////////////////////////////////

osg::Group* Viewer::getGroupProjection ( const std::string &key )
{
  osg::ref_ptr<osg::Group> &group = _projectionMap[ key ];

  // Has the group been created
  if ( !group.valid() )
  {
    // Make a new group
    group = new osg::Group;

    // Set the name
    group->setName( key );

    // Add the group to the scene
    _projectionNode->addChild( group.get() );
    _projectionNode->dirtyBound();
  }

  return group.get();
}
  

///////////////////////////////////////////////////////////////////////////////
//
//  Remove group with given key
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::removeGroupProjection ( const std::string &key )
{
  osg::ref_ptr<osg::Group> &group = _projectionMap[key];
  _projectionNode->removeChild ( group.get() );
  _projectionNode->dirtyBound();
  group = 0x0;

  // Remove key from group map.
  _projectionMap.erase ( key );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is the group created?
//
///////////////////////////////////////////////////////////////////////////////

bool Viewer::hasGroupProjection    ( const std::string& key )
{
  GroupMap::const_iterator i = _projectionMap.find ( key );
  return i != _projectionMap.end();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the text in column and row of the matrix located at x, y.
//  Usul::Interfaces::ITextMatrix
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::setText( float x, float y, unsigned int row, unsigned int col, const std::string& text )
{
  return this->text( x, y, row, col, text );
}

///////////////////////////////////////////////////////////////////////////////
//
//  Create a matrix at location x,y.
//  Usul::Interfaces::ITextMatrix
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::createMatrix ( float x, float y, unsigned int numRows, unsigned int numCols, int rowHeight, int columnWidth )
{
  return this->textCreateMatrix( x, y, numRows, numCols, rowHeight, columnWidth );
}

///////////////////////////////////////////////////////////////////////////////
//
//  Remove the matrix at location x,y.
//  Usul::Interfaces::ITextMatrix
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::removeMatrix ( float x, float y )
{
  return this->textRemoveMatrix( x, y );
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

  GroupPtr group ( this->_getGroup ( OsgTools::Render::Constants::STAGE ) );

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

    osg::StateSet* rootStateSet = _scene->getOrCreateStateSet();
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
  this->_removeGroup ( OsgTools::Render::Constants::STAGE );
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
//  Return the filters that we can export.
//
///////////////////////////////////////////////////////////////////////////////

Viewer::Filters Viewer::filtersExport() const
{
  Filters filters;
  filters.push_back ( Filter ( "OpenSceneGraph Binary (*.ive)", "*.ive" ) );
  filters.push_back ( Filter ( "OpenSceneGraph ASCII (*.osg)",  "*.osg" ) );
  filters.push_back ( Filter ( "OpenFlight (*.flt)",  "*.flt" ) );
  filters.push_back ( Filter ( "JPEG Image (*.jpg)", "*.jpg"   ) );
  filters.push_back ( Filter ( "PNG Image (*.png)", "*.png"    ) );
  filters.push_back ( Filter ( "BMP Image (*.bmp)", "*.bmp"    ) );
  filters.push_back ( Filter ( "RGBA Image (*.rgba)", "*.rgba" ) );
  filters.push_back ( Filter ( "Encapsulated PostScript (*.eps)", "*.eps" ) );
  return filters;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the file.
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
