
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

#include "DbOpenSgArraySetters.h"
#include <OpenSG/OSGSimpleGeometry.h>

using namespace CadKit;


osg::GeoPTypesPtr DbOpenSgVertexSetter::getTypes()
{
  osg::GeoPTypesPtr types = osg::GeoPTypesUI8::create();
  osg::beginEditCP(types);
  for ( size_t i = 0; i < _primitives.size(); ++i )
    types->addValue( _getPrimitiveMode() );
  osg::endEditCP(types);
  return types;
}

osg::GeoPLengthsPtr DbOpenSgVertexSetter::getLengths()
{
  osg::GeoPLengthsPtr lengths = osg::GeoPLengthsUI32::create();
  osg::beginEditCP(lengths);
  for ( size_t i = 0; i < _primitives.size(); ++i )
    lengths->addValue( _primitives[i].length );
  osg::endEditCP(lengths);
  return lengths;
}

osg::GeoPositions3fPtr DbOpenSgVertexSetter::getPositions()
{
  osg::GeoPositions3fPtr points = osg::GeoPositions3f::create();
  osg::beginEditCP ( points );
  
  for ( size_t i = 0; i < _primitives.size(); ++i )
  {
    unsigned start  = _primitives[i].start;
    unsigned length = _primitives[i].length;
    SL_ASSERT ( start + length <= _vertices.size() );
    
    for ( unsigned j = start; j < start + length; ++j )
      points->addValue( _vertices[j] );
  }
  
  osg::endEditCP ( points );
  return points;
}


osg::GeoNormals3fPtr DbOpenSgNormalSetter::getNormals()
{
  osg::GeoNormals3fPtr normals = osg::GeoNormals3f::create();
  osg::beginEditCP ( normals );
  
  for ( size_t i = 0; i < _normals.size(); ++i)
    normals->addValue ( _normals [ i ] );
  
  osg::endEditCP ( normals );
  return normals;
}


osg::GeoColors4fPtr DbOpenSgColorSetter::getColors()
{
  osg::GeoColors4fPtr colors = osg::GeoColors4f::create();
  osg::MFColor4f *c = colors->getFieldPtr();
  osg::beginEditCP ( colors );
  
  for ( size_t i = 0; i < _colors.size(); ++i )
    c->push_back ( _colors [ i ] );
  
  osg::endEditCP ( colors );
  return colors;
}

osg::GeoTexCoords2fPtr DbOpenSgTextureCoordSetter::getTextureCoords()
{
  osg::GeoTexCoords2fPtr coords = osg::GeoTexCoords2f::create();
  osg::beginEditCP ( coords );
  
  for ( size_t i = 0; i < _texCoords.size(); ++i)
    coords->addValue ( _texCoords [ i ] );
  
  osg::endEditCP ( coords );
  return coords;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the data.
//
///////////////////////////////////////////////////////////////////////////////

bool DbOpenSgVertexSetter::setData ( const unsigned int &index, const SlVec3f &vec )
{
  // If we're out of range then resize.
  if ( index >= _vertices.size() )
    if ( false == this->setSize ( index + 1 ) )
      return false;

  // Set the vertex.
  _vertices[index][0] = vec[0];
  _vertices[index][1] = vec[1];
  _vertices[index][2] = vec[2];

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the data.
//
///////////////////////////////////////////////////////////////////////////////

bool DbOpenSgNormalSetter::setData ( const unsigned int &index, const SlVec3f &vec )
{
  // If we're out of range then resize.
  if ( index >= _normals.size() )
    if ( false == this->setSize ( index + 1 ) )
      return false;

  // Set the vertex.
  _normals[index][0] = vec[0];
  _normals[index][1] = vec[1];
  _normals[index][2] = vec[2];

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the data.
//
///////////////////////////////////////////////////////////////////////////////

bool DbOpenSgColorSetter::setData ( const unsigned int &index, const SlVec4f &vec )
{
  // If we're out of range then resize.
  if ( index >= _colors.size() )
    if ( false == this->setSize ( index + 1 ) )
      return false;

  // Set the vertex.
  _colors[index][0] = vec[0];
  _colors[index][1] = vec[1];
  _colors[index][2] = vec[2];
  _colors[index][3] = vec[3];

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the data.
//
///////////////////////////////////////////////////////////////////////////////

bool DbOpenSgTextureCoordSetter::setData ( const unsigned int &index, const SlVec2f &vec )
{
  // If we're out of range then resize.
  if ( index >= _texCoords.size() )
    if ( false == this->setSize ( index + 1 ) )
      return false;

  // Set the vertex.
  _texCoords[index][0] = vec[0];
  _texCoords[index][1] = vec[1];

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the primitive length.
//
///////////////////////////////////////////////////////////////////////////////

bool DbOpenSgVertexSetter::setPrimitiveRange ( const unsigned int &index, const unsigned int &start, const unsigned int &length )
{
  // If we're out of range then resize.
  if ( index >= _primitives.size() )
    if ( false == this->setNumPrimitives ( index + 1 ) )
      return false;

  _primitives[index].start  = start;
  _primitives[index].length = length;

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the primitive mode.
//
///////////////////////////////////////////////////////////////////////////////

GLubyte DbOpenSgVertexSetter::_getPrimitiveMode() const
{
  switch ( _type )
  {
  case CadKit::LINE_STRIP_SET: return GL_LINE_STRIP;
  case CadKit::POINT_SET:      return GL_POINTS;
  case CadKit::POLYGON_SET:    return GL_POLYGON;
  case CadKit::TRI_STRIP_SET:  return GL_TRIANGLE_STRIP;
  case CadKit::TRI_FAN_SET:    return GL_TRIANGLE_FAN;
  default:
    SL_ASSERT ( 0 );                      // What type is this?
    return GL_LINE_STRIP; // Play it safe.
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the number of primitives.
//
///////////////////////////////////////////////////////////////////////////////

bool DbOpenSgVertexSetter::setNumPrimitives ( const unsigned int &num )
{
  SL_ASSERT ( num > 0 );
  _primitives.resize ( num );
  return num == _primitives.size();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the size.
//
///////////////////////////////////////////////////////////////////////////////

bool DbOpenSgVertexSetter::setSize ( const unsigned int &size )
{
  SL_ASSERT ( size > 0 );
  _vertices.resize ( size );
  return size == _vertices.size();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the size.
//
///////////////////////////////////////////////////////////////////////////////

bool DbOpenSgNormalSetter::setSize ( const unsigned int &size )
{
  SL_ASSERT ( size > 0 );
  _normals.resize ( size );
  return size == _normals.size();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the size.
//
///////////////////////////////////////////////////////////////////////////////

bool DbOpenSgColorSetter::setSize ( const unsigned int &size )
{
  SL_ASSERT ( size > 0 );
  _colors.resize ( size );
  return size == _colors.size();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the size.
//
///////////////////////////////////////////////////////////////////////////////

bool DbOpenSgTextureCoordSetter::setSize ( const unsigned int &size )
{
  SL_ASSERT ( size > 0 );
  _texCoords.resize ( size );
  return size == _texCoords.size();
}
