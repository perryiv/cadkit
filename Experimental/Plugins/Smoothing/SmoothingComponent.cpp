
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Component for smoothing triangle models
//
///////////////////////////////////////////////////////////////////////////////


#include "SmoothingComponent.h"

#include "Usul/Interfaces/ISmoothModel.h"
#include "Usul/Interfaces/ISendMessage.h"
#include "Usul/App/Controller.h"
#include "Usul/Documents/Document.h"

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( SmoothingComponent , SmoothingComponent::BaseClass );

///////////////////////////////////////////////////////////////////////////////
//
// Constructor
//
///////////////////////////////////////////////////////////////////////////////

SmoothingComponent::SmoothingComponent() 
{
}

///////////////////////////////////////////////////////////////////////////////
//
// Destructor
//
///////////////////////////////////////////////////////////////////////////////

SmoothingComponent::~SmoothingComponent() 
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *SmoothingComponent::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::IPlugin::IID:
    return static_cast < Usul::Interfaces::IPlugin* > ( this );
  case Usul::Interfaces::ICommand::IID:
    return static_cast < Usul::Interfaces::ICommand* > ( this );
  case Usul::Interfaces::IMenuEntry::IID:
    return static_cast < Usul::Interfaces::IMenuEntry* > ( this );
  default:
    return 0x0;
  }
}



///////////////////////////////////////////////////////////////////////////////
//
// Usul/Interfaces/IPlugin.h implementation
//
///////////////////////////////////////////////////////////////////////////////

std::string SmoothingComponent::getPluginName() const 
{
  return "Smoothing";
}

	
///////////////////////////////////////////////////////////////////////////////
//
//  Smooth the model.
//
///////////////////////////////////////////////////////////////////////////////

void SmoothingComponent::execute ( Usul::Interfaces::IUnknown *caller )
{
  Usul::Interfaces::IDocument::QueryPtr document ( Usul::App::Controller::instance().activeDocument() );
  Usul::Interfaces::ISmoothModel::QueryPtr smooth ( document );
  Usul::Interfaces::ISendMessage::QueryPtr message ( document );

  if( smooth.valid() )
  {
    smooth->smoothModel();

    if ( message.valid() )
    {
      message->sendMessage ( Usul::Interfaces::ISendMessage::ID_BUILD_SCENE );
      message->sendMessage ( Usul::Interfaces::ISendMessage::ID_RENDER_SCENE );
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
//
//  Get the menu text.
//
///////////////////////////////////////////////////////////////////////////////

std::string SmoothingComponent::menuText() const
{
  return "Smooth Model";
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the hot-key text.
//
///////////////////////////////////////////////////////////////////////////////

std::string SmoothingComponent::hotKeyText() const
{
  return "";
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the status-bar text.
//
///////////////////////////////////////////////////////////////////////////////

std::string SmoothingComponent::statusBarText() const
{
  return "Smooth the current model.";
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get Group for the menu.  "" means no group
//
///////////////////////////////////////////////////////////////////////////////

std::string SmoothingComponent::menuGroup() const
{
  return "";
}