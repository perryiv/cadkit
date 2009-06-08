
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Represents an N x M mesh.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _MINERVA_CORE_DATA_REGULAR_MESH_H_
#define _MINERVA_CORE_DATA_REGULAR_MESH_H_

#include "Minerva/Core/Export.h"
#include "Minerva/Core/Data/Geometry.h"

#include "Usul/Math/Vector2.h"
#include "Usul/Math/Vector3.h"
#include "Usul/Math/Vector4.h"

#include <vector>


namespace Minerva {
namespace Core {
namespace Data {
      
      
class MINERVA_EXPORT RegularMesh : public Geometry
{
public:

  typedef Geometry BaseClass;
  typedef std::vector<Usul::Math::Vec3d> Vertices;
  typedef std::vector<Usul::Math::Vec3d> Normals;
  typedef Usul::Math::Vec4f Color;
  typedef std::vector<Color> Colors;
  typedef Usul::Math::Vector4<Color> MaterialColors;
  typedef std::pair<MaterialColors,double> Material;
  typedef Usul::Interfaces::IUnknown IUnknown;
  typedef Usul::Math::Vec2ui Vec2ui;

  USUL_DECLARE_TYPE_ID ( RegularMesh );
  USUL_DECLARE_QUERY_POINTERS ( RegularMesh );

  RegularMesh();

  /// Set the colors.
  void                    colors ( const Colors & );

  /// Is this geometry transparent?
  virtual bool            isSemiTransparent() const;

  /// Set the material.
  void                    material ( const Color &ambient, const Color &diffuse, const Color &specular = Color ( 0.2f, 0.2f, 0.2f, 1.0f ), const Color &emissive = Color ( 0.0f, 0.0f, 0.0f, 1.0f ), double shininess = 100.0f );

  /// Set the normal vectors.
  void                    normals ( const Normals & );

  /// Set the mesh size.
  void                    size ( unsigned int numRows, unsigned int numColumns );

  /// Set the vertices.
  void                    vertices ( const Vertices & );

protected:
  
  /// Use reference counting.
  virtual ~RegularMesh();
  
  /// Build the scene branch.
  virtual osg::Node *     _buildScene ( IUnknown *caller );
  
private:

  Vertices _v;
  Normals _n;
  Colors _c;
  Material _m;
  Vec2ui _size;
};


}
}
}


#endif // _MINERVA_CORE_DATA_REGULAR_MESH_H_
