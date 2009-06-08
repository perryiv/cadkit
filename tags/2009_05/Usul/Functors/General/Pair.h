
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Pair of functors.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_FUNCTORS_PAIR_H_
#define _USUL_FUNCTORS_PAIR_H_


namespace Usul {
namespace Functors {
namespace General {


///////////////////////////////////////////////////////////////////////////////
//
//  Pair of functors.
//
///////////////////////////////////////////////////////////////////////////////

template < class F1_, class F2_ > struct Pair
{
  Pair ( F1_ f1, F2_ f2 ) : _f1 ( f1 ), _f2 ( f2 ){}
  template < class T >
  void operator() ( const T &t )
  {
    _f1 ( t );
    _f2 ( t );
  }
  template < class T1_, class T2_ >
  void operator() ( const T1_ &t1, const T2_ &t2 )
  {
    _f1 ( t1, t2 );
    _f2 ( t1, t2 );
  }
  template < class T1_, class T2_, class T3_ >
  void operator() ( const T1_ &t1, const T2_ &t2, const T3_ &t3 )
  {
    _f1 ( t1, t2, t3 );
    _f2 ( t1, t2, t3 );
  }
private:
  F1_ _f1;
  F2_ _f2;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to make a pair of functors.
//
///////////////////////////////////////////////////////////////////////////////

template < class F1, class F2 > 
inline Pair < F1, F2 > pair ( F1 f1, F2 f2 )
{
  return Pair < F1, F2 > ( f1, f2 );
}


} // namespace General
} // namespace Functors
} // namespace Usul


#endif // _USUL_FUNCTORS_PAIR_H_
