
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


struct FalseFunctor
{
  bool operator () () const
  {
    return false;
  }
};


struct TrueFunctor
{
  bool operator () () const
  {
    return true;
  }
};


template < class ExecuteFunctor, class EnableFunctor = TrueFunctor >
class GenericCommand : public Usul::Commands::Command
{
public:
  typedef Usul::Commands::Command BaseClass;
  typedef GenericCommand < ExecuteFunctor, EnableFunctor > ThisType;

  USUL_DECLARE_COMMAND  ( GenericCommand );

  GenericCommand ( const std::string& name, ExecuteFunctor functor, Usul::Interfaces::IUnknown * caller = 0x0 ) : 
    BaseClass ( caller ),
    _functor ( functor ),
    _enable ()
  {
    this->text ( name );
  }

  GenericCommand ( const std::string& name, ExecuteFunctor functor, EnableFunctor e, Usul::Interfaces::IUnknown * caller = 0x0 ) : 
    BaseClass ( caller ),
    _functor ( functor ),
    _enable ( e )
  {
    this->text ( name );
  }

  // Set the enable functor.
  void enableFunctor ( EnableFunctor f )
  {
    _enable = f;
  }

protected:
  virtual ~GenericCommand ()
  {
  }

  virtual void _execute ()
  {
    _functor ( );
  }

  virtual bool updateEnable () const
  {
    return _enable ();
  }

private:
  ExecuteFunctor _functor;
  EnableFunctor _enable;
};


template < class Functor, class EnableFunctor >
const std::type_info &GenericCommand < Functor, EnableFunctor >::classTypeId() 
{
  return typeid ( ThisType );
}


template < class Functor, class EnableFunctor >
const std::type_info &GenericCommand < Functor, EnableFunctor >::typeId() const 
{
  return ThisType::classTypeId();
}


template < class Functor, class EnableFunctor >
Usul::Commands::Command* GenericCommand < Functor, EnableFunctor >::clone ( ) const 
{
  return new ThisType ( *this );
}


}
}

#endif // __USUL_COMMANDS_GENERIC_COMMAND_H__
