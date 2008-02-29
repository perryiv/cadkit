
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_PLANET_WRAPPER_H__
#define __MINERVA_PLANET_WRAPPER_H__

#include "Usul/Base/Object.h"
#include "Usul/Interfaces/IUnknown.h"
#include "Usul/Math/Vector3.h"
#include "Usul/Pointers/Pointers.h"

#include "Usul/Jobs/Manager.h"

#include "StarSystem/System.h"
#include "StarSystem/Hud.h"


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
  
  Usul::Interfaces::ILayer*                       elevationData();
  Usul::Interfaces::ILayer*                       rasterData();

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
  
  /// Get/Set use skirts.
  void                                            useSkirts( bool b );
  bool                                            useSkirts() const;
 
  // Pre- and post-render notifications.
  void                                            preRender  ( Usul::Interfaces::IUnknown *caller );
  void                                            postRender ( Usul::Interfaces::IUnknown *caller );

  // Update the scene.
  void                                            updateScene ( Usul::Interfaces::IUnknown *caller );
  
  // Set the pointer position.
  void                                            pointer ( const osg::Vec3& position );

  /// Get/Set show compass state.
  void                                            showCompass( bool b );
  bool                                            showCompass() const;

protected:

  virtual ~Planet();
  
  // Callback to get the eye position.  This is a bit of a hack and needs to be improved.
  class Callback : public osg::NodeCallback
  {
  public:
    typedef osg::NodeCallback BaseClass;
    
    Callback () : BaseClass(), _hpr(), _eye(), _planet ( 0x0 )
    {
    }
    
    virtual void operator()( osg::Node* node, osg::NodeVisitor* nv );
      
    osg::Vec3d _hpr;
    osg::Vec3d _eye;
    Planet *_planet;
  };

private:
  void                                            _init();

  StarSystem::System::RefPtr _system;
  Usul::Jobs::Manager *      _manager;
  StarSystem::Hud            _hud;
  osg::ref_ptr < Callback >  _callback;
};

}
}

#endif // __MINERVA_PLANET_WRAPPER_H__
