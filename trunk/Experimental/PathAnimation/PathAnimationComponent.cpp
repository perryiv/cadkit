
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
#include "Usul/Interfaces/IFrameDump.h"
#include "Usul/Interfaces/GUI/ILoadFileDialog.h"
#include "Usul/Interfaces/GUI/ISaveFileDialog.h"
#include "Usul/Interfaces/IGroup.h"
#include "Usul/Interfaces/IUpdateSubject.h"
#include "Usul/Interfaces/IViewMatrix.h"
#include "Usul/Interfaces/IWriteMovieFile.h"
#include "Usul/Registry/Database.h"
#include "Usul/Registry/Constants.h"
#include "Usul/Strings/Format.h"
#include "Usul/Trace/Trace.h"
#include "Usul/Threads/Safe.h"

#include "MenuKit/Menu.h"
#include "MenuKit/Button.h"
#include "MenuKit/RadioButton.h"
#include "MenuKit/ToggleButton.h"

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
  _player ( 0x0 ),
  _paused ( false ),
  _degree ( Reg::instance()[Sections::PATH_ANIMATION]["curve"]["degree"].get<unsigned int> ( 3 ) ),
  _writeMovie ( false ),
  _movieFilename(),
  _movieWriter(),
  _caller(),
  _stepSize ( Usul::Interfaces::AnimatePath::DEFAULT_STEP_SIZE ),
  _root ( 0x0 ),
  _showPath ( false ),
  _looping ( false )
{
  USUL_TRACE_SCOPE;

  // Add ourself as a listener. Do not use smart pointer on "this" 
  // inside constructor, the dereference will delete this object!
  Usul::Documents::Manager::instance().addActiveViewListener ( this );

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

  // Save properties in registry.
  Reg::instance()[Sections::PATH_ANIMATION]["curve"]["degree"] = _degree;

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
    menu->append ( new Button ( UC::genericCommand ( "Save Path...", UA::bind1<void> ( caller, UA::memberFunction<void> ( this, &PathAnimationComponent::_saveCurrentPath ) ), UA::memberFunction<bool> ( this, &PathAnimationComponent::_isCurrentPathModified ) ) ) );
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
  menu->append ( new Button ( UC::genericCommand ( "Play &Forward",  UA::memberFunction<void> ( this, &PathAnimationComponent::_playForward  ), UA::memberFunction<bool> ( this, &PathAnimationComponent::_canPlay   ) ) ) );
  menu->append ( new Button ( UC::genericCommand ( "Play &Backward", UA::memberFunction<void> ( this, &PathAnimationComponent::_playBackward ), UA::memberFunction<bool> ( this, &PathAnimationComponent::_canPlay   ) ) ) );
  menu->append ( new Button ( UC::genericCommand ( "&Stop",          UA::memberFunction<void> ( this, &PathAnimationComponent::_stopPlaying  ), UA::memberFunction<bool> ( this, &PathAnimationComponent::_isPlaying ) ) ) );

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

  typedef std::vector<double> Doubles;
  Doubles stepSizes;
  stepSizes.push_back ( 0.001 );
  stepSizes.push_back ( 0.005 );
  stepSizes.push_back ( 0.01 );
  stepSizes.push_back ( 0.05 );
  stepSizes.push_back ( 0.10 );
  stepSizes.push_back ( 0.20 );

  MenuKit::Menu::RefPtr steps ( new MenuKit::Menu ( "Step Size" ) );
  for ( Doubles::const_iterator iter = stepSizes.begin(); iter != stepSizes.end(); ++iter )
  {
    const double s ( *iter );
    steps->append ( new RadioButton ( UC::genericCheckCommand ( Usul::Strings::format ( s ), UA::bind1<void> ( s, UA::memberFunction<void> ( this, &PathAnimationComponent::_setStepSize ) ), UA::bind1<bool> ( s, UA::memberFunction<bool> ( this, &PathAnimationComponent::_isStepSize ) ) ) ) );
  }
  menu->append ( steps );

  menu->addSeparator();
  menu->append ( new ToggleButton ( UC::genericToggleCommand ( "&Loop", UA::memberFunction<void> ( this, &PathAnimationComponent::_setLooping ), UA::memberFunction<bool> ( this, &PathAnimationComponent::_isLooping ) ) ) );
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

void PathAnimationComponent::_playPathForward ( const CameraPath *path, double step, bool loop )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  // Handle no path.
  if ( 0x0 == path )
    return;

  // Make sure we're not paused.
  this->_pause ( false );

  // Make sure we have a player.
  if ( false == _player.valid() )
    _player = new CurvePlayer;

  // Set properties.
  _player->stepSize ( step );
  _player->looping ( loop );

  // Play the animation forward.
  _player->playForward ( path, _degree, Usul::Documents::Manager::instance().activeView() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Start the animation.
//
///////////////////////////////////////////////////////////////////////////////

void PathAnimationComponent::_playPathBackward ( const CameraPath *path, double step, bool loop )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  // Handle no path.
  if ( 0x0 == path )
    return;

  // Make sure we're not paused.
  this->_pause ( false );

  // Make sure we have a player.
  if ( false == _player.valid() )
    _player = new CurvePlayer;

  // Set properties.
  _player->stepSize ( step );
  _player->looping ( loop );

  // Play the animation forward.
  _player->playBackward ( path, _degree, Usul::Documents::Manager::instance().activeView() );
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
  this->_playPathForward ( _currentPath.get(), _stepSize, this->_isLooping() );
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
  this->_playPathBackward ( _currentPath.get(), _stepSize, this->_isLooping() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Start the animation.
//
///////////////////////////////////////////////////////////////////////////////

void PathAnimationComponent::_stopPlaying()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  // Stop the player.
  if ( true == _player.valid() )
  {
    _player->stopPlaying ( Usul::Documents::Manager::instance().activeView() );
  }

  // Make sure we're not paused.
  this->_pause ( false );

  // No more player.
  _player = 0x0;
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

  if ( true == this->_isPlaying() )
    return false;

  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is there a path being played?
//
///////////////////////////////////////////////////////////////////////////////

bool PathAnimationComponent::_isPlaying() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return ( ( true == _player.valid() ) && ( true == _player->playing() ) && ( false == this->_isPaused() ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Are we paused?
//
///////////////////////////////////////////////////////////////////////////////

bool PathAnimationComponent::_isPaused() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _paused;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the paused state.
//
///////////////////////////////////////////////////////////////////////////////

void PathAnimationComponent::_pause ( bool state )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  _paused = state;
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
    Usul::Interfaces::IGroup::QueryPtr group ( oldView );
    if ( true == group.valid() )
    {
      _root = group->getGroup ( this->getPluginName() );
    }
  }
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

  // Should we update?
  if ( ( false == _player.valid() ) || ( false == this->_isPlaying() ) )
    return;

  // Update the player.
  _player->update ( caller );

  // Did we reach the end?
  if ( false == _player->playing() )
  {
    // This sets "_player" to null.
    this->_stopPlaying();

    // If we are suppose to write a movie.
    this->_writeMovieFile ( caller );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write a movie if we're supposed to.
//
///////////////////////////////////////////////////////////////////////////////

void PathAnimationComponent::_writeMovieFile ( IUnknown *caller )
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
    fd->dumpFrames ( false );
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

void PathAnimationComponent::animatePath ( const IAnimatePath::PackedMatrices &matrices )
{
  USUL_TRACE_SCOPE;

  // Redirect with our current step size.
  this->animatePath ( matrices, Usul::Threads::Safe::get( this->mutex(), _stepSize ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Animate through the given path.
//
///////////////////////////////////////////////////////////////////////////////

void PathAnimationComponent::animatePath ( const IAnimatePath::PackedMatrices &matrices, double step )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  typedef IAnimatePath::PackedMatrices Matrices;
  typedef IAnimatePath::PackedMatrix Matrix;

  // Make sure we are not playing.
  this->_stopPlaying();

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
  this->_playPathForward ( path.get(), step, false );
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
    CameraPath::RefPtr path ( new CameraPath );
    if ( path->canOpen ( *iter ) )
    {
      // Open the file.
      path->open ( *iter, caller );
      path->fileName ( *iter );
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

  // Rebuild the path menu.
  this->_buildPathsMenu();
  
  // Rebuild the camera menu.
  this->_buildCameraMenu();
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
  CameraPath::RefPtr path ( 0x0 );
  {
    Guard guard ( this );
    path = _currentPath;
  }

  // Return if we don't have a current path.
  if ( false == path.valid() )
    return;

  // If the filename is valid, save.  If not, prompt for filename.
  if ( path->fileValid() )
    path->save( caller );
  else
    this->_saveAsCurrentPath ( caller );
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
    fd->dumpFrames ( true );

    // Play the movie.
    this->_playForward();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is there an interface to open a path..
//
///////////////////////////////////////////////////////////////////////////////

bool PathAnimationComponent::_canOpenPath ( Usul::Interfaces::IUnknown* caller ) const
{
  Usul::Interfaces::ILoadFileDialog::QueryPtr fd ( caller );
  return fd.valid();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is there an interface to save a path..
//
///////////////////////////////////////////////////////////////////////////////

bool PathAnimationComponent::_canSavePath ( Usul::Interfaces::IUnknown* caller ) const
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
  this->animatePath ( matrices );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is the given number the current step size?
//
///////////////////////////////////////////////////////////////////////////////

bool PathAnimationComponent::_isStepSize ( double step ) const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return step == _stepSize;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the step size.
//
///////////////////////////////////////////////////////////////////////////////

void PathAnimationComponent::_setStepSize ( double step )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  _stepSize = step;
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
