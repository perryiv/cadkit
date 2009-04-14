
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Specialized layer for roads.
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Layers/OSM/RoadLayer.h"
#include "Minerva/Layers/OSM/LineJob.h"

#include "Usul/Factory/RegisterCreator.h"
#include "Usul/Strings/Format.h"
#include "Usul/User/Directory.h"

using namespace Minerva::Layers::OSM;

USUL_FACTORY_REGISTER_CREATOR_WITH_NAME ( "OSM:RoadLayer", RoadLayer );

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

RoadLayer::RoadLayer() : BaseClass()
{
  this->_initializeCache ( "road_layer" );

  // See http://wiki.openstreetmap.org/wiki/Map_Features for key, value pairs.
  const Predicate motorway ( "highway", "motorway" );
  const Predicate motorwayLink ( "highway", "motorway_link" );
  const Predicate trunk ( "highway", "trunk" );
  const Predicate primary ( "highway", "primary" );
  const Predicate primaryLink ( "highway", "primary_link" );
  const Predicate secondary ( "highway", "secondary" );
  const Predicate secondaryLink ( "highway", "secondary_link" );
  const Predicate tertiary ( "highway", "tertiary" );
  const Predicate unclassified ( "highway", "unclassified" );
  const Predicate road ( "highway", "road" );
  const Predicate residential ( "highway", "residential" );

  // Add the request starting at given level.
  this->addRequest ( 5, motorway );
  
  this->addRequest ( 10, motorwayLink );
  this->addRequest ( 10, trunk );
  this->addRequest ( 10, primary );
  this->addRequest ( 10, primaryLink );
  this->addRequest ( 10, secondary );
  this->addRequest ( 10, secondaryLink );

  this->addRequest ( 15, tertiary );
  this->addRequest ( 15, unclassified );
  this->addRequest ( 15, road );
  this->addRequest ( 15, residential );

  Style::RefPtr orange ( new Style );
  orange->linestyle ( Minerva::Core::Data::LineStyle::create ( Usul::Math::Vec4f ( 1.0f, 0.6f, 0.0f, 0.7f ), 2.0f ) );
  this->addStyle ( motorway, orange );

  Style::RefPtr yellow ( new Style );
  yellow->linestyle ( Minerva::Core::Data::LineStyle::create ( Usul::Math::Vec4f ( 1.0f, 1.0f, 0.0f, 0.7f ), 2.0f ) );
  this->addStyle ( motorwayLink, orange );
  this->addStyle ( trunk, yellow );
  this->addStyle ( primary, yellow );
  this->addStyle ( primaryLink, yellow );
  this->addStyle ( secondary, yellow );
  this->addStyle ( secondaryLink, yellow );

  Style::RefPtr white ( new Style );
  white->linestyle ( Minerva::Core::Data::LineStyle::create ( Usul::Math::Vec4f ( 1.0f, 1.0f, 1.0f, 0.7f ), 2.0f ) );
  this->addStyle ( tertiary, white );
  this->addStyle ( unclassified, white );
  this->addStyle ( road, white );
  this->addStyle ( residential, white );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

RoadLayer::~RoadLayer()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Launch a job for the predicate.
//
///////////////////////////////////////////////////////////////////////////////

RoadLayer::JobPtr RoadLayer::_launchJob ( 
    const Predicate& predicate, 
    const Extents& extents, 
    unsigned int level, 
    Usul::Jobs::Manager *manager, 
    Usul::Interfaces::IUnknown::RefPtr caller )
{
  LineJob::RefPtr job ( new LineJob ( manager, this->_getCache(), this->url(), extents, level, predicate, caller ) );

  Style::RefPtr style ( this->getStyle ( predicate ) );
  job->lineStyle ( ( style.valid() ) ? style->linestyle() : 0x0 );
  return job.get();
}
