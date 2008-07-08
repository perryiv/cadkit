
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Layers/Kml/KmlLayer.h"
#include "Minerva/Layers/Kml/Download.h"
#include "Minerva/Layers/Kml/NetworkLink.h"
#include "Minerva/Layers/Kml/Feature.h"
#include "Minerva/Layers/Kml/TimeSpan.h"
#include "Minerva/Layers/Kml/LoadModel.h"
#include "Minerva/Layers/Kml/Factory.h"
#include "Minerva/Core/Data/DataObject.h"
#include "Minerva/Core/Factory/Readers.h"
#include "Minerva/Core/Data/Line.h"
#include "Minerva/Core/Data/Model.h"
#include "Minerva/Core/Data/Point.h"
#include "Minerva/Core/Data/Polygon.h"

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
#include "Usul/Predicates/FileExists.h"
#include "Usul/Scope/Caller.h"
#include "Usul/Scope/Reset.h"
#include "Usul/Strings/Case.h"
#include "Usul/Strings/Trim.h"
#include "Usul/System/Directory.h"
#include "Usul/System/Host.h"
#include "Usul/Threads/Safe.h"

#include "osg/Material"

#include "boost/algorithm/string/find.hpp"
#include "boost/filesystem/operations.hpp"
#include "Usul/File/Boost.h"

#include <sstream>

using namespace Minerva::Layers::Kml;

///////////////////////////////////////////////////////////////////////////////
//
//  Typedefs.
//
///////////////////////////////////////////////////////////////////////////////

typedef XmlTree::Node::Children    Children;
typedef Usul::Convert::Type<std::string,double> ToDouble;


///////////////////////////////////////////////////////////////////////////////
//
//  Register readers with the factory.
//
///////////////////////////////////////////////////////////////////////////////

namespace
{
  Minerva::Core::Factory::RegisterReader < Minerva::Core::Factory::TypeCreator < KmlLayer > > _creator_for_KmlLayer0 ( "Google Earth (*.kml)", "*.kml" );
  Minerva::Core::Factory::RegisterReader < Minerva::Core::Factory::TypeCreator < KmlLayer > > _creator_for_KmlLayer1 ( "Google Earth Archive (*.kmz)", "*.kmz" );
}


USUL_IMPLEMENT_IUNKNOWN_MEMBERS( KmlLayer, KmlLayer::BaseClass );
USUL_FACTORY_REGISTER_CREATOR ( KmlLayer );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

