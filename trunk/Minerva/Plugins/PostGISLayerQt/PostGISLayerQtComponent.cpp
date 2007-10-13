
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

#if ( QT_VERSION < 0x040300 )
#include "QtGui/QFrame"
#endif

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


///////////////////////////////////////////////////////////////////////////////
//
//  Get the widget that adds a layer.
//
///////////////////////////////////////////////////////////////////////////////

QWidget* PostGISLayerQtComponent::layerAddGUI()
{
#if ( QT_VERSION >= 0x040300 )
  _widget = new AddPostGISLayerWidget;
#else
  _widget = new QFrame;
#endif
   return _widget;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the name.
//
///////////////////////////////////////////////////////////////////////////////

std::string PostGISLayerQtComponent::name() const 
{
  return "PostGIS";
}


///////////////////////////////////////////////////////////////////////////////
//
//  Apply the caller to the widget.
//
///////////////////////////////////////////////////////////////////////////////

void PostGISLayerQtComponent::apply ( Usul::Interfaces::IUnknown* caller )
{
#if ( QT_VERSION >= 0x040300 )
  if ( 0x0 != _widget )
    _widget->apply ( caller );
#endif
  _widget = 0x0;
}
