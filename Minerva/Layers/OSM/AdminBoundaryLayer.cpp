
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Specialized layer for city names.
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Layers/OSM/AdminBoundaryLayer.h"
#include "Minerva/Layers/OSM/LineJob.h"

#include "Usul/Factory/RegisterCreator.h"
#include "Usul/Strings/Format.h"
#include "Usul/User/Directory.h"

using namespace Minerva::Layers::OSM;

USUL_FACTORY_REGISTER_CREATOR ( AdminBoundaryLayer );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

AdminBoundaryLayer::AdminBoundaryLayer() : BaseClass(),
  _cache ( 0x0 )
{
  const std::string filename ( Usul::Strings::format ( Usul::User::Directory::program ( true ), "admin_boundaries.db" ) );
  CadKit::Database::SQLite::Connection::RefPtr connection ( new CadKit::Database::SQLite::Connection ( filename ) );
  _cache = new Cache ( connection );

  /// Possible place values: http://wiki.openstreetmap.org/wiki/Key:boundary
  this->addRequest ( 0, Predicate ( "boundary", "administrative" ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

AdminBoundaryLayer::~AdminBoundaryLayer()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Launch a job for the predicate.
//
///////////////////////////////////////////////////////////////////////////////

AdminBoundaryLayer::JobPtr AdminBoundaryLayer::_launchJob ( 
    const Predicate& predicate, 
    const Extents& extents, 
    unsigned int level, 
    Usul::Jobs::Manager *manager, 
    Usul::Jobs::Manager *downloadManager, 
    Usul::Interfaces::IUnknown::RefPtr caller )
{
  return new LineJob ( manager, downloadManager, this->_getCache(), this->url(), extents, level, predicate );
}
