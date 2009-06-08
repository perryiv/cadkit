
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Helper command to execute a functor.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_COMMANDS_GENERIC_CHECK_COMMAND_H__
#define __USUL_COMMANDS_GENERIC_CHECK_COMMAND_H__

#include "Usul/Commands/Command.h"
#include "Usul/Commands/GenericCommand.h"

namespace Usul {
namespace Commands {

  template < class E, class C >
  struct TogglePolicy
  {
    void operator () ( E e, C c )
    {
      e ( !c() );
    }
  };

  template < class E, class C >
  struct ExecutePolicy
  {
    void operator () ( E e, C c )
    {
      e ( );
    }
  };

template < class ExecuteFunctor,
           class CheckFunctor, 
           class EnableFunctor = TrueFunctor,
           class ExecutePolicy_ = TogglePolicy < ExecuteFunctor, CheckFunctor > >
class GenericCheckCommand : public Usul::Commands::Command
{
public:
  typedef Usul::Commands::Command                                             BaseClass;
  typedef ExecutePolicy_                                                      ExecutePolicy;
  typedef GenericCheckCommand < ExecuteFunctor, CheckFunctor, EnableFunctor, ExecutePolicy > ThisType;

  USUL_DECLARE_QUERY_POINTERS  ( GenericCheckCommand );

  GenericCheckCommand ( const std::string& name, 
                       ExecuteFunctor functor, 
                       CheckFunctor check, 
                       Usul::Interfaces::IUnknown * caller = 0x0 ) : 
    BaseClass ( caller ),
    _functor ( functor ),
    _enable (),
    _check ( check )
  {
    this->text ( name );
  }

  GenericCheckCommand ( const std::string& name, 
                        ExecuteFunctor functor, 
                        CheckFunctor check, 
                        EnableFunctor e, 
                        Usul::Interfaces::IUnknown * caller = 0x0 ) : 
    BaseClass ( caller ),
    _functor ( functor ),
    _enable ( e ),
    _check ( check )
  {
    this->text ( name );
  }

  virtual Usul::Commands::Command* clone ( ) const { return new ThisType ( *this ); }

  static const std::type_info &classTypeId() { return typeid ( ThisType ); }
  virtual const std::type_info &typeId() const { return ThisType::classTypeId(); }

protected:
  virtual ~GenericCheckCommand () {}

  virtual void _execute ()
  {
    ExecutePolicy p;
    p ( _functor, _check );
    //_functor ( !_check() );
  }

  virtual bool updateEnable () const
  {
    return _enable ();
  }

  virtual bool updateCheck () const
  {
    return _check ();
  }

private:
  ExecuteFunctor _functor;
  EnableFunctor _enable;
  CheckFunctor _check;
};


// Helper function.
template
<
  class ExecuteFunctor, 
  class CheckFunctor
>
Usul::Commands::Command *genericCheckCommand ( 
  const std::string &name, 
  ExecuteFunctor f, 
  CheckFunctor c,
  Usul::Interfaces::IUnknown * caller = 0x0 )
{
  typedef ExecutePolicy < ExecuteFunctor, CheckFunctor > Policy;
  typedef GenericCheckCommand < ExecuteFunctor, CheckFunctor, TrueFunctor, Policy > Cmd;
  return new Cmd ( name, f, c, caller );
}


template
<
  class ExecuteFunctor, 
  class CheckFunctor,
  class EnableFunctor
>
Usul::Commands::Command *genericCheckCommand ( 
  const std::string &name, 
  ExecuteFunctor f, 
  CheckFunctor c,
  EnableFunctor e,
  Usul::Interfaces::IUnknown * caller = 0x0 )
{
  typedef ExecutePolicy < ExecuteFunctor, CheckFunctor > Policy;
  typedef GenericCheckCommand < ExecuteFunctor, CheckFunctor, EnableFunctor, Policy > Cmd;
  return new Cmd ( name, f, c, e, caller );
}


template
<
  class ExecuteFunctor, 
  class CheckFunctor
>
Usul::Commands::Command *genericToggleCommand ( 
  const std::string &name, 
  ExecuteFunctor f, 
  CheckFunctor c,
  Usul::Interfaces::IUnknown * caller = 0x0 )
{
  typedef TogglePolicy < ExecuteFunctor, CheckFunctor > Policy;
  typedef GenericCheckCommand < ExecuteFunctor, CheckFunctor, TrueFunctor, Policy > Cmd;
  return new Cmd ( name, f, c, caller );
}


}
}

#endif // __USUL_COMMANDS_GENERIC_CHECK_COMMAND_H__
