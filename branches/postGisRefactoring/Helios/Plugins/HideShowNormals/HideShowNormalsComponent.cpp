
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


#include "HideShowNormalsComponent.h"

#include "Usul/App/Controller.h"
#include "Usul/Documents/Document.h"
#include "Usul/Interfaces/IActiveView.h"
#include "Usul/Interfaces/IOpenSceneGraph.h"
#include "Usul/Interfaces/IGroup.h"
#include "Usul/Interfaces/IDocument.h"
#include "Usul/Interfaces/ISendMessage.h"

#include "OsgTools/Utilities/FindNormals.h"

#include "osg/ref_ptr"
#include "osg/Node"

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( HideShowNormalsComponent , HideShowNormalsComponent::BaseClass );

///////////////////////////////////////////////////////////////////////////////
//
// Constructor
//
///////////////////////////////////////////////////////////////////////////////

HideShowNormalsComponent::HideShowNormalsComponent() 
{
}

///////////////////////////////////////////////////////////////////////////////
//
// Destructor
//
///////////////////////////////////////////////////////////////////////////////

HideShowNormalsComponent::~HideShowNormalsComponent() 
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *HideShowNormalsComponent::queryInterface ( unsigned long iid )
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
//  Usul::Interfaces::IPlugin implementation
//
///////////////////////////////////////////////////////////////////////////////

std::string HideShowNormalsComponent::getPluginName() const 
{
  return "Hide Show Normals";
}

	
///////////////////////////////////////////////////////////////////////////////
//
//  Show/Hide new primitives.
//
///////////////////////////////////////////////////////////////////////////////

void HideShowNormalsComponent::execute ( Usul::Interfaces::IUnknown *caller )
{
  // Get the interface we need.
  Usul::Interfaces::IActiveView::ValidQueryPtr activeView ( caller );
  Usul::Interfaces::IOpenSceneGraph::ValidQueryPtr osg ( activeView->getActiveView() );
  Usul::Interfaces::IDocument::QueryPtr document ( Usul::App::Controller::instance().activeDocument() );
  Usul::Interfaces::ISendMessage::QueryPtr message ( document );
  Usul::Interfaces::IGroup::QueryPtr group ( activeView->getActiveView() );

  // Get the scene.
  osg::ref_ptr<osg::Node> model ( dynamic_cast < osg::Node * > ( osg->osgReferenced() ) );

  // Handle bad input.
  if ( false == model.valid() )
    return;

  
  if ( group.valid () )
  {
    const std::string groupName ( "SHOW_NORMALS" );

    if ( group->hasGroup ( groupName ) )
      group->removeGroup ( groupName );
    else
    {
      osg::ref_ptr< osg::Group > g ( group->getGroup ( groupName ) );

      osg::ref_ptr < OsgTools::Utilities::FindNormals > visitor ( new OsgTools::Utilities::FindNormals );

      osg::BoundingBox bb;
      bb.expandBy( model->getBound() );

      visitor->boundingBox ( bb );

      model->accept( *visitor );

      g->addChild ( visitor->normals() );
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

std::string HideShowNormalsComponent::menuText() const
{
  return "Hide/Show normals.";
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the hot-key text.
//
///////////////////////////////////////////////////////////////////////////////

std::string HideShowNormalsComponent::hotKeyText() const
{
  return "";
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the status-bar text.
//
///////////////////////////////////////////////////////////////////////////////

std::string HideShowNormalsComponent::statusBarText() const
{
  return "Hide/Show normals.";
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get Group for the menu.  "" means no group
//
///////////////////////////////////////////////////////////////////////////////

std::string HideShowNormalsComponent::menuGroup() const
{
  return "Primitives";
}

