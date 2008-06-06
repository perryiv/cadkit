
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Predicate class to test if the values are within tolerance of each other.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_PREDICATES_TOLERANCE_H_
#define _USUL_PREDICATES_TOLERANCE_H_


namespace Usul {
namespace Predicates {


template
<
  class DataType1, 
  class DataType2 = DataType1, 
  class TolType   = DataType1
>
struct Tolerance : public std::binary_function < const DataType1 &, const DataType2 &, bool >
{
  Tolerance ( const TolType &tol ) : _tol ( tol ){}
  Tolerance ( const Tolerance &tol ) : _tol ( tol._tol ){}
  Tolerance &operator = ( const Tolerance &tol ) { _tol = tol._tol; }
  bool operator () ( const DataType1 &v1, const DataType2 &v2 ) const
  {
    // Some redundancy here, but it makes debugging easier.
    if ( v1 == v2 )
      return true;
    else if ( v1 < v2 )
      return ( v1 + _tol >= v2 );
    else // v1 >= v2
      return ( v2 + _tol >= v1 );
  }
protected:
  TolType _tol;
};


} // namespace Predicates
} // namespace Usul


#endif // _USUL_PREDICATES_TOLERANCE_H_
