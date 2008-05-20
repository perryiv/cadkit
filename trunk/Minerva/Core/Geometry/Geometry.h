
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
#include "Minerva/Interfaces/IOffset.h"

#include "Usul/Base/Object.h"
#include "Usul/Interfaces/IBuildScene.h"
#include "Usul/Interfaces/IElevationDatabase.h"
#include "Usul/Interfaces/ILayerExtents.h"

#include "osg/Vec2d"
#include "osg/Vec3f"
#include "osg/Vec4"

#include <vector>

namespace Minerva {
namespace Core {
namespace Geometry {

class MINERVA_EXPORT Geometry : public Usul::Base::Object,
                                public Usul::Interfaces::IBuildScene,
                                public Usul::Interfaces::ILayerExtents,
                                public Minerva::Interfaces::IOffset
{
public:
  typedef Usul::Base::Object                  BaseClass;
  typedef Minerva::Core::Extents<osg::Vec2d>  Extents;
  typedef Usul::Interfaces::IUnknown          Unknown;
  
  USUL_DECLARE_QUERY_POINTERS( Geometry );
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  Geometry();
  
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
  virtual osg::Node*    buildScene( const Options& options = Options(), Usul::Interfaces::IUnknown* caller = 0x0 );
  
  /// Get/Set the color.
  const osg::Vec4&      color () const;
  void                  color ( const osg::Vec4& );
  
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

  /// Get/Set the spatial reference id.
  unsigned int          srid () const;
  void                  srid ( unsigned int srid );
  
  /// Is this geometry transparent?
  bool                  transparent() const;
  
  // Is it valid?
  bool                  valid() const;

  /// Usul::Interfaces::IOffset
  virtual const osg::Vec3f&   spatialOffset () const;
  virtual void                spatialOffset ( const osg::Vec3f& );
  
protected:
  virtual ~Geometry();

  /// Build the scene branch.
  virtual osg::Node*    _buildScene( Usul::Interfaces::IUnknown* caller ) = 0;
  
  template<class Vertex>
  double                _elevation ( const Vertex& point, Usul::Interfaces::IElevationDatabase* elevation )
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
  int          _srid;
  osg::Vec4    _color;
  osg::Vec3f   _offset;
  bool         _dirty;
  bool         _extrude;
  unsigned int _renderBin;
  Extents      _extents;
};

}
}
}

#endif // __MINERVA_CORE_GEOMETRY_H__
