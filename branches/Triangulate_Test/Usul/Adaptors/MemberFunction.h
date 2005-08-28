
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Member function adaptors.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_ADAPTORS_MEMBER_FUNCTION_H_
#define _USUL_ADAPTORS_MEMBER_FUNCTION_H_


namespace Usul {
namespace Adaptors {


///////////////////////////////////////////////////////////////////////////////
//
//  Member function adaptor.
//
///////////////////////////////////////////////////////////////////////////////

template
<
  class ObjectType,
  class FunctionType
>
struct MemberFunction
{
  MemberFunction() : _o ( 0x0 ), _f ( 0x0 ){} // For gcc's stl containers.
  MemberFunction ( ObjectType o, FunctionType f ) : _o ( o ), _f ( f ){}
  MemberFunction ( const MemberFunction &mf ) : _o ( mf._o ), _f ( mf._f ){}

  MemberFunction &operator = ( const MemberFunction &mf )
  {
    _o = mf._o;
    _f = mf._f;
    return *this;
  }
  
  void operator()()
  {
    ((*_o).*_f)();
  }

  template < class Arg >
  void operator () ( const Arg &arg )
  {
    ((*_o).*_f) ( arg );
  }

  template < class Arg1, class Arg2 >
  void operator () ( const Arg1 &arg1, const Arg2 &arg2 )
  {
    ((*_o).*_f) ( arg1, arg2 );
  }

private:

  ObjectType _o;
  FunctionType _f;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Function to create a member function adaptor.
//
///////////////////////////////////////////////////////////////////////////////

template
<
  class ObjectType,
  class FunctionType
>
MemberFunction < ObjectType, FunctionType > 
memberFunction ( ObjectType object, FunctionType function )
{
  return MemberFunction < ObjectType, FunctionType > ( object, function );
}


}; // namespace Adaptors
}; // namespace Usul


#endif // _USUL_ADAPTORS_MEMBER_FUNCTION_H_
