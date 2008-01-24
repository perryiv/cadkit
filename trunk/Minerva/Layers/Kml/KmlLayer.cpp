
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Layers/Kml/KmlLayer.h"
#include "Minerva/Core/Factory/Readers.h"
#include "Minerva/Core/DataObjects/Model.h"

#include "XmlTree/XercesLife.h"
#include "XmlTree/Document.h"

#include "Usul/Convert/Convert.h"
#include "Usul/Factory/RegisterCreator.h"
#include "Usul/File/Path.h"
#include "Usul/Scope/Reset.h"
#include "Usul/Strings/Case.h"
#include "Usul/System/Directory.h"

#include "osgDB/ReadFile"

using namespace Minerva::Layers::Kml;

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
  _currentLayer ( 0x0 )
{
  this->_addMember ( "filename", _filename );
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
  };
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the file.
//
///////////////////////////////////////////////////////////////////////////////

void KmlLayer::read ( const std::string &filename, Usul::Interfaces::IUnknown *caller, Usul::Interfaces::IUnknown *progress )
{
  this->name ( filename );
  
  {
    Guard guard ( this );
    _filename = filename;
  }
  
  const std::string ext ( Usul::Strings::lowerCase ( Usul::File::extension ( filename ) ) );

  if ( "kmz" == ext )
  {
      // TODO: unzip.
  }
  else if ( "kml" == ext )
  {
    this->_parseKml ( filename, caller, progress );
  }
  
  // Make sure.
  _currentLayer = 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Load a kml file.
//
///////////////////////////////////////////////////////////////////////////////

void KmlLayer::_parseKml ( const std::string& filename, Usul::Interfaces::IUnknown *caller, Usul::Interfaces::IUnknown *progress )
{
  Guard guard ( this );
  
  XmlTree::XercesLife life;
  XmlTree::Document::ValidRefPtr document ( new XmlTree::Document );
  document->load ( filename );
  
  typedef XmlTree::Node::Children    Children;
  
  Children children ( document->children() );
  for ( Children::iterator iter = children.begin(); iter != children.end(); ++iter )
  {
    // Scope the current layer.
    Minerva::Core::Layers::VectorGroup::RefPtr currentLayer ( _currentLayer );
    Usul::Scope::Reset<Minerva::Core::Layers::VectorGroup::RefPtr> reset ( _currentLayer, this, currentLayer );
    
    XmlTree::Node::RefPtr node ( *iter );
      
    if ( node.valid() )
      this->_parseNode( *node );
  }
  //Children analogs    ( document->find ( "analog",    true ) );
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
    this->_parseFolder( node );
  }
  else if ( "Placemark" == name )
  {
    DataObject::RefPtr object ( this->_parsePlacemark( node ) );
    
    // Add the data object to the current layer.
    if ( _currentLayer.valid() )
      _currentLayer->addDataObject ( object );
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
  typedef XmlTree::Node::Children    Children;
  
  // Make a new layer.
  Minerva::Core::Layers::VectorGroup::RefPtr layer ( new Minerva::Core::Layers::VectorGroup );
  Minerva::Core::Layers::VectorGroup::RefPtr currentLayer ( _currentLayer );
  Usul::Scope::Reset<Minerva::Core::Layers::VectorGroup::RefPtr> reset ( _currentLayer, layer, currentLayer );
  
  // Get the children.
  Children children ( node.children() );
  for ( Children::iterator iter = children.begin(); iter != children.end(); ++iter )
  {
    XmlTree::Node::RefPtr child ( *iter );
    
    if ( child.valid() )
    {
      if ( "name" == child->name() )
        layer->name ( child->value() );
      else if ( "visibility" == child->name() )
      {
        bool visible ( "0" != child->value() );
        layer->showLayer ( visible );
      }
      else
        this->_parseNode( *child );
    }
  }
  
  // Add the layer to the parent.
  currentLayer->addLayer ( layer.get() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Parse a placemark.
//
///////////////////////////////////////////////////////////////////////////////

KmlLayer::DataObject* KmlLayer::_parsePlacemark ( const XmlTree::Node& node )
{
  typedef XmlTree::Node::Children    Children;
  
  Children polygon ( node.find ( "Polygon", false ) );
  Children point   ( node.find ( "Point", false ) );
  Children linestring ( node.find ( "LineString", false ) );
  Children linering ( node.find ( "LineRing", false ) );
  Children model ( node.find ( "Model", false ) );
  
  DataObject::RefPtr object ( 0x0 );
  
  // Pick which function to redirect to.
  if ( false == model.empty() )
    object = this->_parseModel ( *model.front() );
  
  return object.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Parse a placemark.
//
///////////////////////////////////////////////////////////////////////////////

KmlLayer::DataObject* KmlLayer::_parseModel ( const XmlTree::Node& node )
{
  Minerva::Core::DataObjects::Model::RefPtr model ( new Minerva::Core::DataObjects::Model );
  
  typedef XmlTree::Node::Children    Children;
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
  }
  
  Children href ( node.find ( "href", true ) );
  if ( false == href.empty() )
    filename = href.front()->value();
  
  if ( false == filename.empty() )
  {
    osg::ref_ptr<osg::Node> node ( osgDB::readNodeFile ( Usul::File::directory ( _filename, true ) + filename ) );
    if ( node.valid() )
      model->model ( node.get() );
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

osg::Vec3 KmlLayer::_buildVec3 ( const XmlTree::Node& node )
{
  osg::Vec3 v;
  
  typedef Usul::Convert::Type<std::string,double> ToDouble;
  typedef XmlTree::Node::Children    Children;
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
