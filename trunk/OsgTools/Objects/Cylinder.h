
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Represents a cylinder.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _OSG_TOOLS_OBJECTS_CYLINDER_OBJECT_H_
#define _OSG_TOOLS_OBJECTS_CYLINDER_OBJECT_H_

#include "OsgTools/Objects/RegularGrid.h"

#include "Usul/Math/Vector3.h"
#include "Usul/Math/Vector2.h"

#include "osg/PrimitiveSet"
#include "osg/ref_ptr"


namespace OsgTools {
namespace Objects {


class OSG_TOOLS_EXPORT Cylinder : public OsgTools::Objects::RegularGrid
{
public:

  // Typedefs.
  typedef OsgTools::Objects::RegularGrid BaseClass;
  typedef Usul::Math::Vec3d Vec3d;
  typedef Usul::Math::Vec2d Vec2d;
  typedef osg::DrawElementsUShort Indices;
  typedef osg::ref_ptr < Indices > IndicesPtr;

  // Boiler-plate code.
  OSG_TOOLS_OBJECTS_DECLARE_CLASS ( Cylinder );

  // Constructors
  Cylinder();
  Cylinder ( const Vec2d &radii, double height );
  Cylinder ( const Vec2d &radii, const Vec3d &bottom, const Vec3d &top );
  Cylinder ( const Vec2d &radii, double height, const Vec3d &bottom, const Vec3d &axis );
  Cylinder ( const Cylinder & );

  // Get/set the axis.
  Vec3d                               axis() const;
  void                                axis ( const Vec3d & );

  // Get/set the bottom.
  Vec3d                               bottom() const;
  void                                bottom ( const Vec3d & );

  // Get/set the height.
  double                              height() const;
  void                                height ( double );

  // Get/set the radii.
  Vec2d                               radii() const;
  void                                radii ( const Vec2d & );

  // Get/set the top. Setting the top will modify the axis and height.
  Vec3d                               top() const;
  void                                top ( const Vec3d & );

  // Update the object.
  virtual void                        update ( Usul::Interfaces::IUnknown *caller );

protected:

  virtual ~Cylinder();

  virtual void                        _setPointValues();
  virtual void                        _setColorValues();
  virtual void                        _setTexCoordValues();

private:

  // No assignment.
  Cylinder &operator = ( const Cylinder & );

  void                                _destroy();

  Vec2d _radii;
  double _height;
  Vec3d _bottom;
  Vec3d _axis;
  IndicesPtr _indices;
};


}; // namespace Objects
}; // namespace OsgTools


#endif // _OSG_TOOLS_OBJECTS_CYLINDER_OBJECT_H_
