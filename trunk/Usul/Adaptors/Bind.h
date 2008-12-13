
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Function objects that bind arguments.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_ADAPTORS_BIND_H_
#define _USUL_ADAPTORS_BIND_H_


namespace Usul {
namespace Adaptors {


///////////////////////////////////////////////////////////////////////////////
//
//  Bind a single argument.
//
///////////////////////////////////////////////////////////////////////////////

template
<
  class ReturnType,
  class ArgumentType,
  class FunctionType
>
struct Bind1
{
  Bind1() : _a ( 0x0 ), _f ( 0x0 ){} // For gcc's stl containers.
  Bind1 ( ArgumentType a, FunctionType f ) : _a ( a ), _f ( f ){}
  Bind1 ( const Bind1 &b ) : _a ( b._a ), _f ( b._f ){}

  Bind1 &operator = ( const Bind1 &b )
  {
    _a = b._a;
    _f = b._f;
    return *this;
  }

  ReturnType operator()()
  {
    return _f ( _a );
  }

private:

  ArgumentType _a;
  FunctionType _f;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Bind a single argument.
//
///////////////////////////////////////////////////////////////////////////////

template
<
  class ArgumentType,
  class FunctionType
>
struct Bind1 < void, ArgumentType, FunctionType >
{
  Bind1() : _a ( 0x0 ), _f ( 0x0 ){} // For gcc's stl containers.
  Bind1 ( ArgumentType a, FunctionType f ) : _a ( a ), _f ( f ){}
  Bind1 ( const Bind1 &b ) : _a ( b._a ), _f ( b._f ){}

  Bind1 &operator = ( const Bind1 &b )
  {
    _a = b._a;
    _f = b._f;
    return *this;
  }
  
  void operator()()
  {
    _f ( _a );
  }

  template < class FirstArgument > void operator() ( const FirstArgument &arg )
  {
    _f ( arg, _a );
  }

private:

  ArgumentType _a;
  FunctionType _f;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Bind a single argument.
//
///////////////////////////////////////////////////////////////////////////////

template
<
  class ArgumentType,
  class FunctionType
>
struct Bind1 < bool, ArgumentType, FunctionType >
{
  Bind1() : _a ( 0x0 ), _f ( 0x0 ){} // For gcc's stl containers.
  Bind1 ( ArgumentType a, FunctionType f ) : _a ( a ), _f ( f ){}
  Bind1 ( const Bind1 &b ) : _a ( b._a ), _f ( b._f ){}

  Bind1 &operator = ( const Bind1 &b )
  {
    _a = b._a;
    _f = b._f;
    return *this;
  }
  
  bool operator()() const
  {
    return _f ( _a );
  }

  template < class FirstArgument > bool operator() ( const FirstArgument &arg ) const
  {
    return _f ( arg, _a );
  }

private:

  ArgumentType _a;
  FunctionType _f;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Bind two arguments.
//
///////////////////////////////////////////////////////////////////////////////

template
<
  class ReturnType,
  class ArgumentType1,
  class ArgumentType2,
  class FunctionType
>
struct Bind2
{
  Bind2() : _a1 ( 0x0 ), _a2 ( 0x0 ), _f ( 0x0 ){} // For gcc's stl containers.
  Bind2 ( ArgumentType1 a1, ArgumentType2 a2, FunctionType f ) : _a1 ( a1 ), _a2 ( a2 ), _f ( f ){}
  Bind2 ( const Bind2 &b ) : _a1 ( b._a1 ), _a2 ( b._a2 ), _f ( b._f ){}

  Bind2 &operator = ( const Bind2 &b )
  {
    _a1 = b._a1;
    _a2 = b._a2;
    _f = b._f;
    return *this;
  }
  
  ReturnType operator()()
  {
    return _f ( _a1, _a2 );
  }

private:

  ArgumentType1 _a1;
  ArgumentType2 _a2;
  FunctionType _f;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Bind two arguments.
//
///////////////////////////////////////////////////////////////////////////////

template
<
  class ArgumentType1,
  class ArgumentType2,
  class FunctionType
>
struct Bind2 < void, ArgumentType1, ArgumentType2, FunctionType >
{
  Bind2() : _a1 ( 0x0 ), _a2 ( 0x0 ), _f ( 0x0 ){} // For gcc's stl containers.
  Bind2 ( ArgumentType1 a1, ArgumentType2 a2, FunctionType f ) : _a1 ( a1 ), _a2 ( a2 ), _f ( f ){}
  Bind2 ( const Bind2 &b ) : _a1 ( b._a1 ), _a2 ( b._a2 ), _f ( b._f ){}

