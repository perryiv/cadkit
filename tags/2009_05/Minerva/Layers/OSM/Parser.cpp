
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Helper class to parse xml.
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Layers/OSM/Parser.h"
#include "Minerva/Layers/OSM/LineStringParser.h"

#include "Minerva/Core/Data/TimeStamp.h"

#include "XmlTree/Document.h"
#include "XmlTree/XercesLife.h"

#include "Usul/Convert/Convert.h"

#include "boost/foreach.hpp"

#include "xercesc/sax2/SAX2XMLReader.hpp"
#include "xercesc/sax2/XMLReaderFactory.hpp"

using namespace Minerva::Layers::OSM;
using Minerva::Layers::OSM::Extents;


///////////////////////////////////////////////////////////////////////////////
//
//  Typedefs.
//
///////////////////////////////////////////////////////////////////////////////

typedef Minerva::Layers::OSM::Object OSMObject;
typedef OSMObject::IdType IdType;
typedef OSMObject::Date DateType;
typedef OSMObject::Tags Tags;
typedef Node::Location LocationType;
typedef std::map<IdType,Node::RefPtr> NodeMap;
typedef XmlTree::Node::Children Children;
typedef XmlTree::Node::Attributes Attributes;


///////////////////////////////////////////////////////////////////////////////
//
//  Set the bounds.
//
///////////////////////////////////////////////////////////////////////////////

Extents Parser::parseExtents ( const XmlTree::Node& node )
{
  const std::string minlat ( node.attribute ( "minlat" ) );
  const std::string minlon ( node.attribute ( "minlon" ) );
  const std::string maxlat ( node.attribute ( "maxlat" ) );
  const std::string maxlon ( node.attribute ( "maxlon" ) );
  
  typedef Usul::Convert::Type<std::string,double> ToDouble;
  
  Extents::Vertex mn, mx;
  
  mn.set ( minlon.empty() ? 0.0 : ToDouble::convert ( minlon ), minlat.empty() ? 0.0 : ToDouble::convert ( minlat ) );
  mx.set ( maxlon.empty() ? 0.0 : ToDouble::convert ( maxlon ), maxlat.empty() ? 0.0 : ToDouble::convert ( maxlat ) );
  
  return Extents ( mn, mx );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Parse the xml into ways and nodes.
//
///////////////////////////////////////////////////////////////////////////////

void Parser::parseNodesAndWays ( const XmlTree::Node& xmlNode, Nodes& nodes, Ways& ways )
{
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

    // Tags for the node.
    Tags tags;
    Parser::_parseTags ( *xmlNode, tags );

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
//  Parse the xml into ways and nodes.
//
///////////////////////////////////////////////////////////////////////////////

void Parser::parseNodesAndWays ( const std::string& filename, Nodes& nodes, Ways& ways )
{
  XmlTree::XercesLife life;
  XmlTree::Document::RefPtr doc ( new XmlTree::Document );
  doc->load ( filename );

  // Redirect
  parseNodesAndWays ( *doc, nodes, ways );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Parse the tags.
//
///////////////////////////////////////////////////////////////////////////////

void Parser::_parseTags ( const XmlTree::Node& node, Tags& tags )
{
  // Parse the tags.
  const Children& children ( node.children() );
  BOOST_FOREACH ( XmlTree::Node::ValidRefPtr child, children )
  {
    if ( "tag" == child->name() )
    {
      // Get the attributes.
      Attributes a ( child->attributes() );

      // Add the tag's key and value.
      tags.insert ( std::make_pair ( a["k"], a["v"] ) );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Parse file into lines.
//
///////////////////////////////////////////////////////////////////////////////

void Parser::parseLines ( const std::string& filename, Lines& lines )
{
  XmlTree::XercesLife life;

  xercesc::SAX2XMLReader* parser ( xercesc::XMLReaderFactory::createXMLReader() );

  Detail::LineStringParser handler ( lines );
  parser->setContentHandler ( &handler );
  parser->setErrorHandler ( &handler );

  parser->parse ( filename.c_str() );
}
