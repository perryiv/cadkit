
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

#include "Minerva/Plugins/GeoRSSQt/GeoRSSQtComponent.h"

#include "Usul/Components/Factory.h"

USUL_DECLARE_COMPONENT_FACTORY ( GeoRSSQtComponent );
USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( GeoRSSQtComponent, GeoRSSQtComponent::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
// Constructor
//
///////////////////////////////////////////////////////////////////////////////

GeoRSSQtComponent::GeoRSSQtComponent() : BaseClass(),
  _widget ( 0x0 )
{
}


///////////////////////////////////////////////////////////////////////////////
//
// Destructor
//
///////////////////////////////////////////////////////////////////////////////

GeoRSSQtComponent::~GeoRSSQtComponent() 
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *GeoRSSQtComponent::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::IPlugin::IID:
    return static_cast < Usul::Interfaces::IPlugin* > ( this );
  case Usul::Interfaces::ILayerAddGUIQt::IID:
    return static_cast < Usul::Interfaces::ILayerAddGUIQt * > ( this );
  default:
    return 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Usul::Interfaces::IPlugin implementation
//
///////////////////////////////////////////////////////////////////////////////

std::string GeoRSSQtComponent::getPluginName() const 
{
  return "GeoRSS Qt GUI";
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the widget to add the gui.
//
///////////////////////////////////////////////////////////////////////////////

QWidget* GeoRSSQtComponent::layerAddGUI ( Usul::Interfaces::IUnknown* caller )
{
  _widget = new AddGeoRSSLayerWidget ( caller );
  return _widget;
}


///////////////////////////////////////////////////////////////////////////////
//
//  The name of the layer type.
//
///////////////////////////////////////////////////////////////////////////////

std::string GeoRSSQtComponent::name() const 
{
  return "GeoRSS";
}


///////////////////////////////////////////////////////////////////////////////
//
//  Apply.
//
///////////////////////////////////////////////////////////////////////////////

void GeoRSSQtComponent::apply ( Usul::Interfaces::IUnknown* parent, Usul::Interfaces::IUnknown* caller )
{
  if ( 0x0 != _widget )
    _widget->apply ( parent, caller );

  _widget = 0x0;
}
