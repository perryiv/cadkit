
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
//  Functions to create a adaptors.
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


} // namespace Adaptors
} // namespace Usul


#endif // _USUL_ADAPTORS_BIND_H_
