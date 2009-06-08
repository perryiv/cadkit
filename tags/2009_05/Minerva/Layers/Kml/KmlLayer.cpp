
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Layers/Kml/KmlLayer.h"
#include "Minerva/Layers/Kml/NetworkLink.h"
#include "Minerva/Layers/Kml/LoadModel.h"
#include "Minerva/Layers/Kml/Factory.h"
#include "Minerva/Layers/Kml/ZipFile.h"
#include "Minerva/Core/Data/DataObject.h"
#include "Minerva/Core/Factory/Readers.h"
#include "Minerva/Core/Data/Line.h"
#include "Minerva/Core/Data/Model.h"
#include "Minerva/Core/Data/Point.h"
#include "Minerva/Core/Data/Polygon.h"
#include "Minerva/Core/Data/ModelCache.h"
#include "Minerva/Core/Utilities/Download.h"

#include "XmlTree/XercesLife.h"
#include "XmlTree/Document.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Adaptors/Bind.h"
#include "Usul/Bits/Bits.h"
#include "Usul/Components/Manager.h"
#include "Usul/Convert/Convert.h"
#include "Usul/Factory/RegisterCreator.h"
#include "Usul/File/Path.h"
#include "Usul/File/Remove.h"
#include "Usul/File/Rename.h"
#include "Usul/File/Temp.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Interfaces/ITimerService.h"
#include "Usul/Jobs/Job.h"
#include "Usul/Jobs/Manager.h"
#include "Usul/Predicates/FileExists.h"
#include "Usul/Resources/TextWindow.h"
#include "Usul/Scope/Caller.h"
#include "Usul/Scope/Reset.h"
#include "Usul/Strings/Case.h"
#include "Usul/Strings/Trim.h"
#include "Usul/System/Clock.h"
#include "Usul/Threads/Safe.h"