KmlLayer::KmlLayer() :
  BaseClass(),
  _filename(),
  _directory(),
  _link ( 0x0 ),
  _lastUpdate( 0.0 ),
  _flags ( 0 ),
	_styles()
{
  this->_addMember ( "filename", _filename );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

KmlLayer::KmlLayer( const XmlTree::Node& node, const std::string& filename, const std::string& directory, const Styles& styles ) :
  BaseClass(),
  _filename( filename ),
  _directory( directory ),
  _link ( 0x0 ),
  _lastUpdate( 0.0 ),
  _flags ( 0 ),
	_styles( styles )
{
  this->_addMember ( "filename", _filename );
  this->_parseFolder ( node );
  this->dirtyData ( false );
  this->dirtyScene ( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

KmlLayer::KmlLayer( Link* link, const Styles& styles ) :
  BaseClass(),
  _filename(),
  _directory(),
  _link ( link ),
  _lastUpdate ( 0.0 ),
  _flags ( 0 ),
	_styles( styles )
{
  this->_addMember ( "filename", _filename );

  // Update the link.
  this->_updateLink();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

KmlLayer::~KmlLayer()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query Interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown* KmlLayer::queryInterface ( unsigned long iid )
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

void KmlLayer::read ( Usul::Interfaces::IUnknown *caller, Usul::Interfaces::IUnknown *progress )
{
  std::string filename ( Usul::Threads::Safe::get ( this->mutex(), _filename ) );
  
  // Read.
  this->_read ( filename, caller, progress );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the file.
//
///////////////////////////////////////////////////////////////////////////////

void KmlLayer::read ( const std::string &filename, Usul::Interfaces::IUnknown *caller, Usul::Interfaces::IUnknown *progress )
{
  // Set our name to the filename if it's empty.
  const std::string name ( this->name() );
  if ( true == name.empty() )
    this->name ( filename );
  
  // Set our filename.
  Usul::Threads::Safe::set ( this->mutex(), filename, _filename );
  
  // Read.
  this->read ( caller, progress );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the file.
//
///////////////////////////////////////////////////////////////////////////////

void KmlLayer::_read ( const std::string &filename, Usul::Interfaces::IUnknown *caller, Usul::Interfaces::IUnknown *progress )
{
  // Help shorten lines.
  namespace UA = Usul::Adaptors;
  
  // Scope the reading flag.
  Usul::Scope::Caller::RefPtr scope ( Usul::Scope::makeCaller ( UA::bind1 ( true,  UA::memberFunction ( this, &KmlLayer::reading ) ), 
                                                                UA::bind1 ( false, UA::memberFunction ( this, &KmlLayer::reading ) ) ) );
  
  const std::string ext ( Usul::Strings::lowerCase ( Usul::File::extension ( filename ) ) );
  
  // Clear what we have.
  this->clear();
  
  // See if we need to unzip...
  if ( "kmz" == ext )
  {
    std::string dir ( Usul::File::Temp::directory( true ) + Usul::File::base ( filename ) + "/" );
    
#ifndef _MSC_VER
    std::string command ( "/usr/bin/unzip -o " + filename + " -d " + dir );
#else
    std::string command ( "7za.exe x -y -o\"" + dir + "\" \"" + filename + "\"" );
#endif
    ::system ( command.c_str() );
    
    // The filenames to load.
    std::vector<std::string> filenames;
    Usul::File::findFiles ( dir, "kml", filenames );
    for ( std::vector<std::string>::const_iterator iter = filenames.begin(); iter != filenames.end(); ++iter )
      Usul::Functions::safeCallR1R2R3 ( Usul::Adaptors::memberFunction ( this, &KmlLayer::_parseKml ), *iter, caller, progress, "3815332372" );
  }
  else if ( "kml" == ext )
  {
    Usul::Functions::safeCallR1R2R3 ( Usul::Adaptors::memberFunction ( this, &KmlLayer::_parseKml ), filename, caller, progress, "3910186017" );
  }
  
  // Our data is no longer dirty.
  this->dirtyData ( false );
  
  // Our scene needs rebuilt.
  this->dirtyScene ( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Load a kml file.
//
///////////////////////////////////////////////////////////////////////////////

void KmlLayer::_parseKml ( const std::string& filename, Usul::Interfaces::IUnknown *caller, Usul::Interfaces::IUnknown *progress )
{
  XmlTree::XercesLife life;

  Usul::Scope::Reset<std::string> reset ( _directory, Usul::File::directory ( filename, true ), _directory );
  
  XmlTree::Document::ValidRefPtr document ( new XmlTree::Document );
  document->load ( filename );
  
  // Loop over the children.
  Children children ( document->children() );
  for ( Children::iterator iter = children.begin(); iter != children.end(); ++iter )
  {
    XmlTree::Node::RefPtr node ( *iter );

    if ( node.valid() )
    {
      std::string name ( node->name() );
      
      // Handle folders or documents at the top level.
      if ( "Folder" == name || "Document" == name )
        this->_parseFolder( *node );
      else
        this->_parseNode( *node );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Parse a node.
//
///////////////////////////////////////////////////////////////////////////////

void KmlLayer::_parseNode ( const XmlTree::Node& node )
{
  std::string name ( node.name() );
  
  if ( "Style" == name )
  {
    this->_parseStyle( node );
  }
  // For now treat documents as folders.
  else if ( "Folder" == name || "Document" == name )
  {
    // Get the filename and directory.
    const std::string filename ( Usul::Threads::Safe::get ( this->mutex(), _filename ) );
    const std::string directory ( Usul::Threads::Safe::get ( this->mutex(), _directory ) );

    // Get the current styles map.
    Styles styles ( Usul::Threads::Safe::get ( this->mutex(), _styles ) );

    // Make a new layer.
    Minerva::Layers::Kml::KmlLayer::RefPtr layer ( new Minerva::Layers::Kml::KmlLayer ( node, filename, directory, styles ) );
    
    // Make sure the scene gets built.
    layer->dirtyScene ( true );
    
    // Add the layer to the parent.
    this->add ( Usul::Interfaces::IUnknown::QueryPtr ( layer.get() ) );
    
    this->dirtyScene ( true );
  }
  else if ( "NetworkLink" == name )
  {
    NetworkLink::RefPtr networkLink ( new NetworkLink ( node ) );
    if ( networkLink.valid() )
    {
      Link::RefPtr link ( networkLink->link() );
      if ( link.valid() )
      {
        // Get the current styles map.
        Styles styles ( Usul::Threads::Safe::get ( this->mutex(), _styles ) );

        // Make a new layer.
        KmlLayer::RefPtr layer ( new KmlLayer ( link.get(), styles ) );
        layer->name( networkLink->name() );
        layer->read ( 0x0, 0x0 );
        this->add ( Usul::Interfaces::IUnknown::QueryPtr ( layer.get() ) );
      }
    }
  }
  else if ( "Placemark" == name )
  {
    this->_parsePlacemark( node );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Parse a style.
//
///////////////////////////////////////////////////////////////////////////////

void KmlLayer::_parseStyle ( const XmlTree::Node& node )
{
	Style::RefPtr style ( new Style ( node ) );
	_styles[style->objectId()] = style;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Parse a folder.
//
///////////////////////////////////////////////////////////////////////////////

void KmlLayer::_parseFolder ( const XmlTree::Node& node )
{
  // Get the children.
  Children children ( node.children() );
  for ( Children::iterator iter = children.begin(); iter != children.end(); ++iter )
  {
    XmlTree::Node::RefPtr child ( *iter );
    
    if ( child.valid() )
    {
      if ( "name" == child->name() )
        this->name ( child->value() );
      else if ( "visibility" == child->name() )
      {
        bool visible ( "0" != child->value() );
        this->showLayer ( visible );
      }
      else
        this->_parseNode( *child );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Parse a placemark.
//
///////////////////////////////////////////////////////////////////////////////

void KmlLayer::_parsePlacemark ( const XmlTree::Node& node )
{ 
	// Make the feature.
	Feature::RefPtr feature ( new Feature ( node ) );

	// Get the style, if any.
	Style::RefPtr style ( this->_style ( feature->styleUrl() ) );

	// Look for the geometry.
  Children multiGeometry ( node.find ( "MultiGeometry", false ) );
  Children polygon ( node.find ( "Polygon", false ) );
  Children point   ( node.find ( "Point", false ) );
  Children linestring ( node.find ( "LineString", false ) );
  Children linering ( node.find ( "LineRing", false ) );
  Children model ( node.find ( "Model", false ) );

  DataObject::RefPtr object ( new DataObject );
  Geometry::RefPtr geometry ( 0x0 );
  
  // Pick which function to redirect to.
  if ( false == model.empty() )
    geometry = this->_parseModel ( *model.front(), style.get() );
  else if ( false == point.empty() )
  {
    geometry =  this->_parsePoint( *point.front(), style.get() );
    
    // Google Earth only appears to label points.
    object->label ( feature->name() );
  }
  else if ( false == polygon.empty() )
    geometry = this->_parsePolygon( *polygon.front(), style.get() );
  else if ( false == linestring.empty() )
    geometry = this->_parseLineString( *linestring.front(), style.get() );
  else if ( false == linering.empty() )
    geometry = this->_parseLineRing( *linering.front(), style.get() );
  else if ( false == multiGeometry.empty() )
    this->_parseMultiGeometry ( *multiGeometry.back(), style.get(), *object );
  
  object->addGeometry ( geometry );
  
  // Set the data object members.
  object->name ( feature->name() );
  object->labelColor ( osg::Vec4 ( 1.0, 1.0, 1.0, 1.0 ) );
  object->showLabel ( true );
  object->visibility ( feature->visiblity() );
  
  if ( TimeSpan *span = dynamic_cast<TimeSpan*> ( feature->timePrimitive() ) )
  {
    object->firstDate ( span->begin() );
    object->lastDate ( span->end() );
  }

  // Add the data object.
  this->add ( Usul::Interfaces::IUnknown::QueryPtr ( object ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Parse a point.
//
///////////////////////////////////////////////////////////////////////////////

KmlLayer::Geometry * KmlLayer::_parsePoint ( const XmlTree::Node& node, Style *style )
{
  return Factory::instance().createPoint ( node );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Parse a polygon.
//
///////////////////////////////////////////////////////////////////////////////

KmlLayer::Geometry* KmlLayer::_parsePolygon ( const XmlTree::Node& node, Style *style )
{
  // Get style properties.
	const Usul::Math::Vec4f defaultColor ( 0.8, 0.8, 0.8, 1.0 );
	const Usul::Math::Vec4f color ( 0x0 != style ? ( 0x0 != style->polystyle() ? style->polystyle()->color() : defaultColor ) : defaultColor );
  const Usul::Math::Vec4f borderColor ( 0x0 != style ? ( 0x0 != style->linestyle() ? style->linestyle()->color() : defaultColor ) : defaultColor );
  
  const float width ( 0x0 != style ? ( 0x0 != style->linestyle() ? style->linestyle()->width() : 1.0 ) : 1.0f );

  const bool fill    ( 0x0 != style ? ( 0x0 != style->polystyle() ? style->polystyle()->fill()    : true ) : true );
  const bool outline ( 0x0 != style ? ( 0x0 != style->polystyle() ? style->polystyle()->outline() : true ) : true );

  // Make the data object.
  Minerva::Core::Data::Polygon::RefPtr polygon ( Factory::instance().createPolygon ( node ) );

  if ( polygon.valid() )
  {
    polygon->color ( color );
    polygon->borderColor ( borderColor );
    polygon->width ( width );
    polygon->showBorder ( outline );
    polygon->showInterior ( fill );
  }

  return polygon.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Parse a line string.
//
///////////////////////////////////////////////////////////////////////////////

KmlLayer::Geometry* KmlLayer::_parseLineString ( const XmlTree::Node& node, Style *style )
{
  // Get style properties.
	Usul::Math::Vec4f defaultColor ( 1.0, 1.0, 1.0, 1.0 );
	Usul::Math::Vec4f color ( 0x0 != style ? ( 0x0 != style->linestyle() ? style->linestyle()->color() : defaultColor ) : defaultColor );

  const float width ( 0x0 != style ? ( 0x0 != style->linestyle() ? style->linestyle()->width() : 1.0f ) : 1.0f );

  Minerva::Core::Data::Line::RefPtr line ( Factory::instance().createLine ( node ) );
  
  if ( line.valid() )
  {
    line->width ( width );
    line->color ( color );
  }
  
  return line.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Parse a line ring.
//
///////////////////////////////////////////////////////////////////////////////

KmlLayer::Geometry* KmlLayer::_parseLineRing ( const XmlTree::Node& node, Style *style )
{
  return this->_parseLineString( node, style );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Parse a multi-geometry.
//
///////////////////////////////////////////////////////////////////////////////

void KmlLayer::_parseMultiGeometry ( const XmlTree::Node& node, Style *style, DataObject& object )
{
  Children polygon ( node.find ( "Polygon", false ) );
  for ( Children::iterator iter = polygon.begin(); iter != polygon.end(); ++iter )
  {
    object.addGeometry ( this->_parsePolygon ( *(*iter), style ) );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Parse a model.
//
///////////////////////////////////////////////////////////////////////////////

KmlLayer::Geometry* KmlLayer::_parseModel ( const XmlTree::Node& node, Style * )
{
  Minerva::Core::Data::Model::RefPtr model ( Factory::instance().createModel ( node ) );
  
  if ( model.valid() )
  {
    // Make the link
    Children links ( node.find ( "Link", true ) );
    Link::RefPtr link ( false == links.empty() ? new Link ( *links.front() ) : 0x0 );
    
    // Make the filename.
    std::string filename ( this->_buildFilename ( link ) );

    if ( false == filename.empty() )
    {
      LoadModel load;
      osg::ref_ptr<osg::Node> node ( load ( filename ) );
      if ( node.valid() )
      {
        model->model ( node.get() );
        model->toMeters ( load.toMeters() );
      }
    }
  }

  return model.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Deserialize.
//
///////////////////////////////////////////////////////////////////////////////

void KmlLayer::deserialize( const XmlTree::Node &node )
{
  BaseClass::deserialize ( node );
  this->read ( _filename );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update.
//
///////////////////////////////////////////////////////////////////////////////

void KmlLayer::updateNotify ( Usul::Interfaces::IUnknown *caller )
{
  // Call the base class first.
  BaseClass::updateNotify ( caller );
  
  // Return now if we are already downloading or reading.
  if ( true == this->isDownloading() || this->isReading() )
    return;
  
  // Guard rest of function.
  Guard guard ( this->mutex() );
  
  // Check the link to see if we need to update.
  if ( _link.valid() )
  {
    // See if it's the right kind of refresh mode.
    if ( Link::ON_INTERVAL == _link->refreshMode() )
    {
      Usul::Interfaces::IFrameStamp::QueryPtr fs ( caller );
      const double time ( fs.valid () ? fs->frameStamp()->getReferenceTime () : 0.0 );
      
      // Set the last time if it hasn't been set.
      if ( 0.0 == _lastUpdate )
        _lastUpdate = time;
      
      // Duration between last time the date was incremented.
      const double duration ( time - _lastUpdate );
      
      // If we are suppose to update...
      if ( duration > _link->refreshInterval() )
      {
        // Create a job to update the file.
        Usul::Jobs::Job::RefPtr job ( Usul::Jobs::create (  Usul::Adaptors::bind1 ( caller, 
                                                                                    Usul::Adaptors::memberFunction ( this, &KmlLayer::_updateLink ) ) ) );
        
        if ( true == job.valid() )
        {
          // Set the downloading flag now so we don't launch another job before this one starts.
          this->downloading ( true );
          
          // Add job to manager.
          Usul::Jobs::Manager::instance().addJob ( job.get() );
        }
      }
    }
  }
  
  // See if our data is dirty.
  if ( this->dirtyData() )
  {
    // Get our filename.
    std::string filename ( Usul::Threads::Safe::get ( this->mutex(), _filename ) );
                          
    // Create a job to read the file.
    Usul::Jobs::Job::RefPtr job ( Usul::Jobs::create ( Usul::Adaptors::bind3 ( filename, caller, static_cast<Usul::Interfaces::IUnknown*> ( 0x0 ),
                                                                               Usul::Adaptors::memberFunction ( this, &KmlLayer::_read ) ) ) );
    
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

void KmlLayer::_updateLink( Usul::Interfaces::IUnknown* caller )
{
  // Help shorten lines.
  namespace UA = Usul::Adaptors;
  
  // Scope the downloading flag.
  Usul::Scope::Caller::RefPtr scope ( Usul::Scope::makeCaller ( UA::bind1 ( true,  UA::memberFunction ( this, &KmlLayer::downloading ) ), 
                                                                UA::bind1 ( false, UA::memberFunction ( this, &KmlLayer::downloading ) ) ) );
  
  // Get the link.
  Link::RefPtr link ( Usul::Threads::Safe::get ( this->mutex(), _link ) );
  
  // Make the filename.
  std::string filename ( this->_buildFilename ( link ) );
  if ( false == filename.empty() )
  {
    // Check the extension.
    const std::string ext ( Usul::Strings::lowerCase ( Usul::File::extension ( filename ) ) );

    // If there is no extension, attempt to find out what the file is.
    if ( "kml" != ext && "kmz" != ext )
    {
      std::ifstream fin ( filename.c_str() );

      if ( fin.is_open() )
      {
        std::string line;
        std::getline ( fin, line );

        const std::string ext ( boost::algorithm::find_first ( line, "<?xml" ) ? ".kml" : ".kmz" );
        const std::string newFilename ( filename + ext );

        fin.close();

        Usul::File::rename ( filename.c_str(), newFilename.c_str(), true );
        filename = newFilename; 
      }
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

bool KmlLayer::isDownloading() const
{
  Guard guard ( this );
  return Usul::Bits::has<unsigned int, unsigned int> ( _flags, KmlLayer::DOWNLOADING );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set downloading flag.
//
///////////////////////////////////////////////////////////////////////////////

void KmlLayer::downloading( bool b )
{
  Guard guard ( this );
  _flags = Usul::Bits::set<unsigned int, unsigned int> ( _flags, KmlLayer::DOWNLOADING, b );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get reading flag.
//
///////////////////////////////////////////////////////////////////////////////

bool KmlLayer::isReading() const
{
  Guard guard ( this );
  return Usul::Bits::has<unsigned int, unsigned int> ( _flags, KmlLayer::READING );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set reading flag.
//
///////////////////////////////////////////////////////////////////////////////

void KmlLayer::reading( bool b )
{
  Guard guard ( this );
  _flags = Usul::Bits::set<unsigned int, unsigned int> ( _flags, KmlLayer::READING, b );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the style.
//
///////////////////////////////////////////////////////////////////////////////

Style* KmlLayer::_style ( const std::string& url )
{
	if ( true == url.empty() )
		return 0x0;

  const bool hasHttp ( boost::algorithm::find_first ( url, "http://" ) );
  const bool local ( '#' == url[0] || false == hasHttp );

	if ( local )
	{
    std::string name ( url );
    Usul::Strings::trimLeft ( name, '#' );
		Styles::const_iterator iter ( _styles.find ( name ) );
		return ( iter != _styles.end() ? iter->second.get() : 0x0 );
	}

	return 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Filename from link.  Will download if needed.
//
///////////////////////////////////////////////////////////////////////////////

std::string KmlLayer::_buildFilename ( Link *link ) const
{
  if ( 0x0 != link )
  {
    const std::string href ( link->href() );
    const bool hasHttp ( boost::algorithm::find_first ( href, "http://" ) );

    if ( true == hasHttp )
    {
      return Minerva::Layers::Kml::download ( href );
    }
    else
    {
      return Usul::Threads::Safe::get ( this->mutex(), _directory ) + href;
    }
  }

  return "";
}
