
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

#ifndef __USUL_COMMANDS_GENERIC_COMMAND_H__
#define __USUL_COMMANDS_GENERIC_COMMAND_H__

#include "Usul/Commands/Command.h"

namespace Usul {
namespace Commands {


template < class Functor >
class GenericCommand : public Usul::Commands::Command
{
public:
  typedef Usul::Commands::Command BaseClass;

  USUL_DECLARE_COMMAND  ( GenericCommand );

  GenericCommand ( const std::string& name, Functor functor, Usul::Interfaces::IUnknown * caller = 0x0 ) : 
    BaseClass ( caller ),
    _functor ( functor )
  {
    this->text ( name );
  }

protected:
  virtual ~GenericCommand ()
  {
  }

  virtual void _execute ()
  {
    _functor ( );
  }

private:
  Functor _functor;
};


template < class Functor >
const std::type_info &GenericCommand < Functor >::classTypeId() 
{
  return typeid ( GenericCommand < Functor > );
}


template < class Functor >
const std::type_info &GenericCommand < Functor >::typeId() const 
{
  return GenericCommand < Functor >::classTypeId();
}


template < class Functor >
Usul::Commands::Command* GenericCommand < Functor >::clone ( ) const 
{
  return new GenericCommand < Functor > ( *this );
}


}
}

#endif // __USUL_COMMANDS_GENERIC_COMMAND_H__
