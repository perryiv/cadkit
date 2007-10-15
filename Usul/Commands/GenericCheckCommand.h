
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

template < class ExecuteFunctor, class CheckFunctor, class EnableFunctor = TrueFunctor >
class GenericCheckCommand : public Usul::Commands::Command
{
public:
  typedef Usul::Commands::Command                                             BaseClass;
  typedef GenericCheckCommand < ExecuteFunctor, CheckFunctor, EnableFunctor > ThisType;

  USUL_DECLARE_COMMAND  ( GenericCheckCommand );

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


protected:
  virtual ~GenericCheckCommand ()
  {
  }

  virtual void _execute ()
  {
    _functor ( !_check() );
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


template < class Functor, class CheckFunctor, class EnableFunctor >
const std::type_info &GenericCheckCommand < Functor, CheckFunctor, EnableFunctor >::classTypeId() 
{
  return typeid ( ThisType );
}


template < class Functor, class CheckFunctor, class EnableFunctor >
const std::type_info &GenericCheckCommand < Functor, CheckFunctor, EnableFunctor >::typeId() const 
{
  return ThisType::classTypeId();
}


template < class Functor, class CheckFunctor, class EnableFunctor >
Usul::Commands::Command* GenericCheckCommand < Functor, CheckFunctor, EnableFunctor >::clone ( ) const 
{
  return new ThisType ( *this );
}


}
}

#endif // __USUL_COMMANDS_GENERIC_CHECK_COMMAND_H__
