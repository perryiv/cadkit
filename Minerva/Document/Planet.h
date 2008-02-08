
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_PLANET_WRAPPER_H__
#define __MINERVA_PLANET_WRAPPER_H__

#include "Minerva/Config.h"

#include "Usul/Base/Object.h"
#include "Usul/Interfaces/IUnknown.h"
#include "Usul/Math/Vector3.h"
#include "Usul/Pointers/Pointers.h"

#if USE_STAR_SYSTEM

#include "Usul/Jobs/Manager.h"

#include "StarSystem/System.h"
#include "StarSystem/Hud.h"

#else // OssimPlanet"

#include "osgDB/DatabasePager"

class ossimPlanet;
class ossimPlanetLandModel;
class ossimPlanetTextureLayer;
class ossimPlanetExtents;

#endif

#include "osg/NodeCallback"
#include "osgUtil/CullVisitor"

namespace Usul { namespace Interfaces { struct ILayer; } }

namespace Minerva {
namespace Document {

class Planet : public Usul::Base::Object
{
public:
  typedef Usul::Base::Object BaseClass;
  
  USUL_DECLARE_REF_POINTERS ( Planet );

  Planet();

  void                                            addLayer    ( Usul::Interfaces::ILayer *layer );  
  void                                            removeLayer ( Usul::Interfaces::ILayer *layer );

  osg::Group*                                     buildScene();
  
  /// Convert to planet coordinates.
  virtual void                                    convertToPlanet ( const Usul::Math::Vec3d& orginal, Usul::Math::Vec3d& planetPoint ) const;
  virtual void                                    convertFromPlanet ( const Usul::Math::Vec3d& planetPoint, Usul::Math::Vec3d& latLonPoint ) const;

  /// Dirty.
  virtual void                                    dirty ( Usul::Interfaces::IUnknown *caller );
  
  // Matrix to place items on the planet (i.e. local coordinates to world coordinates).
  virtual osg::Matrixd                            planetRotationMatrix ( double lat, double lon, double elevation, double heading ) const;
  
  // Get the elevation at a lat, lon (IElevationDatabase).
  virtual double                                  elevationAtLatLong ( double lat, double lon ) const;

  /// Get/Set the split metric.
  void                                            splitDistance ( double );
  double                                          splitDistance () const;
 
  // Pre- and post-render notifications.
  void                                            preRender  ( Usul::Interfaces::IUnknown *caller );
  void                                            postRender ( Usul::Interfaces::IUnknown *caller );
  
  // Initialize the cull and update visitors of the caller.
  void                                            initVisitors ( Usul::Interfaces::IUnknown *caller );

  // Update the scene.
  void                                            updateScene ( Usul::Interfaces::IUnknown *caller );
  
  // Set the pointer position.
  void                                            pointer ( const osg::Vec3& position );
protected:

  virtual ~Planet();
  
  // Callback to get the eye position.
  class Callback : public osg::NodeCallback
  {
  public:
    typedef osg::NodeCallback BaseClass;
    
    Callback () : BaseClass()
    {
    }
    
    virtual void operator()( osg::Node* node, osg::NodeVisitor* nv )
    { 
      switch( nv->getVisitorType() )
      {
        case osg::NodeVisitor::CULL_VISITOR:
        {
          osgUtil::CullVisitor* cullVisitor = dynamic_cast<osgUtil::CullVisitor*>( nv );
          if( cullVisitor )
          {
            _eye = cullVisitor->getEyePoint();
          }
          break;
        }
      }
          
      this->traverse( node, nv );
    }
      
      osg::Vec3d _eye;
  };

private:
  void                                            _init();

#if USE_STAR_SYSTEM
  StarSystem::System::RefPtr _system;
  Usul::Jobs::Manager *      _manager;
  StarSystem::Hud            _hud;
  osg::ref_ptr < Callback >  _callback;
#else
  ossimPlanet *                                   _planet;
  osg::ref_ptr< osgDB::DatabasePager >            _databasePager;
#endif
};

}
}

#endif // __MINERVA_PLANET_WRAPPER_H__
