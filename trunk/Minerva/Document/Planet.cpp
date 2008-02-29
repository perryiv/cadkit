
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Document/Planet.h"

#include "Usul/Interfaces/IElevationDatabase.h"
#include "Usul/Interfaces/IFrameStamp.h"
#include "Usul/Interfaces/ILayer.h"
#include "Usul/Interfaces/IRasterLayer.h"
#include "Usul/Interfaces/ITextMatrix.h"
#include "Usul/Interfaces/IUpdateSceneVisitor.h"
#include "Usul/Interfaces/IViewMatrix.h"
#include "Usul/Interfaces/IViewport.h"
#include "Usul/Math/Constants.h"
#include "Usul/Math/Functions.h"
#include "Usul/Trace/Trace.h"

#include "osgDB/Registry"

#include "StarSystem/BuildScene.h"
#include "StarSystem/Extents.h"
#include "StarSystem/LandModelEllipsoid.h"
#include "StarSystem/LandModelFlat.h"
#include "StarSystem/SplitCallbacks.h"

#include "osg/CoordinateSystemNode"

using namespace Minerva::Document;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Planet::Planet() : 
  BaseClass(),
  _system ( 0x0 ),
  _manager ( new Usul::Jobs::Manager ( 5, true ) ),
  _hud(),
  _callback ( 0x0 )
{
  // Make the system.
  _system = new StarSystem::System ( _manager );
  
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
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize.
//
///////////////////////////////////////////////////////////////////////////////

void Planet::_init()
{
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
  
  // Set the state.
  //osg::ref_ptr < osg::StateSet > ss ( _planet->getOrCreateStateSet () );
  //osg::ref_ptr< osg::PolygonOffset > offset ( new osg::PolygonOffset( 1.0f, 1.0f ) );
  //ss->setMode ( GL_POLYGON_OFFSET_FILL, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE );
  //ss->setAttribute( offset.get(), osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a layer.
//
///////////////////////////////////////////////////////////////////////////////

void Planet::addLayer( Usul::Interfaces::ILayer *layer  )
{
  Usul::Interfaces::IElevationDatabase::QueryPtr elevation ( layer );
  Usul::Interfaces::IRasterLayer::QueryPtr rl ( layer );
  if ( rl.valid() )
  {
    if ( elevation.valid() )
      _system->body()->elevationAppend ( rl.get() );
    else
      _system->body()->rasterAppend ( rl.get() );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove a layer.
//
///////////////////////////////////////////////////////////////////////////////

void Planet::removeLayer( Usul::Interfaces::ILayer *layer )
{
  Usul::Interfaces::IRasterLayer::QueryPtr rl ( layer );
  if ( rl.valid() )
    _system->body()->rasterRemove ( rl.get() );
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
}


///////////////////////////////////////////////////////////////////////////////
//
//  Pre-render notification.
//
///////////////////////////////////////////////////////////////////////////////

void Planet::preRender ( Usul::Interfaces::IUnknown *caller )
{
  USUL_TRACE_SCOPE;
  _system->preRender ( caller );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Pre-render notification.
//
///////////////////////////////////////////////////////////////////////////////

void Planet::postRender ( Usul::Interfaces::IUnknown *caller )
{
  USUL_TRACE_SCOPE;
  _system->postRender ( caller );
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
  osg::Vec3d out;
  _system->body()->latLonHeightToXYZ ( orginal[1], orginal[0], orginal[2], out );
  planetPoint.set ( out[0], out[1], out[2] );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Convert from planet coordinates.
//
///////////////////////////////////////////////////////////////////////////////

void Planet::convertFromPlanet ( const Usul::Math::Vec3d& planetPoint, Usul::Math::Vec3d& latLonPoint ) const
{
  _system->body()->xyzToLatLonHeight ( osg::Vec3f ( planetPoint[0], planetPoint[1], planetPoint[2] ), latLonPoint[1], latLonPoint[0], latLonPoint[2] );
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
  return _system->body()->elevation ( lat, lon );
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


Usul::Interfaces::ILayer* Planet::elevationData()
{
  return Usul::Interfaces::ILayer::QueryPtr ( _system->body()->elevationData().get() );
}

Usul::Interfaces::ILayer* Planet::rasterData()
{
  return Usul::Interfaces::ILayer::QueryPtr ( _system->body()->rasterData().get() );
}
