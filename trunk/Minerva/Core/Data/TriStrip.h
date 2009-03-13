
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Represents a tri-strip.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _MINERVA_CORE_DATA_TRI_STRIP_H_
#define _MINERVA_CORE_DATA_TRI_STRIP_H_

#include "Minerva/Core/Export.h"
#include "Minerva/Core/Data/Geometry.h"

#include "Usul/Math/Vector3.h"
#include "Usul/Math/Vector4.h"

#include <vector>


namespace Minerva {
namespace Core {
namespace Data {
      
      
class MINERVA_EXPORT TriStrip : public Geometry
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

  USUL_DECLARE_TYPE_ID ( TriStrip );
  USUL_DECLARE_QUERY_POINTERS ( TriStrip );

  TriStrip();

  /// Set the colors.
  void                    colors ( const Colors & );

  /// Is this geometry transparent?
  virtual bool            isSemiTransparent() const;

  /// Set the material.
  void                    material ( const Color &ambient, const Color &diffuse, const Color &specular = Color ( 0.2f, 0.2f, 0.2f, 1.0f ), const Color &emissive = Color ( 0.0f, 0.0f, 0.0f, 1.0f ), double shininess = 100.0f );

  /// Set the normal vectors.
  void                    normals ( const Normals & );

  /// Set the vertices.
  void                    vertices ( const Vertices & );

protected:
  
  /// Use reference counting.
  virtual ~TriStrip();
  
  /// Build the scene branch.
  virtual osg::Node *     _buildScene ( IUnknown *caller );
  
private:

  Vertices _v;
  Normals _n;
  Colors _c;
  Material _m;
};


}
}
}


#endif // _MINERVA_CORE_DATA_TRI_STRIP_H_
