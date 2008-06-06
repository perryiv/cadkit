
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

#include "Usul/Documents/Manager.h"
#include "Usul/Adaptors/Bind.h"
#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Commands/GenericCheckCommand.h"
#include "Usul/Interfaces/IOpenSceneGraph.h"
#include "Usul/Interfaces/IGroup.h"

#include "MenuKit/Menu.h"
#include "MenuKit/ToggleButton.h"

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
  case Usul::Interfaces::IMenuAdd::IID:
    return static_cast < Usul::Interfaces::IMenuAdd*>(this);
  case Usul::Interfaces::IPlugin::IID:
    return static_cast < Usul::Interfaces::IPlugin * > ( this );
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

namespace Detail
{
  const std::string GROUP_NAME ( "SHOW_NORMALS" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Show/Hide normals.
//
///////////////////////////////////////////////////////////////////////////////

void HideShowNormalsComponent::showNormals ( bool b )
{
  Usul::Interfaces::IView::RefPtr view ( Usul::Documents::Manager::instance().activeView() );

  // Get the interface we need.
  Usul::Interfaces::IOpenSceneGraph::QueryPtr osg ( view );
  Usul::Interfaces::IGroup::QueryPtr group ( view );

  if ( false == group.valid () || false == osg.valid() )
    return;

  // Remove what we have
  group->removeGroup ( Detail::GROUP_NAME );

  if ( b )
  {
    // Get the scene.
    osg::ref_ptr<osg::Node> model ( dynamic_cast < osg::Node * > ( osg->osgReferenced() ) );

    // Handle bad input.
    if ( false == model.valid() )
      return;

    osg::ref_ptr< osg::Group > g ( group->getGroup ( Detail::GROUP_NAME ) );

    osg::ref_ptr < OsgTools::Utilities::FindNormals > visitor ( new OsgTools::Utilities::FindNormals );

    osg::BoundingBox bb;
    bb.expandBy( model->getBound() );

    visitor->boundingBox ( bb );

    model->accept( *visitor );

    g->addChild ( visitor->normals() );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Are normals shown?
//
///////////////////////////////////////////////////////////////////////////////

bool HideShowNormalsComponent::showNormals ( ) const
{
  Usul::Interfaces::IGroup::QueryPtr group ( Usul::Documents::Manager::instance().activeView() );
  return ( group.valid() ? group->hasGroup ( Detail::GROUP_NAME ) : false );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Can normals be shown?
//
///////////////////////////////////////////////////////////////////////////////

bool HideShowNormalsComponent::canHideShowNormals () const
{
  Usul::Interfaces::IView::RefPtr view ( Usul::Documents::Manager::instance().activeView() );

  // Get the interface we need.
  Usul::Interfaces::IOpenSceneGraph::QueryPtr osg ( view );
  Usul::Interfaces::IGroup::QueryPtr group ( view );

  return ( group.valid () && osg.valid() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add to the menu.
//
///////////////////////////////////////////////////////////////////////////////

void HideShowNormalsComponent::menuAdd ( MenuKit::Menu& m, Usul::Interfaces::IUnknown * caller )
{
  typedef MenuKit::ToggleButton ToggleButton;
  typedef void (HideShowNormalsComponent::*BoolFunction) ( bool );
  typedef bool (HideShowNormalsComponent::*CheckFunction) () const;
  typedef Usul::Adaptors::MemberFunction < bool, HideShowNormalsComponent*, CheckFunction > CheckFunctor;
  typedef Usul::Adaptors::MemberFunction < void, HideShowNormalsComponent*, BoolFunction >  BoolFunctor;
  typedef Usul::Commands::GenericCheckCommand < BoolFunctor, CheckFunctor, CheckFunctor > CheckCommand;

  // Build the menu.
  MenuKit::Menu::RefPtr menu ( m.find ( "Tools", true ) );
  
  menu->append ( new ToggleButton ( new CheckCommand ( "Show Normals",  
                                                       BoolFunctor ( this, &HideShowNormalsComponent::showNormals ), 
                                                       CheckFunctor ( this, &HideShowNormalsComponent::showNormals ),
                                                       CheckFunctor ( this, &HideShowNormalsComponent::canHideShowNormals ) ) ) );
}
