
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Helper functions to parse xml and create DataObjects.
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Layers/OSM/Functions.h"

#include "Minerva/Core/Data/DataObject.h"
#include "Minerva/Core/Data/Line.h"
#include "Minerva/Core/Data/Point.h"
#include "Minerva/Core/Data/MultiPoint.h"
#include "Minerva/Core/Data/TimeStamp.h"

#include "Usul/Convert/Convert.h"

#include "boost/foreach.hpp"

using Minerva::Layers::OSM::DataObject;
using Minerva::Layers::OSM::Extents;


///////////////////////////////////////////////////////////////////////////////
//
//  Create a data object.
//
///////////////////////////////////////////////////////////////////////////////

DataObject* Minerva::Layers::OSM::createForAllNodes ( const Nodes& nodes )
{
  // Make a vertex array.
  Minerva::Core::Data::MultiPoint::Vertices points;
  points.reserve ( nodes.size() );
  
  // All all the points.
  for ( Nodes::const_iterator iter = nodes.begin(); iter != nodes.end(); ++iter )
  {
    OSMNodePtr node ( *iter );
    if ( node.valid() )
    {
      points.push_back ( Usul::Math::Vec3d (  node->location()[0], node->location()[1], 0.0 ) );
    }
  }
  
  // Make a point.
  Minerva::Core::Data::MultiPoint::RefPtr point ( new Minerva::Core::Data::MultiPoint );
  point->points ( points );
  point->size ( 6.0 );
  point->color ( Usul::Math::Vec4f ( 1.0, 0.0, 0.0, 1.0 ) );
  
  DataObject::RefPtr object ( new DataObject );
  object->addGeometry ( point.get() );
  object->name ( "All Nodes" );
  
  return object.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create a data object.
//
///////////////////////////////////////////////////////////////////////////////

DataObject* Minerva::Layers::OSM::createForNode ( const Minerva::Layers::OSM::Node& node )
{
  // Make a point.
  Minerva::Core::Data::Point::RefPtr point ( new Minerva::Core::Data::Point );
  point->point ( Usul::Math::Vec3d (  node.location()[0], node.location()[1], 0.0 ) );
  point->size ( 10.0 );
  point->autotransform ( false );
  point->color ( Usul::Math::Vec4f ( 1.0, 0.0, 0.0, 1.0 ) );

  DataObject::RefPtr object ( new DataObject );
  object->addGeometry ( point.get() );
  object->name ( Usul::Strings::format ( node.id() ) );
  object->timePrimitive ( new Minerva::Core::Data::TimeStamp ( node.timestamp() ) );

  return object.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create a data object.
//
///////////////////////////////////////////////////////////////////////////////

DataObject* Minerva::Layers::OSM::createForWay  ( const Minerva::Layers::OSM::Way&  way  )
{
  // Make a line.
  Minerva::Core::Data::Line::RefPtr line ( new Minerva::Core::Data::Line );
  Minerva::Core::Data::Line::Vertices vertices;
  vertices.reserve ( way.numNodes() );

  for ( unsigned int i = 0; i < way.numNodes(); ++i )
  {
    OSMNodePtr node ( way.node ( i ) );

    if ( true == node.valid() )
      vertices.push_back ( Usul::Math::Vec3d ( node->location()[0], node->location()[1], 0.0 ) );
  }

  line->line ( vertices );
  line->lineStyle ( Minerva::Core::Data::LineStyle::create ( Usul::Math::Vec4f ( 1.0, 1.0, 0.0, 1.0 ), 2.0 ) );

  DataObject::RefPtr object ( new DataObject );
  object->addGeometry ( line.get() );
  object->name ( Usul::Strings::format ( way.id() ) );
  object->timePrimitive ( new Minerva::Core::Data::TimeStamp ( way.timestamp() ) );

  return object.release();
}
