
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
// Callback for a button that wraps a member function.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _MENUKIT_MEMBER_FUNCTION_CALLBACK_H_
#define _MENUKIT_MEMBER_FUNCTION_CALLBACK_H_

#include "MenuKit/Item.h"

#include "Usul/Interfaces/IUpdateCheck.h"

namespace MenuKit {


///////////////////////////////////////////////////////////////////////////////
//
// Callback class for member functions that return a boolean.
//
///////////////////////////////////////////////////////////////////////////////

template < class O, class F > class MemFunCallbackReturn
{
public:

  MemFunCallbackReturn ( O o, F f ) : _o ( o ), _f ( f )
  {
  }
  MemFunCallbackReturn ( const MemFunCallbackReturn & mf ) : _o ( mf._o ), _f ( mf._f ) { }
  MemFunCallbackReturn &operator = ( const MemFunCallbackReturn & mf )
  {
    _o = mf._o;
    _f = mf._f;
    return *this;
  }

  ~MemFunCallbackReturn()
  {
  }
  bool operator () ( ) const
  {
    if ( _o && _f )
      return ((*_o).*_f)();

    return false;
  }

protected:
  O _o;
  F _f;
};


template < class O, class F > MemFunCallbackReturn < O, F > memFunCBReturn ( O o, F f )
{
  return MemFunCallbackReturn < O, F > ( o, f );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper command to execute a functor.
//
///////////////////////////////////////////////////////////////////////////////

template < class ExecuteFunctor >
class BasicCommand : public Usul::Commands::Command
{
public:
  typedef Usul::Commands::Command BaseClass;

  USUL_DECLARE_COMMAND  ( BasicCommand );

  BasicCommand ( const std::string& name, ExecuteFunctor functor, Usul::Interfaces::IUnknown * caller = 0x0 ) : 
    BaseClass ( caller ),
    _functor ( functor )
  {
    this->text ( name );
  }

protected:
  virtual ~BasicCommand ()
  {
  }

  virtual void _execute ()
  {
    _functor ( );
  }

private:
  ExecuteFunctor _functor;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Helper command to execute a functor and update check box.
//
///////////////////////////////////////////////////////////////////////////////

template < class ExecuteFunctor, class CheckFunctor >
class CheckCommand : public Usul::Commands::Command,
                     public Usul::Interfaces::IUpdateCheck
{
public:
  typedef Usul::Commands::Command BaseClass;

  USUL_DECLARE_COMMAND  ( CheckCommand );

  CheckCommand ( const std::string& name, ExecuteFunctor functor, CheckFunctor check, Usul::Interfaces::IUnknown * caller = 0x0 ) : 
    BaseClass ( caller ),
    _functor ( functor ),
    _check ( check )
  {
    this->text ( name );
  }

  void ref ()
  {
    BaseClass::ref ();
  }

  void unref ( bool allowDeletion = true )
  {
    BaseClass::unref ( allowDeletion );
  }

  Usul::Interfaces::IUnknown* queryInterface ( unsigned long iid )
  {
    switch ( iid )
    {
    case Usul::Interfaces::IUpdateCheck::IID:
      return static_cast < Usul::Interfaces::IUpdateCheck * > ( this );
    default:
      return BaseClass::queryInterface ( iid );
    }
  }

  virtual bool updateCheck () const
  {
    return _check ( );
  }

protected:
  virtual ~CheckCommand ()
  {
  }

  virtual void _execute ()
  {
    _functor ( !_check() );
  }

private:
  ExecuteFunctor _functor;
  CheckFunctor   _check;
};


template < class E >
const std::type_info &BasicCommand < E >::classTypeId() 
{
  return typeid ( BasicCommand < E > );
}

template < class E >
const std::type_info &BasicCommand < E >::typeId() const 
{
  return BasicCommand < E >::classTypeId();
}
template < class E >
Usul::Commands::Command* BasicCommand < E >::clone ( ) const { return new BasicCommand < E > ( *this ); }


template < class E, class C >
const std::type_info &CheckCommand < E, C >::classTypeId() 
{
  return typeid ( CheckCommand < E, C > );
}

template < class E, class C >
const std::type_info &CheckCommand < E, C >::typeId() const 
{
  return BasicCommand < E >::classTypeId();
}
template < class E, class C >
Usul::Commands::Command* CheckCommand < E, C >::clone ( ) const { return new CheckCommand < E, C > ( *this ); }


}; // namespace MenuKit


#endif // _MENUKIT_MEMBER_FUNCTION_CALLBACK_H_
