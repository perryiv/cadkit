
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_POLYGONS_PREDICATES_H__
#define __USUL_POLYGONS_PREDICATES_H__

#include <functional>


namespace Usul {
namespace Polygons {


///////////////////////////////////////////////////////////////////////////////
//
//  Predicate to determine if one polygon's index is less than anothers
//
///////////////////////////////////////////////////////////////////////////////

template< class Polygon >
struct PolyLess : public std::binary_function< Polygon, Polygon, bool >
{
  bool operator () ( const Polygon* p1, const Polygon* p2 ) const
  {
    return p1->index() < p2->index();
  }
};


///////////////////////////////////////////////////////////////////////////////
//
//  Predicate to determine if one polygon's index equal to another
//
///////////////////////////////////////////////////////////////////////////////

template< class Polygon >
struct PolyEqual : public std::binary_function< Polygon, Polygon, bool >
{
  bool operator () ( const Polygon* p1, const Polygon* p2 ) const
  {
    return p1->index() == p2->index();
  }
};


///////////////////////////////////////////////////////////////////////////////
//
//  Predicate to determine if one polygon's index equal to another.
//
///////////////////////////////////////////////////////////////////////////////

struct IsEqual
{
  IsEqual ( unsigned int index ) :
    _index ( index )
  {
  }
  
  template < class Iter > bool operator () ( const Iter& i ) const
  {
    return ( i->index() == _index );
  }
  
private:
  const unsigned int _index;
};


} //namespace Polygons
} //namespace Usul


#endif // __USUL_POLYGONS_PREDICATES_H__
