
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  DbOpenSgArraySetters: Implementations of the abstract array-setting class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_DATABASE_OPEN_SG_LIBRARY_ARRAY_SETTERS_H_
#define _CADKIT_DATABASE_OPEN_SG_LIBRARY_ARRAY_SETTERS_H_

#ifdef _WIN32
#include <winsock2.h>
#endif

// I need this up here or else I get this VC++ error (and many others):
// SlTemplateSupport.h(79) : error C2039: 'sqrtl' : is not a member of '`global namespace''
#include <math.h>

#include "DbOpenSgFunctions.h"

#include "Interfaces/IQueryVertices.h"
#include "Interfaces/IQueryNormals.h"
#include "Interfaces/IQueryColors.h"
#include "Interfaces/IQueryTexCoords.h"

#include "Standard/SlVec2.h"
#include "Standard/SlVec3.h"
#include "Standard/SlVec4.h"

#include <vector>

#include <OpenSG/OSGGeometry.h>


namespace CadKit
{
///////////////////////////////////////////////////////////////////////////////
//
//  DbOpenSgVertexSetter.
//
///////////////////////////////////////////////////////////////////////////////

class DbOpenSgVertexSetter : public IQueryShapeVerticesVec3f::VertexSetter
{
public:

  //typedef std::vector< osg::ref_ptr<osg::Primitive> > Primitives;

  DbOpenSgVertexSetter ( const VertexSetType &type ) : _type ( type ) {}

  osg::GeoPTypesPtr       getTypes();
  osg::GeoPLengthsPtr     getLengths();
  osg::GeoPositions3fPtr  getPositions();

  virtual bool            setData ( const unsigned int &index, const SlVec3f &vec );
  virtual bool            setSize ( const unsigned int &size );

  virtual bool            setNumPrimitives  ( const unsigned int &num );
  virtual bool            setPrimitiveRange ( const unsigned int &index, const unsigned int &start, const unsigned int &length );

private:

  VertexSetType _type;
  std::vector<osg::Vec3f> _vertices;
  
  struct Primitive {
    Primitive() : start(0), length(0) { }
    unsigned start;
    unsigned length;
  };
  
  std::vector<Primitive> _primitives;

  GLubyte _getPrimitiveMode() const;
};


///////////////////////////////////////////////////////////////////////////////
//
//  DbOpenSgNormalSetter.
//
///////////////////////////////////////////////////////////////////////////////

class DbOpenSgNormalSetter : public IQueryShapeNormalsVec3f::NormalSetter
{
public:

  DbOpenSgNormalSetter() {}

  osg::GeoNormals3fPtr  getNormals();

  virtual bool          setData ( const unsigned int &index, const SlVec3f &vec );
  virtual bool          setSize ( const unsigned int &size );

private:

  std::vector<osg::Vec3f> _normals;
};


///////////////////////////////////////////////////////////////////////////////
//
//  DbOpenSgColorSetter.
//
///////////////////////////////////////////////////////////////////////////////

class DbOpenSgColorSetter : public IQueryShapeColorsVec4f::ColorSetter
{
public:
  
  DbOpenSgColorSetter() {}

  osg::GeoColors4fPtr   getColors();

  virtual bool          setData ( const unsigned int &index, const SlVec4f &vec );
  virtual bool          setSize ( const unsigned int &size );

private:

  std::vector<osg::Color4f> _colors;
};


///////////////////////////////////////////////////////////////////////////////
//
//  DbOpenSgTextureCoordSetter.
//
///////////////////////////////////////////////////////////////////////////////

class DbOpenSgTextureCoordSetter : public IQueryShapeTexCoordsVec2f::TexCoordSetter
{
public:

  DbOpenSgTextureCoordSetter() {}

  osg::GeoTexCoords2fPtr  getTextureCoords();

  virtual bool            setData ( const unsigned int &index, const SlVec2f &vec );
  virtual bool            setSize ( const unsigned int &size );

private:

  std::vector<osg::Vec2f> _texCoords;
};


}; // namespace CadKit


#endif // _CADKIT_DATABASE_OPEN_SG_LIBRARY_ARRAY_SETTERS_H_
