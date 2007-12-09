
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Aashish Chaudhary
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "Magrathea/Planet.h"

#include "Usul/Interfaces/IViewMatrix.h"
#include "Usul/Interfaces/ICullSceneVisitor.h"
#include "Usul/Interfaces/IFrameStamp.h"
#include "Usul/Interfaces/IUpdateSceneVisitor.h"
#include "Usul/Trace/Trace.h"

#include "osgDB/Registry"
#include "osgUtil/CullVisitor"
#include "osgUtil/UpdateVisitor"

#include "ossimPlanet/ossimPlanetDatabasePager.h"
#include "ossimPlanet/ossimPlanetTextureLayerRegistry.h"
#include "ossim/base/ossimKeywordlist.h"

#include "wms/wms.h"

#include "osg/PolygonOffset"
#include "osg/Version"

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

  this->setDefaults();

  _planet->land()->resetGraph(); 	

  _databasePager->setExpiryDelay(0);
  _databasePager->setAcceptNewDatabaseRequests( true );
  _databasePager->setDatabasePagerThreadPause( false );	
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

  // Delete the manipulator.
  _manipulator = 0x0;

  // Remove all children.
  _planet->removeChild ( 0, _planet->getNumChildren () );
  _planet = 0x0;

  // Cancel the database pager.
  if ( _databasePager.valid () )
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
  //const ossimPlanetLandType landType ( ossimPlanetLandType_NORMALIZED_ELLIPSOID );
  const ossimPlanetLandType landType ( ossimPlanetLandType_ELLIPSOID );

  const bool  elevEnabled    ( true  );
  //  const bool  ephemerisFlag  ( false );
  const bool  hudEnabled     ( false );
  const float elevExag       ( 1.0   );
  const int   elevEstimate   ( 16    );    
  const int   levelDetail    ( 16    );    

  const ossimFilename elevCache( "" );

  _planet->land()->setLandType( landType );
  _planet->land()->setElevationEnabledFlag( elevEnabled );
  _planet->land()->setHeightExag( elevExag );
  _planet->land()->setElevationPatchSize( elevEstimate );
  _planet->land()->setMaxLevelDetail( levelDetail );
  _planet->land()->setElevationCacheDir( elevCache );
  //  _planet->setEnableEphemerisFlag( ephemerisFlag );
  _planet->setEnableHudFlag( hudEnabled );
  _planet->land()->setElevationCacheDir( elevCache );
  _planet->land()->resetGraph();

  _layerManipulator->setLand( _planet->land().get() );
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
    this->refreshLandTextures( layer->getExtents().get(), ossimPlanetLandRefreshType_TEXTURE );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a layer.
//
///////////////////////////////////////////////////////////////////////////////