#include "boost/algorithm/string/find.hpp"
#include "boost/algorithm/string/replace.hpp"
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
	_styles(),
  _modelCache ( new ModelCache, true ),
  _timerId ( 0 )
{
  this->_addMember ( "filename", _filename );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

KmlLayer::KmlLayer( const std::string& filename, const std::string& directory, const Styles& styles, ModelCache* cache ) :
  BaseClass(),
  _filename( filename ),
  _directory( directory ),
  _link ( 0x0 ),
  _lastUpdate( 0.0 ),
  _flags ( 0 ),
	_styles ( styles ),
  _modelCache ( cache, false ),
  _timerId ( 0 )
{
  this->_addMember ( "filename", _filename );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

KmlLayer::KmlLayer ( Link* link, const Styles& styles, ModelCache* cache ) :
  BaseClass(),
  _filename(),
  _directory(),
  _link ( link ),
  _lastUpdate ( 0.0 ),
  _flags ( 0 ),
  _styles ( styles ),
  _modelCache ( cache, false ),
  _timerId ( 0 )
{
  this->_addMember ( "filename", _filename );
  
  // Add the timer for updating link.
  this->_addTimer();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create a kml from a node.
//
///////////////////////////////////////////////////////////////////////////////

KmlLayer* KmlLayer::create ( const XmlTree::Node& node, const std::string& filename, const std::string& directory, const Styles& styles, ModelCache* cache )
{
  KmlLayer::RefPtr kml ( new KmlLayer ( filename, directory, styles, cache ) );
  kml->parseFolder ( node );
  kml->dirtyData ( false );
  kml->dirtyScene ( true );
  return kml.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create a kml layer a link.
//
///////////////////////////////////////////////////////////////////////////////

KmlLayer* KmlLayer::create ( Link* link, const Styles& styles, ModelCache* cache )
{
  KmlLayer::RefPtr kml ( new KmlLayer ( link, styles, cache ) );
  
  // Update the link.
  kml->_updateLink();
  
  return kml.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

KmlLayer::~KmlLayer()
{
  // Delete the model cache if we own it.
  if ( _modelCache.second )
    delete _modelCache.first;
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
  case Minerva::Interfaces::IRefreshData::IID:
    return static_cast < Minerva::Interfaces::IRefreshData* > ( this );
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

void KmlLayer::read ( Usul::Interfaces::IUnknown *caller, Usul::Interfaces::IUnknown *progress )
{
  // Initialize start time.
  const Usul::Types::Uint64 start ( Usul::System::Clock::milliseconds() );

  // Set our internal filename.
  const std::string filename ( Usul::Threads::Safe::get ( this->mutex(), _filename ) );
  
  // Read.
  this->_read ( filename, caller, progress );

  // Feedback.
  const double seconds ( static_cast < double > ( Usul::System::Clock::milliseconds() - start ) * 0.001 );
  std::cout << Usul::Strings::format ( seconds, " seconds ... Time to open ", filename ) << Usul::Resources::TextWindow::endl;
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
    boost::algorithm::replace_all ( dir, " ", "_" );
    
#ifndef _MSC_VER
    std::string name ( filename );
    boost::algorithm::replace_all ( name, " ", "\\ " );
    
    std::string command ( "/usr/bin/unzip -o " + name + " -d " + dir );
    ::system ( command.c_str() );

    // The filenames to load.
    std::vector<std::string> filenames;
    Usul::File::findFiles ( dir, "kml", filenames );
    for ( std::vector<std::string>::const_iterator iter = filenames.begin(); iter != filenames.end(); ++iter )
    {
      USUL_TRY_BLOCK
      {
        this->_parseKml ( *iter, caller, progress );
      }
      USUL_DEFINE_SAFE_CALL_CATCH_BLOCKS ( "3815332372" );
    }
#else
    // Make sure the directory is created.
    if ( false == boost::filesystem::exists ( dir ) )
      boost::filesystem::create_directory ( dir );

    // Open the zip file.
    ZipFile zipFile;
    zipFile.open ( filename );

    // Get the all the filenames contained in the file.
    typedef ZipFile::Strings Strings;
    Strings contents;
    zipFile.contents ( contents );

    typedef std::vector<std::string> FileContents;
    FileContents kmlFiles;

    // Loop over the contents and write them to a temporary file.
    for ( Strings::const_iterator iter = contents.begin(); iter != contents.end(); ++iter )
    {
      const std::string filename ( *iter );

      // Read the file into a buffer.
      std::string buffer;
      zipFile.readFile ( filename, buffer );

      if ( "kml" == Usul::File::extension ( filename ) )
      {
        kmlFiles.push_back ( buffer );
      }
      else
      {
        // Create the path.
        const std::string path ( dir + "\\" + filename );

        // Make sure the directory is created.
        boost::filesystem::create_directory ( Usul::File::directory ( path, true ) );

        // Write the buffer to file.
        std::ofstream out ( path.c_str(), std::ios::binary );
        out << buffer;
      }
    }

    // Set the directory to where the .kmz was expanded to.
    Usul::Scope::Reset<std::string> reset ( _directory, dir, _directory );

    XmlTree::XercesLife life;
    for ( FileContents::const_iterator iter = kmlFiles.begin(); iter != kmlFiles.end(); ++iter )
    {
      XmlTree::Document::ValidRefPtr document ( new XmlTree::Document );
      document->loadFromMemory ( *iter );
  
      USUL_TRY_BLOCK
      {
        this->_parseKml ( *document, caller, progress );
      }
      USUL_DEFINE_SAFE_CALL_CATCH_BLOCKS ( "2567846007" );
    }

#endif
  }
  else if ( "kml" == ext )
  {
    USUL_TRY_BLOCK
    {
      this->_parseKml ( filename, caller, progress );
    }
    USUL_DEFINE_SAFE_CALL_CATCH_BLOCKS ( "3910186017" );
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
  
  this->_parseKml ( *document, caller, progress );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Load a kml file using the node.
//
///////////////////////////////////////////////////////////////////////////////

void KmlLayer::_parseKml ( const XmlTree::Node& node, Usul::Interfaces::IUnknown *caller, Usul::Interfaces::IUnknown *progress )
{
  // Loop over the children.
  Children children ( node.children() );
  for ( Children::iterator iter = children.begin(); iter != children.end(); ++iter )
  {
    XmlTree::Node::RefPtr node ( *iter );

    if ( node.valid() )
    {
      std::string name ( node->name() );
      
      // Handle folders or documents at the top level.
      if ( "Folder" == name || "Document" == name )
        this->parseFolder( *node );
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
    Minerva::Layers::Kml::KmlLayer::RefPtr layer ( KmlLayer::create ( node, filename, directory, styles, this->modelCache() ) );
    
    // Make sure the scene gets built.
    layer->dirtyScene ( true );
    
    // Add the layer to the parent.
    this->add ( Usul::Interfaces::IUnknown::QueryPtr ( layer.get() ) );
    
    this->dirtyScene ( true );
  }
  else if ( "NetworkLink" == name )
  {
    NetworkLink::RefPtr networkLink ( Factory::instance().createNetworkLink ( node ) );
    if ( networkLink.valid() )
    {
      Link::RefPtr link ( networkLink->link() );
      if ( link.valid() )
      {
        // Get the current styles map.
        Styles styles ( Usul::Threads::Safe::get ( this->mutex(), _styles ) );

        // Make a new layer.
        KmlLayer::RefPtr layer ( KmlLayer::create ( link.get(), styles, this->modelCache() ) );
        layer->name ( networkLink->name() );
        layer->read ( 0x0, 0x0 );
        this->add ( Usul::Interfaces::IUnknown::QueryPtr ( layer.get() ) );
      }
    }
  }
  else if ( "Placemark" == name )
  {
    this->_parsePlacemark ( node );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Parse a style.
//
///////////////////////////////////////////////////////////////////////////////

void KmlLayer::_parseStyle ( const XmlTree::Node& node )
{
	Style::RefPtr style ( Factory::instance().createStyle ( node ) );
  
  Guard guard ( this->mutex() );
	_styles[style->objectId()] = style;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Parse a folder.
//
///////////////////////////////////////////////////////////////////////////////

void KmlLayer::parseFolder ( const XmlTree::Node& node )
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
	// Look for the geometry.
  Children multiGeometry ( node.find ( "MultiGeometry", false ) );
  Children polygon ( node.find ( "Polygon", false ) );
  Children point   ( node.find ( "Point", false ) );
  Children linestring ( node.find ( "LineString", false ) );
  Children linering ( node.find ( "LineRing", false ) );
  Children model ( node.find ( "Model", false ) );

  // Make the data object.
  DataObject::RefPtr object ( Factory::instance().createPlaceMark ( node ) );
  
  // Get the style, if any.
	Style::RefPtr style ( this->_style ( object->styleUrl() ) );
  
  Geometry::RefPtr geometry ( 0x0 );
  
  // Pick which function to redirect to.
  if ( false == model.empty() )
    geometry = this->_parseModel ( *model.front(), style.get() );
  else if ( false == point.empty() )
  {
    geometry = this->_parsePoint( *point.front(), style.get() );
    
    // Google Earth only appears to label points.
    object->label ( object->name() );
  }
  else if ( false == polygon.empty() )
    geometry = this->_parsePolygon( *polygon.front(), style.get() );
  else if ( false == linestring.empty() )
    geometry = this->_parseLineString( *linestring.front(), style.get() );
  else if ( false == linering.empty() )
    geometry = this->_parseLineRing( *linering.front(), style.get() );
  else if ( false == multiGeometry.empty() )
    this->_parseMultiGeometry ( *multiGeometry.back(), style.get(), *object );

  if ( geometry.valid() )
  {
    object->addGeometry ( geometry );
    object->extents ( geometry->extents() );
  }

  // Add the data object.
  this->add ( Usul::Interfaces::IUnknown::QueryPtr ( object ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Parse a point.
//
///////////////////////////////////////////////////////////////////////////////

KmlLayer::Geometry* KmlLayer::_parsePoint ( const XmlTree::Node& node, Style *style )
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
  typedef Minerva::Core::Data::LineStyle LineStyle;
  typedef Minerva::Core::Data::PolyStyle PolyStyle;
  
  PolyStyle::RefPtr polyStyle ( ( 0x0 != style ) && ( 0x0 != style->polystyle() ) ? style->polystyle() : 0x0 );
  LineStyle::RefPtr lineStyle ( ( 0x0 != style ) && ( 0x0 != style->linestyle() ) ? style->linestyle() : 0x0 );

  // Make the data object.
  Minerva::Core::Data::Polygon::RefPtr polygon ( Factory::instance().createPolygon ( node ) );

  if ( polygon.valid() )
  {
    if ( polyStyle.valid() )
      polygon->polyStyle ( polyStyle );
    if ( lineStyle.valid() )
      polygon->lineStyle ( lineStyle );
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
  typedef Minerva::Core::Data::LineStyle LineStyle;
  
  LineStyle::RefPtr lineStyle ( ( 0x0 != style ) && ( 0x0 != style->linestyle() ) ? style->linestyle() : 0x0 );
  
  Minerva::Core::Data::Line::RefPtr line ( Factory::instance().createLine ( node ) );
  
  if ( line.valid() && lineStyle.valid() )
  {
    line->lineStyle ( lineStyle );
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
    Link::RefPtr link ( false == links.empty() ? Factory::instance().createLink ( *links.front() ) : 0x0 );
    
    // Make the filename.
    std::string filename ( this->_buildFilename ( link ) );

    if ( false == filename.empty() )
    {
      LoadModel load;
      osg::ref_ptr<osg::Node> node ( load ( filename, this->modelCache() ) );
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
//  Serialize.
//
///////////////////////////////////////////////////////////////////////////////

void KmlLayer::serialize ( XmlTree::Node &parent ) const
{
  Serialize::XML::DataMemberMap dataMemberMap ( Usul::Threads::Safe::get ( this->mutex(), _dataMemberMap ) );
  
  // Don't serialize the layers.
  dataMemberMap.erase ( "layers" );
  
  // Serialize.
  dataMemberMap.serialize ( parent );
  
  // TODO: save the kml file using the current state.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update.
//
///////////////////////////////////////////////////////////////////////////////

void KmlLayer::updateNotify ( Usul::Interfaces::IUnknown *caller )
{
  BaseClass::updateNotify ( caller );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update link.
//
///////////////////////////////////////////////////////////////////////////////

void KmlLayer::_updateLink ( Usul::Interfaces::IUnknown* caller )
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

void KmlLayer::downloading ( bool b )
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

void KmlLayer::reading ( bool b )
{
  Guard guard ( this );
  _flags = Usul::Bits::set<unsigned int, unsigned int> ( _flags, KmlLayer::READING, b );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the style.
//
///////////////////////////////////////////////////////////////////////////////

KmlLayer::Style* KmlLayer::_style ( const std::string& url ) const
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
      std::string filename;
      if ( Minerva::Core::Utilities::download ( href, filename ) )
        return filename;
    }
    else
    {
      return Usul::Threads::Safe::get ( this->mutex(), _directory ) + href;
    }
  }

  return "";
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the model cache.
//
///////////////////////////////////////////////////////////////////////////////

KmlLayer::ModelCache* KmlLayer::modelCache() const
{
  Guard guard ( this->mutex() );
  return _modelCache.first;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a timer callback.
//
///////////////////////////////////////////////////////////////////////////////

void KmlLayer::_addTimer()
{
  typedef Usul::Interfaces::ITimerService ITimerService;
  typedef Usul::Components::Manager PluginManager;
  
  ITimerService::QueryPtr service ( PluginManager::instance().getInterface ( ITimerService::IID ) );
  
  // Get the link.
  Link::RefPtr link ( Usul::Threads::Safe::get ( this->mutex(), _link ) );
  
  if ( service.valid() )
  {
    // Remove the one we have if it's valid.
    if ( _timerId > 0 )
      service->timerRemove ( _timerId );
    
    // See if it's the right kind of refresh mode.
    if ( link.valid() && Link::ON_INTERVAL == link->refreshMode() )
    {        
      // Make a new timer.  The timer expects the timeout to be in milliseconds.
      Usul::Interfaces::IUnknown::RefPtr me ( this->queryInterface ( Usul::Interfaces::IUnknown::IID ) );
      _timerId = service->timerAdd ( link->refreshInterval() * 1000, me, true );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the timer fires.
//
///////////////////////////////////////////////////////////////////////////////

void KmlLayer::timerNotify ( TimerID )
{
  // Return if we are currently reading or downloading.
  if ( this->isReading() || this->isDownloading() )
    return;

  // Launch a job to update.
  this->_launchUpdateLinkJob();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the kml.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  class ReadJob : public Usul::Jobs::Job
  {
  public:
    
    typedef Usul::Jobs::Job BaseClass;
    
    ReadJob ( KmlLayer *layer ) : BaseClass(), _layer ( layer )
    {
    }
    
  protected:
    
    virtual ~ReadJob()
    {
    }
    
    virtual void _started()
    {
      if ( _layer.valid() )
        _layer->read ( 0x0, 0x0 );
    }
    
  private:
    
    KmlLayer::RefPtr _layer;
  };
}


///////////////////////////////////////////////////////////////////////////////
//
//  Force a refresh of data (IRefreshData).
//
///////////////////////////////////////////////////////////////////////////////

void KmlLayer::refreshData()
{
  // See if there is a link.
  Link::RefPtr link ( Usul::Threads::Safe::get ( this->mutex(), _link ) );
  
  if ( link.valid() )
  {
    // Launch a job to update.
    this->_launchUpdateLinkJob();
  }
  
  // Launch a job to read.
  else
  {
    // Create a job to update the file.
    Usul::Jobs::Job::RefPtr job ( new Detail::ReadJob ( this ) );
    
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
//  Launch a job to update link.
//
///////////////////////////////////////////////////////////////////////////////

void KmlLayer::_launchUpdateLinkJob()
{
  // Help shorten lines.
  namespace UA = Usul::Adaptors;
  
  // Create a job to update the file.
  Usul::Jobs::Job::RefPtr job ( Usul::Jobs::create (  UA::bind1 ( static_cast <Usul::Interfaces::IUnknown*> ( 0x0 ), 
                                                                 UA::memberFunction ( KmlLayer::RefPtr ( this ), &KmlLayer::_updateLink ) ) ) );
  
  if ( true == job.valid() )
  {
    // Set the downloading flag now so we don't launch another job before this one starts.
    this->downloading ( true );
    
    // Add job to manager.
    Usul::Jobs::Manager::instance().addJob ( job.get() );
  }
}
