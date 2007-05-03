
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Geometry for voxel models.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _VOXEL_MODEL_DRAWABLE_H_
#define _VOXEL_MODEL_DRAWABLE_H_

#include "OsgTools/Export.h"

#include "Usul/Math/Vector3.h"
#include "Usul/Math/Matrix44.h"

#include "osg/Drawable"
#include "osg/Version"

#include <string>
#include <vector>

namespace OsgTools {
namespace Drawables {

class OSG_TOOLS_EXPORT VoxelGeometry : public osg::Drawable
{
public:

  // Useful typedefs.
  typedef osg::Drawable BaseClass;
  typedef Usul::Math::Vec3d Vec3;
  typedef Usul::Math::Matrix44d Matrix44;
  typedef std::pair<Vec3,Vec3> Point; // 3D coordinates texture coordinates.
  typedef std::pair<Point,Point> Line;
  typedef std::vector<Line> Lines;
  typedef std::vector<Point> Polygon;
  typedef std::vector<Polygon> Polygons;

  // Construction.
  VoxelGeometry();
  VoxelGeometry ( const VoxelGeometry &d, const osg::CopyOp &options = osg::CopyOp::SHALLOW_COPY );

  // Assignment.
  VoxelGeometry &operator = ( const VoxelGeometry & );

  // Set/get the bounding box.
  void                        boundingBox ( const osg::BoundingBox &b ) { _bbox = b; }
  const osg::BoundingBox &    boundingBox() const { return _bbox; }

  // Implementation of osg::Object's cloning functions.
  virtual osg::Object         *clone ( const osg::CopyOp &options ) const;
  virtual osg::Object         *cloneType() const;

  // Draw voxels.
#if OSG_VERSION_MAJOR >= 1 && OSG_VERSION_MINOR >= 9
  virtual void                drawImplementation( osg::RenderInfo& renderInfo ) const;
#else
  virtual void                drawImplementation ( osg::State &state ) const;
#endif

  // Set/get the number of planes used.
  unsigned int                numPlanes() const { return _numPlanes; }
  void                        numPlanes ( unsigned int num ) { _numPlanes = num; }

protected:

  // Use reference counting.
  virtual ~VoxelGeometry();

  virtual osg::BoundingBox    computeBound() const;

  void                        _drawImplementation ( osg::State &state ) const;
  void                        _drawLines    ( const Matrix44 &imv, const Lines &lines ) const;
  void                        _drawPoint    ( const Point &p ) const;
  void                        _drawPolygon  ( const Polygon  &polygon  ) const;
  void                        _drawPolygons ( const Polygons &polygons ) const;

  void                        _initBorders     ( Lines &borders ) const;
  void                        _initBoundingBox ( const Matrix44 &mv, Lines &borders, osg::BoundingBox &box ) const;
  void                        _intersect       ( const Matrix44 &imv, const Lines &borders, float z, Polygon &polygon ) const;

  void                        _makePolygons ( const Matrix44 &imv, const osg::BoundingBox &bbox, const Lines &borders, Polygons &polygons ) const;

private:

  unsigned int _numPlanes;
  osg::BoundingBox _bbox;
};

}
}

#endif // _VOXEL_MODEL_DRAWABLE_H_
