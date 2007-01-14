
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "OssimPlanet.h"

#include "Usul/Strings/ManagedToNative.h"

#include "osg/ref_ptr"

#include "ossim/base/ossimKeywordlist.h"
#include "ossim/init/ossimInit.h"

#include "ossimPlanet/ossimPlanet.h"
#include "ossimPlanet/ossimPlanetLand.h"
#include "ossimPlanet/ossimPlanetTextureLayerGroup.h"
#include "ossimPlanet/ossimPlanetDatabasePager.h"
#include "ossimPlanet/ossimPlanetTextureLayerRegistry.h"
#include "ossimPlanet/ossimPlanetManipulator.h"
#include "ossimPlanet/ossimPlanetSrtmElevationDatabase.h"

using namespace CadKit::Plugins::Documents::OSSIMPlanetDocument::Glue;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

OssimPlanet::OssimPlanet() : 
_planet ( 0x0 ), 
_textureLayerGroup ( 0x0 ),
_textureLayers( gcnew TextureLayers )
{
  this->_init();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

OssimPlanet::OssimPlanet( System::String ^ kwl ) : 
_planet ( 0x0 ), 
_textureLayerGroup ( 0x0 ),
_textureLayers( gcnew TextureLayers )
{
  this->_init();

  this->addKeyWordList( kwl );

  //osg::ref_ptr<ossimPlanetTextureLayer> layer = ossimPlanetTextureLayerRegistry::instance()->createLayer( "C:/adam/sdk/ossim/ossim_package_support/images/reference/earth.jpg" );

  /*osg::ref_ptr<ossimPlanetTextureLayer> layer = ossimPlanetTextureLayerRegistry::instance()->createLayer( "E:\\data\\earth\\earth\\land_shallow_topo_east_tiled.tif" );
  osg::ref_ptr<ossimPlanetTextureLayer> layer2 = ossimPlanetTextureLayerRegistry::instance()->createLayer( "E:\\data\\earth\\earth\\land_shallow_topo_west_tiled.tif" );

  _textureLayerGroup->addTop( layer.get() );
  _textureLayerGroup->addBottom( layer2.get() );

  osg::ref_ptr<ossimPlanetSrtmElevationDatabase> database = new ossimPlanetSrtmElevationDatabase;
  if( database->open( "E:\\data\\srtm\\srtm1\\region_04" ) == ossimPlanetTextureLayer_VALID)
  {
    _planet->getLand()->getElevationDatabase()->addBottom(database.get());
  }*/
}

void OssimPlanet::addKeyWordList(System::String ^kwl)
{
  ossimKeywordlist keyWordList ( Usul::Strings::convert ( kwl ).c_str() );

  osg::ref_ptr< ossimPlanetTextureLayer > layer ( ossimPlanetTextureLayerRegistry::instance()->createLayer( keyWordList.toString() ) );

  if( layer.valid() )
    _textureLayerGroup->addTop ( layer.get() );

  _planet->getLand()->resetGraph();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

OssimPlanet::~OssimPlanet()
{
  if( 0x0 != _planet )
  {
    _planet->unref();
    _planet = 0x0;
  }

  if( 0x0 != _textureLayerGroup )
  {
    _textureLayerGroup->unref();
    _textureLayerGroup = 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Finalizer.
//
///////////////////////////////////////////////////////////////////////////////

OssimPlanet::!OssimPlanet()
{
  if( 0x0 != _planet )
  {
    _planet->unref();
    _planet = 0x0;
  }

  if( 0x0 != _textureLayerGroup )
  {
    _textureLayerGroup->unref();
    _textureLayerGroup = 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize.
//
///////////////////////////////////////////////////////////////////////////////

void OssimPlanet::_init()
{
  ossimInit::instance()->initialize();

  _planet = new ossimPlanet();
  _planet->ref();

  _textureLayerGroup = new ossimPlanetTextureLayerGroup();
  _textureLayerGroup->ref();

  _planet->getLand()->setLandType( ossimPlanetLandType_NORMALIZED_ELLIPSOID );
  _planet->getLand()->setElevationEnabledFlag( true );
  _planet->getLand()->setHeightExag( 1.0f );
  _planet->getLand()->setElevationPatchSize( 16 );
  _planet->getLand()->setMaxLevelDetail( 16 );
  //_planet->getLand()->setElevationCacheDir( "C:\\adam\\programs\\ossim\\cache" );
  _planet->setEnableEphemerisFlag( false );
  _planet->setEnableHudFlag( true );

  _planet->getLand()->setCacheSize( ossim_uint64 ( 4096 * 1024 * 1024 ), ossim_uint64 ( 2048 * 1024 * 1024 ) );

  _planet->getLand()->setTextureLayer( _textureLayerGroup, 0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the root of the scene.
//
///////////////////////////////////////////////////////////////////////////////

CadKit::OSG::Glue::Node ^ OssimPlanet::root()
{
  System::IntPtr ptr ( _planet );
  return gcnew CadKit::OSG::Glue::Node ( ptr );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the database pager.
//
///////////////////////////////////////////////////////////////////////////////

CadKit::OSG::Glue::DatabasePager^ OssimPlanet::databasePager()
{
  return gcnew CadKit::OSG::Glue::DatabasePager ( System::IntPtr( new ossimPlanetDatabasePager ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the matrix manipulator.
//
///////////////////////////////////////////////////////////////////////////////

CadKit::OSG::Glue::MatrixManipulator^ OssimPlanet::matrixManipulator()
{
  return gcnew CadKit::OSG::Glue::MatrixManipulator( System::IntPtr ( new ossimPlanetManipulator ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a layer.
//
///////////////////////////////////////////////////////////////////////////////

void OssimPlanet::addLayer(CadKit::OSSIMPlanet::Glue::ImageLayer ^layer)
{
  if( nullptr != layer )
  {
    // Add layer to group.
    _textureLayerGroup->addTop( layer->nativePtr() );

    // Not sure if this is needed.
    _planet->getLand()->resetGraph();

    _textureLayers->Add( layer->Name, layer );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove a layer.
//
///////////////////////////////////////////////////////////////////////////////

void OssimPlanet::removeLayer(CadKit::OSSIMPlanet::Glue::ImageLayer ^layer)
{
  if( nullptr != layer )
  {
    _textureLayerGroup->removeLayer( layer->nativePtr() );
    
    _textureLayers->Remove(layer->Name);
    
    // Not sure if this is needed.
    _planet->getLand()->resetGraph();
  }
}