  Bind2 &operator = ( const Bind2 &b )
  {
    _a1 = b._a1;
    _a2 = b._a2;
    _f = b._f;
    return *this;
  }
  
  void operator()()
  {
    _f ( _a1, _a2 );
  }

private:

  ArgumentType1 _a1;
  ArgumentType2 _a2;
  FunctionType _f;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Bind two arguments.
//
///////////////////////////////////////////////////////////////////////////////

template
<
  class ArgumentType1,
  class ArgumentType2,
  class FunctionType
>
struct Bind2 < bool, ArgumentType1, ArgumentType2, FunctionType >
{
  Bind2() : _a1 ( 0x0 ), _a2 ( 0x0 ), _f ( 0x0 ){} // For gcc's stl containers.
  Bind2 ( ArgumentType1 a1, ArgumentType2 a2, FunctionType f ) : _a1 ( a1 ), _a2 ( a2 ), _f ( f ){}
  Bind2 ( const Bind2 &b ) : _a1 ( b._a1 ), _a2 ( b._a2 ), _f ( b._f ){}

  Bind2 &operator = ( const Bind2 &b )
  {
    _a1 = b._a1;
    _a2 = b._a2;
    _f = b._f;
    return *this;
  }
  
  bool operator()() const
  {
    return _f ( _a1, _a2 );
  }

private:

  ArgumentType1 _a1;
  ArgumentType2 _a2;
  FunctionType _f;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Bind three arguments.
//
///////////////////////////////////////////////////////////////////////////////

template
<
  class ReturnType,
  class ArgumentType1,
  class ArgumentType2,
  class ArgumentType3,
  class FunctionType
>
struct Bind3
{
  Bind3() : _a1 ( 0x0 ), _a2 ( 0x0 ), _a3 ( 0x0 ), _f ( 0x0 ){} // For gcc's stl containers.
  Bind3 ( ArgumentType1 a1, ArgumentType2 a2, ArgumentType3 a3, FunctionType f ) : _a1 ( a1 ), _a2 ( a2 ), _a3 ( a3 ), _f ( f ){}
  Bind3 ( const Bind3 &b ) : _a1 ( b._a1 ), _a2 ( b._a2 ), _a3 ( b._a3 ), _f ( b._f ){}

  Bind3 &operator = ( const Bind3 &b )
  {
    _a1 = b._a1;
    _a2 = b._a2;
    _a3 = b._a3;
    _f = b._f;
    return *this;
  }
  
  ReturnType operator()()
  {
    return _f ( _a1, _a2, _a3 );
  }

private:

  ArgumentType1 _a1;
  ArgumentType2 _a2;
  ArgumentType3 _a3;
  FunctionType _f;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Bind three arguments.
//
///////////////////////////////////////////////////////////////////////////////

template
<
  class ArgumentType1,
  class ArgumentType2,
  class ArgumentType3,
  class FunctionType
>
struct Bind3 < void, ArgumentType1, ArgumentType2, ArgumentType3, FunctionType >
{
  Bind3() : _a1 ( 0x0 ), _a2 ( 0x0 ), _a3 ( 0x0 ), _f ( 0x0 ){} // For gcc's stl containers.
  Bind3 ( ArgumentType1 a1, ArgumentType2 a2, ArgumentType3 a3, FunctionType f ) : _a1 ( a1 ), _a2 ( a2 ), _a3 ( a3 ), _f ( f ){}
  Bind3 ( const Bind3 &b ) : _a1 ( b._a1 ), _a2 ( b._a2 ), _a3 ( b._a3 ), _f ( b._f ){}

  Bind3 &operator = ( const Bind3 &b )
  {
    _a1 = b._a1;
    _a2 = b._a2;
    _a3 = b._a3;
    _f = b._f;
    return *this;
  }
  
  void operator()()
  {
    _f ( _a1, _a2, _a3 );
  }

private:

  ArgumentType1 _a1;
  ArgumentType2 _a2;
  ArgumentType3 _a3;
  FunctionType _f;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Bind three arguments.
//
///////////////////////////////////////////////////////////////////////////////

template
<
  class ArgumentType1,
  class ArgumentType2,
  class ArgumentType3,
  class FunctionType
>
struct Bind3 < bool, ArgumentType1, ArgumentType2, ArgumentType3, FunctionType >
{
  Bind3() : _a1 ( 0x0 ), _a2 ( 0x0 ), _a3 ( 0x0 ), _f ( 0x0 ){} // For gcc's stl containers.
  Bind3 ( ArgumentType1 a1, ArgumentType2 a2, ArgumentType3 a3, FunctionType f ) : _a1 ( a1 ), _a2 ( a2 ), _a3 ( a3 ), _f ( f ){}
  Bind3 ( const Bind3 &b ) : _a1 ( b._a1 ), _a2 ( b._a2 ), _a3 ( b._a3 ), _f ( b._f ){}

