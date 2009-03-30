
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Make a map query that uses the extended API (XAPI).
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Layers/OSM/XAPIMapQuery.h"
#include "Minerva/Layers/OSM/Parser.h"

#include "Minerva/Core/Utilities/Download.h"

#include "Usul/File/Temp.h"
#include "Usul/Scope/RemoveFile.h"
#include "Usul/Strings/Format.h"

using namespace Minerva::Layers::OSM;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

XAPIMapQuery::XAPIMapQuery ( Cache::RefPtr cache, const std::string& url, const Predicate& pred, const Extents& extents ) :
  _cache ( cache ),
  _url ( url ),
  _predicate ( pred ),
  _extents ( extents )
{
  // Check the predicate.  If a key or value is empty, use the wildcard (*).
  if ( _predicate.first.empty() )
  {
    _predicate.first = "*";
  }

  if ( _predicate.second.empty() )
  {
    _predicate.second = "*";
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

XAPIMapQuery::~XAPIMapQuery()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Gets the nodes.
//
///////////////////////////////////////////////////////////////////////////////

void XAPIMapQuery::makeNodesQuery ( Nodes& nodes )
{
  // Get the area of the extents.  XAPI only allows requests of a 100 square degrees.
  const double area ( ( _extents.maxLon() - _extents.minLon() ) * ( _extents.maxLat() - _extents.minLat() ) );
  if ( area > 100.0 )
    return;

  std::string cacheKey ( _predicate.first + "_" + _predicate.second );
  std::replace ( cacheKey.begin(), cacheKey.end(), '*', '_' );

  if ( _cache.valid() && _cache->hasNodeData ( cacheKey, _extents ) )
  {
    _cache->getNodeData ( cacheKey, _extents, nodes );
    return;
  }

  // Build the string for the request.
  const std::string request ( this->_buildRequestUrl ( "node" ) );

  // Download to a temp file.
  const std::string filename ( Usul::File::Temp::file() );
  Usul::Scope::RemoveFile remove ( filename );

  if ( Minerva::Core::Utilities::downloadToFile ( request, filename ) )
  {
    // Parse modes and ways.
    Ways ways;
    Parser::parseNodesAndWays ( filename, nodes, ways );

    if ( _cache.valid() )
    {
      _cache->addNodeData ( cacheKey, _extents, nodes );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Gets the ways.
//
///////////////////////////////////////////////////////////////////////////////

void XAPIMapQuery::makeLinesQuery ( Lines& lines )
{
  // Get the area of the extents.  XAPI only allows requests of a 100 square degrees.
  const double area ( ( _extents.maxLon() - _extents.minLon() ) * ( _extents.maxLat() - _extents.minLat() ) );
  if ( area > 100.0 )
    return;

  std::string cacheKey ( _predicate.first + "_" + _predicate.second );
  std::replace ( cacheKey.begin(), cacheKey.end(), '*', '_' );

  if ( _cache.valid() && _cache->hasLineData ( cacheKey, _extents ) )
  {
    _cache->getLineData ( cacheKey, _extents, lines );
    return;
  }

  // Build the string for the request.
  const std::string request ( this->_buildRequestUrl ( "way" ) );

  // Download to a temp file.
  const std::string filename ( Usul::File::Temp::file() );
  Usul::Scope::RemoveFile remove ( filename );
  const bool success ( Minerva::Core::Utilities::downloadToFile ( request, filename ) );

  if ( success )
  {
    // Parse modes and ways.
    Nodes nodes;
    Ways ways;
    Parser::parseNodesAndWays ( filename, nodes, ways );

    lines.reserve ( ways.size() );
    for ( Ways::const_iterator iter = ways.begin(); iter != ways.end(); ++iter )
    {
      OSMWayPtr way ( *iter );
      if ( way.valid() )
      {
        lines.push_back ( LineString::create ( way ) );
      }
    }

    if ( _cache.valid() )
    {
      _cache->addLineData ( cacheKey, _extents, lines );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the request url.
//
///////////////////////////////////////////////////////////////////////////////

std::string XAPIMapQuery::_buildRequestUrl ( const std::string& requestType ) const
{
  const std::string predicateString ( "[" + _predicate.first + "=" + _predicate.second + "]" );
  const std::string bbox ( Usul::Strings::format ( "[bbox=", _extents.minLon(), ",", _extents.minLat(), ",", _extents.maxLon(), ",", _extents.maxLat(), "]" ) );
  const std::string request ( Usul::Strings::format ( _url, "/api/0.5/", requestType, bbox, predicateString ) );
  return request;
}
