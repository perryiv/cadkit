
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Calls funtion #1 in constructor and function #2 in destructor.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_SCOPE_CALLER_H_
#define _USUL_SCOPE_CALLER_H_

#include "Usul/Base/Referenced.h"
#include "Usul/Pointers/Pointers.h"


namespace Usul {
namespace Scope {


///////////////////////////////////////////////////////////////////////////////
//
//  Base class for convenience.
//
///////////////////////////////////////////////////////////////////////////////

struct Caller : public Usul::Base::Referenced
{
  typedef Usul::Base::Referenced BaseClass;
  USUL_DECLARE_REF_POINTERS ( Caller );
  Caller()
  {
  }
  Caller ( const Caller &c ) : BaseClass ( c )
  {
  }
  virtual ~Caller()
  {
  }
};


///////////////////////////////////////////////////////////////////////////////
//
//  Implementation class.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  template < class F > struct Call1 : public Usul::Scope::Caller
  {
    typedef Usul::Scope::Caller BaseClass;
    Call1 ( F f ) : _f ( f )
    {
    }
    Call1 ( const Call1 &c ) : _f ( c._f )
    {
    }
    virtual ~Call1()
    {
      _f();
    }
  private:
    F _f;
  };
}


///////////////////////////////////////////////////////////////////////////////
//
//  Implementation class.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  template < class F1, class F2 > struct Call2 : public Usul::Scope::Caller
  {
    typedef Usul::Scope::Caller BaseClass;
    Call2 ( F1 f1, F2 f2 ) : _f2 ( f2 )
    {
      f1();
    }
    Call2 ( const Call2 &c ) : _f2 ( c._f2 )
    {
    }
    virtual ~Call2()
    {
      _f2();
    }
  private:
    F2 _f2;
  };
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper functions. Example usage: 
//  Usul::Scope::Caller::RefPtr be ( Usul::Scope::makeCaller 
//    ( Usul::Adaptors::bind1 ( GL_TRIANGLES, ::glBegin ), ::glEnd ) ); 
//
///////////////////////////////////////////////////////////////////////////////

template < class F > inline Caller *makeCaller ( F f )
{
  return new Usul::Scope::Detail::Call1 < F > ( f );
}

template < class F1, class F2 > inline Caller *makeCaller ( F1 f1, F2 f2 )
{
  return new Usul::Scope::Detail::Call2 < F1, F2 > ( f1, f2 );
}


} // namespace Scope
} // namespace Usul


#endif // _USUL_SCOPE_CALLER_H_
