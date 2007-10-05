
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

#include "Minerva/Plugins/PostGISLayerQt/PostGISLayerQtComponent.h"

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( PostGISLayerQtComponent, PostGISLayerQtComponent::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
// Constructor
//
///////////////////////////////////////////////////////////////////////////////

PostGISLayerQtComponent::PostGISLayerQtComponent() : BaseClass(),
_widget ( 0x0 )
{
}


///////////////////////////////////////////////////////////////////////////////
//
// Destructor
//
///////////////////////////////////////////////////////////////////////////////

PostGISLayerQtComponent::~PostGISLayerQtComponent() 
{
//  delete _widget;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *PostGISLayerQtComponent::queryInterface ( unsigned long iid )
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

std::string PostGISLayerQtComponent::getPluginName() const 
{
  return "PostGIS Layer Qt GUI";
}


QWidget* PostGISLayerQtComponent::layerAddGUI ( )
{
  _widget = new AddPostGISLayerWidget;
  return _widget;
}

std::string PostGISLayerQtComponent::name() const 
{
  return "PostGIS";
}

void PostGISLayerQtComponent::apply ( Usul::Interfaces::IUnknown* caller )
{
  if ( 0x0 != _widget )
    _widget->apply ( caller );

  _widget = 0x0;
}
