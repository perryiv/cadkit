
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  DbStlArraySetters: Implementations of the abstract array-setting class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_DATABASE_STEREO_LITHOGRAPHY_LIBRARY_ARRAY_SETTERS_H_
#define _CADKIT_DATABASE_STEREO_LITHOGRAPHY_LIBRARY_ARRAY_SETTERS_H_

// I need this up here or else I get this VC++ error (and many others):
// SlTemplateSupport.h(79) : error C2039: 'sqrtl' : is not a member of '`global namespace''
#include <math.h>

#include "DbStlFunctions.h"

#include "Interfaces/IQueryVertices.h"
#include "Interfaces/IQueryNormals.h"

#include "Standard/SlVec2.h"
#include "Standard/SlVec3.h"
#include "Standard/SlVec4.h"

namespace CadKit
{
///////////////////////////////////////////////////////////////////////////////
//
//  DbStlVertexSetter.
//
///////////////////////////////////////////////////////////////////////////////

class DbStlVertexSetter : public IQueryShapeVerticesVec3f::VertexSetter
{
public:

  DbStlVertexSetter ( const VertexSetType &type ) : _type ( type ), _vertices ( new osg::Vec3Array ){}

  osg::Vec3Array *      getVertices()   { return _vertices.get(); }
  Primitives            getPrimitives() { return _primitives; }

  virtual bool          setData ( const unsigned int &index, const SlVec3f &vec );
  virtual bool          setSize ( const unsigned int &size );

  virtual bool          setNumPrimitives  ( const unsigned int &num ) { /* ignoring primitives */ }
  virtual bool          setPrimitiveRange ( const unsigned int &index, const unsigned int &start, const unsigned int &length ) { /* ignoring primitives */ }

protected:

  VertexSetType _type;
  SlRefPtr<SlVec3f> _vertices;

  osg::Primitive::Mode  _getPrimitiveMode() const;
};

/*namespace CadKit
{
template <const unsigned int id, class EntityHandle, class ArrayType> 
class IQueryVertices : public IQueryArray<id,ArrayType>
{
public:

  // Inherit from this and define the virtual functions.
  class VertexSetter : public IQueryArray<id,ArrayType>::ArraySetter
  {
  public:
    virtual bool setNumPrimitives  ( const unsigned int &num ) = 0;
    virtual bool setPrimitiveRange ( const unsigned int &index, const unsigned int &start, const unsigned int &length ) = 0;
  };

  // Get the set type.
  virtual bool            getVertexSetType ( EntityHandle entity, VertexSetType &type ) const = 0;

  // Get the vertices.
  virtual bool            getVertices ( EntityHandle entity, VertexSetter &setter ) const = 0;
};*/


///////////////////////////////////////////////////////////////////////////////
//
//  DbStlNormalSetter.
//
///////////////////////////////////////////////////////////////////////////////

class DbStlNormalSetter : public IQueryShapeNormalsVec3f::NormalSetter
{
public:
  
  typedef osg::Geometry::AttributeBinding Binding;

  DbStlNormalSetter ( const VertexBinding &binding ) : _binding ( CadKit::getBinding ( binding ) ), _normals ( new osg::Vec3Array ){}

  osg::Vec3Array *      getNormals() { return _normals.get(); }
  Binding               getBinding() { return _binding; }

  virtual bool          setData ( const unsigned int &index, const SlVec3f &vec );
  virtual bool          setSize ( const unsigned int &size );

protected:

  Binding _binding;
  osg::ref_ptr<osg::Vec3Array> _normals;
};



}; // namespace CadKit


#endif // _CADKIT_DATABASE_STEREO_LITHOGRAPHY_LIBRARY_ARRAY_SETTERS_H_
