
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Magrathea/KwlLayer.h"

#include "Usul/Functions/Guid.h"
#include "Usul/System/Directory.h"
#include "Usul/File/Path.h"

#include "Serialize/XML/RegisterCreator.h"

#include "ossim/base/ossimKeywordList.h"
#include "ossimPlanet/ossimPlanetTextureLayerRegistry.h"

using namespace Magrathea;

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( KwlLayer, KwlLayer::BaseClass );
//SERIALIZE_XML_REGISTER_CREATOR ( KwlLayer );

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

KwlLayer::KwlLayer() : BaseClass(),
_guid ( Usul::Functions::GUID::generate() ),
_filename (),
_layer ( 0x0 ),
SERIALIZE_XML_INITIALIZER_LIST
{
  SERIALIZE_XML_ADD_MEMBER ( _guid );
  SERIALIZE_XML_ADD_MEMBER ( _filename );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

KwlLayer::KwlLayer( const std::string & filename ) : BaseClass(),
_guid ( Usul::Functions::GUID::generate() ),
_filename ( filename ),
_layer ( 0x0 )
{
  this->open();

  SERIALIZE_XML_ADD_MEMBER ( _guid );
  SERIALIZE_XML_ADD_MEMBER ( _filename );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

KwlLayer::~KwlLayer()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Open.
//
///////////////////////////////////////////////////////////////////////////////

void KwlLayer::open ( )
{
  // Get the directory the file lives in.
  std::string directory ( Usul::File::directory( _filename, false ) );
  
  // Get the current working directory.
  std::string cwd ( Usul::System::Directory::cwd () );

  // Set the new current working directory.
  Usul::System::Directory::cwd ( directory );

  // Open the kwl.
  ossimKeywordlist kwl( _filename.c_str() );
  _layer = ossimPlanetTextureLayerRegistry::instance()->createLayer( kwl.toString() );

  // Restore the old current working directory.
  Usul::System::Directory::cwd( directory );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the filename.
//
///////////////////////////////////////////////////////////////////////////////

void KwlLayer::filename ( const std::string& filename )
{
  _filename = filename;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filename.
//
///////////////////////////////////////////////////////////////////////////////

const std::string& KwlLayer::filename () const
{
  return _filename;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the guid.
//
///////////////////////////////////////////////////////////////////////////////

std::string KwlLayer::guid() const
{
  return _guid;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the ossimPlanet layer.
//
///////////////////////////////////////////////////////////////////////////////

ossimPlanetTextureLayer* KwlLayer::ossimPlanetLayer()
{
  return _layer.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown* KwlLayer::queryInterface(unsigned long iid)
{
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::ILayer::IID:
    return static_cast < Usul::Interfaces::ILayer * > ( this );
  case Usul::Interfaces::IOssimPlanetLayer::IID:
    return static_cast < Usul::Interfaces::IOssimPlanetLayer * > ( this );
  case Usul::Interfaces::ILayerExtents::IID:
    return static_cast < Usul::Interfaces::ILayerExtents* > ( this );
  case Usul::Interfaces::ISerialize::IID:
    return static_cast < Usul::Interfaces::ISerialize* > ( this );
  default:
    return 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the error code.
//
///////////////////////////////////////////////////////////////////////////////

ossimPlanetTextureLayerStateCode KwlLayer::getStateCode() const
{
  return _layer->getStateCode();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set show layer.
//
///////////////////////////////////////////////////////////////////////////////

void KwlLayer::showLayer( bool b )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get show layer.
//
///////////////////////////////////////////////////////////////////////////////

bool KwlLayer::showLayer() const
{
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the layer extents.
//
///////////////////////////////////////////////////////////////////////////////

void KwlLayer::layerExtents ( double &lat, double &lon, double& height ) const
{
  _layer->getCenterLatLonLength( lat, lon, height );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the name.
//
///////////////////////////////////////////////////////////////////////////////

const std::string&  KwlLayer::name() const
{
  return this->filename();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Deserialize.
//
///////////////////////////////////////////////////////////////////////////////

void KwlLayer::deserialize ( const XmlTree::Node& node )
{
  _dataMemberMap.deserialize( node );
  
  this->open();
}
