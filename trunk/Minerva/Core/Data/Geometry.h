
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_CORE_GEOMETRY_H__
#define __MINERVA_CORE_GEOMETRY_H__

#include "Minerva/Core/Export.h"
#include "Minerva/Core/Extents.h"
#include "Minerva/Core/Data/Object.h"
#include "Minerva/Interfaces/IIntersectNotify.h"

#include "Usul/Base/Object.h"
#include "Usul/Interfaces/IBuildScene.h"
#include "Usul/Interfaces/IElevationDatabase.h"
#include "Usul/Interfaces/ILayerExtents.h"
#include "Usul/Math/Vector3.h"

#include "osg/Vec2d"
#include "osg/Vec3f"
#include "osg/Vec4"

#include <vector>

namespace Minerva {
namespace Core {
namespace Data {

class MINERVA_EXPORT Geometry : public Minerva::Core::Data::Object,
                                public Usul::Interfaces::ILayerExtents,
                                public Minerva::Interfaces::IIntersectNotify
{
public:
  typedef Minerva::Core::Data::Object               BaseClass;
  typedef Minerva::Core::Extents<osg::Vec2d>        Extents;
  typedef Usul::Interfaces::IUnknown                Unknown;
  typedef Usul::Math::Vec3d                         Point;
  typedef Minerva::Interfaces::IIntersectNotify     IIntersectNotify;
  typedef IIntersectNotify::Closest                 Closest;

  USUL_DECLARE_TYPE_ID ( Geometry );
  USUL_DECLARE_QUERY_POINTERS( Geometry );
  USUL_DECLARE_IUNKNOWN_MEMBERS;
  
  // Altitude Mode.
  enum AltitudeMode
  {
    CLAMP_TO_GROUND,
    RELATIVE_TO_GROUND,
    ABSOLUTE_MODE
  };
  
  /// Get/Set the altitude mode.
  void                  altitudeMode ( AltitudeMode mode );
  AltitudeMode          altitudeMode () const;

  /// Build the scene branch.
  osg::Node*            buildScene ( Usul::Interfaces::IUnknown* caller );
  
  /// Get/Set the dirty flag.
  void                  dirty ( bool b );
  bool                  dirty () const;
  
  /// Set/get the extents.
  void                  extents ( const Extents& e );
  Extents               extents() const;
  
  /// Get/Set extrude flag.
  void                  extrude ( bool b );
  bool                  extrude() const;
  
  /// Get the min latitude and min longitude (ILayerExtents).
  virtual double        minLon() const;
  virtual double        minLat() const;
  
  /// Get the max latitude and max longitude (ILayerExtents).
  virtual double        maxLon() const;
  virtual double        maxLat() const;
  
  /// Get/Set the render bin
  unsigned int          renderBin() const;
  void                  renderBin ( unsigned int );

  /// Call this when there is an intersection.
  virtual void          intersectNotify ( double x, double y, double z, double lon, double lat, double elev, Unknown::RefPtr tile, Unknown::RefPtr body, Unknown::RefPtr caller, Closest & );

  /// Is this geometry transparent?
  virtual bool          isSemiTransparent() const;

  Point                 spatialOffset () const;
  void                  spatialOffset ( const Point& );

  /// Update.
  virtual void          updateNotify ( Usul::Interfaces::IUnknown *caller );
  
protected:
	
  /// Construction/Destruction.
  Geometry();
  virtual ~Geometry();

  /// Build the scene branch.
  virtual osg::Node*    _buildScene ( Usul::Interfaces::IUnknown* caller ) = 0;
  
  template<class Vertex>
  double                _elevation ( const Vertex& point, Usul::Interfaces::IElevationDatabase* elevation ) const
  {
    switch ( this->altitudeMode() )
    {
      case CLAMP_TO_GROUND:
        return ( 0x0 != elevation ? elevation->elevationAtLatLong ( point[1], point[0] ) : 0.0 );
      case RELATIVE_TO_GROUND:
        return ( point[2] + ( 0x0 != elevation ? elevation->elevationAtLatLong ( point[1], point[0] ) : 0.0 ) );
      case ABSOLUTE_MODE:
        return point[2];
    }
    return 0.0;
  }
  
private:
  AltitudeMode _altitudeMode;
  Point        _offset;
  bool         _dirty;
  bool         _extrude;
  unsigned int _renderBin;
  Extents      _extents;
};

}
}
}

#endif // __MINERVA_CORE_GEOMETRY_H__
