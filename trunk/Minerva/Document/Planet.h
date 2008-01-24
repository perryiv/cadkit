
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

#else // OssimPlanet

#include "osgDB/DatabasePager"

class ossimPlanet;
class ossimPlanetLandModel;
class ossimPlanetTextureLayer;
class ossimPlanetExtents;

#endif

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
  
  // Get the elevation at a lat, lon (IElevationDatabase).
  virtual double                                  elevationAtLatLong ( double lat, double lon ) const;

  /// Get/Set the split metric.
  void                                            splitMetric ( double );
  double                                          splitMetric () const;
 
  // Pre- and post-render notifications.
  void                                            preRender  ( Usul::Interfaces::IUnknown *caller );
  void                                            postRender ( Usul::Interfaces::IUnknown *caller );
  
  // Initialize the cull and update visitors of the caller.
  void                                            initVisitors ( Usul::Interfaces::IUnknown *caller );

  // Update the scene.
  void                                            updateScene ( Usul::Interfaces::IUnknown *caller );
protected:

  virtual ~Planet();

private:
  void                                            _init();

#if USE_STAR_SYSTEM
  StarSystem::System::RefPtr _system;
  Usul::Jobs::Manager *      _manager;
#else
  ossimPlanet *                                   _planet;
  osg::ref_ptr< osgDB::DatabasePager >            _databasePager;
#endif
};

}
}

#endif // __MINERVA_PLANET_WRAPPER_H__
