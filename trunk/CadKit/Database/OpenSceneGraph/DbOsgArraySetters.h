
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

#ifdef _WIN32
# pragma warning(disable:4482) // Disable warning about use of enums.
#endif

// I need this up here or else I get this VC++ error (and many others):
// SlTemplateSupport.h(79) : error C2039: 'sqrtl' : is not a member of '`global namespace''
#include <math.h>

#include "DbOsgFunctions.h"

#include "Interfaces/IQueryVertices.h"
#include "Interfaces/IQueryNormals.h"
#include "Interfaces/IQueryColors.h"
#include "Interfaces/IQueryTexCoords.h"
#include "Interfaces/IQueryPrimOrigins.h"
#include "Interfaces/IQueryPrimParams.h"

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

  typedef std::vector< osg::ref_ptr<osg::PrimitiveSet> > Primitives;

  DbOsgVertexSetter ( const VertexSetType &type ) : _type ( type ), _vertices ( new osg::Vec3Array ){}

  osg::Vec3Array *      getVertices()   { return _vertices.get(); }
  Primitives            getPrimitives() { return _primitives; }

  virtual bool          setData ( const unsigned int &index, const SlVec3f &vec );
  virtual bool          setSize ( const unsigned int &size );

  virtual bool          setNumPrimitives  ( const unsigned int &num );
  virtual bool          setPrimitiveRange ( const unsigned int &index, const unsigned int &start, const unsigned int &length );

protected:

  VertexSetType _type;
  osg::ref_ptr<osg::Vec3Array> _vertices;
  Primitives _primitives;

  osg::PrimitiveSet::Mode  _getPrimitiveMode() const;
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

  DbOsgTextureCoordSetter() : _binding ( CadKit::getBinding ( CadKit::BINDING_PER_VERTEX ) ), _texCoords ( new osg::Vec2Array ){}

  osg::Vec2Array *      getTextureCoords() { return _texCoords.get(); }
  Binding               getBinding()       { return _binding; }

  virtual bool          setData ( const unsigned int &index, const SlVec2f &vec );
  virtual bool          setSize ( const unsigned int &size );

protected:

  Binding _binding;
  osg::ref_ptr<osg::Vec2Array> _texCoords;
};


///////////////////////////////////////////////////////////////////////////////
//
//  DbOsgOriginSetter.
//
///////////////////////////////////////////////////////////////////////////////

class DbOsgPrimOriginSetter : public IQueryPrimOriginsVec3f::OriginSetter
{
public:

  DbOsgPrimOriginSetter ( const PrimitiveType &type ) : _type ( type ), _origins ( new osg::Vec3Array ){}

  osg::Vec3Array *      getPrimOrigins()   { return _origins.get(); }
  PrimitiveType         getPrimType()      { return _type; }

  virtual bool          setData ( const unsigned int &index, const SlVec3f &vec );
  virtual bool          setSize ( const unsigned int &size );

protected:

  PrimitiveType _type;
  osg::ref_ptr<osg::Vec3Array> _origins;
};

///////////////////////////////////////////////////////////////////////////////
//
//  DbOsgPrimParamSetter.
//
///////////////////////////////////////////////////////////////////////////////

class DbOsgPrimParamSetter : public IQueryPrimParamsFloat::ParamSetter
{
public:

  DbOsgPrimParamSetter ( const PrimitiveType &type ) : _type ( type ), _params ( new osg::FloatArray ) {}

  osg::FloatArray*     getPrimParams()   { return _params.get(); }
  PrimitiveType   getPrimType()     { return _type; }

  virtual bool          setData ( const unsigned int &index, const float &val );
  virtual bool          setSize ( const unsigned int &size );

protected:

  PrimitiveType _type;
  osg::ref_ptr<osg::FloatArray> _params;
};

///////////////////////////////////////////////////////////////////////////////
//
//  DbOsgPrimColorSetter.
//
///////////////////////////////////////////////////////////////////////////////

class DbOsgPrimColorSetter : public IQueryPrimColorsVec4f::ColorSetter
{
public:
  
  typedef osg::Geometry::AttributeBinding Binding;

  DbOsgPrimColorSetter () :  _colors ( new osg::Vec4Array ){}

  osg::Vec4Array *      getColors()  { return _colors.get(); }
  Binding               getBinding() { return osg::Geometry::BIND_OVERALL; }

  virtual bool          setData ( const unsigned int &index, const SlVec4f &vec );
  virtual bool          setSize ( const unsigned int &size );

protected:

  osg::ref_ptr<osg::Vec4Array> _colors;
};


}; // namespace CadKit


#endif // _CADKIT_DATABASE_OPEN_SCENE_GRAPH_LIBRARY_ARRAY_SETTERS_H_
