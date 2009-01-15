
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
#include "Minerva/Plugins/GeoRSSQt/GeoRSSItemWidget.h"
#include "Minerva/Layers/GeoRSS/GeoRSSLayer.h"
#include "Minerva/Layers/GeoRSS/Item.h"
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
    
    // Make sure we have a url.
    if ( false == url.empty() )
    {
      Minerva::Layers::GeoRSS::GeoRSSLayer::RefPtr layer ( new Minerva::Layers::GeoRSS::GeoRSSLayer );
      
      // Set the new members.
      GeoRSSQtComponent::_setLayerMembers ( *_widget, *layer );
      
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

bool GeoRSSQtComponent::handle ( Usul::Interfaces::IUnknown* layer ) const
{
  Minerva::Interfaces::IFeature::QueryPtr f ( layer );
  Minerva::Core::Data::Feature::RefPtr feature ( f.valid() ? f->feature() : 0x0 );
  const bool isGeoRssLayer ( 0x0 != dynamic_cast<Minerva::Layers::GeoRSS::GeoRSSLayer*> ( feature.get() ) );
  
#ifdef _DEBUG
  const bool isGeoRssItem  ( 0x0 != dynamic_cast<Minerva::Layers::GeoRSS::Item*> ( feature.get() ) );
  return isGeoRssLayer || isGeoRssItem;
#else
  return isGeoRssLayer;
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Show a dialog to modify properties.
//
///////////////////////////////////////////////////////////////////////////////

void GeoRSSQtComponent::showModifyGUI ( Usul::Interfaces::IUnknown* layer, Usul::Interfaces::IUnknown* parent, Usul::Interfaces::IUnknown* caller )
{
  typedef Minerva::Layers::GeoRSS::GeoRSSLayer GeoRSSLayer;
  
  Minerva::Interfaces::IFeature::QueryPtr f ( layer );
  Minerva::Core::Data::Feature::RefPtr feature ( f.valid() ? f->feature() : 0x0 );
  
  GeoRSSLayer *geoRss ( dynamic_cast<GeoRSSLayer*> ( feature.get() ) );
  
  if ( 0x0 != geoRss )
  {
    this->_displayGeoRSSLayerGUI ( *geoRss, caller );
  }
  
  else if ( GeoRSSItem *item = dynamic_cast<GeoRSSItem*> ( feature.get() ) )
  {
    this->_displayGeoRSSItemGUI ( *item, caller );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the layer members.
//
///////////////////////////////////////////////////////////////////////////////

void GeoRSSQtComponent::_setLayerMembers ( AddGeoRSSLayerWidget& widget, GeoRSSLayer& layer )
{
  // Set the new properties.
  layer.url ( widget.url() );
  layer.refreshRate ( widget.refreshRate() );
  layer.filteringEnabled ( widget.enableFiltering() );
  layer.filter ( GeoRSSLayer::Filter ( widget.element(), widget.value() ) );
  layer.color ( widget.color() );
  layer.maximumItems ( widget.maximumItems() );
  layer.maximumAge ( widget.maximumAge() );
  layer.useRegEx ( widget.useRegEx() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Show the GUI to edit the GeoRSSLayer.
//
///////////////////////////////////////////////////////////////////////////////

void GeoRSSQtComponent::_displayGeoRSSLayerGUI ( GeoRSSLayer& geoRss, Usul::Interfaces::IUnknown* caller )
{
  QDialog dialog ( 0x0 );
  
  // Make page to edit geoRSS values.
  AddGeoRSSLayerWidget *page ( new AddGeoRSSLayerWidget ( &dialog ) );
  page->url ( geoRss.url() );
  page->refreshRate ( geoRss.refreshRate() );
  page->enableFiltering ( geoRss.filteringEnabled() );
  page->element ( geoRss.filter().first );
  page->value ( geoRss.filter().second );
  page->color ( geoRss.color() );
  page->maximumItems ( geoRss.maximumItems() );
  page->maximumAge ( geoRss.maximumAge() );
  page->useRegEx ( geoRss.useRegEx() );
  
  const QDialogButtonBox::StandardButtons buttons ( QDialogButtonBox::Cancel|QDialogButtonBox::NoButton|QDialogButtonBox::Ok );
  QDialogButtonBox *buttonBox ( new QDialogButtonBox ( buttons, Qt::Horizontal, &dialog ) );
  
  QObject::connect ( buttonBox, SIGNAL ( accepted() ), &dialog, SLOT ( accept() ) );
  QObject::connect ( buttonBox, SIGNAL ( rejected() ), &dialog, SLOT ( reject() ) );
  
  QVBoxLayout *topLayout ( new QVBoxLayout );
  dialog.setLayout ( topLayout );
  
  topLayout->addWidget ( page );
  topLayout->addWidget ( buttonBox );
  
  // Set the title.
  dialog.setWindowTitle ( QString ( "Edit " ) + QString ( geoRss.name().c_str() ) );
  
	const int result ( dialog.exec() );
  
	if ( QDialog::Accepted == result )
	{
    // Set the new members.
    GeoRSSQtComponent::_setLayerMembers ( *page, geoRss );
    
    // Start the download.
    geoRss.downloadFeed();
    
		// Query for needed interfaces.
		Usul::Interfaces::IDocument::QueryPtr document ( caller );
    
		if ( document.valid() )
			document->requestRedraw();
	}
}


///////////////////////////////////////////////////////////////////////////////
//
//  Show the GUI to edit the GeoRSSItem.
//
///////////////////////////////////////////////////////////////////////////////

void GeoRSSQtComponent::_displayGeoRSSItemGUI ( GeoRSSItem& item, Usul::Interfaces::IUnknown* caller )
{
  QDialog dialog ( 0x0 );
  
  // Make page to edit geoRSS values.
  GeoRSSItemWidget *page ( new GeoRSSItemWidget ( &dialog ) );
  page->title ( item.name() );
  page->description ( item.description() );
  
  const QDialogButtonBox::StandardButtons buttons ( QDialogButtonBox::NoButton|QDialogButtonBox::Ok );
  QDialogButtonBox *buttonBox ( new QDialogButtonBox ( buttons, Qt::Horizontal, &dialog ) );
  
  QObject::connect ( buttonBox, SIGNAL ( accepted() ), &dialog, SLOT ( accept() ) );
  
  QVBoxLayout *topLayout ( new QVBoxLayout );
  dialog.setLayout ( topLayout );
  
  topLayout->addWidget ( page );
  topLayout->addWidget ( buttonBox );
  
  // Set the title.
  dialog.setWindowTitle ( QString ( item.name().c_str() ) );
  
	dialog.exec();
}
