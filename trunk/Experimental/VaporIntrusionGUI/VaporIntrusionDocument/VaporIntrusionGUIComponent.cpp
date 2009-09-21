
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Jeff Conner
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The component class.
//
///////////////////////////////////////////////////////////////////////////////

#include "VaporIntrusionGUIComponent.h"
#include "VaporIntrusionGUIDocument.h"

#include "Usul/Components/Factory.h"

#include <algorithm>

USUL_DECLARE_COMPONENT_FACTORY ( VaporIntrusionGUIComponent );
USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( VaporIntrusionGUIComponent, VaporIntrusionGUIComponent::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

VaporIntrusionGUIComponent::VaporIntrusionGUIComponent() : BaseClass()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

VaporIntrusionGUIComponent::~VaporIntrusionGUIComponent()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *VaporIntrusionGUIComponent::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::IPlugin::IID:
    return static_cast < Usul::Interfaces::IPlugin * > ( this );
  case Usul::Interfaces::IDocumentCreate::IID:
    return static_cast < Usul::Interfaces::IDocumentCreate * > ( this );
  default:
    return 0x0;
  }
}


/////////////////////////////////////////////////////////////////////////////
//
//  Create document.
//
/////////////////////////////////////////////////////////////////////////////

Usul::Documents::Document *VaporIntrusionGUIComponent::createDocument ( Unknown *caller )
{
  VaporIntrusionGUIDocument::ValidRefPtr document ( new VaporIntrusionGUIDocument );
  return document.release();
}

