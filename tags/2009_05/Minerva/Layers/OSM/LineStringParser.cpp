
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Internal class to provide sax xml parsing of line strings.
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Layers/OSM/LineStringParser.h"

using namespace Minerva::Layers::OSM::Detail;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

LineStringParser::LineStringParser ( Lines& lines ) : BaseClass(),
  _lines ( lines ),
  _locations(),
  _currentId ( 0 ),
  _currentDate(),
  _currentTags(),
  _currentVertices(),
  _currentNodeIds()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

LineStringParser::~LineStringParser()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Beginning of element.
//
///////////////////////////////////////////////////////////////////////////////

void LineStringParser::startElement ( 
  const XMLCh* const uri, 
  const XMLCh* const localname,  
  const XMLCh* const qname, 
  const xercesc::Attributes&	attributes )
{
  const std::string name ( XmlTree::toNative ( localname ) );
  if ( "node" == name )
  {
    // Get the id.
    const IdType id ( this->_getAttribute<IdType> ( attributes, "id", 0 ) );

    // Lat/Lon position.
    const double lat ( this->_getAttribute<double> ( attributes, "lat", 0.0 ) );
    const double lon ( this->_getAttribute<double> ( attributes, "lon", 0.0 ) );

    _locations.insert ( std::make_pair ( id, Node::Location ( lon, lat ) ) );
  }
  else if ( "way" == name )
  {
    // Get the id.
    _currentId = this->_getAttribute<IdType> ( attributes, "id", 0 );

    // Get the date.
    _currentDate = DateType::createFromKml ( this->_getAttribute<std::string> ( attributes, "timestamp", DateType().toString() ) );
  }
  else if ( "tag" == name )
  {
    // Add the tag's key and value.
    _currentTags.insert ( std::make_pair ( this->_getAttribute<std::string> ( attributes, "k" ), this->_getAttribute<std::string> ( attributes, "v" ) ) );
  }
  else if ( "nd" == name )
  {
    // Get the node id.
    const IdType nodeId ( this->_getAttribute<IdType> ( attributes, "ref", 0 ) );

    // Get the location.
    LocationMap::const_iterator iter ( _locations.find ( nodeId ) );
    if ( iter != _locations.end() )
    {
      _currentVertices.push_back ( iter->second );
      _currentNodeIds.push_back ( nodeId );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  End of element.
//
///////////////////////////////////////////////////////////////////////////////

void LineStringParser::endElement (
  const XMLCh* const uri, 
  const XMLCh* const localname,
  const XMLCh* const qname )
{
  bool clearTags ( false );
  bool clearWayData ( false );

  const std::string name ( XmlTree::toNative ( localname ) );
  if ( "node" == name )
  {
    clearTags = true;
  }
  else if ( "way" == name )
  {
    clearTags = true;
    clearWayData = true;

    _lines.push_back ( LineString::create ( _currentId, _currentDate, _currentTags, _currentVertices, _currentNodeIds ) );
  }

  if ( clearTags )
  {
    _currentTags.clear();
  }

  if ( clearWayData )
  {
    _currentVertices.clear();
    _currentNodeIds.clear();
  }
}
