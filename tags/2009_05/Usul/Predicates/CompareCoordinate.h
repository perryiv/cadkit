
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
  CompareCoordinate ( unsigned int i, bool less = true ) : _index ( i ), _less ( less )
  {
  }
  template < class VectorType > bool operator () ( const VectorType &a, const VectorType &b )
  {
    return ( ( _less ) ? ( a[_index] < b[_index] ) : ( b[_index] < a[_index] ) );
  }
private:
  unsigned int _index;
  bool _less;
};


} // namespace Predicates
} // namespace Usul


#endif // _USUL_PREDICATES_COMPARE_COORDINATE_H_
