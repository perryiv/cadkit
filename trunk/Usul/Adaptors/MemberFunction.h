
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
  class ReturnType,
  class ObjectType,
  class FunctionType
>
struct MemberFunction
{
  typedef ReturnType result_type;

  MemberFunction() : _o ( 0x0 ), _f ( 0x0 ){} // For gcc's stl containers.
  MemberFunction ( ObjectType o, FunctionType f ) : _o ( o ), _f ( f ){}
  MemberFunction ( const MemberFunction &mf ) : _o ( mf._o ), _f ( mf._f ){}

  MemberFunction &operator = ( const MemberFunction &mf )
  {
    _o = mf._o;
    _f = mf._f;
    return *this;
  }
  
  ReturnType operator()()
  {
    return ((*_o).*_f)();
  }

  template < class Arg >
  ReturnType operator () ( const Arg &arg )
  {
    return ((*_o).*_f) ( arg );
  }

  template < class Arg1, class Arg2 >
  ReturnType operator () ( const Arg1 &arg1, const Arg2 &arg2 )
  {
    return ((*_o).*_f) ( arg1, arg2 );
  }

  template < class Arg1, class Arg2, class Arg3 >
  ReturnType operator () ( const Arg1 &arg1, const Arg2 &arg2, const Arg3 &arg3 )
  {
    return ((*_o).*_f) ( arg1, arg2, arg3 );
  }
  
  template < class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6 >
  ReturnType operator () ( const Arg1 &arg1, const Arg2 &arg2, const Arg3 &arg3, const Arg4 &arg4, const Arg5 &arg5, const Arg6 &arg6 )
  {
    return ((*_o).*_f) ( arg1, arg2, arg3, arg4, arg5, arg6 );
  }

private:

  ObjectType _o;
  FunctionType _f;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Specialization of member function adaptor that returns void.
//
///////////////////////////////////////////////////////////////////////////////

template
<
  class ObjectType,
  class FunctionType
>
struct MemberFunction < void, ObjectType, FunctionType >
{
  typedef void result_type;

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

  template < class Arg1, class Arg2, class Arg3 >
  void operator () ( const Arg1 &arg1, Arg2 arg2, Arg3 arg3 )
  {
    ((*_o).*_f) ( arg1, arg2, arg3 );
  }

  template < class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6 >
  void operator () ( const Arg1 &arg1, const Arg2 &arg2, const Arg3 &arg3, const Arg4 &arg4, const Arg5 &arg5, const Arg6 &arg6 )
  {
    ((*_o).*_f) ( arg1, arg2, arg3, arg4, arg5, arg6 );
  }

private:

  ObjectType _o;
  FunctionType _f;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Specialization of member function adaptor that returns bool.
//
///////////////////////////////////////////////////////////////////////////////

template
<
  class ObjectType,
  class FunctionType
>
struct MemberFunction < bool, ObjectType, FunctionType >
{
  typedef bool result_type;

  MemberFunction() : _o ( 0x0 ), _f ( 0x0 ){} // For gcc's stl containers.
  MemberFunction ( ObjectType o, FunctionType f ) : _o ( o ), _f ( f ){}
  MemberFunction ( const MemberFunction &mf ) : _o ( mf._o ), _f ( mf._f ){}

  MemberFunction &operator = ( const MemberFunction &mf )
  {
    _o = mf._o;
    _f = mf._f;
    return *this;
  }
  
  bool operator()() const
  {
    return ((*_o).*_f)();
  }

  template < class Arg >
  bool operator () ( const Arg &arg ) const
  {
    return ((*_o).*_f) ( arg );
  }

  template < class Arg1, class Arg2 >
  bool operator () ( const Arg1 &arg1, const Arg2 &arg2 ) const
  {
    return ((*_o).*_f) ( arg1, arg2 );
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
MemberFunction < void, ObjectType, FunctionType > 
memberFunction ( ObjectType object, FunctionType function )
{
  return MemberFunction < void, ObjectType, FunctionType > ( object, function );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Function to create a member function adaptor.
//
///////////////////////////////////////////////////////////////////////////////

#if 0 // Needed?
template
<
  class ObjectType,
  class FunctionType
>
MemberFunction < bool, ObjectType, FunctionType > 
memberFunction ( ObjectType object, FunctionType function )
{
  return MemberFunction < bool, ObjectType, FunctionType > ( object, function );
}
#endif


///////////////////////////////////////////////////////////////////////////////
//
//  Function to create a member function adaptor.
//
///////////////////////////////////////////////////////////////////////////////

template
<
  class ReturnType,
  class ObjectType,
  class FunctionType
>
MemberFunction < ReturnType, ObjectType, FunctionType > 
memberFunction ( ObjectType object, FunctionType function )
{
  return MemberFunction < ReturnType, ObjectType, FunctionType > ( object, function );
}


} // namespace Adaptors
} // namespace Usul


#endif // _USUL_ADAPTORS_MEMBER_FUNCTION_H_