void Planet::addLayer( const std::string& filename )
{
  osg::ref_ptr< ossimPlanetTextureLayer > layer ( ossimPlanetTextureLayerRegistry::instance()->createLayer(filename.c_str() ) );
  this->addLayer( layer.get() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a layer.
//
///////////////////////////////////////////////////////////////////////////////

void Planet::addLayer( ossimPlanetTextureLayer *layer  )
{
  if( 0x0 != layer )
  {    
    _planet->land()->referenceLayer()->addTop ( layer );

    //_textureLayerGroup->addTop( layer );    
    //index = _textureLayerGroup->findLayerIndex( layer );
    this->refreshLandTextures( layer->getExtents().get(), ossimPlanetLandRefreshType_TEXTURE );
  }
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
    this->refreshLandTextures( layer->getExtents().get(), ossimPlanetLandRefreshType_TEXTURE );
  }

  return index;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove a layer.
//
///////////////////////////////////////////////////////////////////////////////

void Planet::removeLayer( ossimPlanetTextureLayer *layer )
{
  _planet->land()->referenceLayer()->removeLayer ( layer );
  //_textureLayerGroup->removeLayer( layer );
  this->refreshLandTextures( layer->getExtents().get(), ossimPlanetLandRefreshType_TEXTURE );
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
  this->refreshLandTextures( layer->getExtents().get(), ossimPlanetLandRefreshType_TEXTURE );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Reset the graph.
//
///////////////////////////////////////////////////////////////////////////////

void Planet::reset()
{  
  _planet->land()->resetGraph(); 
}


///////////////////////////////////////////////////////////////////////////////
//
//  Reset the graph.
//
///////////////////////////////////////////////////////////////////////////////

void Planet::refreshLandTextures( ossimPlanetExtents* extents, ossimPlanetLandRefreshType refreshType )
{
  _planet->land()->resetGraph( extents, refreshType );
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
  return _planet->land()->getElevationEnabledFlag(); 
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set elevation enabled.
//
///////////////////////////////////////////////////////////////////////////////

void Planet::elevationEnabled( bool val ) 
{ 
  _planet->land()->setElevationEnabledFlag( val ); 
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
  //return _planet->getEnableEphemerisFlag();
  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the ephemeris flag.
//
///////////////////////////////////////////////////////////////////////////////

void Planet::ephemerisFlag( bool val )
{ 
  //_planet->setEnableEphemerisFlag( val );
  // Update??
}  


///////////////////////////////////////////////////////////////////////////////
//
//  Get the elevation exageration.
//
///////////////////////////////////////////////////////////////////////////////

float Planet::elevationExag() const
{ 
  return _planet->land()->getHeightExag();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the elevation exageration.
//
///////////////////////////////////////////////////////////////////////////////

void Planet::elevationExag( const float& elevExag )
{ 
  _planet->land()->setHeightExag( elevExag );
  reset(); 
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the elevation patch size.
//
///////////////////////////////////////////////////////////////////////////////

int Planet::elevationPatchSize() const
{ 
  return _planet->land()->getElevationPatchSize();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the elevation patch size.
//
///////////////////////////////////////////////////////////////////////////////

void  Planet::elevationPatchSize( const int& elevEstimate )
{ 
  _planet->land()->setElevationPatchSize( elevEstimate );
  reset(); 
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the maximium level of detail.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Planet::levelDetail() const
{ 
  return _planet->land()->getMaxLevelDetail();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the maximium level of detail.
//
///////////////////////////////////////////////////////////////////////////////

void Planet::levelDetail( unsigned int levelDetail )
{ 
  _planet->land()->setMaxLevelDetail( levelDetail );
  reset(); 
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the land type.
//
///////////////////////////////////////////////////////////////////////////////

void Planet::landType( ossimPlanetLandType landType )
{ 
  _planet->land()->setLandType( landType );
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
  ossimFilename name ( _planet->land()->getElevationCacheDir( ) );
  return name.c_str();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the elevation cache dir.
//
///////////////////////////////////////////////////////////////////////////////

void Planet::elevationCacheDir( const std::string& directory )
{
  _planet->land()->setElevationCacheDir( directory.c_str() );
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


///////////////////////////////////////////////////////////////////////////////
//
//  Pre-render notification.
//
///////////////////////////////////////////////////////////////////////////////

void Planet::preRender ( Usul::Interfaces::IUnknown *caller )
{
  USUL_TRACE_SCOPE;

  Usul::Interfaces::IFrameStamp::QueryPtr fs ( caller );
  if ( ( true == fs.valid() ) && ( 0x0 != fs->frameStamp() ) )
  {
    _databasePager->signalBeginFrame ( fs->frameStamp() );
    _databasePager->updateSceneGraph ( fs->frameStamp()->getReferenceTime() );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Pre-render notification.
//
///////////////////////////////////////////////////////////////////////////////

void Planet::postRender ( Usul::Interfaces::IUnknown *caller )
{
  USUL_TRACE_SCOPE;
 
  _databasePager->signalEndFrame();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize the cull and update visitors of the caller.
//
///////////////////////////////////////////////////////////////////////////////

void Planet::initVisitors ( Usul::Interfaces::IUnknown *caller )
{
  USUL_TRACE_SCOPE;

  // Set cull visitor's database pager.
  {
    Usul::Interfaces::ICullSceneVisitor::QueryPtr getVisitor ( caller );
    if ( true == getVisitor.valid() )
    {
      osg::ref_ptr<osgUtil::CullVisitor> visitor ( getVisitor->getCullSceneVisitor ( 0x0 ) );
      if ( true == visitor.valid() )
      {
        visitor->setDatabaseRequestHandler ( _databasePager.get() );
      }
    }
  }

  // Set update visitor's database pager.
  {
    Usul::Interfaces::IUpdateSceneVisitor::QueryPtr getVisitor ( caller );
    if ( true == getVisitor.valid() )
    {
      osg::ref_ptr<osg::NodeVisitor> visitor ( getVisitor->getUpdateSceneVisitor ( 0x0 ) );
      if ( true == visitor.valid() )
      {
        visitor->setDatabaseRequestHandler ( _databasePager.get() );
      }
    }
  }
}


/// Get/Set the split metric.
void Planet::splitMetric ( double value )
{
  _planet->land()->setSplitMetricRatio ( value );
  this->reset();
}

double Planet::splitMetric () const
{
  return _planet->land()->getSplitMetricRatio();
}


// Get the land model.
ossimPlanetLandModel* Planet::landModel()
{
  return _planet->land()->model().get();
}

const ossimPlanetLandModel* Planet::landModel() const
{
  return _planet->land()->model().get();
}
