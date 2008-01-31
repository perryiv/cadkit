
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
#include "Minerva/Layers/Kml/Link.h"
#include "Minerva/Core/Factory/Readers.h"
#include "Minerva/Core/DataObjects/Model.h"

#include "XmlTree/XercesLife.h"
#include "XmlTree/Document.h"

#include "Usul/Convert/Convert.h"
#include "Usul/Factory/RegisterCreator.h"
#include "Usul/File/Path.h"
#include "Usul/File/Temp.h"
#include "Usul/Network/Curl.h"
#include "Usul/Scope/Reset.h"
#include "Usul/Strings/Case.h"
#include "Usul/System/Directory.h"
#include "Usul/System/Host.h"

#include "OsgTools/Visitor.h"
#include "OsgTools/State/StateSet.h"

#include "osg/Material"

#include "osgDB/ReadFile"

using namespace Minerva::Layers::Kml;


///////////////////////////////////////////////////////////////////////////////
//
//  Typedefs.
//
///////////////////////////////////////////////////////////////////////////////

typedef XmlTree::Node::Children    Children;


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
  _currentFilename(),
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
  }
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
  
  // Read.
  this->_read ( filename, caller, progress );
  
  // Make sure.
  _currentLayer = 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the file.
//
///////////////////////////////////////////////////////////////////////////////

void KmlLayer::_read ( const std::string &filename, Usul::Interfaces::IUnknown *caller, Usul::Interfaces::IUnknown *progress )
{
  const std::string ext ( Usul::Strings::lowerCase ( Usul::File::extension ( filename ) ) );
  
  if ( "kmz" == ext )
  {
    std::string dir ( Usul::File::directory ( filename, true ) + Usul::File::base ( filename ) );
    std::string command ( "/usr/bin/unzip " + filename + " -d " + dir );
    ::system ( command.c_str() );
    this->_parseKml( dir + "/doc.kml", caller, progress );
    // TODO: unzip.
  }
  else if ( "kml" == ext )
  {
    this->_parseKml ( filename, caller, progress );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Load a kml file.
//
///////////////////////////////////////////////////////////////////////////////

void KmlLayer::_parseKml ( const std::string& filename, Usul::Interfaces::IUnknown *caller, Usul::Interfaces::IUnknown *progress )
{
  Guard guard ( this );
  
  try
  {
    // Scope the current filename.
    Usul::Scope::Reset<std::string> resetFilename ( _currentFilename, filename, _currentFilename );
    
    XmlTree::XercesLife life;
    XmlTree::Document::ValidRefPtr document ( new XmlTree::Document );
    document->load ( filename );
    
    // Loop over the children.
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
  }
  catch ( const std::exception& e )
  {
  }
  catch ( ... )
  {
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
    this->_parseFolder( node );
  }
  else if ( "NetworkLink" == name )
  {
    NetworkLink::RefPtr networkLink ( this->_parseNetworkLink( node ) );
    if ( networkLink.valid() )
    {
      Link::RefPtr link ( networkLink->link() );
      if ( link.valid() )
      {
        std::string href ( link->href() );
        
        std::string::size_type pos ( href.rfind ( '/' ) );
        
        if ( pos != std::string::npos )
        {
          std::string filename ( href.begin() + pos + 1, href.end() );
          filename = Usul::File::Temp::directory ( true ) + filename;
          
          Usul::Network::Curl curl ( href, filename );
          
          try
          {
            curl.download();
            this->_read ( filename, 0x0, 0x0 );
          }
          catch ( const std::exception& e )
          {
            std::cout << "Error 1638679894: Standard exception caught while trying to retrive file: " << filename << "." << std::endl
                      << "Message: " << e.what() << std::endl;
          }
          catch ( ... )
          {
            std::cout << "Error 2094380149: Unknown exception caught while trying to retrive file: " << filename << "." << std::endl;
          }
        }
        
      }
    }
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

        OsgTools::State::StateSet::setTwoSidedLighting ( ss.get(), true );

        if ( ss.valid() )
        {
          if ( osg::Texture* texture = dynamic_cast<osg::Texture*> ( ss->getTextureAttribute ( 0, osg::StateAttribute::TEXTURE ) ) )
          {
            texture->setResizeNonPowerOfTwoHint ( false );
            texture->setFilter ( osg::Texture::MIN_FILTER, osg::Texture::LINEAR );
            texture->setFilter ( osg::Texture::MAG_FILTER, osg::Texture::LINEAR );
          }
          else
          {
#if 0
            std::cout << "Changing material" << std::endl;
            osg::ref_ptr<osg::Material> mat ( new osg::Material );
            mat->setAmbient ( osg::Material::FRONT_AND_BACK, osg::Vec4 ( 1.0, 0.0, 0.0, 1.0 ) );
            mat->setDiffuse ( osg::Material::FRONT_AND_BACK, osg::Vec4 ( 1.0, 0.0, 0.0, 1.0 ) );
            mat->setEmission (  osg::Material::FRONT_AND_BACK, osg::Vec4 ( 0.0, 0.0, 0.0, 1.0 ) );
            ss->setAttributeAndModes ( mat.get(), osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE );
#endif
          }
        }
      }
    }
  };
}


///////////////////////////////////////////////////////////////////////////////
//
//  Parse a placemark.
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
    //osg::ref_ptr<osg::Node> node ( osgDB::readNodeFile ( Usul::File::directory ( _filename, true ) + filename ) );
    osg::ref_ptr<osg::Node> node ( osgDB::readNodeFile ( Usul::File::directory ( _currentFilename, true ) + filename ) );
    if ( node.valid() )
    {
      osg::NotifySeverity level ( osg::getNotifyLevel() );
      osg::setNotifyLevel ( osg::ALWAYS );

      model->model ( node.get() );

      osg::setNotifyLevel ( level );
 
      Detail::ModelPostProcess nv;
      osg::ref_ptr<osg::NodeVisitor> visitor ( OsgTools::MakeVisitor<osg::Node>::make ( nv ) );
      node->accept ( *visitor );
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
  
  typedef Usul::Convert::Type<std::string,double> ToDouble;
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
    
    if ( "Link" == name )
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
  }
  
  return link.release();
}
