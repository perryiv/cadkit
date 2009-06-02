
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "MenuKit/ToggleButton.h"
#include "MenuKit/Visitor.h"

using namespace MenuKit;


///////////////////////////////////////////////////////////////////////////////
//
//  Default constructor.
//
///////////////////////////////////////////////////////////////////////////////

ToggleButton::ToggleButton(): BaseClass()
{
  this->radio ( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Default constructor.
//
///////////////////////////////////////////////////////////////////////////////

ToggleButton::ToggleButton( Usul::Commands::Command* command ): BaseClass()
{
  this->toggle ( true );
  this->command ( command );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Copy constructor.
//
///////////////////////////////////////////////////////////////////////////////

ToggleButton::ToggleButton ( const ToggleButton& b ): BaseClass ( b )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Assignment.
//
///////////////////////////////////////////////////////////////////////////////

ToggleButton& ToggleButton::operator = ( const ToggleButton& b )
{
  BaseClass::operator = ( b );
  return ( *this );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

ToggleButton::~ToggleButton()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Accept the visitor.
//
///////////////////////////////////////////////////////////////////////////////

void ToggleButton::accept ( Visitor &v )
{
  v.apply ( *this );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper command for toggling.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper {

class ToggleCommand : public Usul::Commands::Command
{
public:
  typedef Usul::Commands::Command        BaseClass;
  typedef ToggleButton::ExecuteFunction ExecuteFunctor;
  typedef ToggleButton::EnableFunction  EnableFunctor;
  typedef ToggleButton::CheckFunction CheckFunctor;

  USUL_DECLARE_QUERY_POINTERS  ( ToggleCommand );

  ToggleCommand ( const std::string& name, 
                       ExecuteFunctor functor, 
                       CheckFunctor check ) : 
    BaseClass ( 0x0 ),
    _functor ( functor ),
    _enable (),
    _check ( check )
  {
    this->text ( name );
  }

  ToggleCommand ( const std::string& name, 
                        ExecuteFunctor functor, 
                        CheckFunctor check, 
                        EnableFunctor e ) : 
    BaseClass ( 0x0 ),
    _functor ( functor ),
    _enable ( e ),
    _check ( check )
  {
    this->text ( name );
  }

  virtual Usul::Commands::Command* clone() const { return new ToggleCommand ( *this ); }

  static const std::type_info &classTypeId() { return typeid ( ToggleCommand ); }
  virtual const std::type_info &typeId() const { return ToggleCommand::classTypeId(); }

protected:
  virtual ~ToggleCommand () {}

  virtual void _execute()
  {
    _functor ( !_check() );
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

///////////////////////////////////////////////////////////////////////////////
//
//  Create a toggle button.
//
///////////////////////////////////////////////////////////////////////////////

ToggleButton* ToggleButton::create ( const std::string &name, ExecuteFunction f, CheckFunction c )
{
  Usul::Commands::Command::RefPtr command ( new Helper::ToggleCommand ( name, f, c ) );
  ToggleButton::RefPtr button ( new ToggleButton ( command.get() ) );
  return button.release();
}
