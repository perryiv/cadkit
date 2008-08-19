
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Layers/GeoRSS/GeoRSSLayer.h"
#include "Minerva/Layers/GeoRSS/Item.h"
#include "Minerva/Core/Data/DataObject.h"
#include "Minerva/Core/Data/Point.h"
#include "Minerva/Core/Utilities/Download.h"
#include "Minerva/Core/Visitors/StackPoints.h"

#include "XmlTree/XercesLife.h"
#include "XmlTree/Document.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Adaptors/Bind.h"
#include "Usul/Bits/Bits.h"
#include "Usul/Components/Manager.h"
#include "Usul/Convert/Convert.h"
#include "Usul/Factory/RegisterCreator.h"
#include "Usul/File/Path.h"
#include "Usul/Interfaces/ITimerService.h"
#include "Usul/Jobs/Job.h"
#include "Usul/Jobs/Manager.h"
#include "Usul/Predicates/FileExists.h"
#include "Usul/Scope/Caller.h"
#include "Usul/Scope/RemoveFile.h"
#include "Usul/Threads/Safe.h"

#include "boost/filesystem.hpp"
#include "boost/foreach.hpp"
#include "boost/date_time/gregorian/gregorian.hpp"
#include "boost/date_time/local_time/local_time.hpp"
#include "boost/regex.hpp"

using namespace Minerva::Layers::GeoRSS;

///////////////////////////////////////////////////////////////////////////////
//
//  Typedefs.
//
///////////////////////////////////////////////////////////////////////////////

