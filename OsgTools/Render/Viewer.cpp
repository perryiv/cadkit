
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Viewer class that handles rendering and interaction.
//
///////////////////////////////////////////////////////////////////////////////

#include "OsgTools/Render/Viewer.h"
#include "OsgTools/Render/ClampProjection.h"
#include "OsgTools/Render/Constants.h"
#include "OsgTools/Render/RecordTime.h"
#include "OsgTools/Render/LodCallbacks.h"
#include "OsgTools/Render/Trackball.h"
#include "OsgTools/Render/EventAdapter.h"
#include "OsgTools/Render/ActionAdapter.h"

#include "OsgTools/State/StateSet.h"
#include "OsgTools/State/PolygonMode.h"
#include "OsgTools/State/ShadeModel.h"

#include "OsgTools/Images/TiledImage.h"

#include "OsgTools/Jitter.h"
#include "OsgTools/ScopedProjection.h"
#include "OsgTools/ScopedViewport.h"
#include "OsgTools/DisplayLists.h"
#include "OsgTools/Group.h"
#include "OsgTools/Box.h"
#include "OsgTools/Sphere.h"
#include "OsgTools/SortBackToFrontCallback.h"

#include "Usul/Threads/Variable.h"
#include "Usul/Threads/Guard.h"
#include "Usul/Errors/Stack.h"
#include "Usul/Errors/Assert.h"
#include "Usul/Exceptions/Thrower.h"
#include "Usul/Bits/Bits.h"
#include "Usul/System/LastError.h"
#include "Usul/System/Clock.h"
#include "Usul/Math/Math.h"
#include "Usul/Shared/Preferences.h"
#include "Usul/Interfaces/ITimerServer.h"

#include "osg/MatrixTransform"

#include "osgUtil/UpdateVisitor"

#include "osgDB/WriteFile"

#include <sstream>
#include <stdexcept>
#include <limits>

using namespace OsgTools::Render;


///////////////////////////////////////////////////////////////////////////////
//
//  Convenient macros.
//
///////////////////////////////////////////////////////////////////////////////

#define GUARD_MEMBERS RecursiveGuard guard_members (  _members )
#define GUARD_CONTEXT ContextGuard   guard_context ( *_context )


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

Viewer::Viewer ( ContextMutex *context, IDocument *doc, IUnknown *unknown ) : 
  BaseClass(),
  _members      (),
  _context      ( context ),
  _sceneView    ( new osgUtil::SceneView ),
  _document     ( doc ),
  _numPasses    ( 1 ),
  _flags        ( 0 ),
  _root         ( new osg::Group ),
  _clipped      ( new osg::ClipNode ),
  _unclipped    ( new osg::ClipNode ),
  _model        ( new osg::Group ),
  _decorator    ( new osg::Group ),
  _groupMap     (),
  _frameDump    (),
  _times        (),
  _animation    (),
  _textMap      (),
  _lods         (),
  _navManips    (),
  _navHistory   (),
  _timerServer  ( unknown ),
  _renderNotify ( unknown ),
  _swapBuffers  ( unknown )

