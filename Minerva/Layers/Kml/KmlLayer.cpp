
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
#include "Minerva/Core/Factory/Readers.h"
#include "Minerva/Core/DataObjects/Model.h"
#include "Minerva/Core/DataObjects/Point.h"
#include "Minerva/Core/DataObjects/Line.h"
#include "Minerva/Core/DataObjects/Polygon.h"
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
#include "Usul/File/Temp.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Interfaces/IFrameStamp.h"
#include "Usul/Jobs/Job.h"
#include "Usul/Jobs/Manager.h"
#include "Usul/Network/Curl.h"
#include "Usul/Predicates/FileExists.h"
#include "Usul/Scope/Caller.h"
#include "Usul/Strings/Case.h"
#include "Usul/Strings/Split.h"
#include "Usul/System/Directory.h"
#include "Usul/System/Host.h"
#include "Usul/Threads/Safe.h"

#include "OsgTools/Visitor.h"
#include "OsgTools/State/StateSet.h"

#include "osg/Material"

#include "osgDB/ReadFile"

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
  _link ( 0x0 ),
  _lastUpdate( 0.0 ),
  _flags ( 0 )
{
  this->_addMember ( "filename", _filename );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

KmlLayer::KmlLayer( const XmlTree::Node& node, const std::string& filename ) :
  BaseClass(),
  _filename( filename ),
  _link ( 0x0 ),
  _lastUpdate( 0.0 ),
  _flags ( 0 )
{
  this->_addMember ( "filename", _filename );
  this->_parseFolder ( node );
  this->dirtyData ( false );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

KmlLayer::KmlLayer( Link* link ) :
  BaseClass(),
  _filename(),
  _link ( link ),
  _lastUpdate ( 0.0 ),
  _flags ( 0 )
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
  this->clearLayers();
  this->clearDataObjects();
  
  // See if we need to unzip...
  if ( "kmz" == ext )
  {
    std::string dir ( Usul::File::Temp::directory( true ) + Usul::File::base ( filename ) + "/" );
    
    std::string command ( "/usr/bin/unzip -o " + filename + " -d " + dir );
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
  
  // Our scene needs rebuit.
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
    // Make a new layer.
    Minerva::Layers::Kml::KmlLayer::RefPtr layer ( new Minerva::Layers::Kml::KmlLayer ( node, Usul::Threads::Safe::get ( this->mutex(), _filename ) ) );
    
    // Make sure the scene gets built.
    layer->dirtyScene ( true );
    
    // Add the layer to the parent.
    this->addLayer ( layer.get() );
  }
  else if ( "NetworkLink" == name )
  {
    NetworkLink::RefPtr networkLink ( this->_parseNetworkLink( node ) );
    if ( networkLink.valid() )
    {
      Link::RefPtr link ( networkLink->link() );
      if ( link.valid() )
      {
        KmlLayer::RefPtr layer ( new KmlLayer ( link.get() ) );
        layer->read ( 0x0, 0x0 );
        this->addLayer ( layer.get() );
      }
    }
  }
  else if ( "Placemark" == name )
  {
    DataObject::RefPtr object ( this->_parsePlacemark( node ) );
    
    // Add the data object.
    this->addDataObject ( object );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Parse a style.
//
///////////////////////////////////////////////////////////////////////////////

void KmlLayer::_parseStyle ( const XmlTree::Node& node )
{
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

KmlLayer::DataObject* KmlLayer::_parsePlacemark ( const XmlTree::Node& node )
{ 
  Children polygon ( node.find ( "Polygon", false ) );
  Children point   ( node.find ( "Point", false ) );
  Children linestring ( node.find ( "LineString", false ) );
  Children linering ( node.find ( "LineRing", false ) );
  Children model ( node.find ( "Model", false ) );
  
  DataObject::RefPtr object ( 0x0 );
  
  // Pick which function to redirect to.
  if ( false == model.empty() )
    object = this->_parseModel ( *model.front() );
  else if ( false == point.empty() )
    object = this->_parsePoint( *point.front() );
  else if ( false == polygon.empty() )
    object = this->_parsePolygon( *polygon.front() );
  else if ( false == linestring.empty() )
    object = this->_parseLineString( *linestring.front() );
  else if ( false == linering.empty() )
    object = this->_parseLineRing( *linering.front() );
  
  if ( object.valid () )
  {
    Children children ( node.children() );
    for ( Children::iterator iter = children.begin(); iter != children.end(); ++iter )
    {
      XmlTree::Node::RefPtr node ( *iter );
      std::string name ( node->name() );
      if ( "name" == name )
      {
        object->label ( node->value() );
        object->labelColor ( osg::Vec4 ( 1.0, 1.0, 1.0, 1.0 ) );
        object->showLabel ( true );
      }
    }
  }
  
  return object.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Parse a point.
//
///////////////////////////////////////////////////////////////////////////////

KmlLayer::DataObject* KmlLayer::_parsePoint ( const XmlTree::Node& node )
{
  Minerva::Core::DataObjects::Point::RefPtr point ( new Minerva::Core::DataObjects::Point );
  point->autotransform ( true );
  point->size ( 5 );
  point->primitiveId ( 2 );
  point->color ( osg::Vec4 ( 1.0, 0.0, 0.0, 1.0 ) );
  
  Children children ( node.children() );
  for ( Children::iterator iter = children.begin(); iter != children.end(); ++iter )
  {
    XmlTree::Node::RefPtr node ( *iter );
    std::string name ( node->name() );
    
    if ( "altitudeMode" == name )
      point->altitudeMode ( this->_parseAltitudeMode ( *node ) );
    else if ( "coordinates" == name )
    {
      Vertices vertices;
      this->_parseCoordinates( *node, vertices );
      if ( false == vertices.empty() )
      {
        Minerva::Core::Geometry::Point::RefPtr data ( new Minerva::Core::Geometry::Point );
        data->srid ( 4326 );
        data->point ( vertices.front() );
        point->geometry ( Usul::Interfaces::IUnknown::QueryPtr ( data.get() ) );
        
        Usul::Math::Vec3d p ( data->point() );
        point->labelPosition ( osg::Vec3 ( p[0], p[1], p[2] + 100 ) );
      }
    }
    else if ( "extrude" == name )
    {
      bool extrude ( "1" == node->value() );
      point->extrude ( extrude );
    }
  }
  
  return point.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Parse a polygon.
//
///////////////////////////////////////////////////////////////////////////////

KmlLayer::DataObject* KmlLayer::_parsePolygon ( const XmlTree::Node& node )
{
  Minerva::Core::DataObjects::Polygon::RefPtr polygon ( new Minerva::Core::DataObjects::Polygon );
  polygon->color ( osg::Vec4 ( 0.8, 0.8, 0.8, 1.0 ) );
  
  Minerva::Core::Geometry::Polygon::RefPtr data ( new Minerva::Core::Geometry::Polygon );
  polygon->geometry ( Usul::Interfaces::IUnknown::QueryPtr ( data.get() ) );
  
  Children children ( node.children() );
  for ( Children::iterator iter = children.begin(); iter != children.end(); ++iter )
  {
    XmlTree::Node::RefPtr node ( *iter );
    std::string name ( node->name() );
    
    if ( "altitudeMode" == name )
      polygon->altitudeMode ( this->_parseAltitudeMode ( *node ) );
    else if ( "outerBoundaryIs" == name )
    {
      XmlTree::Node::RefPtr linearRing ( node->children().front() );
      XmlTree::Node::RefPtr coordinates ( linearRing.valid() ? linearRing->children().front() : 0x0 );
      if ( coordinates.valid() )
      {
        Vertices vertices;
        this->_parseCoordinates( *coordinates, vertices );
        data->outerBoundary ( vertices );
      }
    }
    else if ( "innerBoundaryIs" == name )
    {
      XmlTree::Node::RefPtr linearRing ( node->children().front() );
      XmlTree::Node::RefPtr coordinates ( linearRing.valid() ? linearRing->children().front() : 0x0 );
      if ( coordinates.valid() )
      {
        Vertices vertices;
        this->_parseCoordinates( *coordinates, vertices );
        data->addInnerBoundary ( vertices );
      }
    }
  }
  
  return polygon.release();
}

///////////////////////////////////////////////////////////////////////////////
//
//  Parse a line string.
//
///////////////////////////////////////////////////////////////////////////////

KmlLayer::DataObject* KmlLayer::_parseLineString ( const XmlTree::Node& node )
{
  Minerva::Core::DataObjects::Line::RefPtr line ( new Minerva::Core::DataObjects::Line );
  line->width ( 2.0f );
  line->color ( osg::Vec4 ( 1.0, 0.0, 1.0, 1.0 ) );
  
  Children children ( node.children() );
  for ( Children::iterator iter = children.begin(); iter != children.end(); ++iter )
  {
    XmlTree::Node::RefPtr node ( *iter );
    std::string name ( node->name() );
    
    if ( "altitudeMode" == name )
      line->altitudeMode ( this->_parseAltitudeMode ( *node ) );
    else if ( "coordinates" == name )
    {
      Vertices vertices;
      this->_parseCoordinates( *node, vertices );
      if ( false == vertices.empty() )
      {
        Minerva::Core::Geometry::Line::RefPtr data ( new Minerva::Core::Geometry::Line );
        data->srid ( 4326 );
        data->line ( vertices );
        line->geometry ( Usul::Interfaces::IUnknown::QueryPtr ( data.get() ) );
      }
    }
  }
  
  return line.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Parse a line ring.
//
///////////////////////////////////////////////////////////////////////////////

KmlLayer::DataObject* KmlLayer::_parseLineRing ( const XmlTree::Node& node )
{
  return this->_parseLineString( node );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Parse coordinates.
//
///////////////////////////////////////////////////////////////////////////////

void KmlLayer::_parseCoordinates ( const XmlTree::Node& node, Vertices& vertices )
{
  std::istringstream in ( node.value() );
  std::string vertex;
  
  while ( in >> vertex )
  {
    Vertex v;

    std::vector<std::string> strings;
    Usul::Strings::split ( vertex, ",", false, strings );
    
    // Convert vertex.
    for ( unsigned int i = 0; i < strings.size(); ++i )
      v[i] = ToDouble::convert ( strings[i] );
    
    vertices.push_back ( v );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Turn off non-power of two resizing.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  struct ModelPostProcess
  {
    void operator () ( osg::Node * node )
    {
      if ( 0x0 != node )
      {
        osg::ref_ptr<osg::StateSet> ss ( node->getOrCreateStateSet() );

        if ( ss.valid() )
        {
          if ( osg::Texture* texture = dynamic_cast<osg::Texture*> ( ss->getTextureAttribute ( 0, osg::StateAttribute::TEXTURE ) ) )
          {
            texture->setResizeNonPowerOfTwoHint ( false );
            texture->setFilter ( osg::Texture::MIN_FILTER, osg::Texture::LINEAR );
            texture->setFilter ( osg::Texture::MAG_FILTER, osg::Texture::LINEAR );
            
            OsgTools::State::StateSet::setLighting ( ss.get(), false );
          }
        }
      }
    }
  };
}


///////////////////////////////////////////////////////////////////////////////
//
//  Parse a model.
//
///////////////////////////////////////////////////////////////////////////////

KmlLayer::DataObject* KmlLayer::_parseModel ( const XmlTree::Node& node )
{
  Minerva::Core::DataObjects::Model::RefPtr model ( new Minerva::Core::DataObjects::Model );
  
  Children children ( node.children() );
  
  osg::Vec3 location, orientation, scale;
  std::string filename;
  
  for ( Children::iterator iter = children.begin(); iter != children.end(); ++iter )
  {
    XmlTree::Node::RefPtr node ( *iter );
    std::string name ( node->name() );
    
    if ( "Location" == name )
      location = this->_buildVec3 ( *node );
    else if ( "Orientation" == name )
      orientation = this->_buildVec3 ( *node );
    else if ( "Scale" == name )
      scale = this->_buildVec3 ( *node );
    else if ( "altitudeMode" == name )
      model->altitudeMode ( this->_parseAltitudeMode ( *node ) );
  }
  
  Children link ( node.find ( "Link", true ) );
  if ( false == link.empty() )
  {
    Link::RefPtr l ( this->_parseLink ( *link.front() ) );
    filename = ( l.valid() ? l->href() : "" );
  }
  
  if ( false == filename.empty() )
  {
    osg::ref_ptr<osg::Node> node ( osgDB::readNodeFile ( Usul::File::directory ( _filename, true ) + filename ) );
    if ( node.valid() )
    {
      model->model ( node.get() );

      // Post-process.
      Detail::ModelPostProcess nv;
      osg::ref_ptr<osg::NodeVisitor> visitor ( OsgTools::MakeVisitor<osg::Node>::make ( nv ) );
      node->accept ( *visitor );
      
      osg::ref_ptr<osg::StateSet> ss ( node->getOrCreateStateSet() );
      
      //OsgTools::State::StateSet::setTwoSidedLighting ( ss.get(), true );
      OsgTools::State::StateSet::setNormalize ( ss.get(), true );
    }
  }
  
  model->location ( location );
  model->orientation ( orientation[0], orientation[1], orientation[2] );
  model->scale ( scale );
  
  return model.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build a Vec3.
//
///////////////////////////////////////////////////////////////////////////////

KmlLayer::DataObject::AltitudeMode KmlLayer::_parseAltitudeMode ( const XmlTree::Node& node )
{
  DataObject::AltitudeMode mode ( DataObject::CLAMP_TO_GROUND );
  if ( "relativeToGround" == node.value() )
    mode = DataObject::RELATIVE_TO_GROUND;
  if ( "absolute" == node.value() )
    mode = DataObject::ABSOLUTE;
  
  return mode;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build a Vec3.
//
///////////////////////////////////////////////////////////////////////////////

osg::Vec3 KmlLayer::_buildVec3 ( const XmlTree::Node& node )
{
  osg::Vec3 v;
  
  Children children ( node.children() );
  
  if ( 3 == children.size() )
  {
    v[0] = ToDouble::convert ( children[0]->value() );
    v[1] = ToDouble::convert ( children[1]->value() );
    v[2] = ToDouble::convert ( children[2]->value() );
  }
  
  return v;
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
//  Parse network link.
//
///////////////////////////////////////////////////////////////////////////////

NetworkLink* KmlLayer::_parseNetworkLink ( const XmlTree::Node& node )
{
  NetworkLink::RefPtr network ( new NetworkLink );
  
  // Loop over the children.
  Children children ( node.children() );
  for ( Children::iterator iter = children.begin(); iter != children.end(); ++iter )
  {
    XmlTree::Node::RefPtr node ( *iter );
    std::string name ( node->name() );
    
    if ( "Link" == name || "Url" == name ) // Url is an older name, but many elements are the same.
    {
      network->link ( this->_parseLink ( *node ) );
    }
  }
  
  return network.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Parse link.
//
///////////////////////////////////////////////////////////////////////////////

Link* KmlLayer::_parseLink ( const XmlTree::Node& node )
{
  Link::RefPtr link ( new Link );
  
  // Get the children.
  Children children ( node.children() );
  for ( Children::iterator iter = children.begin(); iter != children.end(); ++iter )
  {
    XmlTree::Node::RefPtr node ( *iter );
    std::string name ( node->name() );
    
    if ( "href" == name )
      link->href ( node->value() );
    else if ( "refreshInterval" == name )
      link->refreshInterval ( ToDouble::convert ( node->value() ) );
    else if ( "refreshMode" == name )
    {
      std::string mode ( node->value() );
      if ( "onChange" == mode )
        link->refreshMode ( Link::ON_CHANGE );
      else if ( "onInterval" == mode )
        link->refreshMode ( Link::ON_INTERVAL );
      else if ( "onExpire" == mode )
        link->refreshMode ( Link::ON_EXPIRE );
    }
  }
  
  return link.release();
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
  
  if ( link.valid() )
  {
    std::string href ( link->href() );
    
    std::string::size_type pos ( href.rfind ( '/' ) );
    
    if ( pos != std::string::npos )
    {
      // Build the filename.
      std::string filename ( href.begin() + pos + 1, href.end() );
      filename = Usul::File::Temp::directory ( true ) + filename;
      
      // Set the filename.
      Usul::Threads::Safe::set ( this->mutex(), filename, _filename );
      
      // Download.
      Usul::Network::Curl curl ( href, filename );
      Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( &curl, &Usul::Network::Curl::download ), "1638679894" );
      
      // Get the current time.
      Usul::Interfaces::IFrameStamp::QueryPtr fs ( caller );
      const double time ( fs.valid () ? fs->frameStamp()->getReferenceTime () : 0.0 );
      
      // Set the last update time.
      Usul::Threads::Safe::set ( this->mutex(), time, _lastUpdate );
      
      // Our data is dirty.
      this->dirtyData ( true );
    }
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
