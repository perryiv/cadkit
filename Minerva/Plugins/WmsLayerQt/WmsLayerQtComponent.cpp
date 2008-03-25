
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

#include "Minerva/Plugins/WmsLayerQt/WmsLayerQtComponent.h"
#include "Minerva/Plugins/WmsLayerQt/EditWmsLayerWidget.h"

#include "QtGui/QDialog.h"
#include "QtGui/QVBoxLayout"
#include "QtGui/QHBoxLayout"
#include "QtGui/QPushButton"

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( WmsLayerQtComponent, WmsLayerQtComponent::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
// Constructor
//
///////////////////////////////////////////////////////////////////////////////

WmsLayerQtComponent::WmsLayerQtComponent() : BaseClass(),
  _widget ( 0x0 )
{
}


///////////////////////////////////////////////////////////////////////////////
//
// Destructor
//
///////////////////////////////////////////////////////////////////////////////

WmsLayerQtComponent::~WmsLayerQtComponent() 
{
//  delete _widget;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *WmsLayerQtComponent::queryInterface ( unsigned long iid )
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

std::string WmsLayerQtComponent::getPluginName() const 
{
  return "Wms Layer Qt GUI";
}


///////////////////////////////////////////////////////////////////////////////
//
//  Widget to add a gui for wms servers.
//
///////////////////////////////////////////////////////////////////////////////

QWidget* WmsLayerQtComponent::layerAddGUI ( Usul::Interfaces::IUnknown *caller )
{
  _widget = new AddWmsLayerWidget;
  return _widget;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the name of widget.  This will go on a tab.
//
///////////////////////////////////////////////////////////////////////////////

std::string WmsLayerQtComponent::name() const 
{
  return "Wms";
}


///////////////////////////////////////////////////////////////////////////////
//
//  Apply any selections the user made.
//
///////////////////////////////////////////////////////////////////////////////

void WmsLayerQtComponent::apply ( Usul::Interfaces::IUnknown* parent, Usul::Interfaces::IUnknown* caller )
{
  if ( 0x0 != _widget )
    _widget->apply ( parent, caller );

  _widget = 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Do we handle this layer.
//
///////////////////////////////////////////////////////////////////////////////

bool WmsLayerQtComponent::handle ( Usul::Interfaces::ILayer* layer ) const
{
  return 0x0 != dynamic_cast<Minerva::Core::Layers::RasterLayerWms*> ( layer );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Show a dialog to modify properties.
//
///////////////////////////////////////////////////////////////////////////////

void WmsLayerQtComponent::showModifyGUI ( Usul::Interfaces::ILayer* layer, Usul::Interfaces::IUnknown* caller )
{
  Minerva::Core::Layers::RasterLayerWms::RefPtr wms ( dynamic_cast < Minerva::Core::Layers::RasterLayerWms* > ( layer ) );
  
  // Return now if no layer.
  if ( 0x0 == wms )
    return;
  
  // Save current state.
  std::string name ( wms->name() );
  std::string url ( wms->url() );
  
  EditWmsLayerWidget *page ( new EditWmsLayerWidget ( wms ) );
  
  QDialog dialog ( 0x0 );
  QPushButton *ok ( new QPushButton ( "Ok" ) );
  QPushButton *cancel ( new QPushButton ( "Cancel" ) );
  
  QObject::connect ( ok,     SIGNAL ( clicked() ), &dialog, SLOT ( accept() ) );
  QObject::connect ( cancel, SIGNAL ( clicked() ), &dialog, SLOT ( reject() ) );
  
  QVBoxLayout *topLayout ( new QVBoxLayout );
  dialog.setLayout ( topLayout );
  
  QHBoxLayout *hLayout ( new QHBoxLayout );
  
  topLayout->addWidget ( page );
  topLayout->addLayout ( hLayout );
  
  hLayout->addStretch  ();
  hLayout->addWidget ( ok );
  hLayout->addWidget ( cancel );
  
  if ( QDialog::Rejected == dialog.exec() )
  {
    // Restore the state.
    wms->name ( name );
    wms->url ( url );
  }  
}
