
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Visitor that sets the flags to all items.
//
///////////////////////////////////////////////////////////////////////////////

#include "FlagsVisitor.h"
#include "Menu.h"
#include "Button.h"
#include "Bits.h"

#include <stdexcept>
using namespace MenuKit;


///////////////////////////////////////////////////////////////////////////////
//
//  Default constructor.
//
///////////////////////////////////////////////////////////////////////////////

FlagsVisitor::FlagsVisitor ( Action action, unsigned  int flags ): BaseClass ( BaseClass::ALL ),
  _action ( action ),
  _flags  ( flags )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

FlagsVisitor::~FlagsVisitor()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Apply the visitor to the item.
//
///////////////////////////////////////////////////////////////////////////////

void FlagsVisitor::_apply ( Item &item )
{
  typedef MenuKit::Bits<unsigned int> Bits;

  switch ( _action )
  {
  case ADD:
    item.flags ( Bits::add ( item.flags(), _flags ) );
    break;
  case REMOVE:
    item.flags ( Bits::remove ( item.flags(), _flags ) );
    break;
  case TOGGLE:
    item.flags ( Bits::toggle ( item.flags(), _flags ) );
    break;
  default:
    throw std::runtime_error ( "Error 2410737922, invalid '_action' member." );
    break;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Apply the visitor to the menu.
//
///////////////////////////////////////////////////////////////////////////////

void FlagsVisitor::apply ( Menu &m )
{
  this->_apply ( m );
  this->traverse ( m );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Apply the visitor to the button.
//
///////////////////////////////////////////////////////////////////////////////

void FlagsVisitor::apply ( Button &b )
{
  this->_apply ( b );
}