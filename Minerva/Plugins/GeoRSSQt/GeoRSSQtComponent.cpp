
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
#include "Minerva/Layers/GeoRSS/GeoRSSLayer.h"
#include "Minerva/Interfaces/IAddLayer.h"

#include "Usul/Interfaces/IDocument.h"
#include "Usul/Components/Factory.h"

#include "QtGui/QDialog"
#include "QtGui/QDialogButtonBox"
#include "QtGui/QVBoxLayout"

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
  _widget = new AddGeoRSSLayerWidget;
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
//  Add Geo RSS feed to caller.
//
///////////////////////////////////////////////////////////////////////////////

void GeoRSSQtComponent::apply ( Usul::Interfaces::IUnknown* parent, Usul::Interfaces::IUnknown* caller )
{
  if ( 0x0 != _widget )
  {
    Minerva::Interfaces::IAddLayer::QueryPtr al ( parent );
    
    if ( false == al.valid () )
      return;
    
    const std::string url ( _widget->url() );
    const double refreshRate ( _widget->refreshRate() );
    const bool enableFiltering ( _widget->enableFiltering() );
    const std::string element ( _widget->element() );
    const std::string value ( _widget->value() );
    const Usul::Math::Vec4f color ( _widget->color() );
    
    if ( false == url.empty() )
    {
      Minerva::Layers::GeoRSS::GeoRSSLayer::RefPtr layer ( new Minerva::Layers::GeoRSS::GeoRSSLayer );
      layer->url ( url );
      layer->refreshRate ( refreshRate );
      layer->filteringEnabled ( enableFiltering );
      layer->color ( color );
      
      if ( false == element.empty() && false == value.empty() )
        layer->filter ( Minerva::Layers::GeoRSS::GeoRSSLayer::Filter ( element, value ) );
      
      // Start the download.
      layer->downloadFeed();
      
      // Add the layer.
      al->addLayer ( Usul::Interfaces::IUnknown::QueryPtr ( layer ) );
    }
  }

  _widget = 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Do we handle this layer.
//
///////////////////////////////////////////////////////////////////////////////

bool GeoRSSQtComponent::handle ( Usul::Interfaces::ILayer* layer ) const
{
  const bool handle ( 0x0 != dynamic_cast<Minerva::Layers::GeoRSS::GeoRSSLayer*> ( layer ) );
  return handle;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Show a dialog to modify properties.
//
///////////////////////////////////////////////////////////////////////////////

void GeoRSSQtComponent::showModifyGUI ( Usul::Interfaces::ILayer* layer, Usul::Interfaces::IUnknown* caller )
{
  typedef Minerva::Layers::GeoRSS::GeoRSSLayer GeoRSSLayer;
  GeoRSSLayer *geoRss ( dynamic_cast<GeoRSSLayer*> ( layer ) );
  
  if ( 0x0 == geoRss )
    return;
  
  QDialog dialog ( 0x0 );
  
  // Make page to edit geoRSS values.
  AddGeoRSSLayerWidget *page ( new AddGeoRSSLayerWidget ( &dialog ) );
  page->url ( geoRss->url() );
  page->refreshRate ( geoRss->refreshRate() );
  page->enableFiltering ( geoRss->filteringEnabled() );
  page->element ( geoRss->filter().first );
  page->value ( geoRss->filter().second );
  page->color ( geoRss->color() );
  
  const QDialogButtonBox::StandardButtons buttons ( QDialogButtonBox::Cancel|QDialogButtonBox::NoButton|QDialogButtonBox::Ok );
  QDialogButtonBox *buttonBox ( new QDialogButtonBox ( buttons, Qt::Horizontal, &dialog ) );
  
  QObject::connect ( buttonBox, SIGNAL ( accepted() ), &dialog, SLOT ( accept() ) );
  QObject::connect ( buttonBox, SIGNAL ( rejected() ), &dialog, SLOT ( reject() ) );
  
  QVBoxLayout *topLayout ( new QVBoxLayout );
  dialog.setLayout ( topLayout );
  
  topLayout->addWidget ( page );
  topLayout->addWidget ( buttonBox );
  
  // Set the title.
  dialog.setWindowTitle( QString ( "Edit " ) + QString ( geoRss->name().c_str() ) );

	const int result ( dialog.exec() );
  
	if ( QDialog::Accepted == result )
	{
    const std::string url ( page->url() );
    const double refreshRate ( page->refreshRate() );
    const bool enableFiltering ( page->enableFiltering() );
    const std::string element ( page->element() );
    const std::string value ( page->value() );
    const Usul::Math::Vec4f color ( page->color() );
    
    // Set the new properties.
    geoRss->url ( url );
    geoRss->refreshRate ( refreshRate );
    geoRss->filteringEnabled ( enableFiltering );
    geoRss->color ( color );
    
    if ( false == element.empty() && false == value.empty() )
      geoRss->filter ( GeoRSSLayer::Filter ( element, value ) );
    
    // Start the download.
    geoRss->downloadFeed();
    
		// Query for needed interfaces.
		Usul::Interfaces::IDocument::QueryPtr document ( caller );
    
		if ( document.valid() )
			document->requestRedraw();
	}
}