typedef XmlTree::Node::Attributes Attributes;
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
  _lastDataUpdate( boost::posix_time::not_a_date_time ),
  _href(),
  _refreshInterval ( 300.0 ),
  _flags(),
  _color ( 1.0, 0.0, 0.0, 1.0 ),
  _timerInfo ( 0, false )
{
  this->_addMember ( "href", _href );
  this->_addMember ( "refresh_interval", _refreshInterval );
  this->_addMember ( "color", _color );
  
  this->_addTimer();
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
  case Usul::Interfaces::ITimerNotify::IID:
    return static_cast < Usul::Interfaces::ITimerNotify* > ( this );
  default:
    return BaseClass::queryInterface ( iid );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Parse the date.  Time is returned in UTC.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  boost::posix_time::ptime parseDate ( const std::string& sDate )
  {
    if ( false == sDate.empty() && sDate.size() > 27 )
    {
      const std::string dayOfWeek ( sDate, 0, 3 );
      const std::string day ( sDate, 5, 2 );
      const std::string month ( sDate, 8, 3 );
      const std::string year ( sDate, 12, 4 );
      const std::string hours ( sDate, 17, 2 );
      const std::string minutes ( sDate, 20, 2 );
      const std::string seconds ( sDate, 23, 2 );
      const std::string zone ( sDate, 26 ); // Get the remaining characters.
      
      // The timezone.
      boost::local_time::time_zone_ptr timeZone;
      
      typedef Usul::Convert::Type<std::string,int> ToInt;
      
      // See if the zone is an offset.
      if ( false == zone.empty() && ( '-' == zone[0] || '+' == zone[1] ) )
      {
        int offset ( ToInt::convert ( zone ) );
        int hourOffset ( offset / 100 );
        
        // Offset from UTC.
        boost::posix_time::time_duration utcOffset ( hourOffset, 0, 0 );
        
        // Daylight savings offsets.  TODO: Find out if the RSS feed will have accounted for dst.
        boost::local_time::dst_adjustment_offsets dstOffsets ( boost::posix_time::time_duration ( 0, 0, 0 ),
                                                               boost::posix_time::time_duration ( 0, 0, 0 ),
                                                               boost::posix_time::time_duration ( 0, 0, 0 ) );
        
        boost::shared_ptr<boost::local_time::dst_calc_rule> rules;
        boost::local_time::time_zone_names names ( "", "", "", "" );
        
        timeZone = boost::local_time::time_zone_ptr ( new boost::local_time::custom_time_zone ( names, utcOffset, dstOffsets, rules ) );
      }
      
      boost::posix_time::time_duration time ( ToInt::convert ( hours ), ToInt::convert ( minutes ), ToInt::convert ( seconds ) );
      boost::gregorian::date date ( boost::gregorian::from_simple_string ( year + "-" + month + "-" + day ) );
      boost::posix_time::ptime lastUpdate ( date, time );
      
      boost::posix_time::ptime utcTime ( boost::local_time::local_date_time ( date, time, timeZone, true ).utc_time() );
      return utcTime;
    }
    
    return boost::posix_time::not_a_date_time;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the file.
//
///////////////////////////////////////////////////////////////////////////////

void GeoRSSLayer::_read ( const std::string &filename, Usul::Interfaces::IUnknown *caller, Usul::Interfaces::IUnknown *progress )
{
  // Make sure the file exists before reading.
  if ( false == Usul::Predicates::FileExists::test ( filename ) )
    return;

  // Set the name.
  if ( true == this->name().empty() )
    this->name ( filename );

  // Help shorten lines.
  namespace UA = Usul::Adaptors;

  // Scope the reading flag.
  Usul::Scope::Caller::RefPtr scope ( Usul::Scope::makeCaller ( UA::bind1 ( true,  UA::memberFunction ( this, &GeoRSSLayer::reading ) ), 
                                                                UA::bind1 ( false, UA::memberFunction ( this, &GeoRSSLayer::reading ) ) ) );

  // TODO: I think a SAX parser is more appropraite here, because we can stop the parsing if there isn't new data.
  XmlTree::XercesLife life;
  XmlTree::Document::RefPtr doc ( new XmlTree::Document );
  doc->load ( filename );

  // Get the date the stream was modified.
  Children lastPubDateNode ( doc->find ( "lastBuildDate", true ) );
  const std::string date ( false == lastPubDateNode.empty() ? lastPubDateNode.front()->value() : "" );
  boost::posix_time::ptime utcTime ( Detail::parseDate ( date ) );

  boost::posix_time::ptime lastDataUpdate ( Usul::Threads::Safe::get ( this->mutex(), _lastDataUpdate ) );

  std::cout << "Last data update: " << lastDataUpdate << std::endl;
  std::cout << "Last feed update: " << utcTime << std::endl;

  // Check the date against the time time we updated.
  if ( false == lastDataUpdate.is_not_a_date_time() && false == utcTime.is_not_a_date_time() )
  {
    // Return now if the feed has not been updated.
    if ( lastDataUpdate >= utcTime )
    {
      // Our data doesn't need to be updated.
      this->dirtyData ( false );
      return;
    }
  }

  // Clear what we have.
  this->clear();

  unsigned int i ( 0 );

  // Get all the items.
  Children children ( doc->find ( "item", true ) );
  BOOST_FOREACH ( XmlTree::Node::ValidRefPtr node, children )
  {
    std::cout << "Adding item " << ++i << " of " << children.size() << std::endl;
    this->_parseItem( *node );
  }

  // Stack the points.
  Minerva::Core::Visitors::StackPoints::RefPtr stack ( new Minerva::Core::Visitors::StackPoints );
  this->accept ( *stack );

  // Our data is no longer dirty.
  this->dirtyData ( false );

  // Our scene needs rebuilt.
  this->dirtyScene ( true );

  // Update last time.
  const boost::posix_time::ptime now ( boost::posix_time::second_clock::universal_time() );
  Usul::Threads::Safe::set ( this->mutex(), now, _lastDataUpdate );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper struct to match with regular expression.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  struct RegexMatch
  {
    RegexMatch ( const std::string& expression ) : _e ( expression )
    {
    }
    
    bool operator() ( const std::string& name ) const
    {
      return boost::regex_match ( name, _e );
    }
    
  private:
    boost::regex _e;
  };
}


///////////////////////////////////////////////////////////////////////////////
//
//  Parse the item.
//
///////////////////////////////////////////////////////////////////////////////

void GeoRSSLayer::_parseItem ( const XmlTree::Node& node )
{
  Item::RefPtr object ( new Item );
  
  // Get the color.
  Usul::Math::Vec4f color ( Usul::Threads::Safe::get ( this->mutex(), _color ) );

  // Get the title.
  Children titleNode ( node.find ( "title", false ) );
  const std::string title ( titleNode.empty() ? "" : titleNode.front()->value() );

  // Look for the publication date.
  Children pubDateNode ( node.find ( "pubDate", false ) );
  const std::string pubDate ( pubDateNode.empty() ? "" : pubDateNode.front()->value() );
  object->date ( pubDate );

  // Look for an image.
  Children imageNode ( node.find ( "media:content", true ) );
  //Children imageNode ( node.find ( "media:thumbnail", false ) );
  if ( false == imageNode.empty() )
  {
    XmlTree::Node::RefPtr node ( imageNode.front() );
    const std::string url    ( node->attributes()["url"] );
    const std::string type   ( node->attributes()["type"] );
    const std::string width  ( node->attributes()["width"] );
    const std::string height ( node->attributes()["height"] );
    
    // TODO: Download the thumbnail here and download the larger image when clicked on.
    std::string filename;
    if ( Minerva::Core::Utilities::download ( url, filename, true ) )
    {
      // Determine file extension.  Make it jpg by default.
      std::string ext ( ( type.size() > 6 && '/' == type.at(5) ) ? std::string ( type.begin() + 6, type.end() ) : "jpg" );
      ext = ( "jpeg" == ext ? "jpg" : ext );
      
      if ( ext != Usul::File::extension ( filename ) )
      {
        // The new filename.
        const std::string newName ( filename + "." + ext );
        
        // Move the file.
        boost::filesystem::remove ( newName );
        boost::filesystem::copy_file ( filename, newName );
        //boost::filesystem::remove ( filename );
        
        // Set the new filename.
        filename = newName;
      }
      
      object->imageFilename ( filename );
      unsigned int w ( width.empty() ? 256 : Usul::Convert::Type<std::string, unsigned int>::convert ( width ) );
      unsigned int h ( height.empty() ? 256 : Usul::Convert::Type<std::string, unsigned int>::convert ( height ) );
      object->imageSize ( w, h );
    }
  }
  
  // Look for the geo tag information.  TODO: Handle gml in geoRSS.
  Children latNode ( node.findIf ( true, Helper::RegexMatch ( "geo:(lat|latitude)" ) ) );
  Children lonNode ( node.findIf ( true, Helper::RegexMatch ( "geo:(long|longitude)" ) ) );

  const double lat ( latNode.empty() ? 0.0 : ToDouble::convert ( latNode.front()->value() ) );
  const double lon ( lonNode.empty() ? 0.0 : ToDouble::convert ( lonNode.front()->value() ) );

  // Look for a description.
  Children descriptionNode ( node.find ( "media:description", true ) );
  const std::string description ( descriptionNode.empty() ? "" : descriptionNode.front()->value() );
  
  // Look for the categories.
  Children categoryNodes ( node.find ( "category", true ) );
  Item::Categories categories;
  for ( Children::const_iterator iter = categoryNodes.begin(); iter != categoryNodes.end(); ++iter )
  {
    categories.push_back ( (*iter)->value() );
  }
  object->categories ( categories );

  Minerva::Core::Data::Point::RefPtr point ( new Minerva::Core::Data::Point );
  point->autotransform ( false );
  point->size ( 100 );
  point->primitiveId ( 2 );
  point->color ( color );
  point->point ( Usul::Math::Vec3d ( lon, lat, 0.0 ) );

  object->name ( title );
  object->description ( description );

  // Add the geometry.
  object->addGeometry ( point );

  // Add the data object.
  this->add ( Usul::Interfaces::IUnknown::QueryPtr ( object ) );

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
  
  // Add the timer because the refresh interval may be different.
  this->_addTimer();
  
  // Start download.
  this->_downloadFeed();
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
  
  // Return now if we don't have a valid href to use.
  if ( true == href.empty() )
    return;
  
  std::cout << "Downloading " << href << std::endl;
  std::string filename;
  if ( Minerva::Core::Utilities::download ( href, filename ) )
  {
    Usul::Scope::RemoveFile remove ( filename );
    
    std::cout << "Downloading finished" << std::endl;
    
    // Our data is dirty.
    this->dirtyData ( true );
    
    // Read the file.
    this->_read ( filename, 0x0, 0x0 );
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


///////////////////////////////////////////////////////////////////////////////
//
//  Set the url.
//
///////////////////////////////////////////////////////////////////////////////

void GeoRSSLayer::url ( const std::string& s )
{
  Guard guard ( this->mutex() );
  _href = s;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the url.
//
///////////////////////////////////////////////////////////////////////////////

std::string GeoRSSLayer::url() const
{
  Guard guard ( this->mutex() );
  return _href;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the refresh rate (in seconds).
//
///////////////////////////////////////////////////////////////////////////////

void GeoRSSLayer::refreshRate ( double seconds )
{
  Guard guard ( this->mutex() );
  _refreshInterval = seconds;
  this->_addTimer(); 
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the refresh rate (in seconds).
//
///////////////////////////////////////////////////////////////////////////////

double GeoRSSLayer::refreshRate() const
{
  Guard guard ( this->mutex() );
  return _refreshInterval;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the color.
//
///////////////////////////////////////////////////////////////////////////////

void GeoRSSLayer::color ( const Usul::Math::Vec4f& color )
{
  Guard guard ( this->mutex() );
  _color = color;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the color.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Math::Vec4f GeoRSSLayer::color() const
{
  Guard guard ( this->mutex() );
  return _color;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the timer fires.
//
///////////////////////////////////////////////////////////////////////////////

void GeoRSSLayer::timerNotify ( TimerID )
{
  this->_downloadFeed();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a timer callback.
//
///////////////////////////////////////////////////////////////////////////////

void GeoRSSLayer::_addTimer()
{
  typedef Usul::Interfaces::ITimerService ITimerService;
  typedef Usul::Components::Manager PluginManager;
  
  ITimerService::QueryPtr service ( PluginManager::instance().getInterface ( ITimerService::IID ) );
  
  if ( service.valid() )
  {
    // Remove the one we have if it's valid.
    if ( true == _timerInfo.second )
      service->timerRemove ( _timerInfo.first );
    
    // Make a new timer.  The timer expects the timeout to be in milliseconds.
    Usul::Interfaces::IUnknown::RefPtr me ( this->queryInterface ( Usul::Interfaces::IUnknown::IID ) );
    _timerInfo.first = service->timerAdd ( Usul::Threads::Safe::get ( this->mutex(), _refreshInterval ) * 1000, me );
    _timerInfo.second = true;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Launch a job to download feed.
//
///////////////////////////////////////////////////////////////////////////////

void GeoRSSLayer::_downloadFeed()
{
  // Return now if we are already downloading.
  if ( true == this->isDownloading() )
    return;
  
  // Create a job to download the data.
  Usul::Jobs::Job::RefPtr job ( Usul::Jobs::create (  Usul::Adaptors::bind1 ( static_cast<Usul::Interfaces::IUnknown*> ( 0x0 ), 
                                                                             Usul::Adaptors::memberFunction ( GeoRSSLayer::RefPtr ( this ), &GeoRSSLayer::_updateLink ) ) ) );
  
  if ( true == job.valid() )
  {
    // Set the downloading flag now so we don't launch another job before this one starts.
    this->downloading ( true );
    
    // Add job to manager.
    Usul::Jobs::Manager::instance().addJob ( job.get() );
  }
}
