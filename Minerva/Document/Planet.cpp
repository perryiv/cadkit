
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Document/Planet.h"

#include "Usul/Bits/Bits.h"
#include "Usul/Interfaces/ICullSceneVisitor.h"
#include "Usul/Interfaces/IElevationDatabase.h"
#include "Usul/Interfaces/IFrameStamp.h"
#include "Usul/Interfaces/ILayer.h"
#include "Usul/Interfaces/IRasterLayer.h"
#include "Usul/Interfaces/ITextMatrix.h"
#include "Usul/Interfaces/IUpdateSceneVisitor.h"
#include "Usul/Interfaces/IViewMatrix.h"
#include "Usul/Interfaces/IViewport.h"
#include "Usul/Math/Functions.h"
#include "Usul/Trace/Trace.h"

#include "osgDB/Registry"
#include "osgUtil/CullVisitor"
#include "osgUtil/UpdateVisitor"

#include "ossim/init/ossimInit.h"

#if USE_STAR_SYSTEM
#include "Usul/Network/Names.h"
#include "Usul/Math/Constants.h"

#include "StarSystem/BuildScene.h"
#include "StarSystem/Extents.h"
#include "StarSystem/LandModelEllipsoid.h"
#include "StarSystem/LandModelFlat.h"
#include "StarSystem/SplitCallbacks.h"
#include "StarSystem/ClampNearFar.h"

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
  _manager ( new Usul::Jobs::Manager ( 5, true ) ),
  _hud(),
  _callback ( 0x0 )
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
  
  _hud.showCompass ( true );
  
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
  const double splitDistance ( land->size() * 2.5 );
  
  // Size of the mesh.
  Body::MeshSize meshSize ( 17, 17 );
  
  // Add the body.
  Body::RefPtr body ( new Body ( land, _manager, meshSize, splitDistance ) );
  body->useSkirts ( true );
  
  // Add tiles to the body.
  body->addTile ( Extents ( -180, -90,    0,   90 ) );
  body->addTile ( Extents (    0, -90,  180,   90 ) );

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
  Usul::Interfaces::IElevationDatabase::QueryPtr elevation ( layer );
  Usul::Interfaces::IRasterLayer::QueryPtr rl ( layer );
  if ( rl.valid() )
  {
    if ( elevation.valid() )
      _system->body()->elevationAppend ( rl.get() );
    else
      _system->body()->rasterAppend ( rl.get() );
  }
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
//  Dirty.
//
///////////////////////////////////////////////////////////////////////////////

