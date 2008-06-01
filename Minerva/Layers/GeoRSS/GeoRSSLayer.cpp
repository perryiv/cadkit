
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#endif

#include "Minerva/Layers/GeoRSS/GeoRSSLayer.h"

#include "Minerva/Core/Factory/Readers.h"
#include "Minerva/Core/Geometry/Point.h"
#include "Minerva/Core/Geometry/Line.h"
#include "Minerva/Core/Geometry/Polygon.h"

#include "XmlTree/XercesLife.h"
#include "XmlTree/Document.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Adaptors/Bind.h"
#include "Usul/Bits/Bits.h"
#include "Usul/Convert/Convert.h"
#include "Usul/Factory/RegisterCreator.h"
#include "Usul/File/Path.h"
#include "Usul/File/Remove.h"
#include "Usul/File/Rename.h"
#include "Usul/File/Temp.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Interfaces/IFrameStamp.h"
#include "Usul/Jobs/Job.h"
#include "Usul/Jobs/Manager.h"
#include "Usul/Network/Curl.h"
#include "Usul/Predicates/FileExists.h"
#include "Usul/Scope/Caller.h"
#include "Usul/Scope/Reset.h"
#include "Usul/Strings/Case.h"
#include "Usul/Strings/Split.h"
#include "Usul/System/Directory.h"
#include "Usul/System/Host.h"
#include "Usul/Threads/Safe.h"

#include "OsgTools/Visitor.h"
#include "OsgTools/State/StateSet.h"

#include "osg/Material"

#include "osgDB/ReadFile"

#include "boost/algorithm/string/find.hpp"
#include "boost/algorithm/string/replace.hpp"
#include "boost/filesystem/operations.hpp"
#include "Usul/File/Boost.h"

#include <sstream>

using namespace Minerva::Layers::GeoRSS;

///////////////////////////////////////////////////////////////////////////////
//
//  Typedefs.
//
///////////////////////////////////////////////////////////////////////////////

typedef XmlTree::Node::Children    Children;
typedef Usul::Convert::Type<std::string,double> ToDouble;


USUL_IMPLEMENT_IUNKNOWN_MEMBERS( GeoRSSLayer, GeoRSSLayer::BaseClass );
USUL_FACTORY_REGISTER_CREATOR ( GeoRSSLayer );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

