
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

#include "Minerva/Plugins/Document/MinervaComponent.h"
#include "Minerva/Document/MinervaDocument.h"

#include "Usul/User/Directory.h"
#include "Usul/Strings/Format.h"
#include "Usul/System/Host.h"

#include <algorithm>

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( MinervaComponent, MinervaComponent::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

MinervaComponent::MinervaComponent() : BaseClass(),
  _log ( 0x0 )
{
  _log = new Usul::File::Log ( Usul::Strings::format ( 
    Usul::User::Directory::program ( true ), Usul::System::Host::name(), "_log.txt" ), true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

MinervaComponent::~MinervaComponent()
{
  _log = 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *MinervaComponent::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::IPlugin::IID:
    return static_cast < Usul::Interfaces::IPlugin*>(this);
  case Usul::Interfaces::IDocumentCreate::IID:
    return static_cast < Usul::Interfaces::IDocumentCreate*>(this);
  default:
    return 0x0;
  }
}


/////////////////////////////////////////////////////////////////////////////
//
//  Create document.
//
/////////////////////////////////////////////////////////////////////////////

Usul::Documents::Document *MinervaComponent::createDocument ( Unknown *caller )
{
  Minerva::Document::MinervaDocument::ValidRefPtr document ( new Minerva::Document::MinervaDocument ( _log ) );
  return document.release();
}
