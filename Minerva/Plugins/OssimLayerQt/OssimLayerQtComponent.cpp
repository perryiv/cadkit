
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

#include "Minerva/Plugins/OssimLayerQt/OssimLayerQtComponent.h"

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( OssimLayerQtComponent, OssimLayerQtComponent::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
// Constructor
//
///////////////////////////////////////////////////////////////////////////////

OssimLayerQtComponent::OssimLayerQtComponent() : BaseClass(),
_widget ( 0x0 )
{
}


///////////////////////////////////////////////////////////////////////////////
//
// Destructor
//
///////////////////////////////////////////////////////////////////////////////

OssimLayerQtComponent::~OssimLayerQtComponent() 
{
//  delete _widget;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *OssimLayerQtComponent::queryInterface ( unsigned long iid )
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

std::string OssimLayerQtComponent::getPluginName() const 
{
  return "Ossim Layer Qt GUI";
}


QWidget* OssimLayerQtComponent::layerAddGUI ( )
{
  _widget = new AddOssimLayerWidget;
  return _widget;
}

std::string OssimLayerQtComponent::name() const 
{
  return "Ossim Image";
}

void OssimLayerQtComponent::apply ( Usul::Interfaces::IUnknown* caller )
{
  if ( 0x0 != _widget )
    _widget->apply ( caller );

  _widget = 0x0;
}
