
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Body class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _STAR_SYSTEM_BODY_CLASS_H_
#define _STAR_SYSTEM_BODY_CLASS_H_

#include "StarSystem/Node.h"

#include "Usul/Math/Vector2.h"
#include "Usul/Math/Vector3.h"
#include "Usul/Jobs/Manager.h"

namespace StarSystem { class Tile; class RasterLayer; class RasterGroup; }
namespace osg { class MatrixTransform; class Vec3f; }
class ossimEllipsoid;


namespace StarSystem {


class STAR_SYSTEM_EXPORT Body : public Node
{
public:

  // Useful typedefs.
  typedef Node BaseClass;
  typedef BaseClass::BuildOptions BuildOptions;
  typedef Usul::Math::Vec2d Vec2d;
  typedef Usul::Math::Vec3d Vec3d;

  // Helper macro for repeated code.
  STAR_SYSTEM_DEFINE_NODE_CLASS ( Body );

  // Enumerations.
  enum
  {
    RADIUS_EQUATOR = 0,
    RADIUS_POLAR   = 1
  };

  // Constructors
  Body ( const Vec2d &radii );

  // Set/get the center.
  void                      center ( const Vec3d & );
  Vec3d                     center() const;

  // Computes the "geodetic" radius for a given latitude in degrees.
  double                    geodeticRadius( double latitude ) const;

  // Convert lat, lon, height to x,y,z.
  void                      latLonHeightToXYZ ( double lat, double lon, double elevation, osg::Vec3f& point ) const;

  // Get the maximum radius.
  double                    maxRadius() const;

  // Set/get the radii.
  void                      radii ( const Vec2d & );
  Vec2d                     radii() const;

  // Append raster data.
  void                      rasterAppend ( RasterLayer * );

  // Get the scene.
  const osg::Node *         scene() const;
  osg::Node *               scene();

  // Get the thread pool for this body.
  Usul::Jobs::Manager&      jobManager();

protected:

  // Use reference counting.
  virtual ~Body();

private:

  // No copying or assignment.
  Body ( const Body & );
  Body &operator = ( const Body & );

  void                      _destroy();

  osg::MatrixTransform *_transform;
  ossimEllipsoid *_ellipsoid;
  Tile *_tile;
  RasterGroup *_rasters;
  Usul::Jobs::Manager _manager;
};


} // namespace StarSystem


#endif // _STAR_SYSTEM_BODY_CLASS_H_