  Bind3 &operator = ( const Bind3 &b )
  {
    _a1 = b._a1;
    _a2 = b._a2;
    _a3 = b._a3;
    _f = b._f;
    return *this;
  }
  
  bool operator()() const
  {
    return _f ( _a1, _a2, _a3 );
  }

private:

  ArgumentType1 _a1;
  ArgumentType2 _a2;
  ArgumentType3 _a3;
  FunctionType _f;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Bind four arguments.
//
///////////////////////////////////////////////////////////////////////////////

template
<
  class ReturnType,
  class ArgumentType1,
  class ArgumentType2,
  class ArgumentType3,
  class ArgumentType4,
  class FunctionType
>
struct Bind4
{
  Bind4() : _a1 ( 0x0 ), _a2 ( 0x0 ), _a3 ( 0x0 ), _a4 ( 0x0 ), _f ( 0x0 ){} // For gcc's stl containers.
  Bind4 ( ArgumentType1 a1, ArgumentType2 a2, ArgumentType3 a3, ArgumentType4 a4, FunctionType f ) : _a1 ( a1 ), _a2 ( a2 ), _a3 ( a3 ), _a4 ( a4 ), _f ( f ){}
  Bind4 ( const Bind4 &b ) : _a1 ( b._a1 ), _a2 ( b._a2 ), _a3 ( b._a3 ), _a4 ( b._a4 ), _f ( b._f ){}

  Bind4 &operator = ( const Bind4 &b )
  {
    _a1 = b._a1;
    _a2 = b._a2;
    _a3 = b._a3;
    _a4 = b._a4;
    _f = b._f;
    return *this;
  }
  
  ReturnType operator()()
  {
    return _f ( _a1, _a2, _a3, _a4 );
  }

private:

  ArgumentType1 _a1;
  ArgumentType2 _a2;
  ArgumentType3 _a3;
  ArgumentType4 _a4;
  FunctionType _f;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Bind four arguments.
//
///////////////////////////////////////////////////////////////////////////////

template
<
  class ArgumentType1,
  class ArgumentType2,
  class ArgumentType3,
  class ArgumentType4,
  class FunctionType
>
struct Bind4 < void, ArgumentType1, ArgumentType2, ArgumentType3, ArgumentType4, FunctionType >
{
  Bind4() : _a1 ( 0x0 ), _a2 ( 0x0 ), _a3 ( 0x0 ), _a4 ( 0x0 ), _f ( 0x0 ){} // For gcc's stl containers.
  Bind4 ( ArgumentType1 a1, ArgumentType2 a2, ArgumentType3 a3, ArgumentType4 a4, FunctionType f ) : _a1 ( a1 ), _a2 ( a2 ), _a3 ( a3 ), _a4 ( a4 ), _f ( f ){}
  Bind4 ( const Bind4 &b ) : _a1 ( b._a1 ), _a2 ( b._a2 ), _a3 ( b._a3 ), _a4 ( b._a4 ), _f ( b._f ){}

  Bind4 &operator = ( const Bind4 &b )
  {
    _a1 = b._a1;
    _a2 = b._a2;
    _a3 = b._a3;
    _a4 = b._a4;
    _f = b._f;
    return *this;
  }
  
  void operator()()
  {
    _f ( _a1, _a2, _a3, _a4 );
  }

private:

  ArgumentType1 _a1;
  ArgumentType2 _a2;
  ArgumentType3 _a3;
  ArgumentType4 _a4;
  FunctionType _f;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Bind four arguments.
//
///////////////////////////////////////////////////////////////////////////////

template
<
  class ArgumentType1,
  class ArgumentType2,
  class ArgumentType3,
  class ArgumentType4,
  class FunctionType
>
struct Bind4 < bool, ArgumentType1, ArgumentType2, ArgumentType3, ArgumentType4, FunctionType >
{
  Bind4() : _a1 ( 0x0 ), _a2 ( 0x0 ), _a3 ( 0x0 ), _a4 ( 0x0 ), _f ( 0x0 ){} // For gcc's stl containers.
  Bind4 ( ArgumentType1 a1, ArgumentType2 a2, ArgumentType3 a3, ArgumentType4 a4, FunctionType f ) : _a1 ( a1 ), _a2 ( a2 ), _a3 ( a3 ), _a4 ( a4 ), _f ( f ){}
  Bind4 ( const Bind4 &b ) : _a1 ( b._a1 ), _a2 ( b._a2 ), _a3 ( b._a3 ), _a4 ( b._a4 ), _f ( b._f ){}

