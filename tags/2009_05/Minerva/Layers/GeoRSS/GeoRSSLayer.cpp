
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
#include "Minerva/Core/Data/TimeStamp.h"
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
#include "Usul/Functions/SafeCall.h"
#include "Usul/Interfaces/ITimerService.h"
#include "Usul/Jobs/Job.h"
#include "Usul/Jobs/Manager.h"
#include "Usul/Predicates/FileExists.h"
#include "Usul/Scope/Caller.h"
#include "Usul/Scope/RemoveFile.h"
#include "Usul/Threads/Safe.h"

#include "boost/algorithm/string/find.hpp"
#include "boost/algorithm/string/erase.hpp"
#include "boost/filesystem.hpp"
#include "boost/foreach.hpp"
#include "boost/regex.hpp"

#include <limits>

using namespace Minerva::Layers::GeoRSS;

///////////////////////////////////////////////////////////////////////////////
//
//  Typedefs.
//
///////////////////////////////////////////////////////////////////////////////

typedef XmlTree::Node::Attributes Attributes;
typedef XmlTree::Node::Children    Children;
typedef Usul::Convert::Type<std::string,double> ToDouble;


USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( GeoRSSLayer, GeoRSSLayer::BaseClass );
USUL_FACTORY_REGISTER_CREATOR ( GeoRSSLayer );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