void Planet::dirty ( Usul::Interfaces::IUnknown *caller )
{
  Usul::Interfaces::IRasterLayer::QueryPtr rl ( caller );
  if ( rl.valid() )
    _system->body()->rasterChanged ( rl.get() );
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
    group->addChild ( _hud.buildScene() );
    
    _callback = new Callback;
    _callback->_planet = this;
    group->setCullCallback ( _callback.get() );
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
#else
  Usul::Interfaces::ICullSceneVisitor::QueryPtr csv ( caller );
  if ( csv.valid() )
  {
    osg::ref_ptr<osgUtil::CullVisitor> cv ( csv->getCullSceneVisitor( 0x0 ) );
    cv->setClampProjectionMatrixCallback ( new StarSystem::ClampNearFar ( *cv ) );
    cv->setInheritanceMask ( Usul::Bits::remove ( cv->getInheritanceMask(), osg::CullSettings::CLAMP_PROJECTION_MATRIX_CALLBACK ) );
  }
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the split metric.
//
///////////////////////////////////////////////////////////////////////////////

void Planet::splitDistance ( double value )
{
  _system->body()->splitDistance ( value, true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the split metric.
//
///////////////////////////////////////////////////////////////////////////////

double Planet::splitDistance () const
{
  return _system->body()->splitDistance();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Convert to planet coordinates.
//
///////////////////////////////////////////////////////////////////////////////

void Planet::convertToPlanet ( const Usul::Math::Vec3d& orginal, Usul::Math::Vec3d& planetPoint ) const
{
#if USE_STAR_SYSTEM
  osg::Vec3d out;
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
//  Matrix to place items on the planet (i.e. local coordinates to world coordinates).
//
///////////////////////////////////////////////////////////////////////////////

osg::Matrixd Planet::planetRotationMatrix ( double lat, double lon, double elevation, double heading ) const
{
  return _system->body()->planetRotationMatrix ( lat, lon, elevation, heading ); 
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the elevation at a lat, lon (IElevationDatabase).
//
///////////////////////////////////////////////////////////////////////////////

double Planet::elevationAtLatLong ( double lat, double lon ) const
{
#if USE_STAR_SYSTEM
  return _system->body()->elevation ( lat, lon );
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
//  Convert matrix to heading, pitch, roll.
//  See:
//  http://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToEuler/
//  http://www.euclideanspace.com/maths/geometry/rotations/conversions/eulerToMatrix/index.htm
//
//  Implementation adapted from ossimPlanet (www.ossim.org).
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  void matrixToHpr( osg::Vec3d& hpr, const osg::Matrixd& rotation )
  {
    // Initialize answer.
    hpr.set ( 0.0, 0.0, 0.0 );
    
    osg::Matrixd mat;
    
    // I'm not really sure what the following code inside {} is about.
    {
      osg::Vec3d col1 ( rotation( 0, 0 ), rotation( 0, 1 ), rotation( 0, 2 ) );
      const double s ( col1.length() );
      
      if ( s <= 0.00001 )
      {
        hpr.set(0.0f, 0.0f, 0.0f);
        return;
      }
      
      const double oneOverS ( 1.0f / s );
      for( int i = 0; i < 3; i++ )
        for( int j = 0; j < 3; j++ )
          mat(i, j) = rotation(i, j) * oneOverS;
    }
    
    // Set the pitch
    hpr[1] = ::asin ( Usul::Math::clamp ( mat ( 1, 2 ), -1.0, 1.0 ) );
    
    double cp ( ::cos( hpr[1] ) );
    
    // See if the cosine of the pitch is close to zero.
    // This is for singularities at the north and south poles.
    if ( cp > -0.00001 && cp < 0.00001 )
    {
      const double cr ( Usul::Math::clamp (  mat( 0, 1 ), -1.0, 1.0 ) );
      const double sr ( Usul::Math::clamp ( -mat( 2, 1 ), -1.0, 1.0 ) );
      
      hpr[0] = 0.0f;
      hpr[2] = ::atan2 ( sr, cr );
    }
    else
    {
      // Remove the cosine of pitch from these elements.
      cp = 1.0 / cp ;
      double sr ( Usul::Math::clamp ( ( -mat( 0, 2 ) * cp ), -1.0, 1.0 ) );
      double cr ( Usul::Math::clamp ( (  mat( 2, 2 ) * cp ), -1.0, 1.0 ) );
      double sh ( Usul::Math::clamp ( ( -mat( 1, 0 ) * cp ), -1.0, 1.0 ) );
      double ch ( Usul::Math::clamp ( (  mat( 1, 1 ) * cp ), -1.0, 1.0 ) );
      
      if ( ( sh == 0.0f && ch == 0.0f ) || ( sr == 0.0f && cr == 0.0f ) )
      {
        cr = Usul::Math::clamp (  mat( 0, 1 ), -1.0, 1.0 );
        sr = Usul::Math::clamp ( -mat( 2, 1 ), -1.0, 1.0 );
        
        hpr[0] = 0.0f;
      }
      else
      {
        hpr[0] = ::atan2 ( sh, ch );
      }
      
      hpr[2] = ::atan2 ( sr, cr );
    }
    
    // Convert to degress.
    hpr[0] *= Usul::Math::RAD_TO_DEG;
    hpr[1] *= Usul::Math::RAD_TO_DEG;
    hpr[2] *= Usul::Math::RAD_TO_DEG;
    
    // Sign flip.
    hpr[0] *= -1.0;
  }
  
}

///////////////////////////////////////////////////////////////////////////////
//
//  Update the scene.
//
///////////////////////////////////////////////////////////////////////////////

void Planet::updateScene ( Usul::Interfaces::IUnknown *caller )
{
#if USE_STAR_SYSTEM

  if ( _callback.valid() )
  {
    osg::Vec3d hpr ( _callback->_hpr );
    _hud.hpr (  hpr[0], hpr[1], hpr[2] );
  }

  const unsigned int queued    ( ( 0x0 == _manager ) ? 0 : _manager->numJobsQueued() );
  const unsigned int executing ( ( 0x0 == _manager ) ? 0 : _manager->numJobsExecuting() );
    
  _hud.requests ( queued );
  _hud.running ( executing );
    
  Usul::Interfaces::IViewport::QueryPtr vp ( caller );
  if ( vp.valid() )
    _hud.updateScene( static_cast<unsigned int> ( vp->width() ), static_cast<unsigned int> ( vp->height() ) );
#else
  Usul::Interfaces::ITextMatrix::QueryPtr tm ( caller );
  if ( tm.valid() )
  {
    std::ostringstream os;
    const unsigned int requests  ( _databasePager->getFileRequestListSize()   );
    
    os << ( ( requests > 0 ) ? Usul::Strings::format ( "Requests: ", requests ) : "" );
    std::string text ( os.str() );
    tm->setText ( 15, 15, text, osg::Vec4f ( 1.0, 1.0, 0.0, 1.0 ) );
  }
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the pointer position.
//
///////////////////////////////////////////////////////////////////////////////

void Planet::pointer ( const osg::Vec3& position )
{
  Usul::Math::Vec3d point ( position[0], position[1], position[2] );
  Usul::Math::Vec3d latLonPoint;
  this->convertFromPlanet( point, latLonPoint );
  _hud.position( latLonPoint[1], latLonPoint[0], latLonPoint[2] );
  //_hud.position( position[1], position[1], position[2] );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get data from cull visitor.
//
///////////////////////////////////////////////////////////////////////////////

void Planet::Callback::operator()( osg::Node* node, osg::NodeVisitor* nv )
{ 
  if ( osg::NodeVisitor::CULL_VISITOR ==  nv->getVisitorType() )
  {
    osgUtil::CullVisitor* cullVisitor = dynamic_cast<osgUtil::CullVisitor*>( nv );
    if( cullVisitor )
    {
      // Set the eye position.
      _eye = cullVisitor->getEyePoint();

      if ( 0x0 != _planet )
      {
         // Convert the eye to lat,lon, height.
        Usul::Math::Vec3d point ( _eye[0], _eye[1], _eye[2] );
        Usul::Math::Vec3d latLonPoint;
        _planet->convertFromPlanet( point, latLonPoint );

        // Get the model view matrix from the cull visitor.
        osg::ref_ptr<osg::RefMatrix> m ( cullVisitor->getModelViewMatrix() );

        // Get the inverse of the view matrix.
        osg::Matrixd viewMatrix ( 0x0 != m.get() ? osg::Matrixd::inverse ( *m ) : osg::Matrixd() );
    
        // Get the matrix to point north at the eye position.
        osg::Matrixd localLsr ( _planet->planetRotationMatrix( latLonPoint[1], latLonPoint[0], latLonPoint[2], 0.0 ) ); 
    
        osg::Matrixd invert;
        if ( invert.invert ( localLsr ) )
        {
          osg::Matrixf matrix ( viewMatrix * invert );
          Detail::matrixToHpr( _hpr, matrix );
        }
      }
    }
  }
  
  this->traverse( node, nv );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set show compass state.
//
///////////////////////////////////////////////////////////////////////////////

void Planet::showCompass( bool b )
{
  _hud.showCompass ( b );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get show compass state.
//
///////////////////////////////////////////////////////////////////////////////

bool Planet::showCompass() const
{
  return _hud.showCompass();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set use skirts.
//
///////////////////////////////////////////////////////////////////////////////

void Planet::useSkirts( bool b )
{
  _system->body()->useSkirts ( b );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get use skirts.
//
///////////////////////////////////////////////////////////////////////////////

bool Planet::useSkirts() const
{
  return _system->body()->useSkirts();
}
