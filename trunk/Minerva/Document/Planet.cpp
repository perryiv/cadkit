
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Document/Planet.h"

#include "Usul/Interfaces/ILayer.h"
#include "Usul/Interfaces/IViewMatrix.h"
#include "Usul/Interfaces/ICullSceneVisitor.h"
#include "Usul/Interfaces/IFrameStamp.h"
#include "Usul/Interfaces/IRasterLayer.h"
#include "Usul/Interfaces/ITextMatrix.h"
#include "Usul/Interfaces/IUpdateSceneVisitor.h"
#include "Usul/Trace/Trace.h"

#include "osgDB/Registry"
#include "osgUtil/CullVisitor"
#include "osgUtil/UpdateVisitor"

#include "ossim/init/ossimInit.h"

#if USE_STAR_SYSTEM
#include "Usul/Network/Names.h"

#include "StarSystem/BuildScene.h"
#include "StarSystem/Extents.h"
#include "StarSystem/LandModelEllipsoid.h"
#include "StarSystem/LandModelFlat.h"
#include "StarSystem/RasterLayerOssim.h"
#include "StarSystem/RasterLayerWms.h"
#include "StarSystem/ElevationLayerDem.h"
#include "StarSystem/SplitCallbacks.h"

#include "osg/CoordinateSystemNode"

#else

#include "Usul/Interfaces/IOssimPlanetLayer.h"

#include "ossimPlanet/ossimPlanet.h"
#include "ossimPlanet/ossimPlanetLand.h"
#include "ossimPlanet/ossimPlanetLandModel.h"
#include "ossimPlanet/ossimPlanetTextureLayerRegistry.h"

#include "wms/wms.h"

#include "osg/PolygonOffset"

#endif

using namespace Minerva::Document;


///////////////////////////////////////////////////////////////////////////////
//
//  Helper class to initialize and finalize.
//
///////////////////////////////////////////////////////////////////////////////

