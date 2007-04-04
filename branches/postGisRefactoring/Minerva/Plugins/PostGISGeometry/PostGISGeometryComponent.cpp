
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Plugins/PostGISGeometry/PostGISGeometryComponent.h"

#include "BinaryParser.h"

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( PostGISGeometryComponent, PostGISGeometryComponent::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
// Constructor
//
///////////////////////////////////////////////////////////////////////////////

PostGISGeometryComponent::PostGISGeometryComponent()
{
}


///////////////////////////////////////////////////////////////////////////////
//
// Destructor
//
///////////////////////////////////////////////////////////////////////////////

PostGISGeometryComponent::~PostGISGeometryComponent() 
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *PostGISGeometryComponent::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::IPlugin::IID:
    return static_cast < Usul::Interfaces::IPlugin* > ( this );
  case Usul::Interfaces::ICreateGeometry::IID:
    return static_cast < Usul::Interfaces::ICreateGeometry* > ( this );
  default:
    return 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Usul::Interfaces::IPlugin implementation
//
///////////////////////////////////////////////////////////////////////////////

std::string PostGISGeometryComponent::getPluginName() const 
{
  return "PostGISGeometry";
}

Usul::Interfaces::IUnknown* PostGISGeometryComponent::createFromBinary ( unsigned int srid, unsigned char* buffer, unsigned int length )
{
  return 0x0;
}
