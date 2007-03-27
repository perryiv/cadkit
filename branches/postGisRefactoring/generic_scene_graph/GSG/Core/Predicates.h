
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Predicate classes. For container comparisons.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _GENERIC_SCENE_GRAPH_CORE_PREDICATE_CLASSES_H_
#define _GENERIC_SCENE_GRAPH_CORE_PREDICATE_CLASSES_H_

#include "boost/static_assert.hpp"


namespace GSG {
namespace Detail {


/////////////////////////////////////////////////////////////////////////////
//
//  For sorting containers of 2-dimensional vectors.
//
/////////////////////////////////////////////////////////////////////////////

struct LessVec2
{
  template < class VectorType > 
  bool operator () ( const VectorType &a, const VectorType &b ) const
  {
    BOOST_STATIC_ASSERT ( 2 == VectorType::SIZE );
    if ( a[0] == b[0] )
    {
      return a[1] < b[1];
    }
    else
    {
      return a[0] < b[0];
    }
  }
};


/////////////////////////////////////////////////////////////////////////////
//
//  For sorting containers of 3-dimensional vectors.
//
/////////////////////////////////////////////////////////////////////////////

struct LessVec3
{
  template < class VectorType > 
  bool operator () ( const VectorType &a, const VectorType &b ) const
  {
    BOOST_STATIC_ASSERT ( 3 == VectorType::SIZE );
    if ( a[0] == b[0] )
    {
      if ( a[1] == b[1] )
      {
        return a[2] < b[2];
      }
      else
      {
        return a[1] < b[1];
      }
    }
    else
    {
      return a[0] < b[0];
    }
  }
};


/////////////////////////////////////////////////////////////////////////////
//
//  For sorting containers of 4-dimensional vectors.
//
/////////////////////////////////////////////////////////////////////////////

struct LessVec4
{
  template < class VectorType > 
  bool operator () ( const VectorType &a, const VectorType &b ) const
  {
    BOOST_STATIC_ASSERT ( 4 == VectorType::SIZE );
    if ( a[0] == b[0] )
    {
      if ( a[1] == b[1] )
      {
        if ( a[2] == b[2] )
        {
          return a[3] < b[3];
        }
        else
        {
          return a[2] < b[2];
        }
      }
      else
      {
        return a[1] < b[1];
      }
    }
    else
    {
      return a[0] < b[0];
    }
  }
};


/////////////////////////////////////////////////////////////////////////////
//
//  For searching in a container for objects of a given type.
//
/////////////////////////////////////////////////////////////////////////////

struct IsOfType
{
  IsOfType ( const type_info &t ) : _t ( t ){}
  template < class ContainerType > 
  bool operator () ( const ContainerType &object ) const
  {
    // ContainerType should either be a "Referenced *" 
    // or a smart-pointer to one.
    return ( object->typeId() == _t ) ? true : false; // VC6 warning C4800.
  }
private:
  const type_info &_t;
};


}; // namespace Detail
}; // namespace GSG


#endif // _GENERIC_SCENE_GRAPH_CORE_PREDICATE_CLASSES_H_
