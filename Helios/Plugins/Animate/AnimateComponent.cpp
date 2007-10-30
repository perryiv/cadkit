
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The component class.
//
///////////////////////////////////////////////////////////////////////////////

#include "Helios/Plugins/Animate/AnimateComponent.h"
#include "Helios/Plugins/Animate/KeyFramePath.h"
#include "Helios/Plugins/Animate/RecordedPath.h"

#include "Usul/Adaptors/Bind.h"
#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Commands/GenericCommand.h"
#include "Usul/Commands/GenericCheckCommand.h"
#include "Usul/Documents/Manager.h"
#include "Usul/Trace/Trace.h"
#include "Usul/Interfaces/IUpdateSubject.h"
#include "Usul/Interfaces/IViewMatrix.h"

#include "MenuKit/Menu.h"
#include "MenuKit/Button.h"
#include "MenuKit/RadioButton.h"
#include "MenuKit/ToggleButton.h"

using namespace Animate;

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( AnimateComponent, AnimateComponent::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

AnimateComponent::AnimateComponent() : BaseClass(),
_paths (),
_currentPath ( 0x0 ),
_pathsMenu ( new MenuKit::Menu ( "Current Paths" ) )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

AnimateComponent::~AnimateComponent()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *AnimateComponent::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::IPlugin::IID:
    return static_cast < Usul::Interfaces::IPlugin* > ( this );
  case Usul::Interfaces::IMenuAdd::IID:
    return static_cast < Usul::Interfaces::IMenuAdd * > ( this );
  case Usul::Interfaces::IAnimatePath::IID:
    return static_cast < Usul::Interfaces::IAnimatePath * > ( this );
  default:
    return 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add to the menu.
//
///////////////////////////////////////////////////////////////////////////////

void AnimateComponent::menuAdd ( MenuKit::Menu& m )
{
  typedef void (AnimateComponent::*VoidFunction) ();
  typedef void (AnimateComponent::*BoolFunction) ( bool );
  typedef bool (AnimateComponent::*CheckFunction) () const;
  typedef Usul::Adaptors::MemberFunction < void, AnimateComponent*, VoidFunction >  ExecuteFunctor;
  typedef Usul::Adaptors::MemberFunction < bool, AnimateComponent*, CheckFunction > CheckFunctor;
  typedef Usul::Adaptors::MemberFunction < void, AnimateComponent*, BoolFunction >  BoolFunctor;
  typedef Usul::Commands::GenericCommand < ExecuteFunctor >                         BasicCommand;
  typedef Usul::Commands::GenericCommand < ExecuteFunctor, CheckFunctor >           BasicEnableCommand;
  typedef Usul::Commands::GenericCheckCommand < BoolFunctor, CheckFunctor, CheckFunctor > CheckCommand;
  typedef MenuKit::Button                                                           Button;
  typedef MenuKit::ToggleButton                                                     ToggleButton;

  // Return if there is no active view.
  Usul::Documents::Manager::View::RefPtr view ( Usul::Documents::Manager::instance().activeView() );
  if ( false == view.valid() )
    return;

  MenuKit::Menu::RefPtr menu ( m.find ( "Animate", true ) );

  CheckFunctor validPath ( this, &AnimateComponent::ensureValidPath );
  
  menu->append ( new Button ( new BasicEnableCommand ( "Append", ExecuteFunctor ( this, &AnimateComponent::appendCamera ), CheckFunctor ( this, &AnimateComponent::ensureKeyFramePath ) ) ) );
  menu->append ( new Button ( new BasicEnableCommand ( "Animate", ExecuteFunctor ( this, &AnimateComponent::startAnimation ), validPath ) ) );

  menu->addSeparator();
  menu->append ( new ToggleButton ( new CheckCommand ( "Record",  
                                                       BoolFunctor ( this, &AnimateComponent::recordPath ), 
                                                       CheckFunctor ( this, &AnimateComponent::recordPath ),
                                                       CheckFunctor ( this, &AnimateComponent::ensureRecordedPath ) ) ) );

  menu->addSeparator();
  menu->append ( _pathsMenu );

  menu->addSeparator();
  menu->append ( new Button ( new BasicCommand ( "New Key Frame Path", ExecuteFunctor ( this, &AnimateComponent::createKeyFramePath ) ) ) );
  menu->append ( new Button ( new BasicCommand ( "New Recorded Path", ExecuteFunctor ( this, &AnimateComponent::createRecordedPath ) ) ) );

  menu->addSeparator();

  menu->append ( new Button ( new BasicEnableCommand ( "Clear", ExecuteFunctor ( this, &AnimateComponent::clearAnimation ), validPath ) ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the current path.
//
///////////////////////////////////////////////////////////////////////////////

Animate::Path* AnimateComponent::currentPath ()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _currentPath.get ();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the current path.
//
///////////////////////////////////////////////////////////////////////////////

const Animate::Path* AnimateComponent::currentPath () const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _currentPath.get ();
}


///////////////////////////////////////////////////////////////////////////////
//
// Usul::Interfaces::IAnimatePath
// Set an animation path from a vector of matrix keypoints
//
///////////////////////////////////////////////////////////////////////////////

void AnimateComponent::animatePath( const PackedMatrices & matrices )
{ 
  USUL_TRACE_SCOPE;
#if 0
  Guard guard ( this->mutex() );

  Animate::KeyFramePath::RefPtr path ( new Animate::KeyFramePath() );

  if( true == path.valid() )
  {
    path->acceptNewFrames( true );
    for( std::vector< osg::Matrixf >::iterator iter = matrix.begin(); iter != matrix.end(); ++iter )
    {
      path->append( new Animate::Frame( *iter ) );
    }

    this->setCurrentPath( path.get() );
    this->_buildMenu();

    path->start( Usul::Documents::Manager::instance().activeView() );
  }
#endif
}

///////////////////////////////////////////////////////////////////////////////
//
//  Set the current path.
//
///////////////////////////////////////////////////////////////////////////////

void AnimateComponent::setCurrentPath ( Path::RefPtr path )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  Usul::Interfaces::IUpdateSubject::QueryPtr us ( Usul::Documents::Manager::instance().activeView () );

  if ( us.valid () )
  {
    // Remove the current current path as an update listener.
    {
      Usul::Interfaces::IUpdateListener::QueryPtr ul ( _currentPath );
      us->removeUpdateListener ( ul );
    }

    // Add the new path as a update listener.
    {
      Usul::Interfaces::IUpdateListener::QueryPtr ul ( path );
      us->addUpdateListener ( ul );
    }
  }

  _currentPath = path;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is the given path current?
//
///////////////////////////////////////////////////////////////////////////////

bool AnimateComponent::isCurrentPath ( Path::RefPtr path ) const
{
  return path == this->currentPath();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Append current camera.
//
///////////////////////////////////////////////////////////////////////////////

void AnimateComponent::appendCamera ()
{
  USUL_TRACE_SCOPE;
  Path::RefPtr path ( this->currentPath () );
  if ( path.valid () )
  {
    Usul::Interfaces::IViewMatrix::QueryPtr vm ( Usul::Documents::Manager::instance().activeView () );

    if ( vm.valid () )
    {
      path->acceptNewFrames ( true );
      path->append ( new Animate::Frame ( vm->getViewMatrix () ) );
      path->acceptNewFrames ( false );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Start the animation.
//
///////////////////////////////////////////////////////////////////////////////

void AnimateComponent::startAnimation ()
{
  USUL_TRACE_SCOPE;

  Path::RefPtr path ( this->currentPath () );
  if ( path.valid () )
    path->start ( Usul::Documents::Manager::instance().activeView () );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear the animation.
//
///////////////////////////////////////////////////////////////////////////////

void AnimateComponent::clearAnimation ()
{
  USUL_TRACE_SCOPE;

  Path::RefPtr path ( this->currentPath () );
  if ( path.valid () )
    path->clear ();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create new key frame path.
//
///////////////////////////////////////////////////////////////////////////////

void AnimateComponent::createKeyFramePath ()
{
  USUL_TRACE_SCOPE;

  Path::RefPtr path ( new Animate::KeyFramePath );
  path->defaultFilename();

  {
    Guard guard ( this->mutex () );
    _paths.push_back ( path.get () );
  }

  // Make the new path current.
  this->setCurrentPath ( path.get() );

  // Rebuild the menu.
  this->_buildMenu ();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create new recored path.
//
///////////////////////////////////////////////////////////////////////////////

void AnimateComponent::createRecordedPath ()
{
  USUL_TRACE_SCOPE;

  Path::RefPtr path ( new Animate::RecordedPath );
  path->defaultFilename();

  {
    Guard guard ( this->mutex () );
    _paths.push_back ( path.get () );
  }

  // Make the new path current.
  this->setCurrentPath ( path.get() );

  // Rebuild the menu.
  this->_buildMenu ();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set flag to record path.
//
///////////////////////////////////////////////////////////////////////////////

void AnimateComponent::recordPath ( bool record )
{
  USUL_TRACE_SCOPE;
  
  Path::RefPtr path ( this->currentPath () );
  if ( path.valid () )
  {
    path->acceptNewFrames ( record );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get flag to record path.
//
///////////////////////////////////////////////////////////////////////////////

bool AnimateComponent::recordPath ( ) const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex () );
  if ( _currentPath.valid () )
    return _currentPath->acceptNewFrames ( );

  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the menu for the current paths.
//
///////////////////////////////////////////////////////////////////////////////

void AnimateComponent::_buildMenu ()
{
  typedef void (AnimateComponent::*SetPathFunctionPtr ) ( Path::RefPtr );
  typedef bool (AnimateComponent::*CheckFunction) ( Path::RefPtr ) const;
  typedef Usul::Adaptors::MemberFunction < void, AnimateComponent*, SetPathFunctionPtr > PathFunction;
  typedef Usul::Adaptors::MemberFunction < bool, AnimateComponent*, CheckFunction >      CheckFunctor;

  typedef Usul::Adaptors::Bind1 < void, Path::RefPtr, PathFunction >  SetPathFunctor;
  typedef Usul::Adaptors::Bind1 < bool, Path::RefPtr, CheckFunctor >  CheckPathFunctor;
  typedef Usul::Commands::TrueFunctor TrueFunctor;
  typedef Usul::Commands::ExecutePolicy < SetPathFunctor, CheckPathFunctor > Policy;

  typedef Usul::Commands::GenericCheckCommand < SetPathFunctor, CheckPathFunctor, TrueFunctor, Policy > CheckCommand;
  typedef MenuKit::RadioButton                                                     RadioButton;

  _pathsMenu->clear();

  for ( Paths::iterator iter = _paths.begin(); iter != _paths.end(); ++iter )
  {
    _pathsMenu->append ( new RadioButton ( new CheckCommand ( (*iter)->fileName() + " (" + (*iter)->typeName() + ")",  
                                                             SetPathFunctor ( *iter,  PathFunction ( this, &AnimateComponent::setCurrentPath ) ), 
                                                             CheckPathFunctor ( *iter, CheckFunctor ( this, &AnimateComponent::isCurrentPath ) ) ) ) );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Do we have a valid path?
//
///////////////////////////////////////////////////////////////////////////////

bool AnimateComponent::ensureValidPath () const
{
  return 0x0 != this->currentPath();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Do we have a valid recoreded path?
//
///////////////////////////////////////////////////////////////////////////////

bool AnimateComponent::ensureRecordedPath () const
{
  return 0x0 != dynamic_cast < const RecordedPath* > ( this->currentPath() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Do we have a valid recoreded path?
//
///////////////////////////////////////////////////////////////////////////////

bool AnimateComponent::ensureKeyFramePath () const
{
  return 0x0 != dynamic_cast < const KeyFramePath* > ( this->currentPath() );
}