GeoRSSLayer::GeoRSSLayer() :
  BaseClass(),
  _lastDataUpdate ( boost::posix_time::not_a_date_time ),
  _href(),
  _refreshInterval ( 300.0 ),
  _flags(),
  _color ( 1.0, 0.0, 0.0, 1.0 ),
  _timerInfo ( 0, false ),
	_filter(),
  _filteringEnabled ( false ),
  _useRegEx ( false ),
  _maximumItems ( 1000 ),
  _maximumAge ( boost::posix_time::hours ( 365 * 24 ) ) // Default of 356 days.
{
  this->_addMember ( "href", _href );
  this->_addMember ( "refresh_interval", _refreshInterval );
  this->_addMember ( "color", _color );
  this->_addMember ( "filter_element", _filter.first );
  this->_addMember ( "filter_value", _filter.second );
  this->_addMember ( "filteringEnabled", _filteringEnabled );
  this->_addMember ( "use_reg_ex", _useRegEx );
  this->_addMember ( "maximum_items", _maximumItems );
  
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
    this->name ( this->url() );

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
  boost::posix_time::ptime utcTime ( Minerva::Core::Animate::Date::createFromRSS ( date ) );

  boost::posix_time::ptime lastDataUpdate ( Usul::Threads::Safe::get ( this->mutex(), _lastDataUpdate ) );

  std::cout << "Last data update: " << lastDataUpdate << std::endl;
  std::cout << "Last feed update: " << utcTime << std::endl;

  // Check the date against the time time we updated.
  if ( false == this->dirtyData() && false == lastDataUpdate.is_not_a_date_time() && false == utcTime.is_not_a_date_time() )
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
    
    if ( this->size() >= this->maximumItems() )
      break;
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
    
    static bool test ( const std::string& expression, const std::string& name )
    {
      boost::regex e ( expression, boost::regex::perl );
      return boost::regex_match ( name, e );
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
  
  boost::posix_time::ptime date ( Minerva::Core::Animate::Date::createFromRSS ( pubDate ) );
  object->timePrimitive ( new Minerva::Core::Data::TimeStamp ( date ) );
  
  // Check the age of the item.
  boost::posix_time::time_duration maximumAge ( Usul::Threads::Safe::get ( this->mutex(), _maximumAge ) );
  const boost::posix_time::ptime now ( boost::posix_time::second_clock::universal_time() );
  if ( ( now - maximumAge ) > date )
    return;

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
  std::string description ( descriptionNode.empty() ? "" : descriptionNode.front()->value() );
  boost::algorithm::ierase_all ( description, "<br>" );
  boost::algorithm::ierase_all ( description, "<br/>" );
  boost::algorithm::ierase_all ( description, "<br />" );
  boost::algorithm::ierase_all ( description, "<p>" );
  boost::algorithm::ierase_all ( description, "</p>" );
  
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
  object->addGeometry ( point.get() );

	// See if this object should be filtered.
	bool filtered ( false );

	if ( this->filteringEnabled() )
	{
		// Get the filter.
		const Filter filter ( this->filter() );

		// Get all the children.
		Children children ( node.children() );
		for ( Children::const_iterator iter = children.begin(); iter != children.end(); ++iter )
		{
			XmlTree::Node::ValidRefPtr node ( *iter );
      const std::string name ( node->name() );
      const std::string value ( node->value() );
      
      // Is this element the one we should filter on?
			if ( filter.first == name )
      {
        if ( this->useRegEx() && Helper::RegexMatch::test ( filter.second, value ) )
          filtered = true;
        else if ( !boost::algorithm::find_first ( value, filter.second ) )
          filtered = true;
      }
		}
	}

  // Add the data object.
	if ( false == filtered )
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
  this->downloadFeed();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Serialize.
//
///////////////////////////////////////////////////////////////////////////////

void GeoRSSLayer::serialize ( XmlTree::Node &parent ) const
{
  Serialize::XML::DataMemberMap dataMemberMap ( Usul::Threads::Safe::get ( this->mutex(), _dataMemberMap ) );
  
  // Don't serialize the layers.
  dataMemberMap.erase ( "layers" );
  
  // Serialize.
  dataMemberMap.serialize ( parent );
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
  this->dirtyData ( true );
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
  this->downloadFeed();
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

void GeoRSSLayer::downloadFeed()
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


///////////////////////////////////////////////////////////////////////////////
//
//  Set the filtering enabled flag.
//
///////////////////////////////////////////////////////////////////////////////

void GeoRSSLayer::filteringEnabled ( bool b )
{
	Guard guard ( this->mutex() );
  _filteringEnabled = b;
  this->dirtyData ( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filtering enabled flag.
//
///////////////////////////////////////////////////////////////////////////////

bool GeoRSSLayer::filteringEnabled() const
{
	Guard guard ( this->mutex() );
  return _filteringEnabled;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the filter.
//
///////////////////////////////////////////////////////////////////////////////

void GeoRSSLayer::filter ( const Filter& filter )
{
	Guard guard ( this->mutex() );
	_filter = filter;
  this->dirtyData ( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filter.
//
///////////////////////////////////////////////////////////////////////////////

GeoRSSLayer::Filter GeoRSSLayer::filter() const
{
	Guard guard ( this->mutex() );
	return _filter;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the maximum number of items to show.
//
///////////////////////////////////////////////////////////////////////////////

void GeoRSSLayer::maximumItems ( unsigned int num )
{
  Guard guard ( this->mutex() );
  _maximumItems = num;
  this->dirtyData ( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the maximum number of items to show.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int GeoRSSLayer::maximumItems() const
{
  Guard guard ( this->mutex() );
  return _maximumItems;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the maximum age of items (The current time resolution is days).
//
///////////////////////////////////////////////////////////////////////////////

void GeoRSSLayer::maximumAge ( double days )
{
  Guard guard ( this->mutex() );
  _maximumAge = boost::posix_time::hours ( days * 24 );
  this->dirtyData ( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the maximum age of items (The current time resolution is days).
//
///////////////////////////////////////////////////////////////////////////////

double GeoRSSLayer::maximumAge() const
{
  Guard guard ( this->mutex() );
  return static_cast<double> ( _maximumAge.hours() ) / 24;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the use regular expression when filtering flag.
//
///////////////////////////////////////////////////////////////////////////////

void GeoRSSLayer::useRegEx ( bool b )
{
  Guard guard ( this->mutex() );
  _useRegEx = b;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the use regular expression when filtering flag.
//
///////////////////////////////////////////////////////////////////////////////

bool GeoRSSLayer::useRegEx() const
{
  Guard guard ( this->mutex() );
  return _useRegEx;
}
