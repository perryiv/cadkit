
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
#include "Minerva/Plugins/PostGISLayerQt/PropertyPage.h"
#include "Minerva/Core/Layers/Layer.h"

#include "Usul/Components/Factory.h"

#include "QtGui/QDialog.h"
#include "QtGui/QVBoxLayout"
#include "QtGui/QHBoxLayout"
#include "QtGui/QPushButton"

USUL_DECLARE_COMPONENT_FACTORY ( PostGISLayerQtComponent );
USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( PostGISLayerQtComponent, PostGISLayerQtComponent::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

PostGISLayerQtComponent::PostGISLayerQtComponent() : BaseClass(),
_widget ( 0x0 )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
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
  case Usul::Interfaces::ILayerModifyGUIQt::IID:
    return static_cast < Usul::Interfaces::ILayerModifyGUIQt * > ( this );
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
  _widget = new AddPostGISLayerWidget;
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
  if ( 0x0 != _widget )
    _widget->apply ( caller );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Do we handle this type of layer?
//
///////////////////////////////////////////////////////////////////////////////

bool PostGISLayerQtComponent::handle ( Usul::Interfaces::ILayer* layer ) const
{
  return 0x0 != dynamic_cast < Minerva::Core::Layers::Layer * > ( layer );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Show the property gui.
//
///////////////////////////////////////////////////////////////////////////////

void PostGISLayerQtComponent::showModifyGUI ( Usul::Interfaces::ILayer* layer, Usul::Interfaces::IUnknown* caller ) 
{
  Minerva::Core::Layers::Layer::RefPtr baseLayer ( dynamic_cast < Minerva::Core::Layers::Layer* > ( layer ) );
  PropertyPage *page ( new PropertyPage ( baseLayer ) );

  QDialog dialog ( 0x0 );
  QPushButton *ok ( new QPushButton ( "Ok" ) );
  QPushButton *cancel ( new QPushButton ( "Cancel" ) );

  QVBoxLayout *topLayout ( new QVBoxLayout );
  dialog.setLayout ( topLayout );

  QHBoxLayout *hLayout ( new QHBoxLayout );

  topLayout->addWidget ( page );
  topLayout->addLayout ( hLayout );
  
  hLayout->addStretch  ();
  hLayout->addWidget ( ok );
  hLayout->addWidget ( cancel );

  if ( QDialog::Accepted == dialog.exec() )
  {
  }
}
