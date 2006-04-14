
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


#include "GroupPrimitivesComponent.h"

#include "Usul/Interfaces/IActiveView.h"
#include "Usul/Interfaces/IGetDocument.h"
#include "Usul/Interfaces/IDocument.h"
#include "Usul/Interfaces/ISendMessage.h"
#include "Usul/Interfaces/IGroupPrimitives.h"
#include "Usul/Interfaces/ISendMessage.h"

#include "Usul/State/Busy.h"

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( GroupPrimitivesComponent , GroupPrimitivesComponent::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
// Constructor
//
///////////////////////////////////////////////////////////////////////////////

GroupPrimitivesComponent::GroupPrimitivesComponent() 
{
}


///////////////////////////////////////////////////////////////////////////////
//
// Destructor
//
///////////////////////////////////////////////////////////////////////////////

GroupPrimitivesComponent::~GroupPrimitivesComponent() 
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *GroupPrimitivesComponent::queryInterface ( unsigned long iid )
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

std::string GroupPrimitivesComponent::getPluginName() const 
{
  return "GroupPrimitivesComponent";
}

	
///////////////////////////////////////////////////////////////////////////////
//
//  If the menu button was clicked
//
///////////////////////////////////////////////////////////////////////////////

void GroupPrimitivesComponent::execute ( Usul::Interfaces::IUnknown *caller )
{
  if( !Usul::State::busy() )
  {
    //Set the busy flag
    Usul::State::Busy busy;

    //Get needed interfaces
    Usul::Interfaces::IActiveView::ValidQueryPtr  activeView  ( caller );
    Usul::Interfaces::IGetDocument::ValidQueryPtr getDocument ( activeView->getActiveView() );
    Usul::Interfaces::IDocument::QueryPtr    document    ( getDocument->getDocument() );
    Usul::Interfaces::IGroupPrimitives::QueryPtr  groupPrimitives ( document );
    Usul::Interfaces::ISendMessage::QueryPtr sendMessage ( document );

    if( groupPrimitives.valid() )
    {
      //Use Document
      groupPrimitives->groupPrimitives( caller );

      if ( document.valid() )
        document->updateGUI();

      if( sendMessage.valid() )
        sendMessage->sendMessage( Usul::Interfaces::ISendMessage::ID_BUILD_SCENE );
    }
    else
    {
      //TODO, need to tell user what document to use
      throw std::runtime_error ("This document does not suppport grouping of primitives by connectivity.");
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the menu text.
//
///////////////////////////////////////////////////////////////////////////////

std::string GroupPrimitivesComponent::menuText() const
{
  return "Group Primitives";
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the hot-key text.
//
///////////////////////////////////////////////////////////////////////////////

std::string GroupPrimitivesComponent::hotKeyText() const
{
  return "";
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the status-bar text.
//
///////////////////////////////////////////////////////////////////////////////

std::string GroupPrimitivesComponent::statusBarText() const
{
  return "Group primitives of document based on connectivity.";
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get Group for the menu.  "" means no group
//
///////////////////////////////////////////////////////////////////////////////

std::string GroupPrimitivesComponent::menuGroup() const
{
  return "Primitives";
}


