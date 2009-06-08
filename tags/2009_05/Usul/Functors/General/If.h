
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  If-then functors.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_FUNCTORS_IF_H_
#define _USUL_FUNCTORS_IF_H_


namespace Usul {
namespace Functors {
namespace General {


///////////////////////////////////////////////////////////////////////////////
//
//  An if-then functor.
//
///////////////////////////////////////////////////////////////////////////////

template < class If_, class Then_ > struct IfThen
{
  typedef If_ If;
  typedef Then_ Then;
  IfThen ( If f, Then t ) : _if ( f ), _then ( t )
  {
  }
  template < class T > void operator() ( const T &t )
  {
    if ( _if ( t ) )
      _then ( t );
  }
  const Then &  then()   const { return _then; }
  Then &        then()         { return _then; }
  const If &    ifPred() const { return _if; }
  If &          ifPred()       { return _if; }
private:
  If _if;
  Then _then;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to make an if-then functor.
//
///////////////////////////////////////////////////////////////////////////////

template < class If_, class Then_ > IfThen < If_, Then_ > ifThen ( If_ a, Then_ b )
{
  return IfThen < If_, Then_ > ( a, b );
}


///////////////////////////////////////////////////////////////////////////////
//
//  An if-then-else functor.
//
///////////////////////////////////////////////////////////////////////////////

template < class If_, class Then_, class Else_ > struct IfThenElse
{
  typedef If_ If;
  typedef Then_ Then;
  typedef Else_ Else;
  IfThenElse ( If f, Then t, Else e ) : _if ( f ), _then ( t ), _else ( e )
  {
  }
  template < class T > void operator() ( const T &t )
  {
    if ( _if ( t ) )
      _then ( t );
    else
      _else ( t );
  }
private:
  If _if;
  Then _then;
  Else _else;
};


} // namespace General
} // namespace Functors
} // namespace Usul


#endif // _USUL_FUNCTORS_IF_H_
