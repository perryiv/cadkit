
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
  if ( false == url.empty() )
  {
    Minerva::Layers::GeoRSS::GeoRSSLayer::RefPtr layer ( new Minerva::Layers::GeoRSS::GeoRSSLayer );
    layer->url ( url );
    al->addLayer ( Usul::Interfaces::IUnknown::QueryPtr ( layer ) );
  }
}