  Bind4 &operator = ( const Bind4 &b )
  {
    _a1 = b._a1;
    _a2 = b._a2;
    _a3 = b._a3;
    _a4 = b._a4;
    _f = b._f;
    return *this;
  }
  
  bool operator()() const
  {
    return _f ( _a1, _a2, _a3, _a4 );
  }

private:

  ArgumentType1 _a1;
  ArgumentType2 _a2;
  ArgumentType3 _a3;
  ArgumentType4 _a4;
  FunctionType _f;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Functions to create binder adaptors.
//
///////////////////////////////////////////////////////////////////////////////

template
<
  class ArgumentType,
  class FunctionType
>
Bind1 < void, ArgumentType, FunctionType > 
bind1 ( ArgumentType argument, FunctionType function )
{
  return Bind1 < void, ArgumentType, FunctionType > ( argument, function );
}

template
<
  class ReturnType,
  class ArgumentType,
  class FunctionType
>
Bind1 < ReturnType, ArgumentType, FunctionType > 
bind1 ( ArgumentType argument, FunctionType function )
{
  return Bind1 < ReturnType, ArgumentType, FunctionType > ( argument, function );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Functions to create bind-two adaptors.
//
///////////////////////////////////////////////////////////////////////////////

template
<
  class ArgumentType1,
  class ArgumentType2,
  class FunctionType
>
Bind2 < void, ArgumentType1, ArgumentType2, FunctionType > 
bind2 ( ArgumentType1 argument1, ArgumentType2 argument2, FunctionType function )
{
  return Bind2 < void, ArgumentType1, ArgumentType2, FunctionType > ( argument1, argument2, function );
}

template
<
  class ReturnType,
  class ArgumentType1,
  class ArgumentType2,
  class FunctionType
>
Bind2 < ReturnType, ArgumentType1, ArgumentType2, FunctionType > 
bind2 ( ArgumentType1 argument1, ArgumentType2 argument2, FunctionType function )
{
  return Bind2 < ReturnType, ArgumentType1, ArgumentType2, FunctionType > ( argument1, argument2, function );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Functions to create bind-three adaptors.
//
///////////////////////////////////////////////////////////////////////////////

template
<
  class ArgumentType1,
  class ArgumentType2,
  class ArgumentType3,
  class FunctionType
>
Bind3 < void, ArgumentType1, ArgumentType2, ArgumentType3, FunctionType > 
bind3 ( ArgumentType1 argument1, ArgumentType2 argument2, ArgumentType3 argument3, FunctionType function )
{
  return Bind3 < void, ArgumentType1, ArgumentType2, ArgumentType3, FunctionType > ( argument1, argument2, argument3, function );
}

template
<
  class ReturnType,
  class ArgumentType1,
  class ArgumentType2,
  class ArgumentType3,
  class FunctionType
>
Bind3 < ReturnType, ArgumentType1, ArgumentType2, ArgumentType3, FunctionType > 
bind3 ( ArgumentType1 argument1, ArgumentType2 argument2, ArgumentType3 argument3, FunctionType function )
{
  return Bind3 < ReturnType, ArgumentType1, ArgumentType2, ArgumentType3, FunctionType > ( argument1, argument2, argument3, function );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Functions to create bind-four adaptors.
//
///////////////////////////////////////////////////////////////////////////////

template
<
  class ArgumentType1,
  class ArgumentType2,
  class ArgumentType3,
  class ArgumentType4,
  class FunctionType
>
Bind4 < void, ArgumentType1, ArgumentType2, ArgumentType3, ArgumentType4, FunctionType > 
bind4 ( ArgumentType1 argument1, ArgumentType2 argument2, ArgumentType3 argument3, ArgumentType4 argument4, FunctionType function )
{
  return Bind4 < void, ArgumentType1, ArgumentType2, ArgumentType3, ArgumentType4, FunctionType > ( argument1, argument2, argument3, argument4, function );
}

template
<
  class ReturnType,
  class ArgumentType1,
  class ArgumentType2,
  class ArgumentType3,
  class ArgumentType4,
  class FunctionType
>
Bind4 < ReturnType, ArgumentType1, ArgumentType2, ArgumentType3, ArgumentType4, FunctionType > 
bind4 ( ArgumentType1 argument1, ArgumentType2 argument2, ArgumentType3 argument3, ArgumentType4 argument4, FunctionType function )
{
  return Bind4 < ReturnType, ArgumentType1, ArgumentType2, ArgumentType3, ArgumentType4, FunctionType > ( argument1, argument2, argument3, argument4, function );
}


} // namespace Adaptors
} // namespace Usul


#endif // _USUL_ADAPTORS_BIND_H_
