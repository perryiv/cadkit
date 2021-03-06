
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The component class.
//
///////////////////////////////////////////////////////////////////////////////

#include "SceneDelegateComponent.h"

#include "Usul/Components/Factory.h"

USUL_DECLARE_COMPONENT_FACTORY ( SceneDelegateComponent );
USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( SceneDelegateComponent, SceneDelegateComponent::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

SceneDelegateComponent::SceneDelegateComponent() : BaseClass()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

SceneDelegateComponent::~SceneDelegateComponent()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *SceneDelegateComponent::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::IPlugin::IID:
    return static_cast < Usul::Interfaces::IPlugin*>(this);
  case Usul::Interfaces::IDefaultGUIDelegate::IID:
    return static_cast < Usul::Interfaces::IDefaultGUIDelegate * > ( this );
  default:
    return BaseClass::queryInterface ( iid );
  }
}


/////////////////////////////////////////////////////////////////////////////
//
//  Does this delegate handle the given token?
//
/////////////////////////////////////////////////////////////////////////////

bool SceneDelegateComponent::doesHandle ( const std::string& token ) const
{
  return token == "Scene Document";
}
