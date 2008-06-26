
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __PARADIS_MODEL_GROUP_H__
#define __PARADIS_MODEL_GROUP_H__

#include "Usul/Base/Referenced.h"
#include "Usul/Pointers/Pointers.h"

#include "osg/Array"
#include "osg/ref_ptr"
#include "osg/Geode"
#include "osg/MatrixTransform"
#include "osg/Group"
#include "osg/Material"
#include "osg/Geometry"

#include <map>

namespace Usul 
{ 
  namespace Interfaces 
  { 
    struct IDecimateTriangles; 
    struct ISmoothTriangles; 
    struct ISubdivideTriangles; 
  } 
}

class Group : public Usul::Base::Referenced
{
public:
  /// Typedefs
  typedef Usul::Base::Referenced                 BaseClass;
  typedef osg::Vec3Array                         Vertices;
  typedef osg::ref_ptr< Vertices >               VerticesPtr;
  typedef osg::Vec3Array                         Normals;
  typedef osg::ref_ptr< Normals >                NormalsPtr;
  typedef osg::DrawElementsUInt                  Elements;
  typedef osg::ref_ptr < Elements >              ElementsPtr;
  typedef osg::ref_ptr < osg::Group >            GroupPtr;
  typedef std::map < std::string,std::string >   Options;
  typedef osg::ref_ptr < osg::MatrixTransform >  TransformPtr;
  typedef osg::ref_ptr < osg::Geode >            GeodePtr;
  typedef Usul::Interfaces::IUnknown             Unknown;
  typedef osg::ref_ptr < osg::Material >         MaterialPtr;
  typedef osg::ref_ptr< osg::Geometry >          GeometryPtr;

  /// Type information.
  USUL_IMPLEMENT_INLINE_TYPE_ID ( Group );

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Group );

  Group ( Vertices *vertices, Normals *normalsT, Normals *normalsV, Elements *indices );

  osg::Node* buildScene ( const Options &options = Options(), Unknown *caller = 0x0 );

  /// Decimate
  void                         decimate ( Usul::Interfaces::IDecimateTriangles* decimate, float reduction );

  /// Smooth
  void                         smooth ( Usul::Interfaces::ISmoothTriangles *smooth, unsigned int numIterations );

  /// Subdivide
  void                         subdivide ( Usul::Interfaces::ISubdivideTriangles *subdivide, unsigned int numIterations );

  /// Usul::Interfaces::ISceneElement

  /// Set the visiblity flag.
  virtual void                 setVisibility ( bool b );

  /// Get the visiblity flag.
  virtual bool                 getVisibility ( ) const;

  /// Set the translation.
  virtual void                 setTranslation ( const osg::Vec3f& translate );

  /// Get the translation.
  virtual osg::Vec3f           getTranslation ( ) const;

  /// Set the rotation.
  virtual void                 setRotation ( const osg::Quat& rotation );

  /// Get the rotation.
  virtual osg::Quat            getRotation ( ) const;

  /// Set the diffuse color
  virtual void                 setDiffuseColor ( const osg::Vec4f& );

  /// Get the diffuse color
  virtual const osg::Vec4f&    getDiffuseColor ( ) const;

  /// Set the specular color
  virtual void                 setSpecularColor ( const osg::Vec4f& );

  /// Get the specular color
  virtual const osg::Vec4f&    getSpecularColor () const;

  /// Set the shininess value
  virtual void                 setShininess( float s );

  /// Get the shininess value
  virtual float                getShininess() const;

  /// Set the transparency value
  virtual void                 setTransparency ( float t );

  /// Get the transparency value
  virtual float                getTransparency () const;
protected:

  Group();

  /// Use reference counting.
  virtual ~Group();

  /// Initialize
  void                         _init();

private:

  VerticesPtr   _vertices;
  NormalsPtr    _normalsT;
  NormalsPtr    _normalsV;
  ElementsPtr   _indices;
  GroupPtr      _group;
  TransformPtr  _mt;
  GeodePtr      _geode;
  GeometryPtr   _geometry;
  MaterialPtr   _material;
};

#endif // __PARADIS_MODEL_GROUP_H__