{
  // Configure the scene-view.
  _sceneView->setUpdateVisitor   ( new osgUtil::UpdateVisitor    );
  _sceneView->setDisplaySettings ( new osg::DisplaySettings()    );
  _sceneView->setClearColor      ( OsgTools::Render::CLEAR_COLOR );
  _sceneView->setSceneData       ( _root.get() );

  // Build the scene.
  _root->addChild ( _clipped.get() );
  _root->addChild ( _unclipped.get() );
  _clipped->addChild ( _model.get() );
  _unclipped->addChild ( _decorator.get() );

  // Push a default navigation manipulator.
  this->navManipPush ( new Trackball );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

Viewer::~Viewer()
{
  // Safely...
  try
  {
    // Sanity check.
    USUL_ASSERT ( 0 == _members.count() );

    // Need a local scope.
    {
      GUARD_MEMBERS;
      GUARD_CONTEXT;

      // Release associated display lists if we are the current viewer.
      _sceneView->releaseAllGLObjects();
      _sceneView->flushAllDeletedGLObjects();
    }

    // Sanity check.
    USUL_ASSERT ( 0 == _members.count() );
  }

  // Catch standard exceptions.
  catch ( const std::exception &e )
  {
    Usul::Errors::Stack::instance().push ( e.what() );
    std::ostringstream out;
    out << "Error 4062532086: Standard exception caught in destructor for viewer: " << this;
    Usul::Errors::Stack::instance().push ( out.str() );
  }

  // Catch all exceptions.
  catch ( ... )
  {
    std::ostringstream out;
    out << "Error 3060697241: Unknown exception caught in destructor for viewer: " << this;
    Usul::Errors::Stack::instance().push ( out.str() );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize state.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::init()
{
  // Should be true.
  USUL_ASSERT ( 0 == _members.count() );

  GUARD_MEMBERS;
  GUARD_CONTEXT;

  // See if there is an accumulation buffer.
  GLint red ( 0 ), green ( 0 ), blue ( 0 ), alpha ( 0 );
  ::glGetIntegerv ( GL_ACCUM_RED_BITS,   &red   );
  ::glGetIntegerv ( GL_ACCUM_GREEN_BITS, &green );
  ::glGetIntegerv ( GL_ACCUM_BLUE_BITS,  &blue  );
  ::glGetIntegerv ( GL_ACCUM_ALPHA_BITS, &alpha );
  const bool hasAccum ( red && green && blue && alpha );
  _flags = ( hasAccum ) ? Usul::Bits::add ( _flags, _HAS_ACCUM_BUFFER ) : Usul::Bits::remove ( _flags, _HAS_ACCUM_BUFFER );

  // Set scene-view defaults.
  _sceneView->setDefaults();

  // Counter for display-list id. OSG will handle using the correct display 
  // list for this context.
  static unsigned int count ( 0 );
  _sceneView->getState()->setContextID ( ++count );

  // This is a work-around for the fact that some geometries have a 
  // calculated near or far distance of zero. SceneViewer::cull() does not 
  // handle this case, and the projection matrix ends up with NANs.
  osg::ref_ptr<osgUtil::CullVisitor> cv ( _sceneView->getCullVisitor() );
  cv->setClampProjectionMatrixCallback ( new OsgTools::Render::ClampProjection ( *cv ) );

  // Related to above, and new with 0.9.8-2. osgUtil::SceneView and 
  // osgUtil::CullVisitor both inherit from osg::CullSettings. SceneView 
  // passes "*this" to it's cull visitor's CullSettings::inheritCullSettings, 
  // resetting the clamp-projection callback to null.
  cv->setInheritanceMask ( Usul::Bits::remove ( cv->getInheritanceMask(), osg::CullSettings::CLAMP_PROJECTION_MATRIX_CALLBACK ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface. No need to guard members; should be re-entrant.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *Viewer::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::IFrameDump::IID:
    return static_cast<Usul::Interfaces::IFrameDump*>(this);
  case Usul::Interfaces::ISelectionBox::IID:
    return static_cast<Usul::Interfaces::ISelectionBox*>(this);
  case Usul::Interfaces::IExport::IID:
    return static_cast<Usul::Interfaces::IExport*>(this);
  case Usul::Interfaces::IProjectionMatrix::IID:
    return static_cast<Usul::Interfaces::IProjectionMatrix*>(this);
  case Usul::Interfaces::IRender::IID:
    return static_cast<Usul::Interfaces::IRender*>(this);
  case Usul::Interfaces::IViewport::IID:
    return static_cast<Usul::Interfaces::IViewport*>(this);
  case Usul::Interfaces::ITimerNotify::IID:
    return static_cast<Usul::Interfaces::ITimerNotify*>(this);
  default:
    return 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Increment the reference count.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::ref()
{
  GUARD_MEMBERS;
  BaseClass::ref();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Decrement the reference count.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::unref ( bool allowDeletion )
{
  GUARD_MEMBERS;
  BaseClass::unref ( allowDeletion );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the default background.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::defaultBackground()
{
  this->backgroundColor ( OsgTools::Render::CLEAR_COLOR );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the background color.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::backgroundColor ( const osg::Vec4f &color )
{
  GUARD_MEMBERS;
  _sceneView->setClearColor ( color );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the background color.
//
///////////////////////////////////////////////////////////////////////////////

osg::Vec4f Viewer::backgroundColor() const
{
  GUARD_MEMBERS;
  return _sceneView->getClearColor();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the scene.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::update()
{
  GUARD_MEMBERS;
  OsgTools::Render::RecordTime < Viewer > ut ( "update", *this );
  _sceneView->update();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Render the scene.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::render()
{
  GUARD_MEMBERS;
  GUARD_CONTEXT;

  // Check for errors.
  USUL_ASSERT ( GL_NO_ERROR == ::glGetError() );

  // Handle particles and osg-animations.
  osg::ref_ptr<osg::FrameStamp> fs ( new osg::FrameStamp );
  fs->setFrameNumber ( fs->getFrameNumber() + 1 );
  fs->setReferenceTime ( ::time ( 0x0 )  );
  _sceneView->setFrameStamp ( fs.get() );

  // See if we are supposed to use multiple passes.
  if ( this->numRenderPasses() > 1 )
    this->_multiPassRender();
  else
    this->_singlePassRender();

  // Check for errors.
  USUL_ASSERT ( GL_NO_ERROR == ::glGetError() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Handle single-pass rendering.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::_singlePassRender()
{
  GUARD_MEMBERS;
  GUARD_CONTEXT;

  // If we are doing hidden-line rendering...
  if ( this->hiddenLines() )
  {
    // Get the current model-group.
    osg::ref_ptr<osg::Node> model ( _model.get() );

    // Safely...
    try
    {
      // Temporarily re-structure the scene. Better to do/undo this than keep 
      // it altered. An altered scene may mess up intersections.
      osg::ref_ptr<osg::Group> group  ( new osg::Group );
      osg::ref_ptr<osg::Group> normal ( new osg::Group );
      osg::ref_ptr<osg::Group> hidden ( new osg::Group );
      group->addChild ( normal.get() );
      group->addChild ( hidden.get() );
      normal->addChild ( model.get() );
      hidden->addChild ( model.get() );

      // Set the new scene.
      this->model ( group.get() );

      // Set the state-sets for the branches.
      OsgTools::State::StateSet::hiddenLines ( this->backgroundColor(), normal->getOrCreateStateSet(), hidden->getOrCreateStateSet() );

      // Cull and draw again.
      this->_cullAndDraw();
    }

    // Catch all exceptions.
    catch ( ... )
    {
      // Restore the scene and re-throw.
      this->model ( model.get() );
      throw;
    }

    // Restore the scene.
    this->model ( model.get() );
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
  // Safely...
  try
  {
    GUARD_MEMBERS;
    GUARD_CONTEXT;

    // Mutexes have to go out of scope last.
    {
      // Save original projection matrix.
      OsgTools::ScopedProjection sp ( _sceneView.get() );

      // Clear the accumulation buffer.
      ::glClearAccum ( 0.0f, 0.0f, 0.0f, 0.0f );
      ::glClear ( GL_ACCUM_BUFFER_BIT );

      // Check for errors.
      USUL_ASSERT ( GL_NO_ERROR == ::glGetError() );

      // Needed in the loop.
      osg::Matrixd matrix;
      osg::ref_ptr<osg::Viewport> vp ( _sceneView->getViewport() );
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
        USUL_ASSERT ( GL_NO_ERROR == ::glGetError() );

        // Accumulate the pixels from the frame buffer.
        double value ( 1.0 / static_cast < double > ( this->numRenderPasses() ) );
        ::glAccum ( GL_ACCUM, value );

        // Check for errors.
        USUL_ASSERT ( GL_NO_ERROR == ::glGetError() );
      }

      // Transfer the accumulation buffer into the frame buffer.
      ::glAccum ( GL_RETURN, 1.0f );

      // Check for errors.
      USUL_ASSERT ( GL_NO_ERROR == ::glGetError() );
    }
  }

  // Catch all exceptions and reset the number of passes. Otherwise, you 
  // can get stuck in a loop where the error dialog causes a render, and 
  // the render causes an error dialog, etc.

  // Standard exceptions.
  catch ( const std::exception &e )
  {
    Usul::Errors::Stack::instance().push ( e.what() );
    std::ostringstream message;
    message << "Error 3793469320: Standard exception caught when attempting to render with " << this->numRenderPasses() << " passes";
    this->numRenderPasses ( 1 );
    throw std::runtime_error ( message.str() );
  }

  // Unknown exceptions.
  catch ( ... )
  {
    std::ostringstream message;
    message << "Error 2662451423: Unknown exception caught when attempting to render with " << this->numRenderPasses() << " passes";
    this->numRenderPasses ( 1 );
    throw std::runtime_error ( message.str() );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Handle the cull and draw.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::_cullAndDraw()
{
  GUARD_MEMBERS;
  GUARD_CONTEXT;

  // Cull.
  {
    OsgTools::Render::RecordTime < Viewer > ct ( "cull", *this );
    _sceneView->cull();
  }

  // Draw.
  {
    OsgTools::Render::RecordTime < Viewer > dt ( "draw", *this );
    _sceneView->draw();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the number of rendering passes. Unavailable numbers have no effect.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::numRenderPasses ( unsigned int num )
{
  GUARD_MEMBERS;
  if ( 1 == num || OsgTools::Jitter::instance().available ( num ) )
    _numPasses = num;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the number of rendering passes.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Viewer::numRenderPasses() const
{
  GUARD_MEMBERS;
  const unsigned int num ( _numPasses );
  return num;
}


///////////////////////////////////////////////////////////////////////////////
//
//  See if there are hidden lines.
//
///////////////////////////////////////////////////////////////////////////////

bool Viewer::hiddenLines() const
{
  GUARD_MEMBERS;
  return Usul::Bits::has ( _flags, _HIDDEN_LINES );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the state for hidden lines.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::hiddenLines ( bool h )
{
  GUARD_MEMBERS;
  _flags = ( h ) ? Usul::Bits::remove ( _flags, _HIDDEN_LINES ) : Usul::Bits::add ( _flags, _HIDDEN_LINES );

  // Make sure there is no polygon mode in the viewer's global-state-set.
  if ( h )
    this->polygonModeRemove();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the display-list state.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::displayLists ( bool use, bool release )
{
  GUARD_MEMBERS;
  GUARD_CONTEXT;

  // Set the display-list usage.
  OsgTools::DisplayLists dl ( use );
  dl ( _root.get() );

  // If we are not using display lists, and we are supposed to release them...
  if ( !use && release )
  {
    // Delete all display-lists associated with our context id.
    _sceneView->releaseAllGLObjects();
    _sceneView->flushAllDeletedGLObjects();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Draw a square representing the selection box.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::selectionBoxSet ( const osg::Vec3d &v1, const osg::Vec3d &v2 )
{
  GUARD_MEMBERS;
#ifndef _DEBUG
  TODO
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove the selection box.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::selectionBoxRemove()
{
  this->_removeGroup ( OsgTools::Render::SELECTION_BOX );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove group with given key
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::_removeGroup ( const std::string &key )
{
  GUARD_MEMBERS;

  // Get or create the group.
  osg::ref_ptr<osg::Group> &group = _groupMap[key];
  _decorator->removeChild ( group.get() );
  group = 0x0;

  // Remove key from group map.
  _groupMap.erase ( key );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the frame-dump state.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::frameDumpState ( bool state )
{
  GUARD_MEMBERS;
  _frameDump.dump ( state );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the frame-dump state.
//
///////////////////////////////////////////////////////////////////////////////

bool Viewer::frameDumpState() const
{
  GUARD_MEMBERS;
  return _frameDump.dump();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Dump the current frame to file.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::frameDump() const
{
  if ( this->frameDumpState() )
    this->writeImageFile ( _frameDump.filename() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the current frame-dump file number
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Viewer::frameDumpCurrentFileNum() const
{
  GUARD_MEMBERS;
  const unsigned int num ( _frameDump.current() );
  return num;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Reset the frame-dump counter.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::frameDumpResetCounter()
{
  GUARD_MEMBERS;
  _frameDump.reset();
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
  GUARD_MEMBERS;
  _frameDump.dir    ( dir    );
  _frameDump.base   ( base   );
  _frameDump.ext    ( ext    );
  _frameDump.start  ( start  );
  _frameDump.digits ( digits );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the current frame to the file. Does not do tiled rendering.
//
///////////////////////////////////////////////////////////////////////////////

bool Viewer::writeImageFile ( const std::string &name ) const
{
  GUARD_MEMBERS;
  GUARD_CONTEXT;

  // Get the viewport.
  const osg::Viewport *vp ( _sceneView->getViewport() );
  if ( 0x0 == vp )
    return false;

  // Make the image
  osg::ref_ptr<osg::Image> image ( new osg::Image );

  // Read the screen buffer.
  image->readPixels ( vp->x(), vp->y(), vp->width(), vp->height(), GL_RGB, GL_UNSIGNED_BYTE );

  // Write the image.
  return osgDB::writeImageFile ( *image, name );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the current frame to the file. Does tiled rendering.
//
///////////////////////////////////////////////////////////////////////////////

bool Viewer::writeImageFile ( const std::string &name, double percent ) const
{
  GUARD_MEMBERS;
  GUARD_CONTEXT;

  // Handle trivial case.
  if ( 1.0f == percent )
    return this->writeImageFile ( name );

  // Calculate width and height.
  const unsigned int h ( this->viewportHeight() * percent );
  const unsigned int w ( this->viewportWidth()  * percent );

  // Call the other one.
  return this->writeImageFile ( name, h, w );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the model to the file.
//
///////////////////////////////////////////////////////////////////////////////

bool Viewer::writeImageFile ( const std::string &name, unsigned int height, unsigned int width ) const
{
  GUARD_MEMBERS;

  // Safely...
  try
  {
    Viewer *me ( const_cast < Viewer * > ( this ) );
    return me->_writeImageFile ( name, height, width );
  }

  // Catch standard exceptions.
  catch ( const std::exception &e )
  {
    Usul::Errors::Stack::instance().push ( e.what() );
    std::ostringstream out;
    out << "Error 2221890064: Standard exception caught while writing tiled image";
    Usul::Errors::Stack::instance().push ( out.str() );
  }

  // Catch all exceptions.
  catch ( ... )
  {
    std::ostringstream out;
    out << "Error 2205598475: Unknown exception caught while writing tiled image";
    Usul::Errors::Stack::instance().push ( out.str() );
  }

  // If we get to here then it did not work.
  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Generate the image.
//
///////////////////////////////////////////////////////////////////////////////

bool Viewer::_writeImageFile ( const std::string &name, unsigned int height, unsigned int width )
{
  GUARD_MEMBERS;
  GUARD_CONTEXT;

  // Declare image up here.
  osg::ref_ptr<osg::Image> image ( 0x0 );

  // Need local scope.
  {
    // Save and restore.
    OsgTools::ScopedViewport savedViewport ( _sceneView.get() );
    OsgTools::ScopedProjection savedProjection ( _sceneView.get() );

    // Tile height and width
    const unsigned int tileWidth  ( 256 );
    const unsigned int tileHeight ( 256 );

    // Generate the image.
    image = OsgTools::Images::makeTiledImage ( height, width, this->_unknown() );
  }

  // The scene can get messed up while generating tiles, so render again.
  this->render();

  // Write the image to file.
  return ( ( image.valid() ) ? osgDB::writeImageFile ( *image, name ) : false );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the model to the file.
//
///////////////////////////////////////////////////////////////////////////////

bool Viewer::writeSceneFile ( const std::string &name ) const
{
  GUARD_MEMBERS;
  const osg::Node *model ( this->_getModel() );
  return ( ( 0x0 == model ) ? false : osgDB::writeNodeFile ( *model, name ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the model.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::model ( osg::Node *node )
{
  GUARD_MEMBERS;

  // Set the new node if it is different.
  if ( this->_getModel() != node )
    this->_setModel ( node );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Resize the window.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::resize ( unsigned int w, unsigned int h )
{
  GUARD_MEMBERS;

  // Set the viewport.
  this->viewport ( 0, 0, w, h );

  // Set the projection matrix.
  double fovy  ( OsgTools::Render::CAMERA_FOV_Y );
  double zNear ( OsgTools::Render::CAMERA_Z_NEAR );
  double zFar  ( OsgTools::Render::CAMERA_Z_FAR );
  double width ( w ), height ( h );
  double aspect ( width / height );
  _sceneView->setProjectionMatrixAsPerspective ( fovy, aspect, zNear, zFar );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the polygon mode.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::polygonModeSet ( osg::PolygonMode::Face face, osg::PolygonMode::Mode mode )
{
  GUARD_MEMBERS;
  OsgTools::State::PolygonMode::set ( face, mode, _sceneView->getGlobalStateSet() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Toggle the polygon mode.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::polygonModeToggle ( osg::PolygonMode::Face face, osg::PolygonMode::Mode mode )
{
  GUARD_MEMBERS;
  OsgTools::State::PolygonMode::toggle ( face, mode, _sceneView->getGlobalStateSet() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Toggle the polygon mode.
//
///////////////////////////////////////////////////////////////////////////////

bool Viewer::polygonModeHas ( osg::PolygonMode::Face face, osg::PolygonMode::Mode mode ) const
{
  GUARD_MEMBERS;
  return OsgTools::State::PolygonMode::has ( face, mode, _sceneView->getGlobalStateSet() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Toggle the polygon mode.
//
///////////////////////////////////////////////////////////////////////////////

bool Viewer::polygonModeHas ( osg::PolygonMode::Face face ) const
{
  GUARD_MEMBERS;
  return OsgTools::State::PolygonMode::has ( face, _sceneView->getGlobalStateSet() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Toggle the polygon mode.
//
///////////////////////////////////////////////////////////////////////////////

bool Viewer::polygonModeHas() const
{
  GUARD_MEMBERS;
  return OsgTools::State::PolygonMode::has ( _sceneView->getGlobalStateSet() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove the polygon mode.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::polygonModeRemove()
{
  GUARD_MEMBERS;
  OsgTools::State::PolygonMode::remove ( _sceneView->getGlobalStateSet() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the shade model.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::shadeModelSet ( osg::ShadeModel::Mode mode )
{
  GUARD_MEMBERS;
  OsgTools::State::ShadeModel::set ( mode, _sceneView->getGlobalStateSet() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Toggle the shade model.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::shadeModelToggle ( osg::ShadeModel::Mode mode )
{
  GUARD_MEMBERS;
  OsgTools::State::ShadeModel::toggle ( mode, _sceneView->getGlobalStateSet() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Toggle the shade model.
//
///////////////////////////////////////////////////////////////////////////////

bool Viewer::shadeModelHas ( osg::ShadeModel::Mode mode ) const
{
  GUARD_MEMBERS;
  return OsgTools::State::ShadeModel::has ( mode, _sceneView->getGlobalStateSet() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Toggle the shade model.
//
///////////////////////////////////////////////////////////////////////////////

bool Viewer::shadeModelHas() const
{
  GUARD_MEMBERS;
  return OsgTools::State::ShadeModel::has ( _sceneView->getGlobalStateSet() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the light state.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::lighting ( bool state )
{
  GUARD_MEMBERS;
  OsgTools::State::StateSet::setLighting ( _sceneView->getGlobalStateSet(), state );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the light state.
//
///////////////////////////////////////////////////////////////////////////////

bool Viewer::lighting() const
{
  GUARD_MEMBERS;
  return OsgTools::State::StateSet::getLighting ( _sceneView->getGlobalStateSet() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the light state.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::lightBothSides ( bool state )
{
  GUARD_MEMBERS;
  OsgTools::State::StateSet::setTwoSidedLighting ( _sceneView->getGlobalStateSet(), state );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the light state.
//
///////////////////////////////////////////////////////////////////////////////

bool Viewer::lightBothSides() const
{
  GUARD_MEMBERS;
  return OsgTools::State::StateSet::getTwoSidedLighting ( _sceneView->getGlobalStateSet() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the clipping plane distances.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::nearFar ( double &n, double &f ) const
{
  GUARD_MEMBERS;

  // Get the projection matrix.
  const osg::Matrixd &P ( _sceneView->getProjectionMatrix() );

  // Try to get perspective parameters.
  double left, right, bottom, top;
  if ( P.getFrustum ( left, right, bottom, top, n, f ) )
    return;

  // Try to get orthographic parameters.
  if ( P.getOrtho ( left, right, bottom, top, n, f ) )
    return;

  // If we get to here then it did not work.
  throw ( std::runtime_error ( "Error 1055110277: Failed to calculate far clipping distance" ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Record the start-time.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::timeStart ( const std::string &name )
{
  GUARD_MEMBERS;
  TimePair start ( Usul::System::milliseconds(), 0 );
  _times[name].push_back ( start );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Record the stop-time.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::timeStop ( const std::string &name )
{
  GUARD_MEMBERS;
  TimeHistory &h = _times[name];
  TimePair &p = h.back();
  p.second = Usul::System::milliseconds();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the last time.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Types::Uint64 Viewer::timeLast ( const std::string &name ) const
{
  GUARD_MEMBERS;

  // Try to get the time-history.
  TimeHistories::const_iterator i = _times.find ( name );
  if ( _times.end() == i )
    return 0;

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

Usul::Types::Uint64 Viewer::timeAverage ( const std::string &name ) const
{
  GUARD_MEMBERS;

  // Try to get the time-history.
  TimeHistories::const_iterator i = _times.find ( name );
  if ( _times.end() == i )
    return 0;

  // Sum the durations.
  const TimeHistory &h = i->second;
  Usul::Types::Uint64 sum ( 0 );
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
//  Get a formatted string with time information.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::timeInfo ( std::string &s ) const
{
  // Need a buffer.
  const unsigned int size ( 1024 );
  char buf[size];

  // Convert milliseconds to seconds.
  const double ms2s ( 0.001 );

  // Get necessary components.
  double lu ( ms2s * this->timeLast    ( "update" ) );
  double lc ( ms2s * this->timeLast    ( "cull"   ) );
  double ld ( ms2s * this->timeLast    ( "draw"   ) );
  double au ( ms2s * this->timeAverage ( "update" ) );
  double ac ( ms2s * this->timeAverage ( "cull"   ) );
  double ad ( ms2s * this->timeAverage ( "draw"   ) );

  // Format the text.
  ::sprintf ( buf, "(Last:Avg)   Update: %0.6f:%0.6f   Cull: %0.6f:%0.6f   Draw: %0.6f:%0.6f", 
              lu, au, lc, ac, ld, ad );

  // Set the text.
  s = buf;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Animate from one position and orientation to another.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::animate ( const osg::Vec3d &t1, const osg::Quat &r1, 
                       const osg::Vec3d &t2, const osg::Quat &r2, 
                       double duration )
{
  GUARD_MEMBERS;

  // Punt if not given needed interface.
  if ( false == _timerServer.valid() )
    return;

  // Initialize animation and register timer callback.
  _animation.init ( t1, r1, t2, r2, duration );
  _timerServer->timerCallbackSet ( ID_ANIMATION_TIMEOUT, duration, this->_unknown() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Rotate the scene.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::rotate ( const osg::Vec3d &axis, double radians, double duration, IUnknown *caller )
{
  GUARD_MEMBERS;

  // Get the current state.
  const osg::Matrixd &M1 ( _sceneView->getViewMatrix() );
  osg::Vec3d T1 ( M1.getTrans() );
  osg::Quat R1; M1.get ( R1 );

  // Rotate the current view by this amount.
  const osg::Matrixd M2 ( osg::Matrixd::rotate ( radians, axis ) * M1 );

  // Get the new state.
  osg::Quat R2; M2.get ( R2 );

  // Animate.
  this->animate ( T1, R1, T1, R2, duration, caller );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Animate as configured.
//
///////////////////////////////////////////////////////////////////////////////

bool Viewer::timerEventNotify ( unsigned int id, IUnknown *caller )
{
  GUARD_MEMBERS;

  // See what kind of event it is.
  switch ( id )
  {
    case ID_ANIMATION_TIMEOUT:
      return this->_animate();

    default:
      USUL_ASSERT ( false ); // Heads up
      return false;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Animate as configured.
//
///////////////////////////////////////////////////////////////////////////////

bool Viewer::_animate()
{
  GUARD_MEMBERS;

  // Get the matrix and see if we should continue.
  osg::Matrixd mat;
  const bool keepGoing ( _animation.matrix ( mat ) );

  // Set the viewer's matrix and render.
  _sceneView->setViewMatrix ( mat );
  this->render();

  // Return the result.
  return keepGoing;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Perform an incremental "spin".
//
///////////////////////////////////////////////////////////////////////////////

bool Viewer::spinPerformMotion()
{
  GUARD_MEMBERS;

  // Handle no manipulator.
  if ( 0x0 == this->_navManip() )
    return false;

  // Stop if the global flag says to.
  if ( false == Viewer::spinAllowed() )
    return false;

  // Stop if this instance should.
  if ( false == this->spinState() )
    return false;

  // Make the adapters.
  EventAdapter::ValidRefPtr ea ( new EventAdapter );
  ActionAdapter aa ( *this );
  ea->setWindowSize ( Usul::Math::Vec2ui ( this->viewportWidth(), this->viewportHeight() ) );
  ea->setEventType ( EventAdapter::FRAME );

  // Ask the manipulator to handle the event.
  this->_navManip()->handle ( *ea, aa );

  // Set new matrix and render.
  _sceneView->setViewMatrix ( this->_navManip()->getInverseMatrix() );
  this->render();

  // Keep going.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the flag that says this viewer should be spinning.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::spinState ( bool state )
{
  GUARD_MEMBERS;
  _flags = ( ( state ) ? Usul::Bits::add ( _flags, _SPINNING ) : Usul::Bits::remove ( _flags, _SPINNING ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the flag that says this viewer should be spinning.
//
///////////////////////////////////////////////////////////////////////////////

bool Viewer::spinState() const
{
  GUARD_MEMBERS;
  const bool state ( Usul::Bits::has ( _flags, _SPINNING ) );
  return state;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Convenience function for setting the global "allow spin" flag.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::spinAllowed ( bool state )
{
  Usul::Shared::Preferences::instance().setBool ( OsgTools::Render::ALLOW_SPIN, state );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Convenience function for getting the global "allow spin" flag.
//
///////////////////////////////////////////////////////////////////////////////

bool Viewer::spinAllowed()
{
  return Usul::Shared::Preferences::instance().getBool ( OsgTools::Render::ALLOW_SPIN );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Push the navigation manipulator.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::navManipPush ( NavManip *nav )
{
  GUARD_MEMBERS;

  // If we have a valid navigator...
  if ( nav )
  {
    // Push it onto the stack.
    _navManips.push ( nav );

    // Make sure it has our scene.
    nav->setNode ( _root.get() );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Pop the navigation manipulator.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::navManipPop()
{
  GUARD_MEMBERS;

  // Pop top navigator.
  if ( false == _navManips.empty() )
    _navManips.pop();

  // If we have another one then set it's scene.
  if ( this->_navManip() )
    this->_navManip()->setNode ( _root.get() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Append a copy of the navigator after the current position.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::navHistoryAppend()
{
  GUARD_MEMBERS;

  // If we have a navigator...
  NavManipPtr nav ( this->navManipCloneCurrent() );
  if ( nav.valid() )
  {
    // Order is important! This will trim everything after the position.
    _navHistory.first.resize ( _navHistory.second + 1 );
    _navHistory.first.push_back ( nav );
    _navHistory.second = _navHistory.first.size() - 1;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Can we redo the navigation?
//
///////////////////////////////////////////////////////////////////////////////

bool Viewer::navHistoryCanRedo() const
{
  GUARD_MEMBERS;
  const bool can ( ( _navHistory.first.size() > _navHistory.second + 1 ) && ( this->_navManip() ) );
  return can;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Can we undo the navigation?
//
///////////////////////////////////////////////////////////////////////////////

bool Viewer::navHistoryCanUndo() const
{
  GUARD_MEMBERS;
  const bool can ( ( _navHistory.second > 0 ) && ( this->_navManip() ) );
  return can;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Redo the navigation.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::navHistoryRedo()
{
  GUARD_MEMBERS;
  if ( this->navHistoryCanRedo() && this->_navManip() )
  {
    ++_navHistory.second;
    this->navManipPop();
    this->navManipPush ( this->navHistoryCloneCurrent() );
    _sceneView->setViewMatrix ( this->_navManip()->getInverseMatrix() );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Undo the navigation.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::navHistoryUndo()
{
  GUARD_MEMBERS;
  if ( this->navHistoryCanUndo() && this->_navManip() )
  {
    --_navHistory.second;
    this->navManipPop();
    this->navManipPush ( this->navHistoryCloneCurrent() );
    _sceneView->setViewMatrix ( this->_navManip()->getInverseMatrix() );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Copy the navigation manipulator.
//
///////////////////////////////////////////////////////////////////////////////

Viewer::NavManip *Viewer::navManipCloneCurrent() const
{
  GUARD_MEMBERS;
  NavManipPtr nav ( ( this->_navManip() ) ? ( reinterpret_cast < NavManip * > ( this->_navManip()->clone ( osg::CopyOp::DEEP_COPY_ALL ) ) ) : 0x0 );
  return nav.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return a copy of the current navigator in the history.
//
///////////////////////////////////////////////////////////////////////////////

Viewer::NavManip *Viewer::navHistoryCloneCurrent() const
{
  GUARD_MEMBERS;
  NavManipPtr current ( _navHistory.first.at ( _navHistory.second ) );
  NavManipPtr clone ( reinterpret_cast < NavManip * > ( ( current.valid() ) ? current->clone ( osg::CopyOp::DEEP_COPY_ALL ) : 0x0 ) );
  return clone.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to make a bound.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  template < class BoundType, class Generator > struct MakeBound
  {
    osg::Group *operator () ( const osg::Node &node, const osg::Vec4f &color )
    {
      // Expand by the bounds.
      BoundType bound;
      bound.expandBy ( node.getBound() );

      // Make a new bound.
      Generator generator ( bound );
      generator.color_policy().color ( color );

      // Position it.
      osg::ref_ptr<osg::MatrixTransform> mt ( new osg::MatrixTransform );
      mt->setMatrix ( osg::Matrixd::translate ( bound.center() ) );
      mt->addChild ( generator() );

      // Wire-frame.
      OsgTools::State::StateSet::setPolygonsLines ( mt.get(), true );
      OsgTools::State::StateSet::setLighting ( mt.get(), false );

      // Return the matrix-transform.
      return mt.release();
    }
  };
}


///////////////////////////////////////////////////////////////////////////////
//
//  Show/hide the bounding box.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::boundingBox ( bool state )
{
  GUARD_MEMBERS;

  // Look for the group.
  osg::ref_ptr<osg::Group> &group = _groupMap[OsgTools::Render::BOUNDING_BOX];

  // Remove the bounding box if it exists.
  if ( group.valid() )
  {
    _decorator->removeChild ( group.get() );
    group = 0x0;
  }

  // If we are supposed to show the bounding box...
  if ( state )
  {
    // Generate the bound.
    typedef Detail::MakeBound<osg::BoundingBox,OsgTools::ColorBox> MakeBound;
    MakeBound maker;
    osg::ref_ptr<osg::Group> node ( maker ( *_model, osg::Vec4 ( 0, 0, 1, 1 ) ) );

    // Add it to the group-map and scene.
    group = node.get();
    _decorator->addChild ( group.get() );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Show/hide the bounding sphere.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::boundingSphere ( bool state )
{
  GUARD_MEMBERS;

  // Look for the group.
  osg::ref_ptr<osg::Group> &group = _groupMap[OsgTools::Render::BOUNDING_SPHERE];

  // Remove the bounding sphere if it exists.
  if ( group.valid() )
  {
    _decorator->removeChild ( group.get() );
    group = 0x0;
  }

  // If we are supposed to show the bounding sphere...
  if ( state )
  {
    // Generate the bound.
    typedef Detail::MakeBound<osg::BoundingSphere,OsgTools::ColorSphere> MakeBound;
    MakeBound maker;
    osg::ref_ptr<osg::Group> node ( maker ( *_model, osg::Vec4 ( 1, 0, 0, 1 ) ) );

    // Add it to the group-map and scene.
    group = node.get();
    _decorator->addChild ( group.get() );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is the bounding box showing?
//
///////////////////////////////////////////////////////////////////////////////

bool Viewer::boundingBox() const
{
  GUARD_MEMBERS;
  GroupMap::const_iterator i = _groupMap.find ( OsgTools::Render::BOUNDING_BOX );
  return ( ( _groupMap.end() != i ) && ( i->second.valid() ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is the bounding sphere showing?
//
///////////////////////////////////////////////////////////////////////////////

bool Viewer::boundingSphere() const
{
  GUARD_MEMBERS;
  GroupMap::const_iterator i = _groupMap.find ( OsgTools::Render::BOUNDING_SPHERE );
  return ( ( _groupMap.end() != i ) && ( i->second.valid() ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add text to the given matrix in the row and column.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::textCellSet ( double x, double y, unsigned int row, unsigned int col, const std::string &t )
{
  GUARD_MEMBERS;

  // Get xPos and yPos of upper left most point of the matrix.
  const unsigned int xPos ( this->viewportWidth()  * x );
  const unsigned int yPos ( this->viewportHeight() * y );

  // Find the xy pair if it exists
  const TextKey key ( x, y );
  TextMap::iterator i = _textMap.find ( key );
  if ( _textMap.end() == i )
    this->textMatrixResize ( x, y, 5, 5, 20, 20 ); // TODO, remove hard-coded values?

  // Get the matrix and geode
  TextMatrix &matrix    ( i->second.first.first   );
  GeodePtr geode        ( i->second.first.second  );
  const unsigned int rowHeight   ( i->second.second.first  );
  const unsigned int columnWidth ( i->second.second.second );

  // Do we have enough rows?
  if ( matrix.size() <= row )
    matrix.resize ( row + 1 );

  // Do we have enough columns?
  if ( matrix.at(row).size() <= col )
    matrix.at(row).resize ( col + 1 );

  // Get the osgText from matrix
  TextPtr &text = matrix[row][col];

  // If text is not valid, create a new one
  if ( !text.valid() )
  {
    text = new osgText::Text;

    osg::Vec4f layoutColor ( 1, 0, 0, 1 );
    const double layoutCharacterSize ( 20.0 );

    text->setColor ( layoutColor );
    text->setCharacterSize ( layoutCharacterSize );

    const double x ( xPos + ( col * columnWidth ) );
    const double y ( yPos - ( row * rowHeight ) );
    text->setPosition ( osg::Vec3d ( x, y, 0 ) );

    // Add text to geode
    geode->addDrawable ( text.get() );
  }

  // Set the text string
  text->setText ( t );
  text->update();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Resize the text matrix.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::textMatrixResize ( double x, double y, unsigned int numRows, unsigned int numCols, int rowHeight, int columnWidth )
{
  GUARD_MEMBERS;

  // Get the group for text matrices, if it exists.
  osg::ref_ptr<osg::Group> &group = _groupMap[OsgTools::Render::TEXT_MATRIX];

  if ( !group.valid() )
  {
    osg::ref_ptr<osg::Projection> projection ( new osg::Projection );
    projection->setMatrix ( osg::Matrixd::ortho2D ( 0, this->viewportWidth(), 0, this->viewportHeight() ) );

    // Make sure it's drawn last ( on top )
    osg::ref_ptr<osg::StateSet> ss ( projection->getOrCreateStateSet() );
    ss->setRenderBinDetails ( 100, "RenderBin" );
	  ss->setMode ( GL_DEPTH_TEST, osg::StateAttribute::OFF );

    osg::ref_ptr<osg::MatrixTransform> absolute ( new osg::MatrixTransform );
    absolute->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
    absolute->setMatrix(osg::Matrixd::identity());

    projection->addChild ( absolute.get() );
    group = projection.get();
    _decorator->addChild ( group.get() );
  }

  // This is the matrix transform under the projection matrix
  osg::ref_ptr < osg::Group > node ( group->getChild ( 0 )->asGroup() );

  // Find the xy pair if it exists
  TextKey key ( x, y );
  TextMap::iterator i = _textMap.find ( key );
  if ( _textMap.end() == i )
  {
    // Create a new matrix and geode to hold osgText
    TextMatrix matrix ( numRows );
    for ( unsigned int j = 0; j < numRows; ++j )
      matrix.at(j).resize ( numCols );

    _textMap[key] = TextMatrixData ( TextMatrixPair ( matrix, new osg::Geode ), TextMatrixProperties ( rowHeight, columnWidth ) );
    i = _textMap.find ( key );

    // Add the geode to the text group
    if ( node.valid() )
      node->addChild ( i->second.first.second.get() );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove a text matrix.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::textMatrixRemove ( double x, double y )
{
  GUARD_MEMBERS;

  // Get the group for text matrices, if it exists.
  osg::ref_ptr<osg::Group> &group = _groupMap[OsgTools::Render::TEXT_MATRIX];

  if ( group.valid() )
  {
    TextKey key ( x, y );
    TextMap::iterator i = _textMap.find ( key );
    if ( _textMap.end() != i )
    {
      GeodePtr geode ( i->second.first.second );
      GroupPtr group ( geode->getParent ( 0 ) );
      group->removeChild ( geode.get() );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Filters that correspond to scene formats that can be written.
//
///////////////////////////////////////////////////////////////////////////////

Viewer::Filters Viewer::filtersWriteScene() const
{
  GUARD_MEMBERS;
  Filters filters;
  filters.push_back ( Filter ( "OpenSceneGraph Binary (*.ive)", "*.ive" ) );
  filters.push_back ( Filter ( "OpenSceneGraph ASCII (*.osg)",  "*.osg" ) );
  return filters;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Filters that correspond to image formats that can be written.
//
///////////////////////////////////////////////////////////////////////////////

Viewer::Filters Viewer::filtersWriteImage() const
{
  GUARD_MEMBERS;
  Filters filters;
  filters.push_back ( Filter ( "JPEG Image (*.jpg)", "*.jpg"   ) );
  filters.push_back ( Filter ( "PNG Image (*.png)", "*.png"    ) );
  filters.push_back ( Filter ( "BMP Image (*.bmp)", "*.bmp"    ) );
  filters.push_back ( Filter ( "RGBA Image (*.rgba)", "*.rgba" ) );
  return filters;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the model.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node *Viewer::_getModel()
{
  GUARD_MEMBERS;
  USUL_ASSERT ( 0 == _model->getNumChildren() || 1 == _model->getNumChildren() );
  return ( ( _model->getNumChildren() ) ? _model->getChild ( 0 ) : 0x0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the model.
//
///////////////////////////////////////////////////////////////////////////////

const osg::Node *Viewer::_getModel() const
{
  GUARD_MEMBERS;
  USUL_ASSERT ( 0 == _model->getNumChildren() || 1 == _model->getNumChildren() );
  return ( ( _model->getNumChildren() ) ? _model->getChild ( 0 ) : 0x0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the model.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::_setModel ( osg::Node *node )
{
  GUARD_MEMBERS;

  // Should always be true.
  USUL_ASSERT ( 0 == _model->getNumChildren() || 1 == _model->getNumChildren() );

  // Remove what we have.
  OsgTools::Group::removeAllChildren ( _model.get() );
  _model->dirtyBound();

  // If we are given a valid node.
  if ( node )
  {
    _model->addChild ( node );
    node->dirtyBound();
  }

  // Reset the lod-callback cache.
  _lods.first = false;
  _lods.second.clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get group with given key.  Creates one if doesn't exist
//
///////////////////////////////////////////////////////////////////////////////

osg::Group *Viewer::_getGroup ( const std::string &key )
{
  GUARD_MEMBERS;

  // Get the group.
  osg::ref_ptr<osg::Group> &group = _groupMap[key];

  // Has the group been created
  if ( false == group.valid() )
  {
    // Make a new group
    group = new osg::Group;

    // Set the name
    group->setName ( key );

    // Add the group to the scene
    _decorator->addChild ( group.get() );
  }

  // Return the group.
  return group.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add the clip-box.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::clipPlanesAdd ( const osg::BoundingBox& bb )
{
  GUARD_MEMBERS;
  _clipped->createClipBox ( bb );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a clipping plane to the scene.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::clipPlaneAdd ( const osg::Plane &plane )
{
  GUARD_MEMBERS;

  // Make the clipping plane
  osg::ref_ptr < osg::ClipPlane > clipPlane ( new osg::ClipPlane ( _clipped->getNumClipPlanes(), plane ) );

  // Add the plane
  _clipped->addClipPlane ( clipPlane.get() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a default clipping plane to the scene.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::clipPlaneAddDefault()
{
#if 0
  // Get the bounding box
  osg::BoundingBox bb; 
  bb.expandBy ( _clipped->getBound() );

  // Intial points for the plane
  osg::Vec3 top_left     ( bb.corner ( 1 ) );
  osg::Vec3 bottom_left  ( bb.corner ( 0 ) );
  osg::Vec3 bottom_right ( bb.corner ( 6 ) );
  osg::Vec3 top_right    ( bb.corner ( 7 ) );

  // Make the vertices
  osg::ref_ptr < osg::Vec3Array > vertices ( new osg::Vec3Array );
  vertices->push_back ( top_left );
  vertices->push_back ( bottom_left );
  vertices->push_back ( bottom_right );
  vertices->push_back ( top_right );

  // Make the normals and colors
  osg::ref_ptr < osg::Vec3Array > normals ( new osg::Vec3Array ( 1 ) );
  osg::ref_ptr < osg::Vec4Array > colors  ( new osg::Vec4Array ( 1 ) );
  normals->push_back ( osg::Vec3 ( 0.0f, 0.0f, 1.0f ) );
  colors->push_back  ( osg::Vec4 ( 1.0f, 1.0f, 1.0f, 0.1f ) );

  // Make the geometry and set arrays
  osg::ref_ptr < osg::Geometry > geometry ( new osg::Geometry );
  geometry->setVertexArray ( vertices.get() );
  geometry->setNormalArray ( normals.get()  );
  geometry->setColorArray  ( colors.get()   );

  // Set the bindings
  geometry->setNormalBinding ( osg::Geometry::BIND_OVERALL );
  geometry->setColorBinding  ( osg::Geometry::BIND_OVERALL );

  // Add the primitive set
  geometry->addPrimitiveSet ( new osg::DrawArrays ( osg::PrimitiveSet::QUADS, 0, vertices->size() ) );

  // Get or create the state set
  osg::ref_ptr < osg::StateSet > ss ( geometry->getOrCreateStateSet() );

  // Set needed states from transparency
  ss->setMode ( GL_BLEND,      osg::StateAttribute::ON  );
  ss->setMode ( GL_DEPTH_TEST, osg::StateAttribute::OFF );
  ss->setRenderingHint ( osg::StateSet::TRANSPARENT_BIN );

  // Make the geode and add geometry
  osg::ref_ptr < osg::Geode > geode ( new osg::Geode );
  geode->addDrawable ( geometry.get() );

  // Make a geometry for the edge of plane
  osg::ref_ptr< osg::Geometry > edge ( new osg::Geometry );

  // Make the colors
  osg::ref_ptr < osg::Vec4Array > eColors ( new osg::Vec4Array );
  eColors->push_back ( osg::Vec4 ( 0.0f, 0.0f, 0.0f, 1.0f ) );

  // Set the arrays
  edge->setVertexArray ( vertices.get() );
  edge->setNormalArray ( normals.get() );
  edge->setColorArray  ( eColors.get() );

  // Set the bindings
  edge->setNormalBinding ( osg::Geometry::BIND_OVERALL );
  edge->setColorBinding  ( osg::Geometry::BIND_OVERALL );

  // Add the primitive set
  edge->addPrimitiveSet ( new osg::DrawArrays( osg::PrimitiveSet::LINE_LOOP, 0, vertices->size() ) );

  // Add the geometry
  geode->addDrawable ( edge.get() );

  // Make geode for dragging
  osg::ref_ptr< osg::Geode > draggedGeode ( new osg::Geode );

  // Add the same quad as normal geode
  draggedGeode->addDrawable ( reinterpret_cast < osg::Geometry* > ( geometry->clone ( osg::CopyOp::DEEP_COPY_ALL ) ) );

  // Make a geometry for the edge of plane while dragging
  osg::ref_ptr< osg::Geometry > redEdge ( reinterpret_cast < osg::Geometry* > ( edge->clone ( osg::CopyOp::DEEP_COPY_ALL ) ) );

  // Make color array for red color
  osg::ref_ptr< osg::Vec4Array > redColors ( new osg::Vec4Array ( 1 ) );

  // Set the color value
  redColors->at( 0 ) = osg::Vec4 ( 1.0, 0.0, 0.0, 1.0 );

  // Set the color array
  redEdge->setColorArray  ( redColors.get() );

  // Add the red edge to dragged geode
  draggedGeode->addDrawable( redEdge.get() );

  // Define the plane on the diagonal of the bounding box
  osg::Plane plane ( top_left, bottom_left, top_right );

  //Make the clipping plane
  osg::ref_ptr< osg::ClipPlane > clipPlane ( new osg::ClipPlane( _clipped->getNumClipPlanes(), plane ) );

  // Get the group to place the plane geometrys under
  osg::ref_ptr< osg::Group > group ( this->_getGroup ( OsgTools::Render::CLIPPING_PLANES ) );

  Circular dependency. Need a clip-plane-dragger class.
  typedef OsgNodes::Draggers::Translate1 Dragger;

  // Make a dragger
  osg::ref_ptr< Dragger > dragger ( new Dragger ( Dragger::Z ) );

  // Set the geometrys
  dragger->geometry( Dragger::NORMAL, geode.get() );
  dragger->geometry( Dragger::DRAGGING, draggedGeode.get() );

  // Make the callback
  osg::ref_ptr< OsgFox::Views::ClipPlaneCallback > callback ( new OsgFox::Views::ClipPlaneCallback ( top_left, bottom_left, top_right, clipPlane.get() ) );

  // Add the callback
  dragger->callback( Dragger::MOVE, callback.get() );

  typedef OsgNodes::Draggers::Rotate1 Rotate;

  // Make a dragger to rotate along the x axis
  osg::ref_ptr< Rotate > rx ( new Rotate ( osg::Vec3 ( 1.0, 0.0, 0.0 ) ) );

  osg::ref_ptr<osg::Sphere> sx ( new osg::Sphere );
  
  const double xMid ( ( bb.xMax() - bb.xMin() ) / 2);

  sx->setCenter ( osg::Vec3 ( bb.xMin() + xMid, bb.yMax(), bb.zMax() ) );
  sx->setRadius ( 0.1f );
  osg::ref_ptr<osg::ShapeDrawable> sdx ( new osg::ShapeDrawable ( sx.get() ) );

  osg::ref_ptr < osg::Geode > gx ( new osg::Geode );

  gx->addDrawable ( sdx.get() );

  rx->geometry( Dragger::NORMAL, gx.get() );
  rx->geometry( Dragger::DRAGGING, gx.get() );

  // Make a dragger to rotate along the y axis
  osg::ref_ptr< Rotate > ry ( new Rotate ( osg::Vec3 ( 0.0, 1.0, 0.0 ) ) );

  osg::ref_ptr<osg::Sphere> sy ( new osg::Sphere );

  const double yMid ( ( bb.yMax() - bb.yMin() ) / 2);
  const double zMid ( ( bb.zMax() - bb.zMin() ) / 2);
  
  sy->setCenter ( osg::Vec3 ( bb.xMax(), bb.yMin() + yMid, bb.zMin() + zMid ) );
  sy->setRadius ( 0.1f );
  osg::ref_ptr<osg::ShapeDrawable> sdy ( new osg::ShapeDrawable ( sy.get() ) );

  osg::ref_ptr < osg::Geode > gy ( new osg::Geode );

  gy->addDrawable ( sdy.get() );

  ry->geometry( Dragger::NORMAL, gy.get() );
  ry->geometry( Dragger::DRAGGING, gy.get() );

  // Group for the three draggers
  osg::ref_ptr < osg::Group > draggers ( new osg::Group );

  group->addChild ( draggers.get() );

  // Add the draggers to the scene
  draggers->addChild( rx.get() );
  draggers->addChild( ry.get() );
  draggers->addChild( dragger.get() );

  // Add the plane
  _clipped->addClipPlane ( clipPlane.get() );
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove a clipping plane from the scene
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::clipPlaneRemove ( unsigned int index )
{
  GUARD_MEMBERS;

  // Remove the clipping planes.
  _clipped->removeClipPlane ( index );

  // Remove corresponding geometry from associated group.
  osg::ref_ptr< osg::Group > group ( this->_getGroup ( OsgTools::Render::CLIPPING_PLANES ) );
  group->removeChild ( index );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove all the clipping planes.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::clipPlanesRemoveAll()
{
  GUARD_MEMBERS;

  // Remove all the clipping planes.
  while ( _clipped->getNumClipPlanes() )
    _clipped->removeClipPlane ( _clipped->getNumClipPlanes() - 1 );

  // Remove corresponding geometry from associated group.
  OsgTools::Group::removeAllChildren ( this->_getGroup ( OsgTools::Render::CLIPPING_PLANES ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove all the clipping planes.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Viewer::clipPlanesGetNum() const
{
  GUARD_MEMBERS;
  unsigned int num ( _clipped->getNumClipPlanes() );
  return num;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set projection matrix.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::projectionMatrix ( double left, double right, double bottom, double top, double zNear, double zFar )
{
  GUARD_MEMBERS;
  _sceneView->setProjectionMatrixAsFrustum ( left, right, bottom, top, zNear, zFar );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get projection matrix.
//
///////////////////////////////////////////////////////////////////////////////

bool Viewer::projectionMatrix ( double &fovy, double &aspect, double &zNear, double &zFar )
{
  GUARD_MEMBERS;
  return _sceneView->getProjectionMatrixAsPerspective ( fovy, aspect, zNear, zFar );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the viewport.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::viewport ( int x, int y, unsigned int w, unsigned int h )
{
  GUARD_MEMBERS;

  // Ignore bad values, they make a poor viewport.
  if ( w <= 1 || 
       h <= 1 ||
       w > static_cast < unsigned int > ( std::numeric_limits<int>::max() ) || 
       h > static_cast < unsigned int > ( std::numeric_limits<int>::max() ) )
  {
    // Set the viewport.
    _sceneView->setViewport ( 0, 0, (int) w, (int) h );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the viewport.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::viewport ( int &x, int &y, unsigned int &w, unsigned int &h ) const
{
  GUARD_MEMBERS;
  int width ( 0 ), height ( 0 );
  _sceneView->getViewport ( x, y, width, height );
  w = ( ( width  > 0 ) ? static_cast < unsigned int > ( width  ) : 0 );
  h = ( ( height > 0 ) ? static_cast < unsigned int > ( height ) : 0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the viewport.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Viewer::viewportHeight() const
{
  GUARD_MEMBERS;
  const osg::Viewport *vp ( _sceneView->getViewport() );
  const int h ( ( vp ) ? vp->height() : 0 );
  return ( ( h > 0 ) ? static_cast < unsigned int > ( h ) : 0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the viewport.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Viewer::viewportWidth() const
{
  GUARD_MEMBERS;
  const osg::Viewport *vp ( _sceneView->getViewport() );
  const int w ( ( vp ) ? vp->width() : 0 );
  return ( ( w > 0 ) ? static_cast < unsigned int > ( w ) : 0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get sort back to front state.
//
///////////////////////////////////////////////////////////////////////////////

bool Viewer::sortBackToFront() const
{
  GUARD_MEMBERS;
  const bool answer ( Usul::Bits::has ( _flags, _SORT_BACK_TO_FRONT ) );
  return answer;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set sort back to front state.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::sortBackToFront ( bool b )
{
  GUARD_MEMBERS;

  if ( b )
  {
    _flags = Usul::Bits::add ( _flags, _SORT_BACK_TO_FRONT );
    osg::ref_ptr < OsgTools::SetSortToFrontCallback > visitor ( new OsgTools::SetSortToFrontCallback );
    _root->accept ( *visitor );
  }

  else
  {
    _flags = Usul::Bits::remove ( _flags, _SORT_BACK_TO_FRONT );
    osg::ref_ptr < OsgTools::UnSetSortToFrontCallback > visitor ( new OsgTools::UnSetSortToFrontCallback );
    _root->accept ( *visitor );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the line segment into the scene that goes through the 2D coordinate.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::lineSegment ( const osg::Vec2d &mouse, osg::Vec3d &pt0, osg::Vec3d &pt1 ) const
{
  GUARD_MEMBERS;

  // Get the matrix.
  const osg::Matrixd &M ( _sceneView->getViewMatrix() );
  const osg::Matrixd &P ( _sceneView->getProjectionMatrix() );
  const osg::Matrixd MP ( M * P );
  const osg::Matrixd IMP ( osg::Matrixd::inverse ( MP ) );

  // Calculate the two points for our line-segment.
  pt0 = osg::Vec3d ( mouse[0], mouse[1], -1 ) * IMP;
  pt1 = osg::Vec3d ( mouse[0], mouse[1],  1 ) * IMP;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Convert pixel coordinates to normalized coordinates in the range [-1,1].
//
///////////////////////////////////////////////////////////////////////////////

osg::Vec2d Viewer::normalizedMouse ( const Usul::Math::Vec2ui &mouse ) const
{
  GUARD_MEMBERS;
  const double h ( this->viewportHeight() );
  const double w ( this->viewportWidth() );
  const double mx ( mouse[0] );
  const double my ( h - mouse[1] );
  const double nx ( 2.0 * mx / w - 1.0 );
  const double ny ( 2.0 * my / h - 1.0 );
  return osg::Vec2d ( nx, ny );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Intersect the scene.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::intersect ( const osg::Vec2d &mouse, bool justModel, osgUtil::IntersectVisitor::HitList &hits ) const
{
  GUARD_MEMBERS;

  // Calculate the two points for our line-segment.
  osg::Vec3d pt0, pt1;
  this->lineSegment ( mouse, pt0, pt1 );

  // Declare the pick-visitor.
  typedef osgUtil::IntersectVisitor Visitor;
  osg::ref_ptr<Visitor> visitor ( new Visitor );

  // Make the line segment.
  osg::ref_ptr<osg::LineSegment> ls ( new osg::LineSegment ( pt0, pt1 ) );
  visitor->addLineSegment ( ls.get() );

  // Intersect the scene.
  typedef osgUtil::IntersectVisitor::HitList HitList;
  osg::Node *node ( const_cast < osg::Node * > ( ( justModel ) ? this->_getModel() : _root.get() ) );
  if ( node )
    node->accept ( *visitor );

  // See if there was an intersection.
  if ( false == visitor->hits() )
    return;

  // Get the hit-list for our line-segment.
  const osgUtil::IntersectVisitor::HitList &found ( visitor->getHitList ( ls.get() ) );
  hits.insert ( hits.end(), found.begin(), found.end() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Intersect the scene.
//
///////////////////////////////////////////////////////////////////////////////

bool Viewer::intersect ( const osg::Vec2d &mouse, bool justModel, osgUtil::Hit &hit ) const
{
  GUARD_MEMBERS;

  // Get the hits.
  osgUtil::IntersectVisitor::HitList hits;
  this->intersect ( mouse, justModel, hits );

  // See if we hit anything.
  if ( hits.empty() )
    return false;

  // Assign the hit.
  hit = hits.front();

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the lod-callbacks.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::lodCullCallback ( osg::NodeCallback *cb )
{
  GUARD_MEMBERS;
  OsgTools::Render::setLodCullCallback ( cb, _root.get(), _lods );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the lod-callbacks.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::lodCullCallbackLow()
{
  this->lodCullCallback ( new OsgTools::Render::LowLodCallback );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the lod-callbacks.
//
///////////////////////////////////////////////////////////////////////////////

void Viewer::lodCullCallbackHigh()
{
  this->lodCullCallback ( new OsgTools::Render::HighLodCallback );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the navigation manipulator.
//
///////////////////////////////////////////////////////////////////////////////

Viewer::NavManip *Viewer::_navManip()
{
  GUARD_MEMBERS;
  return ( ( _navManips.empty() ) ? 0x0 : _navManips.top().get() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the navigation manipulator.
//
///////////////////////////////////////////////////////////////////////////////

const Viewer::NavManip *Viewer::_navManip() const
{
  GUARD_MEMBERS;
  return ( ( _navManips.empty() ) ? 0x0 : _navManips.top().get() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return unknown pointer to this instance.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *Viewer::_unknown()
{
  return this->queryInterface ( Usul::Interfaces::IUnknown::IID );
}
