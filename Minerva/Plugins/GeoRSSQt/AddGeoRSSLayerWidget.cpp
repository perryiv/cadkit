
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Plugins/GeoRSSQt/CompileGuard.h"
#include "Minerva/Plugins/GeoRSSQt/AddGeoRSSLayerWidget.h"

#include "Minerva/Layers/GeoRSS/GeoRSSLayer.h"

#include "Minerva/Interfaces/IAddLayer.h"


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

AddGeoRSSLayerWidget::AddGeoRSSLayerWidget( Usul::Interfaces::IUnknown* caller, QWidget *parent ) : BaseClass ( parent ),
  _caller ( caller )
{
  // Initialize code from designer.
  this->setupUi ( this );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

AddGeoRSSLayerWidget::~AddGeoRSSLayerWidget()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add Geo RSS feed to caller.
//
///////////////////////////////////////////////////////////////////////////////

void AddGeoRSSLayerWidget::apply ( Usul::Interfaces::IUnknown* parent, Usul::Interfaces::IUnknown * caller )
{
  Minerva::Interfaces::IAddLayer::QueryPtr al ( parent );

  if ( false == al.valid () )
    return;
  
  const std::string url ( _urlEdit->text().toStdString() );
	const double refreshRate ( _refreshRate->value() );
	const bool enableFiltering ( Qt::Checked == _enableFiltering->checkState() );
	const std::string element ( _elementFilter->text().toStdString() );
	const std::string value ( _valueFilter->text().toStdString() );

  if ( false == url.empty() )
  {
    Minerva::Layers::GeoRSS::GeoRSSLayer::RefPtr layer ( new Minerva::Layers::GeoRSS::GeoRSSLayer );
    layer->url ( url );
		layer->refreshRate ( refreshRate );
		layer->filteringEnabled ( enableFiltering );

		if ( false == element.empty() && false == value.empty() )
			layer->filter ( Minerva::Layers::GeoRSS::GeoRSSLayer::Filter ( element, value ) );

		// Start the download.
		layer->downloadFeed();

		// Add the layer.
    al->addLayer ( Usul::Interfaces::IUnknown::QueryPtr ( layer ) );
  }
}
