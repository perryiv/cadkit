
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Job that uses the extended API (XAPI).
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Layers/OSM/TileVectorJob.h"
#include "Minerva/Layers/OSM/Functions.h"

#include "Minerva/Core/Data/DataObject.h"
#include "Minerva/Core/Utilities/Download.h"

#include "Usul/File/Temp.h"
#include "Usul/Scope/RemoveFile.h"
#include "Usul/Strings/Format.h"

#include "boost/foreach.hpp"

using namespace Minerva::Layers::OSM;

USUL_IMPLEMENT_TYPE_ID ( TileVectorJob );
USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( TileVectorJob, TileVectorJob::BaseClass );

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

TileVectorJob::TileVectorJob ( const std::string& url, const Extents& extents, const Predicate& predicate ) : BaseClass(),
  _url ( url ),
  _extents ( extents ),
  _predicate ( predicate ),
  _data()
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

TileVectorJob::~TileVectorJob()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *TileVectorJob::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::ITileVectorJob::IID:
    return static_cast < Usul::Interfaces::ITileVectorJob * > ( this );
  default:
    return BaseClass::queryInterface ( iid );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Start the job.
//
///////////////////////////////////////////////////////////////////////////////

void TileVectorJob::_started()
{
  // Get the area of the extents.  XAPI only allows requests of a 100 square degrees.
  const double area ( ( _extents.maxLon() - _extents.minLon() ) * ( _extents.maxLat() - _extents.minLat() ) );
  if ( area > 100.0 )
    return;

  // Build the string for the request.
  const std::string predicateString ( "[" + _predicate.first + "=" + _predicate.second + "]" );
  const std::string bbox ( Usul::Strings::format ( "[bbox=", _extents.minLon(), ",", _extents.minLat(), ",", _extents.maxLon(), ",", _extents.maxLat(), "]" ) );
  const std::string request ( _url + "/api/0.5/*" + bbox + predicateString );

  // Download to a temp file.
  const std::string filename ( Usul::File::Temp::file() );
  Usul::Scope::RemoveFile remove ( filename );

  if ( Minerva::Core::Utilities::downloadToFile ( request, filename ) )
  {
    // Parse modes and ways.
    Nodes nodes;
    Ways ways;
    Minerva::Layers::OSM::parseNodesAndWays ( filename, nodes, ways );

    // Add all the nodes.
    Usul::Interfaces::IUnknown::QueryPtr allNodes ( Minerva::Layers::OSM::createForAllNodes ( nodes ) );
    _data.push_back ( allNodes );

    // Add ways.
    BOOST_FOREACH ( OSMWayPtr way, ways )
    {
      if ( true == way.valid() )
      {
        Usul::Interfaces::IUnknown::QueryPtr unknown ( Minerva::Layers::OSM::createForWay ( *way ) );
        _data.push_back ( unknown );
      }
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Cancel the job (ITileVectorJob).
//
///////////////////////////////////////////////////////////////////////////////

void TileVectorJob::cancelVectorJob()
{
  this->cancel();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the container of data (ITileVectorJob).
//
///////////////////////////////////////////////////////////////////////////////

TileVectorJob::Data TileVectorJob::getVectorData() const
{
  return _data;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is the job done (ITileVectorJob)?
//
///////////////////////////////////////////////////////////////////////////////

bool TileVectorJob::isVectorJobDone() const
{
  return this->isDone();
}
