
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
                               Cache::RefPtr cache, 
                               const std::string& url, 
                               const Extents& extents, 
                               const Predicate& predicate ) : BaseClass(),
  _manager ( manager ),
  _cache ( cache ),
  _url ( url ),
  _extents ( extents ),
  _predicate ( predicate ),
  _data(),
  _reAdd ( false )
{
  typedef float FormatType;
  typedef Usul::Convert::Type<FormatType,std::string> Converter;

  this->name ( Usul::Strings::format ( 
    "OSM Map Query: ", 
    _predicate.first, "=", _predicate.second, 
    " Extents: [", 
    Converter::convert ( static_cast<FormatType> ( extents.minimum()[0] ) ), ", ", 
    Converter::convert ( static_cast<FormatType> ( extents.minimum()[1] ) ), ", ", 
    Converter::convert ( static_cast<FormatType> ( extents.maximum()[0] ) ), ", ", 
    Converter::convert ( static_cast<FormatType> ( extents.maximum()[1] ) ), "]" ) );
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
//  Get the extents.
//
///////////////////////////////////////////////////////////////////////////////

Extents TileVectorJob::extents() const
{
  Guard guard ( this );
  return _extents;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make a query object.
//
///////////////////////////////////////////////////////////////////////////////

XAPIMapQuery TileVectorJob::_makeQuery() const
{
  Guard guard ( this );
  return XAPIMapQuery ( _cache, _url, _predicate, _extents );
}
