
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Menu button.
//
///////////////////////////////////////////////////////////////////////////////

#include "Button.h"
#include "Visitor.h"

using namespace MenuKit;


///////////////////////////////////////////////////////////////////////////////
//
//  Default constructor.
//
///////////////////////////////////////////////////////////////////////////////

Button::Button(): BaseClass()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Button::Button ( const std::string &name, const std::string &icon_file, Callback *cb, MenuKit::Message m ) : 
  BaseClass ( name, icon_file, cb, m )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Button::Button ( const std::string &name, const std::string &icon_file ) : 
  BaseClass ( name, icon_file )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Copy constructor.
//
///////////////////////////////////////////////////////////////////////////////

Button::Button ( const Button& b ): BaseClass ( b )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Assignment.
//
///////////////////////////////////////////////////////////////////////////////

Button& Button::operator = ( const Button& b )
{
  BaseClass::operator = ( b );
  return ( *this );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Button::~Button()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Accept the visitor.
//
///////////////////////////////////////////////////////////////////////////////

void Button::accept ( Visitor &v )
{
  v.apply ( *this );
}
