
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "MenuKit/RadioButton.h"
#include "MenuKit/Visitor.h"

using namespace MenuKit;


///////////////////////////////////////////////////////////////////////////////
//
//  Default constructor.
//
///////////////////////////////////////////////////////////////////////////////

RadioButton::RadioButton(): BaseClass()
{
  this->radio ( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Default constructor.
//
///////////////////////////////////////////////////////////////////////////////

RadioButton::RadioButton( Usul::Commands::Command* command ): BaseClass()
{
  this->radio ( true );
  this->command ( command );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Copy constructor.
//
///////////////////////////////////////////////////////////////////////////////

RadioButton::RadioButton ( const RadioButton& b ): BaseClass ( b )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Assignment.
//
///////////////////////////////////////////////////////////////////////////////

RadioButton& RadioButton::operator = ( const RadioButton& b )
{
  BaseClass::operator = ( b );
  return ( *this );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

RadioButton::~RadioButton()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Accept the visitor.
//
///////////////////////////////////////////////////////////////////////////////

void RadioButton::accept ( Visitor &v )
{
  v.apply ( *this );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper check command.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper {

class CheckCommand : public Usul::Commands::Command
{
public:
  typedef Usul::Commands::Command  BaseClass;
  typedef RadioButton::ExecuteFunction ExecuteFunctor;
  typedef RadioButton::EnableFunction EnableFunctor;
  typedef RadioButton::CheckFunction CheckFunctor;
  typedef CheckCommand ThisType;

  USUL_DECLARE_QUERY_POINTERS  ( CheckCommand );

  CheckCommand ( const std::string& name, 
                       ExecuteFunctor functor, 
                       CheckFunctor check ) : 
    BaseClass ( 0x0 ),
    _functor ( functor ),
    _enable(),
    _check ( check )
  {
    this->text ( name );
  }

  CheckCommand ( const std::string& name, 
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

  virtual Usul::Commands::Command* clone() const { return new ThisType ( *this ); }

  static const std::type_info &classTypeId() { return typeid ( ThisType ); }
  virtual const std::type_info &typeId() const { return ThisType::classTypeId(); }

protected:
  virtual ~CheckCommand () {}

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

///////////////////////////////////////////////////////////////////////////////
//
//  Create a RadioButton.
//
///////////////////////////////////////////////////////////////////////////////

RadioButton* RadioButton::create ( const std::string &name, ExecuteFunction f, CheckFunction c )
{
  Usul::Commands::Command::RefPtr command ( new Helper::CheckCommand ( name, f, c ) );
  RadioButton::RefPtr button ( new RadioButton ( command.get() ) );
  return button.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create a RadioButton.
//
///////////////////////////////////////////////////////////////////////////////

RadioButton* RadioButton::create ( const std::string &name, ExecuteFunction f, CheckFunction c, EnableFunction e )
{
  Usul::Commands::Command::RefPtr command ( new Helper::CheckCommand ( name, f, c, e ) );
  RadioButton::RefPtr button ( new RadioButton ( command.get() ) );
  return button.release();
}
