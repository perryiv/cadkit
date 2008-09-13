
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Layers/OSM/OpenStreetMap.h"
#include "Minerva/Core/Factory/Readers.h"

#include "XmlTree/Document.h"
#include "XmlTree/XercesLife.h"

#include "Usul/Convert/Convert.h"
#include "Usul/Factory/RegisterCreator.h"
#include "Usul/Threads/Safe.h"

#include "boost/foreach.hpp"

using namespace Minerva::Layers::OSM;


///////////////////////////////////////////////////////////////////////////////
//
//  Register readers with the factory.
//
///////////////////////////////////////////////////////////////////////////////

namespace
{
  Minerva::Core::Factory::RegisterReader < Minerva::Core::Factory::TypeCreator < OpenStreetMap > > _creator_for_OSM ( "OpenStreetMap (*.osm)", "*.osm" );
}


USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( OpenStreetMap, OpenStreetMap::BaseClass );
USUL_FACTORY_REGISTER_CREATOR ( OpenStreetMap );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

OpenStreetMap::OpenStreetMap() : BaseClass()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

OpenStreetMap::~OpenStreetMap()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query Interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown* OpenStreetMap::queryInterface ( unsigned long iid )
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

void OpenStreetMap::read ( const std::string &filename, Usul::Interfaces::IUnknown *caller, Usul::Interfaces::IUnknown *progress )
{
  Usul::Threads::Safe::set ( this->mutex(), filename, _filename );
  
  this->_read ( filename, caller, progress );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the file.
//
///////////////////////////////////////////////////////////////////////////////

void OpenStreetMap::_read ( const std::string &filename, Usul::Interfaces::IUnknown *caller, Usul::Interfaces::IUnknown *progress )
{
  XmlTree::XercesLife life;
  XmlTree::Document::RefPtr doc ( new XmlTree::Document );
  doc->load ( filename );
  
  // Get the bounds of the data set.
  XmlTree::Node::RefPtr bounds ( doc->child ( "bounds" ) );
  if ( bounds.valid() )
    this->_setBounds ( *bounds );
  
  typedef XmlTree::Node::Children Children;
  Children nodes, ways;
  
  // Get all the nodes.
  doc->find ( "node", false, nodes );
  
  // Add each node.
  BOOST_FOREACH ( XmlTree::Node::ValidRefPtr node, nodes )
  {
    this->_addWay ( *node );
  }
  
  // Get all the ways.
  doc->find ( "way", false, ways );
  
  // Add each way.
  BOOST_FOREACH ( XmlTree::Node::ValidRefPtr way, ways )
  {
    this->_addWay ( *way );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Deserialize.
//
///////////////////////////////////////////////////////////////////////////////

void OpenStreetMap::deserialize( const XmlTree::Node &node )
{
  BaseClass::deserialize ( node );
  this->read ( _filename );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Serialize.
//
///////////////////////////////////////////////////////////////////////////////

void OpenStreetMap::serialize ( XmlTree::Node &parent ) const
{
  Serialize::XML::DataMemberMap dataMemberMap ( Usul::Threads::Safe::get ( this->mutex(), _dataMemberMap ) );
  
  // Don't serialize the layers.
  dataMemberMap.erase ( "layers" );
  
  // Serialize.
  dataMemberMap.serialize ( parent );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the bounds.
//
///////////////////////////////////////////////////////////////////////////////

void OpenStreetMap::_setBounds ( const XmlTree::Node& node )
{
  const std::string minlat ( node.attribute ( "minlat" ) );
  const std::string minlon ( node.attribute ( "minlon" ) );
  const std::string maxlat ( node.attribute ( "maxlat" ) );
  const std::string maxlon ( node.attribute ( "maxlon" ) );
  
  typedef Usul::Convert::Type<std::string,double> ToDouble;
  
  Extents::Vertex mn, mx;
  
  mn.set ( minlon.empty() ? 0.0 : ToDouble::convert ( minlon ), minlat.empty() ? 0.0 : ToDouble::convert ( minlat ) );
  mx.set ( maxlon.empty() ? 0.0 : ToDouble::convert ( maxlon ), maxlat.empty() ? 0.0 : ToDouble::convert ( maxlat ) );
  
  this->extents ( Extents ( mn, mx ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a node.
//
///////////////////////////////////////////////////////////////////////////////

void OpenStreetMap::_addNode ( const XmlTree::Node& node )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a way.
//
///////////////////////////////////////////////////////////////////////////////

void OpenStreetMap::_addWay ( const XmlTree::Node& node )
{
}
