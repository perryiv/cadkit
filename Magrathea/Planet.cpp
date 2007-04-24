
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Aashish Chaudhary
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "Magrathea/Planet.h"

#include "Usul/Interfaces/IViewMatrix.h"

#include "osgDB/Registry"

#include "ossimPlanet/ossimPlanetDatabasePager.h"
#include "ossimPlanet/ossimPlanetTextureLayerRegistry.h"
#include "ossim/base/ossimKeywordlist.h"

#include "wms/wms.h"

#include "osg/PolygonOffset"

using namespace Magrathea;


///////////////////////////////////////////////////////////////////////////////
//
//  Static Members.
//
///////////////////////////////////////////////////////////////////////////////

bool            Planet::_ossimInitialized ( false );
unsigned int    Planet::_planetCount ( 0 );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Planet::Planet() : 
  BaseClass(),
  _planet            ( 0x0 ),
  _databasePager     ( 0x0 ),
  _textureLayerGroup ( 0x0 ),
  _manipulator       ( 0x0 ),
  _latLongGrid       ( 0x0 ),
  _viewer            ( 0x0 )
{
  if( false == _ossimInitialized )
  {
    ossimInit::instance()->initialize();
    _ossimInitialized = true;
  }

  if( 0 == _planetCount )
    wmsInitialize();

  ++_planetCount;

  _planet                       = new ossimPlanet();
  _databasePager                = new osgDB::DatabasePager();   
  _textureLayerGroup            = new ossimPlanetTextureLayerGroup();
  _textureOperationLayerGroup   = new ossimPlanetTextureLayerGroup();  
  _layerManipulator             = new LayerManipulator();
  _manipulator                  = new Manipulator();

  // Set the planet.
  _manipulator->planet( _planet.get() );

  osg::ref_ptr < osg::StateSet > ss ( _planet->getOrCreateStateSet () );
  osg::ref_ptr< osg::PolygonOffset > offset ( new osg::PolygonOffset( 1.0f, 1.0f ) );
  ss->setMode ( GL_POLYGON_OFFSET_FILL, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE );
  ss->setAttribute( offset.get(), osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE );

  _latLongGrid = new LatLongGrid();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Planet::~Planet()
{
  --_planetCount;

  if( 0 == _planetCount )
    wmsFinalize();

  _databasePager->cancel();
  _databasePager = 0x0;
}

///////////////////////////////////////////////////////////////////////////////
//
//  Set the defaults.
//
///////////////////////////////////////////////////////////////////////////////

void Planet::setDefaults()
{
  // Defaults. 
  const ossimPlanetLandType landType ( ossimPlanetLandType_NORMALIZED_ELLIPSOID );

  const bool  elevEnabled    ( true  );
  const bool  ephemerisFlag  ( false );
  const bool  hudEnabled     ( false );
  const float elevExag       ( 1.0   );
  const int   elevEstimate   ( 16    );    
  const int   levelDetail    ( 16    );    

  const ossimFilename elevCache( "" );

  _planet->getLand()->setLandType( landType );
  _planet->getLand()->setElevationEnabledFlag( elevEnabled );
  _planet->getLand()->setHeightExag( elevExag );
  _planet->getLand()->setElevationPatchSize( elevEstimate );
  _planet->getLand()->setMaxLevelDetail( levelDetail );
  _planet->getLand()->setElevationCacheDir( elevCache );
  _planet->setEnableEphemerisFlag( ephemerisFlag );
  _planet->setEnableHudFlag( hudEnabled );
  _planet->getLand()->setElevationCacheDir( elevCache );
  _planet->getLand()->resetGraph();

  _layerManipulator->setLand( _planet->getLand().get() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize.
//
///////////////////////////////////////////////////////////////////////////////

void Planet::init()
{
  try
  {
    this->setDefaults();

    _planet->getLand()->setTextureLayer( _textureLayerGroup.get(), 0 );
    _planet->getLand()->setTextureLayer( _textureOperationLayerGroup.get(), 1 );
    _planet->getLand()->resetGraph(); 	

    //osgDB::Registry::instance()->setDatabasePager( _databasePager.get() );
	  _databasePager->setExpiryDelay(0);
    _databasePager->setUseFrameBlock( true );
	  _databasePager->setAcceptNewDatabaseRequests( true );
	  _databasePager->setDatabasePagerThreadPause( false );	
  }
  catch( const std::exception& e )
  {
    std::cerr << "Error 332911910: Exception caught in init " << std::endl;
    std::cerr << "Message:" << e.what() << std::endl;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read a kwl file.
//
///////////////////////////////////////////////////////////////////////////////

void Planet::readKWL( const std::string& fileName )
{
  ossimKeywordlist kwl( fileName.c_str() );
  osg::ref_ptr< ossimPlanetTextureLayer > layer = ossimPlanetTextureLayerRegistry::instance()->createLayer(kwl.toString());
  if( layer.valid() )
  {
    _textureLayerGroup->addTop( layer.get() );
    this->refreshLandTextures( layer->getExtents().get(), ossimPlanetPagedLandLodRefreshType_TEXTURE );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a layer.
//
///////////////////////////////////////////////////////////////////////////////

int Planet::addLayer( const std::string& filename )
{
  osg::ref_ptr< ossimPlanetTextureLayer > layer ( ossimPlanetTextureLayerRegistry::instance()->createLayer(filename.c_str() ) );
  return this->addLayer( layer.get() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a layer.
//
///////////////////////////////////////////////////////////////////////////////

int Planet::addLayer( ossimPlanetTextureLayer *layer  )
{
  int index ( -1 );
  if( 0x0 != layer )
  {    
    _textureLayerGroup->addTop( layer );    
    index = _textureLayerGroup->findLayerIndex( layer );
    this->refreshLandTextures( layer->getExtents().get(), ossimPlanetPagedLandLodRefreshType_TEXTURE );
  }

  return index;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a layer.
//
///////////////////////////////////////////////////////////////////////////////

int Planet::addLayerOperation( const std::string& filename )
{
  osg::ref_ptr< ossimPlanetTextureLayer > layer ( ossimPlanetTextureLayerRegistry::instance()->createLayer(filename.c_str() ) );
  return this->addLayerOperation( layer.get() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a layer.
//
///////////////////////////////////////////////////////////////////////////////

int Planet::addLayerOperation( ossimPlanetTextureLayer *layer  )
{
  int index ( -1 );
  if( 0x0 != layer )
  {    
    _textureOperationLayerGroup->addTop( layer );
    index = _textureOperationLayerGroup->findLayerIndex( layer );
    this->refreshLandTextures( layer->getExtents().get(), ossimPlanetPagedLandLodRefreshType_TEXTURE );
  }

  return index;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove a layer.
//
///////////////////////////////////////////////////////////////////////////////

void Planet::removeLayer( int index  )
{
  _textureLayerGroup->removeLayer( index );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove a layer.
//
///////////////////////////////////////////////////////////////////////////////

void Planet::removeLayerOperation( int index  )
{
  _textureOperationLayerGroup->removeLayer( index );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove a layer.
//
///////////////////////////////////////////////////////////////////////////////

void Planet::removeLayer( ossimPlanetTextureLayer *layer )
{
  _textureLayerGroup->removeLayer( layer );
  this->refreshLandTextures( layer->getExtents().get(), ossimPlanetPagedLandLodRefreshType_TEXTURE );
}

bool Planet::hasLayer( ossimPlanetTextureLayer *layer )
{
  return _textureLayerGroup->containsLayer( layer );
}

bool Planet::hasLayerOperation( ossimPlanetTextureLayer *layer )
{
  return _textureOperationLayerGroup->containsLayer( layer );
}

///////////////////////////////////////////////////////////////////////////////
//
//  Remove a layer.
//
///////////////////////////////////////////////////////////////////////////////

void Planet::removeLayerOperation( ossimPlanetTextureLayer *layer  )
{
  _textureOperationLayerGroup->removeLayer( layer );
  this->refreshLandTextures( layer->getExtents().get(), ossimPlanetPagedLandLodRefreshType_TEXTURE );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Reset the graph.
//
///////////////////////////////////////////////////////////////////////////////

void Planet::reset()
{  
  _planet->getLand()->resetGraph(); 
}


///////////////////////////////////////////////////////////////////////////////
//
//  Reset the graph.
//
///////////////////////////////////////////////////////////////////////////////

void Planet::refreshLandTextures( ossimPlanetExtents* extents, ossimPlanetPagedLandLodRefreshType refreshType)
{
  _planet->getLand()->resetGraph( extents, refreshType);
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the scene root.
//
///////////////////////////////////////////////////////////////////////////////

osg::Group* Planet::root() const
{
  return _planet.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the database pager.
//
///////////////////////////////////////////////////////////////////////////////

osgDB::DatabasePager* Planet::databasePager() const
{
  return _databasePager.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is elevation enabled?
//
///////////////////////////////////////////////////////////////////////////////

bool Planet::elevationEnabled() const
{ 
  return _planet->getLand()->getElevationEnabledFlag(); 
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set elevation enabled.
//
///////////////////////////////////////////////////////////////////////////////

void Planet::elevationEnabled( bool val ) 
{ 
  _planet->getLand()->setElevationEnabledFlag( val ); 
  reset(); 
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is the HUD enabled?
//
///////////////////////////////////////////////////////////////////////////////

bool Planet::hudEnabled() const
{ 
  return  _planet->getEnableHudFlag();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the hud enabled flag.
//
///////////////////////////////////////////////////////////////////////////////

void Planet::hudEnabled( bool val ) 
{ 
  _planet->setEnableHudFlag( val );
  reset(); 
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is ephemeris enabled?
//
///////////////////////////////////////////////////////////////////////////////

bool Planet::ephemerisFlag() const
{ 
  return _planet->getEnableEphemerisFlag();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the ephemeris flag.
//
///////////////////////////////////////////////////////////////////////////////

void Planet::ephemerisFlag( bool val )
{ 
  _planet->setEnableEphemerisFlag( val );
  // Update??
}  


///////////////////////////////////////////////////////////////////////////////
//
//  Get the elevation exageration.
//
///////////////////////////////////////////////////////////////////////////////

float Planet::elevationExag() const
{ 
  return _planet->getLand()->getHeightExag();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the elevation exageration.
//
///////////////////////////////////////////////////////////////////////////////

void Planet::elevationExag( const float& elevExag )
{ 
  _planet->getLand()->setHeightExag( elevExag );
  reset(); 
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the elevation patch size.
//
///////////////////////////////////////////////////////////////////////////////

int Planet::elevationPatchSize() const
{ 
  return _planet->getLand()->getElevationPatchSize();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the elevation patch size.
//
///////////////////////////////////////////////////////////////////////////////

void  Planet::elevationPatchSize( const int& elevEstimate )
{ 
  _planet->getLand()->setElevationPatchSize( elevEstimate );
  reset(); 
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the maximium level of detail.
//
///////////////////////////////////////////////////////////////////////////////

int Planet::levelDetail() const
{ 
  return _planet->getLand()->getMaxLevelDetail();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the maximium level of detail.
//
///////////////////////////////////////////////////////////////////////////////

void Planet::levelDetail( const int& levelDetail )
{ 
  _planet->getLand()->setMaxLevelDetail( levelDetail );
  reset(); 
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the land type.
//
///////////////////////////////////////////////////////////////////////////////

void Planet::landType( ossimPlanetLandType landType )
{ 
  _planet->getLand()->setLandType( landType );
  reset(); 
}      


///////////////////////////////////////////////////////////////////////////////
//
//  Set the show lat long flag.
//
///////////////////////////////////////////////////////////////////////////////

void Planet::showLatLongGrid( bool b )
{
  if( b )
    _planet->addChild( _latLongGrid.get() );
  else
    _planet->removeChild( _latLongGrid.get() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is the lat long grid showing?
//
///////////////////////////////////////////////////////////////////////////////

bool Planet::showLatLongGrid() const
{
  return _planet->containsNode ( _latLongGrid.get() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the elevation cache dir.
//
///////////////////////////////////////////////////////////////////////////////

std::string Planet::elevationCacheDir() const
{
  ossimFilename name ( _planet->getLand()->getElevationCacheDir( ) );
  return name.c_str();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the elevation cache dir.
//
///////////////////////////////////////////////////////////////////////////////

void Planet::elevationCacheDir( const std::string& directory )
{
  _planet->getLand()->setElevationCacheDir( directory.c_str() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the manipulator.
//
///////////////////////////////////////////////////////////////////////////////

osgGA::MatrixManipulator* Planet::manipulator() const
{
  return _manipulator.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Go to lat, long at given height.
//
///////////////////////////////////////////////////////////////////////////////

void Planet::gotoLocation( double lat, double lon, double height )
{
  //ossimString actionString = ":navigator gotolatlonelevnadir ";
  //actionString += (ossimString::toString(lat) + " " +
  //                  ossimString::toString(lon) + " " +
  //                  ossimString::toString(height));
   
  //ossimPlanetAction ( actionString.c_str() ).execute();
  _manipulator->gotoLocation( lat, lon, height );

  Usul::Interfaces::IViewMatrix::QueryPtr viewMatrix ( _viewer );

  if( viewMatrix.valid() )
    viewMatrix->setViewMatrix ( _manipulator->getInverseMatrix() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the viewer.
//
///////////////////////////////////////////////////////////////////////////////

void Planet::viewer( Usul::Interfaces::IUnknown* viewer )
{
  _viewer = viewer;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the viewer.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown* Planet::viewer()
{
  return _viewer.get();
}



void Planet::opacity( const float& val )
{
  _layerManipulator->opacity( val );
  //reset();
}

float Planet::opacity() const 
{
  return _layerManipulator->opacity();
}

void Planet::horizontalSwipe( const float& val )
{
  _layerManipulator->horizontalSwipe( val );
  //reset();
}

float Planet::horizontalSwipe() const 
{
  return _layerManipulator->horizontalSwipe();
}

void Planet::verticalSwipe( const float& val )
{
  _layerManipulator->verticalSwipe( val );
  //reset();
}

float Planet::verticalSwipe() const 
{
  return _layerManipulator->verticalSwipe();
}

void Planet::top()
{
  _layerManipulator->top();
}

void Planet::reference()
{
  _layerManipulator->reference();
}

void Planet::absoluteDifference()
{
  _layerManipulator->absoluteDifference();
}

void Planet::falseColorReplacement()
{
  _layerManipulator->falseColorReplacement();
}
