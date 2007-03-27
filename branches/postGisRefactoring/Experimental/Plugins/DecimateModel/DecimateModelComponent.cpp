
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

#include "DecimateModelComponent.h"

#include "Usul/Interfaces/IDecimateModel.h"
#include "Usul/Interfaces/ISendMessage.h"
#include "Usul/App/Controller.h"
#include "Usul/Documents/Document.h"

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( DecimateModelComponent , DecimateModelComponent::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
// Constructor
//
///////////////////////////////////////////////////////////////////////////////

DecimateModelComponent::DecimateModelComponent() 
{
}


///////////////////////////////////////////////////////////////////////////////
//
// Destructor
//
///////////////////////////////////////////////////////////////////////////////

DecimateModelComponent::~DecimateModelComponent() 
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *DecimateModelComponent::queryInterface ( unsigned long iid )
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

std::string DecimateModelComponent::getPluginName() const 
{
  return "Decimate Model";
}


///////////////////////////////////////////////////////////////////////////////
//
//  Decimate the model.
//
///////////////////////////////////////////////////////////////////////////////

void DecimateModelComponent::execute ( Usul::Interfaces::IUnknown *caller )
{
  Usul::Interfaces::IDocument::QueryPtr      document ( Usul::App::Controller::instance().activeDocument() );
  Usul::Interfaces::IDecimateModel::QueryPtr decimate ( document );
  Usul::Interfaces::ISendMessage::QueryPtr   message ( document );

  if( decimate.valid() )
  {
    decimate->decimateModel();

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

std::string DecimateModelComponent::menuText() const
{
  return "Decimate Model";
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the hot-key text.
//
///////////////////////////////////////////////////////////////////////////////

std::string DecimateModelComponent::hotKeyText() const
{
  return "";
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the status-bar text.
//
///////////////////////////////////////////////////////////////////////////////

std::string DecimateModelComponent::statusBarText() const
{
  return "Decimate the current model.";
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get Group for the menu.  "" means no group
//
///////////////////////////////////////////////////////////////////////////////

std::string DecimateModelComponent::menuGroup() const
{
  return "Primitives";
}
