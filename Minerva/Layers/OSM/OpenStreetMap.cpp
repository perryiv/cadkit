
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Layers/OSM/OpenStreetMap.h"
#include "Minerva/Core/Data/DataObject.h"
#include "Minerva/Core/Data/Line.h"
#include "Minerva/Core/Data/Point.h"
#include "Minerva/Core/Data/MultiPoint.h"
#include "Minerva/Core/Data/TimeStamp.h"
#include "Minerva/Core/Factory/Readers.h"

#include "XmlTree/Document.h"
#include "XmlTree/XercesLife.h"

#include "Usul/Convert/Convert.h"
#include "Usul/Factory/RegisterCreator.h"
#include "Usul/Threads/Safe.h"

#include "boost/foreach.hpp"

using namespace Minerva::Layers::OSM;


///////////////////////////////////////////////////////////////////////////////
//
//  Register readers with the factory.
//
///////////////////////////////////////////////////////////////////////////////

namespace
{
  Minerva::Core::Factory::RegisterReader < Minerva::Core::Factory::TypeCreator < OpenStreetMap > > _creator_for_OSM ( "OpenStreetMap (*.osm)", "*.osm" );
}


USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( OpenStreetMap, OpenStreetMap::BaseClass );
USUL_FACTORY_REGISTER_CREATOR ( OpenStreetMap );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

OpenStreetMap::OpenStreetMap() : BaseClass()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

OpenStreetMap::~OpenStreetMap()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query Interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown* OpenStreetMap::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IRead::IID:
    return static_cast < Usul::Interfaces::IRead* > ( this );
  default:
    return BaseClass::queryInterface ( iid );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the file.
//
///////////////////////////////////////////////////////////////////////////////

