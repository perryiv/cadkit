
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Predicate that compares a single coordinate of a vector.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_PREDICATES_COMPARE_COORDINATE_H_
#define _USUL_PREDICATES_COMPARE_COORDINATE_H_


namespace Usul {
namespace Predicates {


struct CompareCoordinate
{
  CompareCoordinate ( unsigned int i ) : _i ( i )
  {
  }
  template < class VectorType > bool operator () ( const VectorType &a, const VectorType &b )
  {
    return ( a[_i] < b[_i] );
  }
private:
  unsigned int _i;
};


}; // namespace Predicates
}; // namespace Usul


#endif // _USUL_PREDICATES_COMPARE_COORDINATE_H_
