
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The component class.
//
///////////////////////////////////////////////////////////////////////////////

#include "PathAnimation/PathAnimationComponent.h"

#include "Usul/Adaptors/Bind.h"
#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Commands/GenericCheckCommand.h"
#include "Usul/Components/Factory.h"
#include "Usul/Components/Manager.h"
#include "Usul/Convert/Convert.h"
#include "Usul/Documents/Manager.h"
#include "Usul/File/Path.h"
#include "Usul/File/Temp.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Interfaces/IBusyState.h"
#include "Usul/Interfaces/IFrameDump.h"
#include "Usul/Interfaces/IGroup.h"
#include "Usul/Interfaces/ILoadFileDialog.h"
#include "Usul/Interfaces/ISaveFileDialog.h"
#include "Usul/Interfaces/IRedraw.h"
#include "Usul/Interfaces/ITimerService.h"
#include "Usul/Interfaces/IUpdateSubject.h"
#include "Usul/Interfaces/IViewMatrix.h"
#include "Usul/Interfaces/IWriteMovieFile.h"
#include "Usul/Predicates/FileExists.h"
#include "Usul/Registry/Convert.h"
#include "Usul/Registry/Database.h"
#include "Usul/Registry/Constants.h"
#include "Usul/Scope/Caller.h"
#include "Usul/Strings/Format.h"
#include "Usul/Trace/Trace.h"
#include "Usul/Threads/Safe.h"

#include "MenuKit/Menu.h"
#include "MenuKit/Button.h"
#include "MenuKit/RadioButton.h"
#include "MenuKit/ToggleButton.h"

#include "GN/Algorithms/Fill.h"

#include "osg/Matrixd"
#include "osg/Vec3d"

namespace Sections = Usul::Registry::Sections;
typedef Usul::Registry::Database Reg;

