
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  
//
///////////////////////////////////////////////////////////////////////////////


#include "NewPrimitivesComponent.h"

#include "Usul/Interfaces/ISendMessage.h"
#include "Usul/Interfaces/IGroup.h"
#include "Usul/Interfaces/IShowNewPrimitives.h"

#include "Usul/App/Controller.h"
#include "Usul/Documents/Document.h"

#include "osg/ref_ptr"
#include "osg/Group"

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( NewPrimitivesComponent , NewPrimitivesComponent::BaseClass );

///////////////////////////////////////////////////////////////////////////////
//
// Constructor
//
///////////////////////////////////////////////////////////////////////////////

NewPrimitivesComponent::NewPrimitivesComponent() 
{
}

///////////////////////////////////////////////////////////////////////////////
//
// Destructor
//
///////////////////////////////////////////////////////////////////////////////

NewPrimitivesComponent::~NewPrimitivesComponent() 
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *NewPrimitivesComponent::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::ICommand::IID:
    return static_cast < Usul::Interfaces::ICommand*>(this);
  case Usul::Interfaces::IMenuEntry::IID:
    return static_cast < Usul::Interfaces::IMenuEntry*>(this);
  case Usul::Interfaces::IPlugin::IID:
    return static_cast < Usul::Interfaces::IPlugin* > ( this );
  default:
    return 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
// Usul/Interfaces/IPlugin.h implementation
//
///////////////////////////////////////////////////////////////////////////////

std::string NewPrimitivesComponent::getPluginName() const 
{
  return "New Primitives";
}

	
///////////////////////////////////////////////////////////////////////////////
//
//  Show/Hide new primitives.
//
///////////////////////////////////////////////////////////////////////////////

void NewPrimitivesComponent::execute ( Usul::Interfaces::IUnknown *caller )
{
  Usul::Interfaces::IDocument::QueryPtr document ( Usul::App::Controller::instance().activeDocument() );
  Usul::Interfaces::ISendMessage::QueryPtr message ( document );
  Usul::Interfaces::IGroup::QueryPtr group ( Usul::App::Controller::instance().activeView() );
  Usul::Interfaces::IShowNewPrimitives::QueryPtr show ( document );
  
  if ( group.valid () && show.valid() )
  {
    const std::string groupName ( "SHOW_NEW_PRIMITIVES" );

    if ( group->hasGroup ( groupName ) )
      group->removeGroup ( groupName );
    else
    {
      osg::ref_ptr< osg::Group > g ( group->getGroup ( groupName ) );
      g->addChild ( show->getNewPrimitives() );
    }

    if ( message.valid() )
    {
      message->sendMessage ( Usul::Interfaces::ISendMessage::ID_RENDER_SCENE );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the menu text.
//
///////////////////////////////////////////////////////////////////////////////

std::string NewPrimitivesComponent::menuText() const
{
  return "Hide/Show new primitives.";
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the hot-key text.
//
///////////////////////////////////////////////////////////////////////////////

std::string NewPrimitivesComponent::hotKeyText() const
{
  return "";
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the status-bar text.
//
///////////////////////////////////////////////////////////////////////////////

std::string NewPrimitivesComponent::statusBarText() const
{
  return "Hide/Show new primitives.";
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get Group for the menu.  "" means no group
//
///////////////////////////////////////////////////////////////////////////////

std::string NewPrimitivesComponent::menuGroup() const
{
  return "Primitives";
}
