
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

#include "StarSystemComponent.h"
#include "StarSystemDocument.h"

#include "StarSystem/Body.h"
#include "StarSystem/System.h"
#include "StarSystem/LandModelEllipsoid.h"
#include "StarSystem/LandModelFlat.h"

#include "Usul/Factory/ObjectFactory.h"
#include "Usul/Factory/RegisterCreator.h"
#include "Usul/Factory/TypeCreator.h"
#include "Usul/Trace/Trace.h"

#include "osgDB/Registry"

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( StarSystemComponent, StarSystemComponent::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
//  Registration for serialization. Doing all these in one place, when the 
//  library is loaded, is thead-safe with respect to the factory.
//
///////////////////////////////////////////////////////////////////////////////

namespace StarSystem
{
  USUL_FACTORY_REGISTER_CREATOR_WITH_NAME ( "star_system_system", System );
  USUL_FACTORY_REGISTER_CREATOR_WITH_NAME ( "star_system_body", Body );
  USUL_FACTORY_REGISTER_CREATOR_WITH_NAME ( "star_system_satellites", Group );
  USUL_FACTORY_REGISTER_CREATOR_WITH_NAME ( "star_system_land_model_ellipsoid", LandModelEllipsoid );
  USUL_FACTORY_REGISTER_CREATOR_WITH_NAME ( "star_system_land_model_flat", LandModelFlat );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

StarSystemComponent::StarSystemComponent() : BaseClass()
{
  USUL_TRACE_SCOPE;

  // Getting a mysterious crash while loading the jpeg reader-writer plugin.
  // Thinking that it's related to two or more threads all loading files at 
  // once, and the osgDB registry no being thread-safe. Doing this will 
  // pre-load the plugins.
  osgDB::Registry::instance()->getReaderWriterForExtension ( "jpg" );
  osgDB::Registry::instance()->getReaderWriterForExtension ( "png" );
  osgDB::Registry::instance()->getReaderWriterForExtension ( "tif" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

StarSystemComponent::~StarSystemComponent()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *StarSystemComponent::queryInterface ( unsigned long iid )
{
  USUL_TRACE_SCOPE;

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

Usul::Documents::Document *StarSystemComponent::createDocument ( Unknown *caller )
{
  USUL_TRACE_SCOPE;
  StarSystemDocument::ValidRefPtr document ( new StarSystemDocument );
  return document.release();
}