USUL_DECLARE_COMPONENT_FACTORY ( PathAnimationComponent );
USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( PathAnimationComponent, PathAnimationComponent::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

PathAnimationComponent::PathAnimationComponent() : 
  BaseClass(),
  _pathsMenu ( new MenuKit::Menu ( "Camera &Paths" ) ),
  _cameraMenu ( new MenuKit::Menu ( "&Cameras" ) ),
  _currentPath ( 0x0 ),
  _paths(),
  _players(),
  _degree ( Reg::instance()[Sections::PATH_ANIMATION]["curve"]["degree"].get<unsigned int> ( 3 ) ),
  _writeMovie ( false ),
  _movieFilename(),
  _movieWriter(),
  _caller(),
  _numSteps ( Reg::instance()[Sections::PATH_ANIMATION]["curve"]["num_steps"].get<unsigned int> ( 100 ) ),
  _root ( 0x0 ),
  _showPath ( false ),
  _looping ( false ),
  _dirtyScene ( true ),
  _currentCamera ( 0 ),
  _renderLoop ( false ),
  _timer ( 0 ),
  _milliSeconds ( Reg::instance()[Sections::PATH_ANIMATION]["curve"]["milliseconds"].get<unsigned int> ( 15 ) )
{
  USUL_TRACE_SCOPE;

  // Add ourself as a listener. Do not use smart pointer on "this" 
  // inside constructor, the dereference will delete this object!
  Usul::Documents::Manager::instance().addActiveViewListener ( this );
  Usul::Documents::Manager::instance().addActiveDocumentListener ( this );

  // Start with a new path.
  this->_newPath();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

PathAnimationComponent::~PathAnimationComponent()
{
  USUL_TRACE_SCOPE;

  // Remove ourself as a listener. Do not use smart pointer!
  Usul::Documents::Manager::instance().removeActiveViewListener ( this );
  Usul::Documents::Manager::instance().removeActiveDocumentListener ( this );

  // Remove the timer.
  this->_timerStop();
  
  // Remove scene.
  _root = 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *PathAnimationComponent::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::IPlugin::IID:
    return static_cast < Usul::Interfaces::IPlugin* > ( this );
  case Usul::Interfaces::IMenuAdd::IID:
    return static_cast < Usul::Interfaces::IMenuAdd * > ( this );
  case Usul::Interfaces::IActiveViewListener::IID:
    return static_cast < Usul::Interfaces::IActiveViewListener * > ( this );
  case Usul::Interfaces::IAnimatePath::IID:
    return static_cast < Usul::Interfaces::IAnimatePath * > ( this );
  case Usul::Interfaces::IUpdateListener::IID:
    return static_cast < Usul::Interfaces::IUpdateListener * > ( this );
  case Usul::Interfaces::IAnimateNurbsCurve::IID:
    return static_cast < Usul::Interfaces::IAnimateNurbsCurve * > ( this );
  case Usul::Interfaces::IActiveDocumentListener::IID:
    return static_cast < Usul::Interfaces::IActiveDocumentListener * > ( this );
  case Usul::Interfaces::ITimerNotify::IID:
    return static_cast < Usul::Interfaces::ITimerNotify * > ( this );
  default:
    return 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add to the menu.
//
///////////////////////////////////////////////////////////////////////////////

void PathAnimationComponent::menuAdd ( MenuKit::Menu& m, Usul::Interfaces::IUnknown * c )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  namespace UC = Usul::Commands;
  namespace UA = Usul::Adaptors;

  Usul::Interfaces::IUnknown::QueryPtr caller ( c );

  typedef MenuKit::Button Button;
  typedef MenuKit::ToggleButton ToggleButton;
  typedef MenuKit::RadioButton RadioButton;

  // Return if there is no active view.
  Usul::Documents::Manager::View::RefPtr view ( Usul::Documents::Manager::instance().activeView() );
  if ( false == view.valid() )
    return;

  // Build the menu.
  MenuKit::Menu::RefPtr menu ( m.find ( "&Cameras", true ) );
  menu->append ( new Button ( UC::genericCommand ( "New Path", UA::memberFunction<void> ( this, &PathAnimationComponent::_newPath ), UC::TrueFunctor() ) ) );

  // Only add the open button if the interface is implemented.
  if ( this->_canOpenPath( caller ) )
    menu->append ( new Button ( UC::genericCommand ( "Open Path...", UA::bind1<void> ( caller, UA::memberFunction<void> ( this, &PathAnimationComponent::_openPath ) ), UC::TrueFunctor() ) ) );

  // Only add the save buttons if hte interface is implemented.
  if ( this->_canSavePath ( caller ) )
  {
    menu->append ( new Button ( UC::genericCommand ( "Save Path",       UA::bind1<void> ( caller, UA::memberFunction<void> ( this, &PathAnimationComponent::_saveCurrentPath ) ), UA::memberFunction<bool> ( this, &PathAnimationComponent::_isCurrentPathModified ) ) ) );
    menu->append ( new Button ( UC::genericCommand ( "Save Path As...", UA::bind1<void> ( caller, UA::memberFunction<void> ( this, &PathAnimationComponent::_saveAsCurrentPath ) ), UA::memberFunction<bool> ( this, &PathAnimationComponent::_hasCurrentPath ) ) ) );
    
    // Look to see if we have any plugins to export movie.
    typedef Usul::Components::Manager PluginManager;
    typedef PluginManager::UnknownSet Unknowns;
    Unknowns unknowns ( PluginManager::instance().getInterfaces ( Usul::Interfaces::IWriteMovieFile::IID ) );
    
    // Only add button if we have plugins to create movie file.
    if ( false == unknowns.empty() )
      menu->append ( new Button ( UC::genericCommand ( "Export Movie...", UA::bind1<void> ( caller, UA::memberFunction<void> ( this, &PathAnimationComponent::_exportMovie ) ), UA::memberFunction<bool> ( this, &PathAnimationComponent::_hasCurrentPath ) ) ) );
  }
  
  menu->addSeparator();
  menu->append ( new Button ( UC::genericCommand ( "&Append Camera",          UA::memberFunction<void> ( this, &PathAnimationComponent::_currentCameraAppend  ), UA::memberFunction<bool> ( this, &PathAnimationComponent::_hasCurrentPath ) ) ) );
  menu->append ( new Button ( UC::genericCommand ( "&Prepend Camera",         UA::memberFunction<void> ( this, &PathAnimationComponent::_currentCameraPrepend ), UA::memberFunction<bool> ( this, &PathAnimationComponent::_hasCurrentPath ) ) ) );
  menu->append ( new Button ( UC::genericCommand ( "&Insert Between Closest", UA::memberFunction<void> ( this, &PathAnimationComponent::_currentCameraInsert  ), UA::memberFunction<bool> ( this, &PathAnimationComponent::_hasCurrentPath ) ) ) );
  menu->append ( new Button ( UC::genericCommand ( "Remove Closest",          UA::memberFunction<void> ( this, &PathAnimationComponent::_currentCameraRemove  ), UA::memberFunction<bool> ( this, &PathAnimationComponent::_hasCurrentPath ) ) ) );
  menu->append ( new Button ( UC::genericCommand ( "Replace Closest",         UA::memberFunction<void> ( this, &PathAnimationComponent::_currentCameraReplace ), UA::memberFunction<bool> ( this, &PathAnimationComponent::_hasCurrentPath ) ) ) );
  menu->append ( new Button ( UC::genericCommand ( "&Close Path",             UA::memberFunction<void> ( this, &PathAnimationComponent::_closeCameraPath      ), UA::memberFunction<bool> ( this, &PathAnimationComponent::_canClosePath   ) ) ) );

  menu->addSeparator();
  menu->append ( new Button ( UC::genericCommand ( "Play &Forward",    UA::memberFunction<void> ( this, &PathAnimationComponent::_playForward    ), UA::memberFunction<bool> ( this, &PathAnimationComponent::_canPlay   ) ) ) );
  menu->append ( new Button ( UC::genericCommand ( "Play &Backward",   UA::memberFunction<void> ( this, &PathAnimationComponent::_playBackward   ), UA::memberFunction<bool> ( this, &PathAnimationComponent::_canPlay   ) ) ) );
  menu->append ( new Button ( UC::genericCommand ( "&Next Camera",     UA::memberFunction<void> ( this, &PathAnimationComponent::_goToNextCamera ), UA::memberFunction<bool> ( this, &PathAnimationComponent::_canPlay   ) ) ) );
  menu->append ( new Button ( UC::genericCommand ( "Pre&vious Camera", UA::memberFunction<void> ( this, &PathAnimationComponent::_goToPrevCamera ), UA::memberFunction<bool> ( this, &PathAnimationComponent::_canPlay   ) ) ) );
  menu->append ( new Button ( UC::genericCommand ( "&Stop",            UA::memberFunction<void> ( this, &PathAnimationComponent::stopPlaying    ), UA::memberFunction<bool> ( this, &PathAnimationComponent::isPlaying ) ) ) );

  menu->addSeparator();
  
  menu->append ( _cameraMenu.get() );
  menu->append ( _pathsMenu.get() );

  menu->addSeparator();
  MenuKit::Menu::RefPtr sub ( new MenuKit::Menu ( "&Degree" ) );
  sub->append ( new RadioButton ( UC::genericCheckCommand ( "&1", UA::bind1<void> ( 1, UA::memberFunction<void> ( this, &PathAnimationComponent::_setDegree ) ), UA::bind1<bool> ( 1, UA::memberFunction<bool> ( this, &PathAnimationComponent::_isDegree ) ) ) ) );
  sub->append ( new RadioButton ( UC::genericCheckCommand ( "&2", UA::bind1<void> ( 2, UA::memberFunction<void> ( this, &PathAnimationComponent::_setDegree ) ), UA::bind1<bool> ( 2, UA::memberFunction<bool> ( this, &PathAnimationComponent::_isDegree ) ) ) ) );
  sub->append ( new RadioButton ( UC::genericCheckCommand ( "&3", UA::bind1<void> ( 3, UA::memberFunction<void> ( this, &PathAnimationComponent::_setDegree ) ), UA::bind1<bool> ( 3, UA::memberFunction<bool> ( this, &PathAnimationComponent::_isDegree ) ) ) ) );
  sub->append ( new RadioButton ( UC::genericCheckCommand ( "&4", UA::bind1<void> ( 4, UA::memberFunction<void> ( this, &PathAnimationComponent::_setDegree ) ), UA::bind1<bool> ( 4, UA::memberFunction<bool> ( this, &PathAnimationComponent::_isDegree ) ) ) ) );
  menu->append ( sub.get() );

  typedef std::vector<unsigned int> NumSteps;
  NumSteps numStep;
  numStep.push_back (   10 ); numStep.push_back (   20 ); numStep.push_back (   30 ); numStep.push_back (   40 ); numStep.push_back (   50 );
  numStep.push_back (  100 ); numStep.push_back (  200 ); numStep.push_back (  300 ); numStep.push_back (  400 ); numStep.push_back (  500 );
  numStep.push_back ( 1000 ); numStep.push_back ( 2000 ); numStep.push_back ( 3000 ); numStep.push_back ( 4000 ); numStep.push_back ( 5000 );

  MenuKit::Menu::RefPtr steps ( new MenuKit::Menu ( "Steps" ) );
  for ( NumSteps::const_iterator iter = numStep.begin(); iter != numStep.end(); ++iter )
  {
    const unsigned int num ( *iter );
    steps->append ( new RadioButton ( UC::genericCheckCommand ( Usul::Strings::format ( num ), UA::bind1<void> ( num, UA::memberFunction<void> ( this, &PathAnimationComponent::_setNumSteps ) ), UA::bind1<bool> ( num, UA::memberFunction<bool> ( this, &PathAnimationComponent::_isNumSteps ) ) ) ) );
  }
  menu->append ( steps );

  menu->addSeparator();
  menu->append ( new ToggleButton ( UC::genericToggleCommand ( "&Loop",      UA::memberFunction<void> ( this, &PathAnimationComponent::_setLooping  ), UA::memberFunction<bool> ( this, &PathAnimationComponent::_isLooping     ) ) ) );
  menu->append ( new ToggleButton ( UC::genericToggleCommand ( "S&how Path", UA::memberFunction<void> ( this, &PathAnimationComponent::_setShowPath ), UA::memberFunction<bool> ( this, &PathAnimationComponent::_isShowingPath ) ) ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create new path.
//
///////////////////////////////////////////////////////////////////////////////

void PathAnimationComponent::_newPath()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  // If there is a current path and it's empty then do nothing.
  if ( ( true == _currentPath.valid() ) && ( 0 == _currentPath->size() ) )
    return;

  // Make new path with default name.
  CameraPath::RefPtr path ( new CameraPath );
  path->fileName ( Usul::Strings::format ( "Path", _paths.size() ) );
  path->name ( Usul::Strings::format ( "Path ", _paths.size() ) );

  // Append new path.
  _paths.push_back ( path );

  // Make the new path current.
  this->_setCurrentPath ( path.get() );

  // Rebuild the menu.
  this->_buildPathsMenu();

  // The scene is dirty.
  _dirtyScene = true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the menu for the current paths.
//
///////////////////////////////////////////////////////////////////////////////

void PathAnimationComponent::_buildPathsMenu()
{
  typedef void (PathAnimationComponent::*SetPathFunctionPtr ) ( CameraPath::RefPtr );
  typedef bool (PathAnimationComponent::*CheckFunction) ( CameraPath::RefPtr ) const;
  typedef Usul::Adaptors::MemberFunction < void, PathAnimationComponent*, SetPathFunctionPtr > PathFunction;
  typedef Usul::Adaptors::MemberFunction < bool, PathAnimationComponent*, CheckFunction > CheckFunctor;

  typedef Usul::Adaptors::Bind1 < void, CameraPath::RefPtr, PathFunction > SetPathFunctor;
  typedef Usul::Adaptors::Bind1 < bool, CameraPath::RefPtr, CheckFunctor > CheckPathFunctor;
  typedef Usul::Commands::TrueFunctor TrueFunctor;
  typedef Usul::Commands::ExecutePolicy < SetPathFunctor, CheckPathFunctor > Policy;

  typedef Usul::Commands::GenericCheckCommand < SetPathFunctor, CheckPathFunctor, TrueFunctor, Policy > CheckCommand;
  typedef MenuKit::RadioButton RadioButton;

  // Clear current menu.
  _pathsMenu->clear();

  // Add menu buttons for each path.
  for ( Paths::iterator i = _paths.begin(); i != _paths.end(); ++i )
  {
    CameraPath::RefPtr path ( *i );
    if ( true == path.valid() )
    {
      _pathsMenu->append ( new RadioButton ( new CheckCommand ( path->fileName(),  
        SetPathFunctor   ( *i, PathFunction ( this, &PathAnimationComponent::_setCurrentPath ) ), 
        CheckPathFunctor ( *i, CheckFunctor ( this, &PathAnimationComponent::_isCurrentPath ) ) ) ) );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to call the camera's function.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  template < class F > void callCameraFunction ( F f, const osg::Matrixd &m )
  {
    USUL_TRACE_SCOPE_STATIC;

    try
    {
      // Get the lookat data.
      osg::Vec3d e ( 0, 0, 0 );
      osg::Vec3d c ( 0, 0, 0 );
      osg::Vec3d u ( 0, 0, 0 );
      typedef CameraPath::Vec3d Vec3;
      m.getLookAt ( e, c, u );

      // Make sure up-vector is normalized.
      u.normalize();

      // Call the function.
      f ( Vec3 ( e[0], e[1], e[2] ), Vec3 ( c[0], c[1], c[2] ), Vec3 ( u[0], u[1], u[2] ) );
    }
    USUL_DEFINE_SAFE_CALL_CATCH_BLOCKS ( "3485778525" );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to call the camera's function.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  template < class F > void callCameraFunction ( F f )
  {
    USUL_TRACE_SCOPE_STATIC;

    // Get needed interface.
    Usul::Interfaces::IViewMatrix::QueryPtr vm ( Usul::Documents::Manager::instance().activeView() );
    if ( false == vm.valid() )
      return;

    // Call the other one.
    Helper::callCameraFunction ( f, vm->getViewMatrix() );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Append current camera.
//
///////////////////////////////////////////////////////////////////////////////

void PathAnimationComponent::_currentCameraAppend()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  // Handle no path.
  if ( false == _currentPath.valid() )
    return;

  // Use helper function.
  Helper::callCameraFunction ( Usul::Adaptors::memberFunction ( _currentPath.get(), &CameraPath::append ) );

  // Build the camera menu.
  this->_buildCameraMenu();

  // The scene is dirty.
  _dirtyScene = true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Prepend current camera.
//
///////////////////////////////////////////////////////////////////////////////

void PathAnimationComponent::_currentCameraPrepend()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  // Handle no path.
  if ( false == _currentPath.valid() )
    return;

  // Use helper function.
  Helper::callCameraFunction ( Usul::Adaptors::memberFunction ( _currentPath.get(), &CameraPath::prepend ) );

  // Build the camera menu.
  this->_buildCameraMenu();

  // The scene is dirty.
  _dirtyScene = true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Insert between two nearest cameras.
//
///////////////////////////////////////////////////////////////////////////////

void PathAnimationComponent::_currentCameraInsert()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  // Handle no path.
  if ( false == _currentPath.valid() )
    return;

  // Use helper function.
  Helper::callCameraFunction ( Usul::Adaptors::memberFunction ( _currentPath.get(), &CameraPath::insertBetweenNearest ) );

  // Build the camera menu.
  this->_buildCameraMenu();

  // The scene is dirty.
  _dirtyScene = true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove the nearest camera.
//
///////////////////////////////////////////////////////////////////////////////

void PathAnimationComponent::_currentCameraRemove()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  // Handle no path.
  if ( false == _currentPath.valid() )
    return;

  // Use helper function.
  Helper::callCameraFunction ( Usul::Adaptors::memberFunction ( _currentPath.get(), &CameraPath::removeNearest ) );

  // Build the camera menu.
  this->_buildCameraMenu();

  // The scene is dirty.
  _dirtyScene = true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Replace the nearest camera.
//
///////////////////////////////////////////////////////////////////////////////

void PathAnimationComponent::_currentCameraReplace()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  // Handle no path.
  if ( false == _currentPath.valid() )
    return;

  // Use helper function.
  Helper::callCameraFunction ( Usul::Adaptors::memberFunction ( _currentPath.get(), &CameraPath::replaceNearest ) );

  // Build the camera menu.
  this->_buildCameraMenu();

  // The scene is dirty.
  _dirtyScene = true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Close the path by appending the first point.
//
///////////////////////////////////////////////////////////////////////////////

void PathAnimationComponent::_closeCameraPath()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  // Handle no path.
  if ( false == _currentPath.valid() )
    return;

  // Close the path.
  _currentPath->closePath();

  // Build the camera menu.
  this->_buildCameraMenu();

  // The scene is dirty.
  _dirtyScene = true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the current paths.
//
///////////////////////////////////////////////////////////////////////////////

void PathAnimationComponent::_setCurrentPath ( CameraPath::RefPtr path )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  _currentPath = path;
  this->_buildCameraMenu();

  // The scene is dirty.
  _dirtyScene = true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  See if the given path is the current path.
//
///////////////////////////////////////////////////////////////////////////////

bool PathAnimationComponent::_isCurrentPath ( CameraPath::RefPtr path ) const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return ( _currentPath.get() == path.get() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is there a current path?
//
///////////////////////////////////////////////////////////////////////////////

bool PathAnimationComponent::_hasCurrentPath() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return ( true == _currentPath.valid() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Can the path be closed?
//
///////////////////////////////////////////////////////////////////////////////

bool PathAnimationComponent::_canClosePath() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return ( ( true == _currentPath.valid() ) ? _currentPath->canClose() : false );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Start the animation.
//
///////////////////////////////////////////////////////////////////////////////

void PathAnimationComponent::_playPathForward ( const CameraPath *path, unsigned int steps, bool loop, IUnknown::RefPtr caller )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  // Handle no path.
  if ( 0x0 == path )
    return;

  // Make a new player.
  CurvePlayer::RefPtr player ( new CurvePlayer );
  _players.push_back ( player );

  // Set properties.
  player->numStepsPerSpan ( steps );
  player->looping ( loop );

  // Play the animation forward.
  player->playForward ( path, _degree, caller );

  // Turn on timer.
  this->_timerStart();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Start the animation.
//
///////////////////////////////////////////////////////////////////////////////

void PathAnimationComponent::_playPathBackward ( const CameraPath *path, unsigned int steps, bool loop, IUnknown::RefPtr caller )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  // Handle no path.
  if ( 0x0 == path )
    return;

  // Make a new player.
  CurvePlayer::RefPtr player ( new CurvePlayer );
  _players.push_back ( player );

  // Set properties.
  player->numStepsPerSpan ( steps );
  player->looping ( loop );

  // Play the animation backward.
  player->playBackward ( path, _degree, caller );

  // Turn on timer.
  this->_timerStart();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Start the animation.
//
///////////////////////////////////////////////////////////////////////////////

void PathAnimationComponent::_playForward()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  this->_playPathForward ( _currentPath.get(), _numSteps, this->_isLooping(), Usul::Documents::Manager::instance().activeView() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Start the animation.
//
///////////////////////////////////////////////////////////////////////////////

void PathAnimationComponent::_playBackward()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  this->_playPathBackward ( _currentPath.get(), _numSteps, this->_isLooping(), Usul::Documents::Manager::instance().activeView() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Start the animation.
//
///////////////////////////////////////////////////////////////////////////////

void PathAnimationComponent::stopPlaying()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  // No more players.
  _players.clear();

  // Restore the render-loop.
  this->_timerStop();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Go to the next camera.
//
///////////////////////////////////////////////////////////////////////////////

void PathAnimationComponent::_goToNextCamera()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  // Are there any values?
  if ( ( false == _currentPath.valid() ) || ( 0 == _currentPath->size() ) )
    return;

  // Increment the current camera but keep it in range.
  ++_currentCamera;
  _currentCamera = ( ( _currentCamera >= _currentPath->size() ) ? 0 : _currentCamera );

  // Go to this position.
  this->_setCameraPosition ( _currentCamera );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Go to the previous camera.
//
///////////////////////////////////////////////////////////////////////////////

void PathAnimationComponent::_goToPrevCamera()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  // Are there any values?
  if ( ( false == _currentPath.valid() ) || ( 0 == _currentPath->size() ) )
    return;

  // Decrement the current camera but keep it in range.
  _currentCamera = ( ( _currentCamera > 0 ) ? _currentCamera - 1 : _currentPath->size() - 1 );

  // Go to this position.
  this->_setCameraPosition ( _currentCamera );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Can we play the animation?
//
///////////////////////////////////////////////////////////////////////////////

bool PathAnimationComponent::_canPlay() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  if ( false == _currentPath.valid() )
    return false;

  if ( false == _currentPath->canPlay() )
    return false;

  if ( true == this->isPlaying() )
    return false;

  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is there a path being played?
//
///////////////////////////////////////////////////////////////////////////////

bool PathAnimationComponent::isPlaying() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return ( false == _players.empty() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the active view changes.
//
///////////////////////////////////////////////////////////////////////////////

void PathAnimationComponent::activeViewChanged ( Usul::Interfaces::IUnknown *oldView, Usul::Interfaces::IUnknown *newView )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  {
    // Remove us from the old view.
    Usul::Interfaces::IUpdateSubject::QueryPtr subject ( oldView );
    if ( true == subject.valid() )
    {
      subject->removeUpdateListener ( this->queryInterface ( Usul::Interfaces::IUpdateListener::IID ) );
    }

    // Remove our root node.
    Usul::Interfaces::IGroup::QueryPtr group ( oldView );
    if ( true == group.valid() )
    {
      group->removeGroup ( this->getPluginName() );
      _root = 0x0;
    }
  }

  {
    // Add us to the new view.
    Usul::Interfaces::IUpdateSubject::QueryPtr subject ( newView );
    if ( true == subject.valid() )
    {
      subject->addUpdateListener ( this->queryInterface ( Usul::Interfaces::IUpdateListener::IID ) );
    }

    // Add our root node.
    Usul::Interfaces::IGroup::QueryPtr group ( newView );
    if ( true == group.valid() )
    {
      _root = group->getGroup ( this->getPluginName() );
    }
  }

  // The scene is dirty.
  _dirtyScene = true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when it's time to update.
//
///////////////////////////////////////////////////////////////////////////////

void PathAnimationComponent::updateNotify ( IUnknown *caller )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  // Always update the scene.
  Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( this, &PathAnimationComponent::_updateScene ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when it's time to update the current path.
//
///////////////////////////////////////////////////////////////////////////////

void PathAnimationComponent::_updatePath ( IUnknown *caller )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  // This is causing problems with seek.  Disable until working properly.
#if 0
  
  // Query the active document to see if we can proceed.
  Usul::Interfaces::IBusyState::QueryPtr busyState ( Usul::Documents::Manager::instance().activeDocument() );
  const bool busy ( busyState.valid() ? busyState->busyStateGet() : false );

  // Return now if the document is busy.
  if ( true == busy )
    return;
  
#endif
  
  // Get the current player.
  CurvePlayer::RefPtr player ( ( false == _players.empty() ) ? _players.front() : CurvePlayer::RefPtr ( 0x0 ) );
  
  // Should we update?
  if ( ( false == player.valid() ) || ( false == this->isPlaying() ) )
    return;
  
  // Update the player.
  player->update ( caller );
  
  // Did we reach the end?
  if ( false == player->playing() )
  {
    // Pop this player off the list.
    _players.pop_front();
    
    // Restore the render-loop state.
    this->_timerStop();
    
    // If we are suppose to write a movie.
    this->_writeMovieFile ( caller );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write a movie if we're supposed to.
//
///////////////////////////////////////////////////////////////////////////////

void PathAnimationComponent::_writeMovieFile ( IUnknown::RefPtr caller )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  // Check state.
  if ( false == _writeMovie )
    return;

  // Look for the frame dump interface.
  Usul::Interfaces::IFrameDump::QueryPtr fd ( caller );

  if ( fd.valid() )
  {
    // Get the filenames for the movie.
    Usul::Interfaces::IFrameDump::Filenames filenames ( fd->filenames() );

    // Write move interface.
    Usul::Interfaces::IWriteMovieFile::QueryPtr wm ( _movieWriter );
    if ( wm.valid() && false == _movieFilename.empty() )
      wm->writeMovie ( _movieFilename, filenames, _caller );
    
    // Turn off frame dumping.
    fd->setFrameDumpState ( Usul::Interfaces::IFrameDump::NEVER_DUMP );
    fd->saveNames ( false );
  }

  // We wrote the move, clear all data members.
  _writeMovie = false;
  _caller = static_cast < IUnknown * > ( 0x0 );
  _movieWriter = static_cast < IUnknown * > ( 0x0 );
  _movieFilename.clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Animate through the given path.
//
///////////////////////////////////////////////////////////////////////////////

void PathAnimationComponent::animatePath ( const IAnimatePath::PackedMatrices &matrices, IUnknown::RefPtr caller )
{
  USUL_TRACE_SCOPE;

  // Redirect with our current number of steps.
  this->animatePath ( matrices, Usul::Threads::Safe::get ( this->mutex(), _numSteps ), caller );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Animate through the given path.
//
///////////////////////////////////////////////////////////////////////////////

void PathAnimationComponent::animatePath ( const IAnimatePath::PackedMatrices &matrices, unsigned int steps, IUnknown::RefPtr caller )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  typedef IAnimatePath::PackedMatrices Matrices;
  typedef IAnimatePath::PackedMatrix Matrix;

  // Make sure we are not playing.
  this->stopPlaying();

  // Make a new path.
  CameraPath::RefPtr path ( new CameraPath );

  // Loop through all the matrices.
  typedef Matrices::const_iterator Itr;
  for ( Itr i = matrices.begin(); i != matrices.end(); ++i )
  {
    // Get the matrix.
    const Matrix &value ( *i );
    if ( 16 == value.size() )
    {
      // Get the matrix (camera).
      const osg::Matrixd m ( &value[0] );

      // Append the camera.
      Helper::callCameraFunction ( Usul::Adaptors::memberFunction ( path.get(), &CameraPath::append ), m );
    }
  }

  // Play this path forward.
  this->_playPathForward ( path.get(), steps, false, caller );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Animate through the given curve.
//
///////////////////////////////////////////////////////////////////////////////

void PathAnimationComponent::animateNurbsCurve ( const ControlPoints &ctrPts, const KnotVector &knots, unsigned int degree, unsigned int steps )
{
  USUL_TRACE_SCOPE;
  
  // Make the parameters as even steps.
  Parameters params ( steps );
  GN::Algorithms::fill ( params, 0, 1 );

  // Animate the curve with the even parameters.
  this->animateNurbsCurve ( ctrPts, knots, degree, params );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Animate through the given curve.
//
///////////////////////////////////////////////////////////////////////////////

void PathAnimationComponent::animateNurbsCurve ( const ControlPoints &ctrPts, const KnotVector &knots, unsigned int degree, const Parameters &params )
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the curve degree.
//
///////////////////////////////////////////////////////////////////////////////

void PathAnimationComponent::_setDegree ( unsigned int degree )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  _degree = degree;
  Reg::instance()[Sections::PATH_ANIMATION]["curve"]["degree"] = _degree;

  // The scene is dirty.
  _dirtyScene = true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Check the curve degree.
//
///////////////////////////////////////////////////////////////////////////////

bool PathAnimationComponent::_isDegree ( unsigned int degree ) const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return ( _degree == degree );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Open a path.
//
///////////////////////////////////////////////////////////////////////////////

void PathAnimationComponent::_openPath ( Usul::Interfaces::IUnknown::QueryPtr caller )
{
  USUL_TRACE_SCOPE;

  // Query for the interface.
  Usul::Interfaces::ILoadFileDialog::QueryPtr dialog ( caller );
  if ( false == dialog.valid() )
    return;

  typedef Usul::Interfaces::ILoadFileDialog::FilesResult FilesResult;
  typedef Usul::Interfaces::ILoadFileDialog::FileNames Filenames;

  CameraPath::RefPtr path ( new CameraPath );

  // Get the filenames.
  FilesResult result ( dialog->getLoadFileNames ( "Load Camera Path", path->filtersOpen() ) );
  Filenames names ( result.first );

  // Loop through each file...
  for ( Filenames::const_iterator iter = names.begin(); iter != names.end(); ++iter )
  {
    // Load the path.
    this->_loadPath ( *iter, caller );
  }

  // Rebuild the path menu.
  this->_buildPathsMenu();
  
  // Rebuild the camera menu.
  this->_buildCameraMenu();

  // The scene is dirty.
  _dirtyScene = true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Open a path.
//
///////////////////////////////////////////////////////////////////////////////

void PathAnimationComponent::_loadPath ( const std::string& name, Usul::Interfaces::IUnknown::QueryPtr caller )
{
  USUL_TRACE_SCOPE;

  CameraPath::RefPtr path ( new CameraPath );
  if ( path->canOpen ( name ) )
  {
    // Open the file.
    path->open ( name, caller );
    path->fileName ( name );
    path->name ( path->fileName() );

    // Append new path.
    {
      Guard guard ( this );
      _paths.push_back ( path );
    }

    // Make the new path current.
    this->_setCurrentPath ( path.get() );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Save the current path.
//
///////////////////////////////////////////////////////////////////////////////

void PathAnimationComponent::_saveCurrentPath ( Usul::Interfaces::IUnknown::QueryPtr caller )
{
  USUL_TRACE_SCOPE;

  // Get the current path.
  CameraPath::RefPtr path ( Usul::Threads::Safe::get ( this->mutex(), _currentPath ) );

  // Return if we don't have a current path.
  if ( false == path.valid() )
    return;

  // If the filename is valid, save.  If not, prompt for filename.
  if ( path->fileValid() )
    path->save( caller );
  else
    this->_saveAsCurrentPath ( caller );

  // Get the filename of the path.
  const std::string name ( path->fileName() );

  // Get the current document.
  Usul::Interfaces::IDocument::RefPtr document ( Usul::Documents::Manager::instance().activeDocument() );

  // Get string key for document.
  std::string doc ( ( ( true == document.valid() ) ? document->registryTagName() : "" ) );
  if ( true == doc.empty() )
    return;

  // Save properties in registry.
  Usul::Registry::Node &reg ( Reg::instance()[Sections::PATH_ANIMATION][doc]["paths"] );

  typedef std::list<std::string> Strings;

  Strings strings;
  strings = reg.get<Strings> ( strings );
  strings.push_back ( name );
  strings.erase ( std::unique ( strings.begin(), strings.end() ), strings.end() );
  reg = strings;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Save the current path.
//
///////////////////////////////////////////////////////////////////////////////

void PathAnimationComponent::_saveAsCurrentPath ( Usul::Interfaces::IUnknown::QueryPtr caller )
{
  USUL_TRACE_SCOPE;

  // Query for the needed interface.
  Usul::Interfaces::ISaveFileDialog::QueryPtr dialog ( caller );
  if ( false == dialog.valid() )
    return;

  // Get the current path.
  CameraPath::RefPtr path ( 0x0 );
  {
    Guard guard ( this );
    path = _currentPath;
  }

  // Return if we don't have a current path.
  if ( false == path.valid() )
    return;

  typedef Usul::Interfaces::ISaveFileDialog::FileResult FileResult;

  // Get the filename.
  FileResult result ( dialog->getSaveFileName ( "Save Camera Path", path->filtersOpen() ) );
  std::string filename ( result.first );

  // Save the camera file.
  if ( false == filename.empty() )
  {
    path->saveAs ( filename, caller ); 

    // Rebuild the menu.
    this->_buildPathsMenu();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is the current path modified?
//
///////////////////////////////////////////////////////////////////////////////

bool PathAnimationComponent::_isCurrentPathModified() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return ( _currentPath.valid() ? _currentPath->modified() : false );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Export the movie.
//
///////////////////////////////////////////////////////////////////////////////

void PathAnimationComponent::_exportMovie ( Usul::Interfaces::IUnknown::QueryPtr caller )
{
  USUL_TRACE_SCOPE;

  // Query for needed interfaces.
  Usul::Interfaces::ISaveFileDialog::QueryPtr dialog ( caller );
  Usul::Interfaces::IFrameDump::QueryPtr fd ( Usul::Documents::Manager::instance().activeView() );

  // Return if we don't have a save dialog or frame dumping.
  if ( false == dialog.valid() || false == fd.valid () )
    return;

  // Useful typedefs.
  typedef Usul::Interfaces::ISaveFileDialog::Filters Filters;
  typedef Usul::Interfaces::ISaveFileDialog::FileResult FileResult;
  typedef Usul::Components::Manager PluginManager;
  typedef PluginManager::UnknownSet Unknowns;

  Unknowns unknowns ( PluginManager::instance().getInterfaces ( Usul::Interfaces::IWriteMovieFile::IID ) );

  // Return if we didn't find any plugins to create a movie.
  if ( unknowns.empty() )
    return;

  // Export filters.
  Filters filters;

  // Append all filters into one list.
  for ( Unknowns::const_iterator iter = unknowns.begin(); iter != unknowns.end(); ++iter )
  {
    Usul::Interfaces::IWriteMovieFile::QueryPtr wm ( (*iter).get() );
    Filters f ( wm->filtersWrite() );
    filters.insert ( filters.end(), f.begin(), f.end() );
  }

  // Get the filename.
  FileResult result ( dialog->getSaveFileName ( "Export Movie", filters ) );
  std::string filename ( result.first );

  // Save the movie.
  if ( false == filename.empty() )
  {
    Guard guard ( this );

    // Find the movie writer.
    for ( Unknowns::const_iterator iter = unknowns.begin(); iter != unknowns.end(); ++iter )
    {
      Usul::Interfaces::IWriteMovieFile::QueryPtr wm ( (*iter).get() );
      if ( wm->canWrite ( filename ) )
      {
        _movieWriter = (*iter).get();
        break;
      }
    }

    // Save the caller.
    _caller = caller;

    // We want to save a movie.
    _writeMovie = true;

    // Save the name.
    _movieFilename = filename;

    // Get the base name.
    std::string base ( Usul::File::base ( filename ) );

    // Get the directory.
    std::string directory ( Usul::File::Temp::directory() );

    // Set the frame dump properties.
    fd->frameDumpProperties ( directory, base, ".jpg" );
    fd->resetFrameDumpCounter();
    fd->saveNames ( true );
    fd->setFrameDumpState ( Usul::Interfaces::IFrameDump::ALWAYS_DUMP );

    // Play the movie.
    this->_playForward();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is there an interface to open a path..
//
///////////////////////////////////////////////////////////////////////////////

bool PathAnimationComponent::_canOpenPath ( Usul::Interfaces::IUnknown::RefPtr caller ) const
{
  Usul::Interfaces::ILoadFileDialog::QueryPtr fd ( caller );
  return fd.valid();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is there an interface to save a path..
//
///////////////////////////////////////////////////////////////////////////////

bool PathAnimationComponent::_canSavePath ( Usul::Interfaces::IUnknown::RefPtr caller ) const
{
  Usul::Interfaces::ISaveFileDialog::QueryPtr fd ( caller );
  return fd.valid();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the camera menu.
//
///////////////////////////////////////////////////////////////////////////////

void PathAnimationComponent::_buildCameraMenu()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  // Clear what we have.
  _cameraMenu->clear();

  if ( false == _currentPath.valid() )
    return;

  // Number of frames.
  const unsigned int numFrames ( _currentPath->size() );

  namespace UC = Usul::Commands;
  namespace UA = Usul::Adaptors;

  // Build the buttons.
  for ( unsigned int i = 0; i < numFrames; ++ i )
  {
    std::string label ( Usul::Convert::Type<unsigned int,std::string>::convert ( i + 1 ) );
    label.insert ( ( 1 == label.size() ) ? 0 : label.size() - 1, "&" );
    label = Usul::Strings::format ( "Camera ", label );
    _cameraMenu->append ( new MenuKit::Button ( UC::genericCommand ( label, UA::bind1<void> 
      ( i, UA::memberFunction ( this, &PathAnimationComponent::_setCameraPosition ) ), UC::TrueFunctor() ) ) );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the view to the given camera position.
//
///////////////////////////////////////////////////////////////////////////////

void PathAnimationComponent::_setCameraPosition ( unsigned int num )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  // Get needed interface.
  Usul::Interfaces::IViewMatrix::QueryPtr vm ( Usul::Documents::Manager::instance().activeView() );
  if ( false == vm.valid() )
    return;

  // Check state.
  if ( false == _currentPath.valid() )
    return;

  // Check input.
  if ( num >= _currentPath->size() )
    return;

  // Get current view matrix.
  const osg::Matrixd m1 ( vm->getViewMatrix() );

  // Get camera properties. 
  Usul::Math::Vec3d eye, center, up;
  _currentPath->camera ( eye, center, up, num );

  // Make sure the vectors are normalized.
  up.normalize();

  // Make the new matrix.
  osg::Matrixd m2 ( osg::Matrixd::identity() );
  m2.makeLookAt ( osg::Vec3d ( eye[0], eye[1], eye[2] ), osg::Vec3d ( center[0], center[1], center[2] ), osg::Vec3d ( up[0], up[1], up[2] ) );

  // Prepare path.
  PackedMatrices matrices;
  matrices.push_back ( IAnimatePath::PackedMatrix ( m1.ptr(), m1.ptr() + 16 ) );
  matrices.push_back ( IAnimatePath::PackedMatrix ( m2.ptr(), m2.ptr() + 16 ) );

  // Animate through the path.
  this->animatePath ( matrices, Usul::Documents::Manager::instance().activeView() );

  // Make this the new current camera.
  _currentCamera = num;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is the given number the current step size?
//
///////////////////////////////////////////////////////////////////////////////

bool PathAnimationComponent::_isNumSteps ( unsigned int steps ) const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return ( steps == _numSteps );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the step size.
//
///////////////////////////////////////////////////////////////////////////////

void PathAnimationComponent::_setNumSteps ( unsigned int steps )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  _numSteps = steps;
  Reg::instance()[Sections::PATH_ANIMATION]["curve"]["num_steps"] = _numSteps;

  // The scene is dirty.
  _dirtyScene = true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the looping flag.
//
///////////////////////////////////////////////////////////////////////////////

void PathAnimationComponent::_setLooping ( bool state )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  _looping = state;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Are we looping?
//
///////////////////////////////////////////////////////////////////////////////

bool PathAnimationComponent::_isLooping() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _looping;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the flag to show the path.
//
///////////////////////////////////////////////////////////////////////////////

void PathAnimationComponent::_setShowPath ( bool state )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  _showPath = state;

  // The scene is dirty.
  _dirtyScene = true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Are we showing the path?
//
///////////////////////////////////////////////////////////////////////////////

bool PathAnimationComponent::_isShowingPath() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _showPath;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Are we showing the path?
//
///////////////////////////////////////////////////////////////////////////////

void PathAnimationComponent::_updateScene()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  // Are we dirty?
  if ( false == _dirtyScene )
    return;

  // We are no longer dirty.
  _dirtyScene = false;

  // Check state.
  if ( false == _root.valid() )
    return;

  // Remove what we have.
  _root->removeChildren ( 0, _root->getNumChildren() );

  // Are we showing the path?
  if ( true == this->_isShowingPath() )
  {
    // Add the curve.
    _root->addChild ( this->_buildCurve() );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the curve.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node *PathAnimationComponent::_buildCurve() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  // Make a temporary player.
  CurvePlayer::RefPtr player ( new CurvePlayer );
  if ( false == player.valid() )
    return new osg::Group;

  // Ask the player to build a curve.
  return player->buildCurve ( _currentPath.get(), _degree, _numSteps, Usul::Documents::Manager::instance().activeView() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  The active document has changed.
//
///////////////////////////////////////////////////////////////////////////////

void PathAnimationComponent::activeDocumentChanged ( IUnknown *oldDoc, IUnknown *newDoc )
{
  USUL_TRACE_SCOPE;

  // Get the current document.
  Usul::Interfaces::IDocument::QueryPtr document ( newDoc );

  // Get string key for document.
  std::string doc ( ( ( true == document.valid() ) ? document->registryTagName() : "" ) );
  if ( true == doc.empty() )
    return;

  // Save the current path.
  CameraPath::RefPtr currentPath ( Usul::Threads::Safe::get ( this->mutex(), _currentPath ) );

  // Make sure current path is restored.
  Usul::Scope::Caller::RefPtr restorePath ( Usul::Scope::makeCaller ( Usul::Adaptors::bind1 ( currentPath, Usul::Adaptors::memberFunction ( this, &PathAnimationComponent::_setCurrentPath ) ) ) );

  // Save properties in registry.
  Usul::Registry::Node &reg ( Reg::instance()[Sections::PATH_ANIMATION][doc]["paths"] );

  typedef std::list<std::string> Strings;

  Strings strings;
  strings = reg.get<Strings> ( strings );
  strings.erase ( std::unique ( strings.begin(), strings.end() ), strings.end() );

  for ( Strings::const_iterator iter = strings.begin(); iter != strings.end(); ++iter )
  {
    // Get the path of the camera file.
    const std::string path ( *iter );
    
    // Make sure the file exists before opening.
    if ( true == Usul::Predicates::FileExists::test ( path ) )
      this->_loadPath ( *iter, static_cast<Usul::Interfaces::IUnknown*> ( 0x0 ) );
  }

  // Rebuild the path menu.
  this->_buildPathsMenu();
  
  // Rebuild the camera menu.
  this->_buildCameraMenu();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Turn on the render loop. Save the current state if there is only one player.
//
///////////////////////////////////////////////////////////////////////////////

void PathAnimationComponent::_timerStart()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  
  this->_timerStop();
  
  // Handle repeated calls.
  if ( 0 != _timer )
    return;
  
  // Get the timer-server interface.
  Usul::Interfaces::ITimerService::QueryPtr ts ( Usul::Components::Manager::instance().getInterface ( Usul::Interfaces::ITimerService::IID ) );
  if ( false == ts.valid() )
    return;
  
  // Add the timer.
  _timer = ts->timerAdd ( _milliSeconds, Usul::Interfaces::IUnknown::QueryPtr ( this ), true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Restore the render loop.
//
///////////////////////////////////////////////////////////////////////////////

void PathAnimationComponent::_timerStop()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  
  // Handle repeated calls.
  if ( 0 == _timer )
    return;
  
  // Get the timer-server interface.
  Usul::Interfaces::ITimerService::QueryPtr ts ( Usul::Components::Manager::instance().getInterface ( Usul::Interfaces::ITimerService::IID ) );
  if ( false == ts.valid() )
    return;
  
  // Remove the timer.
  ts->timerRemove ( _timer );
  
  // This is important!
  _timer = 0;  
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the timer fires (ITimerNotify).
//
///////////////////////////////////////////////////////////////////////////////

void PathAnimationComponent::timerNotify ( TimerID )
{
  USUL_TRACE_SCOPE;
  this->_updatePath ( Usul::Documents::Manager::instance().activeView() );
  
  // Redraw.
  Usul::Interfaces::IRedraw::QueryPtr draw ( Usul::Documents::Manager::instance().activeDocument() );
  if ( true == draw.valid() )
  {
    draw->redraw();
  }
}
