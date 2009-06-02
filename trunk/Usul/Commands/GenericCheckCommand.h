
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

#include "boost/function.hpp"

namespace Usul {
namespace Commands {


class GenericCheckCommand : public Usul::Commands::Command
{
public:
  typedef Usul::Commands::Command  BaseClass;
  typedef boost::function<void ()> ExecuteFunctor;
  typedef boost::function<bool ()> EnableFunctor;
  typedef boost::function<bool ()> CheckFunctor;
  typedef GenericCheckCommand ThisType;

  USUL_DECLARE_QUERY_POINTERS  ( GenericCheckCommand );

  GenericCheckCommand ( const std::string& name, 
                       ExecuteFunctor functor, 
                       CheckFunctor check, 
                       Usul::Interfaces::IUnknown * caller = 0x0 ) : 
    BaseClass ( caller ),
    _functor ( functor ),
    _enable(),
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

  virtual Usul::Commands::Command* clone() const { return new ThisType ( *this ); }

  static const std::type_info &classTypeId() { return typeid ( ThisType ); }
  virtual const std::type_info &typeId() const { return ThisType::classTypeId(); }

protected:
  virtual ~GenericCheckCommand () {}

  virtual void _execute()
  {
    _functor();
  }

  virtual bool updateEnable() const
  {
    return ( _enable ? _enable() : true );
  }

  virtual bool updateCheck() const
  {
    return _check();
  }

private:
  ExecuteFunctor _functor;
  EnableFunctor _enable;
  CheckFunctor _check;
};


}
}

#endif // __USUL_COMMANDS_GENERIC_CHECK_COMMAND_H__
