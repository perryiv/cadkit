
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
#include "Minerva/Layers/OSM/XAPIMapQuery.h"

#include "Usul/Convert/Convert.h"
#include "Usul/File/Temp.h"
#include "Usul/Scope/RemoveFile.h"
#include "Usul/Jobs/Manager.h"
#include "Usul/System/Sleep.h"
#include "Usul/Threads/Safe.h"

using namespace Minerva::Layers::OSM;

USUL_IMPLEMENT_TYPE_ID ( TileVectorJob );
USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( TileVectorJob, TileVectorJob::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

TileVectorJob::TileVectorJob ( Usul::Jobs::Manager* manager,
                               Usul::Jobs::Manager* downloadManager, 
                               Cache::RefPtr cache, 
                               const std::string& url, 
                               const Extents& extents, 
                               const Predicate& predicate ) : BaseClass(),
  _manager ( manager ),
  _downloadManager ( downloadManager ),
  _downloadJob ( 0x0 ),
  _cache ( cache ),
  _url ( url ),
  _filename(),
  _extents ( extents ),
  _predicate ( predicate ),
  _data(),
  _reAdd ( false )
{
  typedef Usul::Convert::Type<float,std::string> Converter;

  this->name ( Usul::Strings::format ( 
    "OSM Map Query: ", 
    _predicate.first, "=", _predicate.second, 
    "Extents: [", 
      Converter::convert ( extents.minimum()[0] ), ", ", 
      Converter::convert ( extents.minimum()[1] ), ", ", 
      Converter::convert ( extents.maximum()[0] ), ", ", 
      Converter::convert ( extents.maximum()[1] ), "]" ) );

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
  if ( false == _filename.empty() )
    Usul::File::remove ( _filename );
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
//  Cancel the job (ITileVectorJob).
//
///////////////////////////////////////////////////////////////////////////////

void TileVectorJob::cancelVectorJob()
{
  {
    Guard guard ( this );
    _reAdd = false;
  }

  if ( 0x0 != _manager )
  {
    _manager->removeQueuedJob ( this );
  }

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


///////////////////////////////////////////////////////////////////////////////
//
//  Add data.
//
///////////////////////////////////////////////////////////////////////////////

void TileVectorJob::_addData ( Usul::Interfaces::IUnknown* unknown )
{
  _data.push_back ( unknown );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make enough room.
//
///////////////////////////////////////////////////////////////////////////////

void TileVectorJob::_reserveDataSize ( unsigned int size )
{
  _data.reserve ( size );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Start.
//
///////////////////////////////////////////////////////////////////////////////

void TileVectorJob::_started()
{
  // Get the area of the extents.  XAPI only allows requests of a 100 square degrees.
  const double area ( ( _extents.maxLon() - _extents.minLon() ) * ( _extents.maxLat() - _extents.minLat() ) );
  if ( area > 100.0 )
    return;

  DownloadJob::RefPtr job ( Usul::Threads::Safe::get ( this->mutex(), _downloadJob ) );
  if ( false == job.valid() )
  {
    this->_makeRequest();

    // Only re-add if we have a valid download job.
    Guard guard ( this );
    if ( true == _downloadJob.valid() )
    {
      _reAdd = true;
      this->cancel();
    }
  }
  else
  {
    if ( job->isDone() )
    {
      if ( job->success() )
      {
        this->_downloadFinished ( job->filename() );
        Guard guard ( this );
        _downloadJob = 0x0;
        _reAdd = false;
      }
      else
      {
        // Job was not a success.
        {
          Guard guard ( this );
          _downloadJob = 0x0;
          _reAdd = false;
        }

        // TODO: should probably start the request again.
      }
    }
    else
    {
      Usul::System::Sleep::milliseconds ( 20 );
      // We are still waiting for a download.
      {
        Guard guard ( this );
        _reAdd = true;
      }
      this->cancel();
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Job was cancelled.  See if we should re-add.
//
///////////////////////////////////////////////////////////////////////////////

void TileVectorJob::_cancelled()
{
  Guard guard ( this );
  if ( true == _reAdd && 0x0 != _manager )
  {
    _manager->addJob ( this );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Start a download of the given url.
//
///////////////////////////////////////////////////////////////////////////////

void TileVectorJob::_startDownload ( const std::string& url )
{
  std::string filename ( Usul::File::Temp::file() );
  DownloadJob::RefPtr job ( new DownloadJob ( url, filename ) );

  Guard guard ( this );
  if ( 0x0 != _downloadManager )
  {
    _downloadManager->addJob ( Usul::Jobs::Job::RefPtr ( job ) );
    _downloadJob = job;
    _filename = filename;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the cache.
//
///////////////////////////////////////////////////////////////////////////////

Cache::RefPtr TileVectorJob::cache() const
{
  Guard guard ( this );
  return _cache;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the cache key.
//
///////////////////////////////////////////////////////////////////////////////

std::string TileVectorJob::cacheKey() const
{
  Guard guard ( this );
  std::string cacheKey ( _predicate.first + "_" + _predicate.second );
  std::replace ( cacheKey.begin(), cacheKey.end(), '*', '_' );
  return cacheKey;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the url to request from.
//
///////////////////////////////////////////////////////////////////////////////

std::string TileVectorJob::_buildRequestUrl ( const std::string& requestType ) const
{
  Guard guard ( this );
  const std::string predicateString ( "[" + _predicate.first + "=" + _predicate.second + "]" );
  const std::string bbox ( Usul::Strings::format ( "[bbox=", _extents.minLon(), ",", _extents.minLat(), ",", _extents.maxLon(), ",", _extents.maxLat(), "]" ) );
  const std::string request ( Usul::Strings::format ( _url, "/api/0.5/", requestType, bbox, predicateString ) );
  return request;
}



///////////////////////////////////////////////////////////////////////////////
//
//  Get the extents.
//
///////////////////////////////////////////////////////////////////////////////

Extents TileVectorJob::extents() const
{
  Guard guard ( this );
  return _extents;
}
