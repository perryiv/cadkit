
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Layers/GeoRSS/GeoRSSLayer.h"
#include "Minerva/Layers/GeoRSS/GeoRSSCallback.h"
#include "Minerva/Core/Data/DataObject.h"
#include "Minerva/Core/Data/Point.h"
#include "Minerva/Core/Utilities/Download.h"

#include "XmlTree/XercesLife.h"
#include "XmlTree/Document.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Adaptors/Bind.h"
#include "Usul/Bits/Bits.h"
#include "Usul/Convert/Convert.h"
#include "Usul/Factory/RegisterCreator.h"
#include "Usul/Interfaces/IFrameStamp.h"
#include "Usul/Jobs/Job.h"
#include "Usul/Jobs/Manager.h"
#include "Usul/Scope/Caller.h"
#include "Usul/Threads/Safe.h"

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
  _filename(),
  _href(),
  _refreshInterval ( 0.0 ),
  _lastUpdate( 0.0 ),
  _flags(),
  _color ( 1.0, 0.0, 0.0, 1.0 )
{
  this->_addMember ( "href", _href );
  this->_addMember ( "refresh_interval", _refreshInterval );
  this->_addMember ( "color", _color );
  
  this->dirtyData ( true );
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
  
  // TODO: I think a SAX parser is more appropraite here, because we can stop the parsing if there isn't new data.
  XmlTree::XercesLife life;
  XmlTree::Document::RefPtr doc ( new XmlTree::Document );
  doc->load ( filename );
  
  // TODO: Check the lastPubDate against a last update data member before proceeding.
  
  // Clear what we have.
  this->clear();
  
  // Get all the items.
  Children children ( doc->find ( "item", true ) );
  for ( Children::const_iterator iter = children.begin(); iter != children.end(); ++iter )
  {
    XmlTree::Node::ValidRefPtr node ( *iter );
    
    this->_parseItem( *node );
  }
  
  // Our data is no longer dirty.
  this->dirtyData ( false );
  
  // Our scene needs rebuilt.
  this->dirtyScene ( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Parse the item.
//
///////////////////////////////////////////////////////////////////////////////

void GeoRSSLayer::_parseItem ( const XmlTree::Node& node )
{
  // Get the color.
  Usul::Math::Vec4f color ( Usul::Threads::Safe::get ( this->mutex(), _color ) );
  
  // Make a new callback.
  GeoRSSCallback::RefPtr cb ( new GeoRSSCallback );
  
  // Get the title.
  Children titleNode ( node.find ( "title", false ) );
  const std::string title ( titleNode.empty() ? "" : titleNode.front()->value() );
  
  // Look for an image.
  Children imageNode ( node.find ( "media:content", false ) );
  if ( false == imageNode.empty() )
  {
    XmlTree::Node::RefPtr node ( imageNode.front() );
    const std::string url    ( node->attributes()["url"] );
    const std::string type   ( node->attributes()["type"] );
    const std::string width  ( node->attributes()["width"] );
    const std::string height ( node->attributes()["height"] );
    
    std::string filename;
    if ( Minerva::Core::Utilities::download ( url, filename ) )
    {
      cb->imageFilename ( filename );
      unsigned int w ( Usul::Convert::Type<std::string, unsigned int>::convert ( width ) );
      unsigned int h ( Usul::Convert::Type<std::string, unsigned int>::convert ( height ) );
      cb->imageSize ( w, h );
    }
  }
  
  // Look for the geo tag information.  TODO: Handle gml in geoRSS.
  Children latNode ( node.find ( "geo:lat", true ) );
  Children lonNode ( node.find ( "geo:long", true ) );
  
  const double lat ( latNode.empty() ? 0.0 : ToDouble::convert ( latNode.front()->value() ) );
  const double lon ( lonNode.empty() ? 0.0 : ToDouble::convert ( lonNode.front()->value() ) );
  
  Minerva::Core::Data::Point::RefPtr point ( new Minerva::Core::Data::Point );
  point->autotransform ( false );
  point->size ( 100 );
  point->primitiveId ( 2 );
  point->color ( color );
  point->point ( Usul::Math::Vec3d ( lon, lat, 0.0 ) );
  
  DataObject::RefPtr object ( new DataObject );
  object->name ( title );
  object->clickedCallback ( cb.get() );
  
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
  
  this->_updateLink ( 0x0 );
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
  osg::ref_ptr<osg::FrameStamp> frameStamp ( fs.valid () ? fs->frameStamp() : 0x0 );
  const double time ( frameStamp.valid () ? frameStamp->getReferenceTime () : 0.0 );
  
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
                                                                              Usul::Adaptors::memberFunction ( GeoRSSLayer::RefPtr ( this ), &GeoRSSLayer::_read ) ) ) );
    
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
  std::string filename;
  if ( Minerva::Core::Utilities::download ( href, filename ) )
  {
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