namespace
{
  struct OssimInit
  {
    OssimInit()
    {
      ossimInit::instance()->initialize();
    }
    ~OssimInit()
    {
      ossimInit::instance()->finalize();
    }
  } _ossimInitializer;
  
#if USE_STAR_SYSTEM == 0
  struct WmsInit
  {
    WmsInit()
    {
      wmsInitialize();
    }
    ~WmsInit()
    {
      wmsFinalize();
    }
  } _wmsInitializer;
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Planet::Planet() : 
  BaseClass(),
#if USE_STAR_SYSTEM
  _system ( 0x0 ),
  _manager ( new Usul::Jobs::Manager ( 5, true ) )
#else
  _planet            ( 0x0 ),
  _databasePager     ( new osgDB::DatabasePager() )
#endif
{
#if USE_STAR_SYSTEM

  // Make the system.
  _system = new StarSystem::System ( _manager );
#else  
  _planet                       = new ossimPlanet();
  Usul::Pointers::reference ( _planet );
#endif
  
  this->_init();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Planet::~Planet()
{
#if USE_STAR_SYSTEM
  // Delete the star-system.
  _system = 0x0;
  
  // Clean up job manager.
  if ( 0x0 != _manager )
  {
    // Remove all queued jobs and cancel running jobs.
    _manager->cancel();
    
    // Wait for remaining jobs to finish.
    _manager->wait();
    
    // Delete the manager.
    delete _manager; _manager = 0x0;
  }
#else
  // Remove all children.
  _planet->removeChild ( 0, _planet->getNumChildren () );
  
  // Unreference the planet.
  Usul::Pointers::unreference ( _planet ); _planet = 0x0;

  // Cancel the database pager.
  if ( _databasePager.valid () )
    _databasePager->cancel();

  _databasePager = 0x0;
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize.
//
///////////////////////////////////////////////////////////////////////////////

void Planet::_init()
{
#if USE_STAR_SYSTEM
  // Local typedefs to shorten the lines.
  typedef StarSystem::Body Body;
  typedef Body::Extents Extents;
  
  // Make the land model.
  typedef StarSystem::LandModelEllipsoid Land;
  Land::Vec2d radii ( osg::WGS_84_RADIUS_EQUATOR, osg::WGS_84_RADIUS_POLAR );
  Land::RefPtr land ( new Land ( radii ) );
  
  //StarSystem::LandModel::RefPtr land ( new StarSystem::LandModelFlat ( 32612 ) ); // UTM 12 WGS 84
  //StarSystem::LandModel::RefPtr land ( new StarSystem::LandModelFlat ( 32212 ) ); // UTM 12 WGS 72
  //StarSystem::LandModel::RefPtr land ( new StarSystem::LandModelFlat ( 26712 ) ); // UTM 12 NAD 27
  //StarSystem::LandModel::RefPtr land ( new StarSystem::LandModelFlat ( 26912 ) ); // UTM 12 NAD 83
  
  // Make a good split distance.
  const double splitDistance ( land->size() * 3 );
  
  // Size of the mesh.
  Body::MeshSize meshSize ( 17, 17 );
  
  // Add the body.
  Body::RefPtr body ( new Body ( land, _manager, meshSize, splitDistance ) );
  body->useSkirts ( true );
  
  // Add tiles to the body.
  body->addTile ( Extents ( -180, -90,    0,   90 ) );
  body->addTile ( Extents (    0, -90,  180,   90 ) );
  
  {
    const std::string url ( "http://onearth.jpl.nasa.gov/wms.cgi" );
    
    typedef StarSystem::RasterLayerWms::Options Options;
    Options options;
    options[Usul::Network::Names::LAYERS]  = "BMNG,global_mosaic";
    options[Usul::Network::Names::STYLES]  = "Jul,visual";
    options[Usul::Network::Names::SRS]     = "EPSG:4326";
    options[Usul::Network::Names::REQUEST] = "GetMap";
    options[Usul::Network::Names::FORMAT]  = "image/jpeg";
    
    typedef StarSystem::Body::Extents Extents;
    typedef Extents::Vertex Vertex;
    const Extents maxExtents ( Vertex ( -180, -90 ), Vertex ( 180, 90 ) );
    
    StarSystem::RasterLayerWms::RefPtr layer ( new StarSystem::RasterLayerWms ( maxExtents, url, options ) );
    body->rasterAppend ( layer.get() );
  }
  
  _system->body ( body.get() );
  
#else
  // Set the state.
  osg::ref_ptr < osg::StateSet > ss ( _planet->getOrCreateStateSet () );
  osg::ref_ptr< osg::PolygonOffset > offset ( new osg::PolygonOffset( 1.0f, 1.0f ) );
  ss->setMode ( GL_POLYGON_OFFSET_FILL, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE );
  ss->setAttribute( offset.get(), osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE );
  
  // Defaults. 
  //const ossimPlanetLandType landType ( ossimPlanetLandType_NORMALIZED_ELLIPSOID );
  const ossimPlanetLandType landType ( ossimPlanetLandType_ELLIPSOID );

  const bool  elevEnabled    ( true  );
  const bool  hudEnabled     ( false );
  const float elevExag       ( 1.0   );
  const int   elevEstimate   ( 16    );
  const int   levelDetail    ( 16    );

  _planet->land()->setLandType( landType );
  _planet->land()->setElevationEnabledFlag( elevEnabled );
  _planet->land()->setHeightExag( elevExag );
  _planet->land()->setElevationPatchSize( elevEstimate );
  _planet->land()->setMaxLevelDetail( levelDetail );
  _planet->setEnableHudFlag( hudEnabled );
  _planet->land()->resetGraph();
  
  _databasePager->setExpiryDelay(0);
  _databasePager->setAcceptNewDatabaseRequests( true );
  _databasePager->setDatabasePagerThreadPause( false );
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a layer.
//
///////////////////////////////////////////////////////////////////////////////

void Planet::addLayer( Usul::Interfaces::ILayer *layer  )
{
#if USE_STAR_SYSTEM
  Usul::Interfaces::IRasterLayer::QueryPtr rl ( layer );
  if ( rl.valid() )
    _system->body()->rasterAppend ( rl.get() );
#else
  Usul::Interfaces::IOssimPlanetLayer::QueryPtr ossim ( layer );
  if( ossim.valid() )
  {
    osg::ref_ptr<ossimPlanetTextureLayer> texture ( ossim->ossimPlanetLayer() );
    _planet->land()->referenceLayer()->addTop ( texture );
    
    // Force planet to build new textures.
    _planet->land()->resetGraph( texture->getExtents().get(), ossimPlanetLandRefreshType_TEXTURE );
  }
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove a layer.
//
///////////////////////////////////////////////////////////////////////////////

void Planet::removeLayer( Usul::Interfaces::ILayer *layer )
{
#if USE_STAR_SYSTEM
  Usul::Interfaces::IRasterLayer::QueryPtr rl ( layer );
  if ( rl.valid() )
    _system->body()->rasterRemove ( rl.get() );
#else
  Usul::Interfaces::IOssimPlanetLayer::QueryPtr ossim ( layer );
  if( ossim.valid() )
  {
    osg::ref_ptr<ossimPlanetTextureLayer> texture ( ossim->ossimPlanetLayer() );
    _planet->land()->referenceLayer()->removeLayer ( texture );
    
    // Force planet to build new textures.
    _planet->land()->resetGraph( texture->getExtents().get(), ossimPlanetLandRefreshType_TEXTURE );
  }
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the scene root.
//
///////////////////////////////////////////////////////////////////////////////

osg::Group* Planet::buildScene()
{
#if USE_STAR_SYSTEM
  osg::ref_ptr<osg::Group> group ( new osg::Group );
  if ( _system.valid() )
  {
    StarSystem::BuildScene::RefPtr builder ( new StarSystem::BuildScene );
    _system->accept ( *builder );
    group->addChild ( builder->scene() );
  }
  return group.release();
#else
  return _planet;
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Pre-render notification.
//
///////////////////////////////////////////////////////////////////////////////

void Planet::preRender ( Usul::Interfaces::IUnknown *caller )
{
  USUL_TRACE_SCOPE;

#if USE_STAR_SYSTEM
  _system->preRender ( caller );
#else
  Usul::Interfaces::IFrameStamp::QueryPtr fs ( caller );
  if ( ( true == fs.valid() ) && ( 0x0 != fs->frameStamp() ) && _databasePager.valid() )
  {
    _databasePager->signalBeginFrame ( fs->frameStamp() );
    _databasePager->updateSceneGraph ( fs->frameStamp()->getReferenceTime() );
  }
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Pre-render notification.
//
///////////////////////////////////////////////////////////////////////////////

void Planet::postRender ( Usul::Interfaces::IUnknown *caller )
{
  USUL_TRACE_SCOPE;
 
#if USE_STAR_SYSTEM
  _system->postRender ( caller );
#else
  if ( _databasePager.valid() )
    _databasePager->signalEndFrame();
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize the cull and update visitors of the caller.
//
///////////////////////////////////////////////////////////////////////////////

void Planet::initVisitors ( Usul::Interfaces::IUnknown *caller )
{
  USUL_TRACE_SCOPE;

#if USE_STAR_SYSTEM == 0
  
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
  
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the split metric.
//
///////////////////////////////////////////////////////////////////////////////

void Planet::splitMetric ( double value )
{
#if USE_STAR_SYSTEM == 0
  _planet->land()->setSplitMetricRatio ( value );
  _planet->land()->resetGraph();
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the split metric.
//
///////////////////////////////////////////////////////////////////////////////

double Planet::splitMetric () const
{
#if USE_STAR_SYSTEM
  return 0.0;
#else
  return _planet->land()->getSplitMetricRatio();
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Convert to planet coordinates.
//
///////////////////////////////////////////////////////////////////////////////

void Planet::convertToPlanet ( const Usul::Math::Vec3d& orginal, Usul::Math::Vec3d& planetPoint ) const
{
#if USE_STAR_SYSTEM
  osg::Vec3f out;
  _system->body()->latLonHeightToXYZ ( orginal[1], orginal[0], orginal[2], out );
#else
  const osg::Vec3d in ( orginal[1], orginal[0], orginal[2] );
  osg::Vec3d out;
  
  _planet->land()->model()->forward( in, out );
#endif
  planetPoint.set ( out[0], out[1], out[2] );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Convert from planet coordinates.
//
///////////////////////////////////////////////////////////////////////////////

void Planet::convertFromPlanet ( const Usul::Math::Vec3d& planetPoint, Usul::Math::Vec3d& latLonPoint ) const
{
#if USE_STAR_SYSTEM
  _system->body()->xyzToLatLonHeight ( osg::Vec3f ( planetPoint[0], planetPoint[1], planetPoint[2] ), latLonPoint[1], latLonPoint[0], latLonPoint[2] );
#else
  const osg::Vec3d in ( planetPoint[0], planetPoint[1], planetPoint[2] );
  osg::Vec3d out;
  
  _planet->land()->model()->inverse( in, out );
  
  latLonPoint.set ( out[1], out[0], out[2] );
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the elevation at a lat, lon (IElevationDatabase).
//
///////////////////////////////////////////////////////////////////////////////

double Planet::elevationAtLatLong ( double lat, double lon ) const
{
#if USE_STAR_SYSTEM
  return 0.0;
#else
  ossimGpt point ( lat, lon );
  double height (  ossimElevManager::instance()->getHeightAboveMSL( point ) );
  if( ossim::isnan ( height ) )
    height = 0.0;
  
  return height + ossimGeoidManager::instance()->offsetFromEllipsoid( point );
#endif  
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the scene.
//
///////////////////////////////////////////////////////////////////////////////

void Planet::updateScene ( Usul::Interfaces::IUnknown *caller )
{
  Usul::Interfaces::ITextMatrix::QueryPtr tm ( caller );
  if ( tm.valid() )
  {
    std::ostringstream os;
#if USE_STAR_SYSTEM
    const unsigned int queued    ( ( 0x0 == _manager ) ? 0 : _manager->numJobsQueued() );
    const unsigned int executing ( ( 0x0 == _manager ) ? 0 : _manager->numJobsExecuting() );
    const unsigned int total     ( queued + executing );
    
    const std::string out ( ( total > 0 ) ? ( Usul::Strings::format ( "Queued: ", queued, ", Running: ", executing ) ) : "" );
    os << out;
#else
    const unsigned int requests  ( _databasePager->getFileRequestListSize()   );
    
    os << ( ( requests > 0 ) ? Usul::Strings::format ( "Requests: ", requests ) : "" );
#endif
    std::string text ( os.str() );
    tm->setText ( 15, 15, text, osg::Vec4f ( 1.0, 1.0, 1.0, 1.0 ) );
  }
}