void OpenStreetMap::read ( const std::string &filename, Usul::Interfaces::IUnknown *caller, Usul::Interfaces::IUnknown *progress )
{
  Usul::Threads::Safe::set ( this->mutex(), filename, _filename );
  
  this->_read ( filename, caller, progress );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the file.
//
///////////////////////////////////////////////////////////////////////////////

void OpenStreetMap::_read ( const std::string &filename, Usul::Interfaces::IUnknown *caller, Usul::Interfaces::IUnknown *progress )
{
  XmlTree::XercesLife life;
  XmlTree::Document::RefPtr doc ( new XmlTree::Document );
  doc->load ( filename );
  
  // Get the bounds of the data set.
  XmlTree::Node::RefPtr bounds ( doc->child ( "bounds" ) );
  if ( bounds.valid() )
    this->_setBounds ( *bounds );
  
  // Nodes and ways.
  Nodes nodes;
  Ways ways;

  // Parse.
  OpenStreetMap::_parse ( *doc, nodes, ways );

  Usul::Interfaces::IUnknown::QueryPtr allNodes ( OpenStreetMap::_createForAllNodes ( nodes ) );
  this->add ( allNodes );
  
  // Add nodes.
#if 0
  BOOST_FOREACH ( OSMNodePtr node, nodes )
  {
    if ( true == node.valid() )
    {
      Usul::Interfaces::IUnknown::QueryPtr unknown ( OpenStreetMap::_createForNode ( *node ) );
      this->add ( unknown );
    }
  }
#endif

  // Add ways.
  BOOST_FOREACH ( OSMWayPtr way, ways )
  {
    if ( true == way.valid() )
    {
      Usul::Interfaces::IUnknown::QueryPtr unknown ( OpenStreetMap::_createForWay ( *way ) );
      this->add ( unknown );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Deserialize.
//
///////////////////////////////////////////////////////////////////////////////

void OpenStreetMap::deserialize( const XmlTree::Node &node )
{
  BaseClass::deserialize ( node );
  this->read ( _filename );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Serialize.
//
///////////////////////////////////////////////////////////////////////////////

void OpenStreetMap::serialize ( XmlTree::Node &parent ) const
{
  Serialize::XML::DataMemberMap dataMemberMap ( Usul::Threads::Safe::get ( this->mutex(), _dataMemberMap ) );
  
  // Don't serialize the layers.
  dataMemberMap.erase ( "layers" );
  
  // Serialize.
  dataMemberMap.serialize ( parent );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the bounds.
//
///////////////////////////////////////////////////////////////////////////////

void OpenStreetMap::_setBounds ( const XmlTree::Node& node )
{
  const std::string minlat ( node.attribute ( "minlat" ) );
  const std::string minlon ( node.attribute ( "minlon" ) );
  const std::string maxlat ( node.attribute ( "maxlat" ) );
  const std::string maxlon ( node.attribute ( "maxlon" ) );
  
  typedef Usul::Convert::Type<std::string,double> ToDouble;
  
  Extents::Vertex mn, mx;
  
  mn.set ( minlon.empty() ? 0.0 : ToDouble::convert ( minlon ), minlat.empty() ? 0.0 : ToDouble::convert ( minlat ) );
  mx.set ( maxlon.empty() ? 0.0 : ToDouble::convert ( maxlon ), maxlat.empty() ? 0.0 : ToDouble::convert ( maxlat ) );
  
  this->extents ( Extents ( mn, mx ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Parse the xml into ways and nodes.
//
///////////////////////////////////////////////////////////////////////////////

void OpenStreetMap::_parse ( const XmlTree::Node& xmlNode, Nodes& nodes, Ways& ways )
{
  // Typedefs.
  typedef Minerva::Layers::OSM::Object OSMObject;
  typedef OSMObject::IdType IdType;
  typedef OSMObject::Date DateType;
  typedef OSMObject::Tags Tags;
  typedef Node::Location LocationType;
  typedef Way::Nodes Nodes;
  typedef std::map<IdType,Node::RefPtr> NodeMap;
  typedef XmlTree::Node::Children Children;
  typedef XmlTree::Node::Attributes Attributes;

  // Get all the children.
  const Children& children ( xmlNode.children() );

  // Map to retrieve nodes when parsing a Way.
  NodeMap map;
  
  // Add each node.
  BOOST_FOREACH ( XmlTree::Node::ValidRefPtr xmlNode, children )
  {
    // Get the attributes.
    Attributes attributes ( xmlNode->attributes() );
    
    // Get the id.
    IdType id ( Usul::Convert::Type<std::string,IdType>::convert ( attributes["id"] ) );

    // Get the date.
    DateType date ( DateType::createFromKml ( attributes["timestamp"] ) );

#ifdef _DEBUG
    const std::string temp ( date.toString() );
#endif

    // Tags for the node.
    Tags tags;

    // Parse the tags.
    BOOST_FOREACH ( XmlTree::Node::ValidRefPtr child, xmlNode->children() )
    {
      if ( "tag" == child->name() )
      {
        // Get the attributes.
        Attributes a ( child->attributes() );

        // Add the tag's key and value.
        tags.insert ( std::make_pair ( a["k"], a["v"] ) );
      }
    }

    // Is the xml element a node?
    if ( "node" == xmlNode->name() )
    {
      // Lat/Lon position.
      const double lat ( Usul::Convert::Type<std::string,double>::convert ( attributes["lat"] ) );
      const double lon ( Usul::Convert::Type<std::string,double>::convert ( attributes["lon"] ) );

      // Create the node.
      Node::RefPtr osmNode ( Node::create ( id, LocationType ( lon, lat ), date, tags ) );
      nodes.push_back ( osmNode );
      map.insert ( std::make_pair ( id, osmNode ) );
    }

    // Is the xml element a way?
    else if ( "way" == xmlNode->name() )
    {
      // Nodes for this way.
      Nodes nodes;

      // Parse the nodes.
      BOOST_FOREACH ( XmlTree::Node::ValidRefPtr child, xmlNode->children() )
      {
        if ( "nd" == child->name() )
        {
          // Get the attributes.
          Attributes a ( child->attributes() );

          // Get the node id.
          const IdType nodeId ( Usul::Convert::Type<std::string,IdType>::convert ( a["ref"] ) );

          // Get the node.
          Node::RefPtr osmNode ( map[nodeId] );

          // Add the node.
          if ( true == osmNode.valid() )
            nodes.push_back ( osmNode );
        }
      }

      ways.push_back ( Way::create ( id, date, tags, nodes ) );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create a data object.
//
///////////////////////////////////////////////////////////////////////////////

OpenStreetMap::DataObject* OpenStreetMap::_createForAllNodes ( const Nodes& nodes )
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

OpenStreetMap::DataObject* OpenStreetMap::_createForNode ( const Minerva::Layers::OSM::Node& node )
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

OpenStreetMap::DataObject* OpenStreetMap::_createForWay  ( const Minerva::Layers::OSM::Way&  way  )
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
