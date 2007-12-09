
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

  /// Accept the visitor.
  virtual void            accept ( Minerva::Core::Visitor& visitor );

  /// Get the center.
  osg::Vec3               center () const;

  /// Get/Set the size.
  float                   size () const;
  void                    size ( float );

  /// Get/Set the secondary size.
  float                   secondarySize () const;
  void                    secondarySize ( float );

  ///Get/Set the primitiveId.
  unsigned int            primitiveId () const;
  void                    primitiveId ( unsigned int );

  /// Get/Set quality
  void                    quality( float value );
  float                   quality() const;

  /// Get/Set use auto transform flag.
  void                    autotransform ( bool b );
  bool                    autotransform () const;

protected:

  // Use reference counting
  virtual ~Point();

  osg::Node*            _buildGeometry( Usul::Interfaces::IUnknown* caller );

  osg::Node*            _buildPoint();
  osg::Node*            _buildSphere();
  osg::Node*            _buildCone( bool invert );
  osg::Node*            _buildDisk();
  osg::Node*            _buildCube();
  osg::Node*            _buildCylinder( Usul::Interfaces::IUnknown * caller );

  /// Build the scene branch for the data object.
  virtual osg::Node*    _preBuildScene( Usul::Interfaces::IUnknown* caller = 0x0 );

private:
  float        _size;
  float        _secondarySize;
  unsigned int _primitiveId;
  float        _quality;
  osg::Vec3    _center;
  osg::Vec3    _centerEarth;
  bool         _autotransform;

  osg::ref_ptr < osg::Material >         _material;
};


}
}
}

#endif // __DATA_OBJECTS_POINT_H__
