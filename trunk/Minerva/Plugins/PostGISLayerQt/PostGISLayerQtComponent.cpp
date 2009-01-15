
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
#include "Minerva/Layers/PostGIS/Layer.h"
#include "Minerva/Core/Commands/AddLayer.h"
#include "Minerva/Core/Commands/RemoveLayer.h"
#include "Minerva/Interfaces/IFeature.h"

#include "Usul/Components/Factory.h"
#include "Usul/Documents/Manager.h"

#include "QtGui/QDialog"
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

QWidget* PostGISLayerQtComponent::layerAddGUI( Usul::Interfaces::IUnknown *caller )
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

void PostGISLayerQtComponent::apply ( Usul::Interfaces::IUnknown* parent, Usul::Interfaces::IUnknown* caller )
{
  if ( 0x0 != _widget )
    _widget->apply ( parent, caller );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Do we handle this type of layer?
//
///////////////////////////////////////////////////////////////////////////////

bool PostGISLayerQtComponent::handle ( Usul::Interfaces::IUnknown* layer ) const
{
  Minerva::Interfaces::IFeature::QueryPtr f ( layer );
  Minerva::Core::Data::Feature::RefPtr feature ( f.valid() ? f->feature() : 0x0 );
  return 0x0 != dynamic_cast < Minerva::Layers::PostGIS::Layer * > ( feature.get() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Show the property gui.
//
///////////////////////////////////////////////////////////////////////////////

void PostGISLayerQtComponent::showModifyGUI ( Usul::Interfaces::IUnknown* layer, Usul::Interfaces::IUnknown* parent, Usul::Interfaces::IUnknown* caller ) 
{
  Minerva::Interfaces::IFeature::QueryPtr f ( layer );
  Minerva::Core::Data::Feature::RefPtr feature ( f.valid() ? f->feature() : 0x0 );
  Minerva::Layers::PostGIS::Layer::RefPtr baseLayer ( dynamic_cast < Minerva::Layers::PostGIS::Layer* > ( feature.get() ) );

  // Return now if no layer.
  if ( 0x0 == baseLayer )
    return;

  // Make a copy.
  Usul::Interfaces::IUnknown::RefPtr clone ( baseLayer->clone() );

  Minerva::Layers::PostGIS::Layer::RefPtr clonedLayer ( dynamic_cast < Minerva::Layers::PostGIS::Layer* > ( clone.get() ) );

  PropertyPage *page ( new PropertyPage ( clonedLayer ) );

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
  
  hLayout->addStretch();
  hLayout->addWidget ( ok );
  hLayout->addWidget ( cancel );

  if ( QDialog::Accepted == dialog.exec() )
  {
    // Remove the old one.
    Usul::Interfaces::IUnknown::QueryPtr unknown ( baseLayer );
    Minerva::Core::Commands::RemoveLayer::RefPtr removeLayer ( new Minerva::Core::Commands::RemoveLayer ( unknown.get() ) );
    removeLayer->execute ( parent );

    // Add the new one.
    Minerva::Core::Commands::AddLayer::RefPtr addLayer ( new Minerva::Core::Commands::AddLayer ( parent, Usul::Interfaces::IUnknown::QueryPtr ( clonedLayer ) ) );
    addLayer->execute ( caller );
  }
}
