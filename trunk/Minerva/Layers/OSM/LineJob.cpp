
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Job to download lines.
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Layers/OSM/LineJob.h"

#include "Minerva/Layers/OSM/Parser.h"

#include "Minerva/Core/Data/DataObject.h"
#include "Minerva/Core/Data/Line.h"

using namespace Minerva::Layers::OSM;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

LineJob::LineJob ( Usul::Jobs::Manager* manager, Usul::Jobs::Manager* downloadManager, Cache::RefPtr cache, const std::string& url, const Extents& extents, unsigned int level, const Predicate& predicate ) : 
  BaseClass ( manager, downloadManager, cache, url, extents, predicate )
{
  this->priority ( static_cast<int> ( level ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

LineJob::~LineJob()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make the request.  Check the cache first.
//
///////////////////////////////////////////////////////////////////////////////

void LineJob::_makeRequest()
{
  std::string cacheKey ( this->cacheKey() );
  Cache::RefPtr cache ( this->cache() );

  bool isCached ( cache.valid() && cache->hasLineData ( cacheKey, this->extents() ) );
  if ( isCached )
  {
    Lines lines;
    cache->getLineData ( cacheKey, this->extents(), lines );
    this->_buildDataObjects ( lines );

    return;
  }

  this->_startDownload ( this->_buildRequestUrl ( "way" ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  The downloading has finished.
//
///////////////////////////////////////////////////////////////////////////////

void LineJob::_downloadFinished ( const std::string& filename )
{
  // Parse modes and ways.
  Nodes nodes;
  Ways ways;
  Parser::parseNodesAndWays ( filename, nodes, ways );

  Lines lines;
  lines.reserve ( ways.size() );
  for ( Ways::const_iterator iter = ways.begin(); iter != ways.end(); ++iter )
  {
    OSMWayPtr way ( *iter );
    if ( way.valid() )
    {
      lines.push_back ( LineString::create ( way ) );
    }
  }

  Cache::RefPtr cache ( this->cache() );
  std::string cacheKey ( this->cacheKey() );
  if ( cache.valid() )
  {
    cache->addLineData ( cacheKey, this->extents(), lines );
  }

  this->_buildDataObjects ( lines );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the data objects.
//
///////////////////////////////////////////////////////////////////////////////

void LineJob::_buildDataObjects ( const Lines& lines )
{
  this->_reserveDataSize ( lines.size() );

  // Add all the nodes.
  for ( Lines::const_iterator iter = lines.begin(); iter != lines.end(); ++iter )
  {
    LineString::RefPtr line ( *iter );
    if ( line.valid() )
    {
      // Make a line.
      typedef Minerva::Core::Data::Line Line;
      typedef Minerva::Core::Data::LineStyle LineStyle;

      DataObject::RefPtr object ( new DataObject );
      object->addGeometry ( line->buildGeometry ( LineStyle::create ( Usul::Math::Vec4f ( 1.0, 1.0, 0.0, 1.0 ), 2.0 ) ) );
      object->name ( Usul::Strings::format ( line->tag<std::string> ( "ref" ) ) );

      Usul::Interfaces::IUnknown::QueryPtr unknown ( object );
      this->_addData ( unknown );
    }
  }
}
