
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

#include "DbStlArraySetters.h"

using namespace CadKit;


///////////////////////////////////////////////////////////////////////////////
//
//  Set the data.
//
///////////////////////////////////////////////////////////////////////////////

bool DbStlVertexSetter::setData ( const unsigned int &index, const SlVec3f &vec )
{
/* 
  // If we're out of range then resize.
  if ( index >= _vertices->size() )
    if ( false == this->setSize ( index + 1 ) )
      return false;

  // Set the vertex.
  (*_vertices)[index].set ( vec[0], vec[1], vec[2] );

  // It worked.
  return true;
*/
//TODO call FacetManager setData function
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the data.
//
///////////////////////////////////////////////////////////////////////////////

bool DbStlNormalSetter::setData ( const unsigned int &index, const SlVec3f &vec )
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
//  Get the primitive mode.
//
///////////////////////////////////////////////////////////////////////////////

osg::Primitive::Mode DbStlVertexSetter::_getPrimitiveMode() const
{
  switch ( _type )
  {
  case CadKit::LINE_STRIP_SET:
    return osg::Primitive::LINE_STRIP;
  case CadKit::POINT_SET:
    return osg::Primitive::POINTS;
  case CadKit::POLYGON_SET:
    return osg::Primitive::POLYGON;
  case CadKit::TRI_STRIP_SET:
    return osg::Primitive::TRIANGLE_STRIP;
  case CadKit::TRI_FAN_SET:
    return osg::Primitive::TRIANGLE_FAN;
  default:
    SL_ASSERT ( 0 );                      // What type is this?
    return osg::Primitive::LINE_STRIP; // Play it safe.
  }
}



///////////////////////////////////////////////////////////////////////////////
//
//  Set the size.
//
///////////////////////////////////////////////////////////////////////////////

bool DbStlVertexSetter::setSize ( const unsigned int &size )
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

bool DbStlNormalSetter::setSize ( const unsigned int &size )
{
  SL_ASSERT ( size > 0 );
  _normals->resize ( size );
  return size == _normals->size();
}


