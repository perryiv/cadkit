
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

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Commands/GenericCommand.h"
#include "Usul/Documents/Manager.h"
#include "Usul/Trace/Scope.h"
#include "Usul/Interfaces/IUpdateSubject.h"
#include "Usul/Interfaces/IViewMatrix.h"

#include "MenuKit/Menu.h"
#include "MenuKit/Button.h"
#include "MenuKit/ToggleButton.h"
#include "MenuKit/MemFunCallback.h"

using namespace Animate;

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( AnimateComponent , AnimateComponent::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

AnimateComponent::AnimateComponent() : BaseClass(),
_paths (),
_currentPath ( 0x0 )
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
  typedef Usul::Adaptors::MemberFunction < void, AnimateComponent*, VoidFunction > ExecuteFunctor;
  typedef MenuKit::MemFunCallbackReturn < AnimateComponent*, CheckFunction >       CheckFunctor;
  typedef Usul::Adaptors::MemberFunction < void, AnimateComponent*, BoolFunction > BoolFunctor;
  typedef Usul::Commands::GenericCommand < ExecuteFunctor >                        BasicCommand;
  typedef MenuKit::CheckCommand < BoolFunctor, CheckFunctor >                      CheckCommand;
  typedef MenuKit::Button                                                          Button;
  typedef MenuKit::ToggleButton                                                    ToggleButton;

  MenuKit::Menu::RefPtr menu ( m.findOrCreateMenu ( "Animate" ) );
  
  menu->append ( new Button ( new BasicCommand ( "Append", ExecuteFunctor ( this, &AnimateComponent::appendCamera ) ) ) );
  menu->append ( new Button ( new BasicCommand ( "Animate", ExecuteFunctor ( this, &AnimateComponent::startAnimation ) ) ) );

  menu->addSeparator();
  menu->append ( new ToggleButton ( new CheckCommand ( "Record",  BoolFunctor ( this, &AnimateComponent::recordPath ), CheckFunctor ( this, &AnimateComponent::recordPath ) ) ) );

  menu->addSeparator();
  menu->append ( new Button ( new BasicCommand ( "New Key Frame Path", ExecuteFunctor ( this, &AnimateComponent::createKeyFramePath ) ) ) );
  menu->append ( new Button ( new BasicCommand ( "New Recorded Path", ExecuteFunctor ( this, &AnimateComponent::createRecordedPath ) ) ) );

  menu->addSeparator();

  menu->append ( new Button ( new BasicCommand ( "Clear", ExecuteFunctor ( this, &AnimateComponent::clearAnimation ) ) ) );
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
//  Set the current path.
//
///////////////////////////////////////////////////////////////////////////////

void AnimateComponent::currentPath ( Path * path )
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
    path->start ( this->queryInterface ( Usul::Interfaces::IUnknown::IID ) );
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

  {
    Guard guard ( this->mutex () );
    _paths.push_back ( path.get () );
  }

  // Make the new path current.
  this->currentPath ( path.get() );

  // Rebuild the menu.
  //this->_initMenu ();
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

  {
    Guard guard ( this->mutex () );
    _paths.push_back ( path.get () );
  }

  // Make the new path current.
  this->currentPath ( path.get() );

  // Rebuild the menu.
  //this->_initMenu ();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set flag to record path.
//
///////////////////////////////////////////////////////////////////////////////

void AnimateComponent::recordPath ( bool b )
{
  USUL_TRACE_SCOPE;
  
  Path::RefPtr path ( this->currentPath () );
  if ( path.valid () )
    path->acceptNewFrames ( b );
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