GeoRSSLayer::GeoRSSLayer() :
  BaseClass(),
  _filename(),
  _href(),
  _refreshInterval ( 0.0 ),
  _lastUpdate( 0.0 ),
  _flags ( 0 )
{
  this->_addMember ( "href", _href );
  this->_addMember ( "refresh_interval", _refreshInterval );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

GeoRSSLayer::~GeoRSSLayer()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query Interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown* GeoRSSLayer::queryInterface ( unsigned long iid )
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

void GeoRSSLayer::read ( const std::string &filename, Usul::Interfaces::IUnknown *caller, Usul::Interfaces::IUnknown *progress )
{
  this->name ( filename );
  
  // Set our filename.
  Usul::Threads::Safe::set ( this->mutex(), filename, _filename );
  
  // Read.
  this->_read ( filename, caller, progress );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the file.
//
///////////////////////////////////////////////////////////////////////////////

void GeoRSSLayer::_read ( const std::string &filename, Usul::Interfaces::IUnknown *caller, Usul::Interfaces::IUnknown *progress )
{
  // Help shorten lines.
  namespace UA = Usul::Adaptors;
  
  // Scope the reading flag.
  Usul::Scope::Caller::RefPtr scope ( Usul::Scope::makeCaller ( UA::bind1 ( true,  UA::memberFunction ( this, &GeoRSSLayer::reading ) ), 
                                                                UA::bind1 ( false, UA::memberFunction ( this, &GeoRSSLayer::reading ) ) ) );
  
  // Our data is no longer dirty.
  this->dirtyData ( false );
  
  // Our scene needs rebuilt.
  this->dirtyScene ( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Deserialize.
//
///////////////////////////////////////////////////////////////////////////////

void GeoRSSLayer::deserialize( const XmlTree::Node &node )
{
  BaseClass::deserialize ( node );
  

}


///////////////////////////////////////////////////////////////////////////////
//
//  Update.
//
///////////////////////////////////////////////////////////////////////////////

void GeoRSSLayer::updateNotify ( Usul::Interfaces::IUnknown *caller )
{
  // Call the base class first.
  BaseClass::updateNotify ( caller );
  
  // Return now if we are already downloading or reading.
  if ( true == this->isDownloading() || this->isReading() )
    return;

  Usul::Interfaces::IFrameStamp::QueryPtr fs ( caller );
  const double time ( fs.valid () ? fs->frameStamp()->getReferenceTime () : 0.0 );
  
  // Get variables needed below.
  const double refreshInterval ( Usul::Threads::Safe::get ( this->mutex(), _refreshInterval ) );
  double lastUpdate ( Usul::Threads::Safe::get ( this->mutex(), _lastUpdate ) );

  // Set the last time if it hasn't been set.
  if ( 0.0 == lastUpdate )
  {
    Usul::Threads::Safe::set ( this->mutex(), time, _lastUpdate );
    lastUpdate = time;
  }
  
  // Duration between last time the date was incremented.
  const double duration ( time - lastUpdate );
  
  // If we are suppose to update...
  if ( duration > refreshInterval )
  {
    // Create a job to update the file.
    Usul::Jobs::Job::RefPtr job ( Usul::Jobs::create (  Usul::Adaptors::bind1 ( caller, 
                                                                                Usul::Adaptors::memberFunction ( this, &GeoRSSLayer::_updateLink ) ) ) );

    if ( true == job.valid() )
    {
      // Set the downloading flag now so we don't launch another job before this one starts.
      this->downloading ( true );
      
      // Add job to manager.
      Usul::Jobs::Manager::instance().addJob ( job.get() );
    }
  }
  
  // See if our data is dirty.
  if ( this->dirtyData() )
  {
    // Get our filename.
    std::string filename ( Usul::Threads::Safe::get ( this->mutex(), _filename ) );
                          
    // Create a job to read the file.
    Usul::Jobs::Job::RefPtr job ( Usul::Jobs::create ( Usul::Adaptors::bind3 ( filename, caller, static_cast<Usul::Interfaces::IUnknown*> ( 0x0 ),
                                                                               Usul::Adaptors::memberFunction ( this, &GeoRSSLayer::_read ) ) ) );
    
    if ( true == job.valid() )
    {
      // Set the reading flag now so we don't launch another job before this one starts.
      this->reading ( true );
      
      // Add job to manager.
      Usul::Jobs::Manager::instance().addJob ( job.get() );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update link.
//
///////////////////////////////////////////////////////////////////////////////

void GeoRSSLayer::_updateLink( Usul::Interfaces::IUnknown* caller )
{
  // Help shorten lines.
  namespace UA = Usul::Adaptors;
  
  // Scope the downloading flag.
  Usul::Scope::Caller::RefPtr scope ( Usul::Scope::makeCaller ( UA::bind1 ( true,  UA::memberFunction ( this, &GeoRSSLayer::downloading ) ), 
                                                                UA::bind1 ( false, UA::memberFunction ( this, &GeoRSSLayer::downloading ) ) ) );
  
  // Get the link.
  const std::string href ( Usul::Threads::Safe::get ( this->mutex(), _href ) );
  
  std::string::size_type pos ( href.rfind ( '/' ) );
  
  if ( pos != std::string::npos )
  {
    // Build the filename.
    std::string filename ( href.begin() + pos + 1, href.end() );
    filename = Usul::File::Temp::directory ( true ) + filename;

    // Replace illegal characters for filename.
    boost::algorithm::replace_all ( filename, "?", "_" );
    boost::algorithm::replace_all ( filename, "=", "_" );
    
    // Download.
    {
		  boost::filesystem::remove ( filename );
      Usul::Network::Curl curl ( href, filename );
      Usul::Functions::safeCallV1V2 ( Usul::Adaptors::memberFunction ( &curl, &Usul::Network::Curl::download ), static_cast<std::ostream*> ( 0x0 ), "", "4264015553" );
    }

    // Set the filename.
    Usul::Threads::Safe::set ( this->mutex(), filename, _filename );
    
    // Get the current time.
    Usul::Interfaces::IFrameStamp::QueryPtr fs ( caller );
    const double time ( fs.valid () ? fs->frameStamp()->getReferenceTime () : 0.0 );
    
    // Set the last update time.
    Usul::Threads::Safe::set ( this->mutex(), time, _lastUpdate );
    
    // Our data is dirty.
    this->dirtyData ( true );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get downloading flag.
//
///////////////////////////////////////////////////////////////////////////////

bool GeoRSSLayer::isDownloading() const
{
  Guard guard ( this );
  return Usul::Bits::has<unsigned int, unsigned int> ( _flags, GeoRSSLayer::DOWNLOADING );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set downloading flag.
//
///////////////////////////////////////////////////////////////////////////////

void GeoRSSLayer::downloading( bool b )
{
  Guard guard ( this );
  _flags = Usul::Bits::set<unsigned int, unsigned int> ( _flags, GeoRSSLayer::DOWNLOADING, b );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get reading flag.
//
///////////////////////////////////////////////////////////////////////////////

bool GeoRSSLayer::isReading() const
{
  Guard guard ( this );
  return Usul::Bits::has<unsigned int, unsigned int> ( _flags, GeoRSSLayer::READING );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set reading flag.
//
///////////////////////////////////////////////////////////////////////////////

void GeoRSSLayer::reading( bool b )
{
  Guard guard ( this );
  _flags = Usul::Bits::set<unsigned int, unsigned int> ( _flags, GeoRSSLayer::READING, b );
}
