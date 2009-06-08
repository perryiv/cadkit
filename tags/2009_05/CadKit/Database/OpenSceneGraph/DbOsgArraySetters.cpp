
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

#include "DbOsgArraySetters.h"

using namespace CadKit;


///////////////////////////////////////////////////////////////////////////////
//
//  Set the data.
//
///////////////////////////////////////////////////////////////////////////////

bool DbOsgVertexSetter::setData ( const unsigned int &index, const SlVec3f &vec )
{
  // If we're out of range then resize.
  if ( index >= _vertices->size() )
    if ( false == this->setSize ( index + 1 ) )
      return false;

  // Set the vertex.
  (*_vertices)[index].set ( vec[0], vec[1], vec[2] );

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the data.
//
///////////////////////////////////////////////////////////////////////////////

bool DbOsgNormalSetter::setData ( const unsigned int &index, const SlVec3f &vec )
{
  // If we're out of range then resize.
  if ( index >= _normals->size() )
    if ( false == this->setSize ( index + 1 ) )
      return false;

  // Set the vertex.
  (*_normals)[index].set ( vec[0], vec[1], vec[2] );

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the data.
//
///////////////////////////////////////////////////////////////////////////////

bool DbOsgColorSetter::setData ( const unsigned int &index, const SlVec4f &vec )
{
  // If we're out of range then resize.
  if ( index >= _colors->size() )
    if ( false == this->setSize ( index + 1 ) )
      return false;

  // Set the vertex.
  (*_colors)[index].set ( vec[0], vec[1], vec[2], vec[3] );

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the data.
//
///////////////////////////////////////////////////////////////////////////////

bool DbOsgTextureCoordSetter::setData ( const unsigned int &index, const SlVec2f &vec )
{
  // If we're out of range then resize.
  if ( index >= _texCoords->size() )
    if ( false == this->setSize ( index + 1 ) )
      return false;

  // Set the vertex.
  (*_texCoords)[index].set ( vec[0], vec[1] );

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the data.
//
///////////////////////////////////////////////////////////////////////////////

bool DbOsgPrimOriginSetter::setData ( const unsigned int &index, const SlVec3f &vec )
{
  // If we're out of range then resize.
  if ( index >= _origins->size() )
    if ( false == this->setSize ( index + 1 ) )
      return false;

  // Set the vertex.
  (*_origins)[index].set ( vec[0], vec[1], vec[2] );

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the data.
//
///////////////////////////////////////////////////////////////////////////////

bool DbOsgPrimParamSetter::setData ( const unsigned int &index, const float &val )
{
  // If we're out of range then resize.
  if ( index >= _params->size() )
    if ( false == this->setSize ( index + 1 ) )
      return false;

  // Set the vertex.
  (*_params)[index] = val;

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the data.
//
///////////////////////////////////////////////////////////////////////////////

bool DbOsgPrimColorSetter::setData ( const unsigned int &index, const SlVec4f &vec )
{
  // If we're out of range then resize.
  if ( index >= _colors->size() )
    if ( false == this->setSize ( index + 1 ) )
      return false;

  // Set the vertex.
  (*_colors)[index].set ( vec[0], vec[1], vec[2], vec[3] );

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the primitive length.
//
///////////////////////////////////////////////////////////////////////////////

bool DbOsgVertexSetter::setPrimitiveRange ( const unsigned int &index, const unsigned int &start, const unsigned int &length )
{
  // If we're out of range then resize.
  if ( index >= _primitives.size() )
    if ( false == this->setNumPrimitives ( index + 1 ) )
      return false;

  // Map our type to osg's mode.
  osg::PrimitiveSet::Mode mode = this->_getPrimitiveMode();

  // Set the primitive.
  _primitives[index] = new osg::DrawArrays ( mode, start, length );

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the primitive mode.
//
///////////////////////////////////////////////////////////////////////////////

osg::PrimitiveSet::Mode DbOsgVertexSetter::_getPrimitiveMode() const
{
  switch ( _type )
  {
  case CadKit::LINE_STRIP_SET:
    return osg::PrimitiveSet::LINE_STRIP;
  case CadKit::POINT_SET:
    return osg::PrimitiveSet::POINTS;
  case CadKit::POLYGON_SET:
    return osg::PrimitiveSet::POLYGON;
  case CadKit::TRI_STRIP_SET:
    return osg::PrimitiveSet::TRIANGLE_STRIP;
  case CadKit::TRI_FAN_SET:
    return osg::PrimitiveSet::TRIANGLE_FAN;
  default:
    SL_ASSERT ( 0 );                      // What type is this?
    return osg::PrimitiveSet::LINE_STRIP; // Play it safe.
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the number of primitives.
//
///////////////////////////////////////////////////////////////////////////////

bool DbOsgVertexSetter::setNumPrimitives ( const unsigned int &num )
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

bool DbOsgVertexSetter::setSize ( const unsigned int &size )
{
  SL_ASSERT ( size > 0 );
  _vertices->resize ( size );
  return size == _vertices->size();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the size.
//
///////////////////////////////////////////////////////////////////////////////

bool DbOsgNormalSetter::setSize ( const unsigned int &size )
{
  SL_ASSERT ( size > 0 );
  _normals->resize ( size );
  return size == _normals->size();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the size.
//
///////////////////////////////////////////////////////////////////////////////

bool DbOsgColorSetter::setSize ( const unsigned int &size )
{
  SL_ASSERT ( size > 0 );
  _colors->resize ( size );
  return size == _colors->size();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the size.
//
///////////////////////////////////////////////////////////////////////////////

bool DbOsgTextureCoordSetter::setSize ( const unsigned int &size )
{
  SL_ASSERT ( size > 0 );
  _texCoords->resize ( size );
  return size == _texCoords->size();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the size.
//
///////////////////////////////////////////////////////////////////////////////

bool DbOsgPrimOriginSetter::setSize ( const unsigned int &size )
{
  SL_ASSERT ( size > 0 );
  _origins->resize ( size );
  return size == _origins->size();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the size.
//
///////////////////////////////////////////////////////////////////////////////

bool DbOsgPrimParamSetter::setSize ( const unsigned int &size )
{
  SL_ASSERT ( size > 0 );
  _params->resize( size );
  return size == _params->size();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the size.
//
///////////////////////////////////////////////////////////////////////////////

bool DbOsgPrimColorSetter::setSize ( const unsigned int &size )
{
  SL_ASSERT ( size > 0 );
  _colors->resize ( size );
  return size == _colors->size();
}



