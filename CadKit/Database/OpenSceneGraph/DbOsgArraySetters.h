
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  DbOsgArraySetters: Implementations of the abstract array-setting class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_DATABASE_OPEN_SCENE_GRAPH_LIBRARY_ARRAY_SETTERS_H_
#define _CADKIT_DATABASE_OPEN_SCENE_GRAPH_LIBRARY_ARRAY_SETTERS_H_

// I need this up here or else I get this VC++ error (and many others):
// SlTemplateSupport.h(79) : error C2039: 'sqrtl' : is not a member of '`global namespace''
#include <math.h>

#include "DbOsgFunctions.h"

#include "Interfaces/IQueryVertices.h"
#include "Interfaces/IQueryNormals.h"
#include "Interfaces/IQueryColors.h"
#include "Interfaces/IQueryTexCoords.h"

#include "Standard/SlVec2.h"
#include "Standard/SlVec3.h"
#include "Standard/SlVec4.h"

#include "osg/Geometry"


namespace CadKit
{
///////////////////////////////////////////////////////////////////////////////
//
//  DbOsgVertexSetter.
//
///////////////////////////////////////////////////////////////////////////////

class DbOsgVertexSetter : public IQueryShapeVerticesVec3f::VertexSetter
{
public:

  typedef std::vector< osg::ref_ptr<osg::Primitive> > Primitives;

  DbOsgVertexSetter ( const IQueryShapeVerticesVec3f::Type &type ) : _type ( type ), _vertices ( new osg::Vec3Array ){}

  osg::Vec3Array *      getVertices()   { return _vertices.get(); }
  Primitives            getPrimitives() { return _primitives; }

  virtual bool          setData ( const unsigned int &index, const SlVec3f &vec );
  virtual bool          setSize ( const unsigned int &size );

  virtual bool          setNumPrimitives  ( const unsigned int &num );
  virtual bool          setPrimitiveRange ( const unsigned int &index, const unsigned int &start, const unsigned int &length );

protected:

  IQueryShapeVerticesVec3f::Type _type;
  osg::ref_ptr<osg::Vec3Array> _vertices;
  Primitives _primitives;

  osg::Primitive::Mode _getPrimitiveMode() const;
};


///////////////////////////////////////////////////////////////////////////////
//
//  DbOsgNormalSetter.
//
///////////////////////////////////////////////////////////////////////////////

class DbOsgNormalSetter : public IQueryShapeNormalsVec3f::NormalSetter
{
public:
  
  typedef osg::Geometry::AttributeBinding Binding;

  DbOsgNormalSetter ( const VertexBinding &binding ) : _binding ( CadKit::getBinding ( binding ) ), _normals ( new osg::Vec3Array ){}

  osg::Vec3Array *      getNormals() { return _normals.get(); }
  Binding               getBinding() { return _binding; }

  virtual bool          setData ( const unsigned int &index, const SlVec3f &vec );
  virtual bool          setSize ( const unsigned int &size );

protected:

  Binding _binding;
  osg::ref_ptr<osg::Vec3Array> _normals;
};


///////////////////////////////////////////////////////////////////////////////
//
//  DbOsgColorSetter.
//
///////////////////////////////////////////////////////////////////////////////

class DbOsgColorSetter : public IQueryShapeColorsVec4f::ColorSetter
{
public:
  
  typedef osg::Geometry::AttributeBinding Binding;

  DbOsgColorSetter ( const VertexBinding &binding ) : _binding ( CadKit::getBinding ( binding ) ), _colors ( new osg::Vec4Array ){}

  osg::Vec4Array *      getColors()  { return _colors.get(); }
  Binding               getBinding() { return _binding; }

  virtual bool          setData ( const unsigned int &index, const SlVec4f &vec );
  virtual bool          setSize ( const unsigned int &size );

protected:

  Binding _binding;
  osg::ref_ptr<osg::Vec4Array> _colors;
};


///////////////////////////////////////////////////////////////////////////////
//
//  DbOsgTextureCoordSetter.
//
///////////////////////////////////////////////////////////////////////////////

class DbOsgTextureCoordSetter : public IQueryShapeTexCoordsVec2f::TexCoordSetter
{
public:
  
  typedef osg::Geometry::AttributeBinding Binding;

  DbOsgTextureCoordSetter() : _texCoords ( new osg::Vec2Array ), _binding ( osg::Geometry::BIND_OFF ){}

  osg::Vec2Array *      getTextureCoords() { return _texCoords.get(); }
  Binding               getBinding()       { return _binding; }

  virtual bool          setData ( const unsigned int &index, const SlVec2f &vec );
  virtual bool          setSize ( const unsigned int &size );

protected:

  Binding _binding;
  osg::ref_ptr<osg::Vec2Array> _texCoords;
};


}; // namespace CadKit


#endif // _CADKIT_DATABASE_OPEN_SCENE_GRAPH_LIBRARY_ARRAY_SETTERS_H_
