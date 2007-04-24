
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Class to represent point data.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __DATA_OBJECTS_POINT_H__
#define __DATA_OBJECTS_POINT_H__

#include "Minerva/Core/Export.h"
#include "Minerva/Core/DataObjects/DataObject.h"

#include "osg/ref_ptr"
#include "osg/Node"
#include "osg/Vec3"
#include "osg/Vec4"
#include "osg/MatrixTransform"
#include "osg/Material"
#include "osg/Geode"

namespace osg { class Drawable; }

namespace Minerva {
namespace Core {
namespace DataObjects {


class MINERVA_EXPORT Point : public DataObject
{
public:
  typedef Minerva::Core::DataObjects::DataObject BaseClass;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Point );

  enum PrimitiveTypes
  {
    POINT = 1,
    SPHERE,
    CONE,
    DISK,
    CUBE,
    INVERTED_CONE,
    CYLINDER
  };

  Point();

  /// Get/Set the size.
  float                   size () const;
  void                    size ( float );

  ///Get/Set the primitiveId.
  unsigned int            primitiveId () const;
  void                    primitiveId ( unsigned int );

  /// Build the scene branch for the data object.
  virtual osg::Node*      buildScene();

  /// Get/Set quality
  void                    quality( float value );
  float                   quality() const;

protected:

  // Use reference counting
  virtual ~Point();

  osg::Node*            _buildGeometry();

  osg::Node*            _buildPoint();
  osg::Node*            _buildSphere();
  osg::Node*            _buildCone( bool invert );
  osg::Node*            _buildDisk();
  osg::Node*            _buildCube();
  osg::Node*            _buildCylinder();

private:
  float        _size;
  unsigned int _primitiveId;
  float        _quality;
  unsigned int _srid;
  osg::Vec3    _orginalCenter;
  osg::Vec3    _convertedCenter;

  osg::ref_ptr < osg::Material >         _material;
  osg::ref_ptr< osg::Group >             _group;
};


}
}
}

#endif // __DATA_OBJECTS_POINT_H__
