
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

#include "ModelPresentationComponent.h"
#include "ModelPresentationDocument.h"



#include <algorithm>

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( ModelPresentationComponent, ModelPresentationComponent::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

ModelPresentationComponent::ModelPresentationComponent() : BaseClass()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

ModelPresentationComponent::~ModelPresentationComponent()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *ModelPresentationComponent::queryInterface ( unsigned long iid )
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

Usul::Documents::Document *ModelPresentationComponent::createDocument ( Unknown *caller )
{
  ModelPresentationDocument::ValidRefPtr document ( new ModelPresentationDocument );
  return document.release();
}
