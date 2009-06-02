
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

#include "boost/bind.hpp"
#include "boost/function.hpp"

namespace Usul {
namespace Commands {


class GenericCommand : public Usul::Commands::Command
{
public:
  typedef Usul::Commands::Command BaseClass;
  typedef boost::function<void ()> ExecuteFunctor;
  typedef boost::function<bool ()> EnableFunctor;
  typedef GenericCommand ThisType;

  USUL_DECLARE_QUERY_POINTERS  ( GenericCommand );

  GenericCommand ( const std::string& name, ExecuteFunctor functor, Usul::Interfaces::IUnknown * caller = 0x0 ) : 
    BaseClass ( caller ),
    _functor ( functor ),
    _enable()
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

  virtual Usul::Commands::Command* clone() const { return new ThisType ( *this ); }

  static const std::type_info &classTypeId() { return typeid ( ThisType ); }
  virtual const std::type_info &typeId() const { return ThisType::classTypeId(); }

protected:

  virtual ~GenericCommand() {}

  /// Execute the command.
  virtual void _execute()
  {
    _functor();
  }


  /// Update the enable state.
  virtual bool updateEnable() const
  {
    return ( _enable ? _enable() : true );
  }

private:
  ExecuteFunctor _functor;
  EnableFunctor _enable;
};


// Helper function.
inline GenericCommand *genericCommand ( const std::string &name, GenericCommand::ExecuteFunctor f )
{
  return new GenericCommand ( name, f );
}

// Helper function.
inline GenericCommand *genericCommand ( const std::string &name, GenericCommand::ExecuteFunctor f, GenericCommand::EnableFunctor e )
{
  return new GenericCommand ( name, f, e );
}


// Helper function.
inline GenericCommand *makeCommandWithIcon ( const std::string &name, const std::string& icon, GenericCommand::ExecuteFunctor f )
{
  GenericCommand::RefPtr cmd ( new GenericCommand ( name, f ) );
  cmd->iconPath ( icon );
  return cmd.release();
}


// Helper function.
inline GenericCommand *makeCommandWithIcon ( const std::string &name, const std::string& icon, GenericCommand::ExecuteFunctor f, GenericCommand::EnableFunctor e )
{
  GenericCommand::RefPtr cmd ( new GenericCommand ( name, f, e ) );
  cmd->iconPath ( icon );
  return cmd.release();
}
  
}
}

#define USUL_MAKE_COMMAND(name,icon,object,fun) \
Usul::Commands::genericCommand ( name, icon, boost::bind ( fun, object ) )

#define USUL_MAKE_COMMAND_ENABLE(name,icon,object,fun,enable_fun) \
Usul::Commands::makeCommandWithIcon ( name, icon, boost::bind ( fun, object ), boost::bind ( enable_fun, object ) )

#define USUL_MAKE_COMMAND_ARG0(name,icon,object,fun,arg0) \
  Usul::Commands::makeCommandWithIcon ( name, icon, boost::bind ( fun, object, arg0 ) )

#endif // __USUL_COMMANDS_GENERIC_COMMAND_H__
