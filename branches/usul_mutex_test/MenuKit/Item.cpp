
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant and Perry L. Miller IV.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Base class for all menu items.
//
///////////////////////////////////////////////////////////////////////////////

#include "Item.h"
#include "Visitor.h"
#include "Errors.h"

#include "Usul/Bits/Bits.h"

#include <algorithm>
#include <sstream>
#include <stdexcept>

using namespace MenuKit;


///////////////////////////////////////////////////////////////////////////////
//
//  Default constructor.
//
///////////////////////////////////////////////////////////////////////////////

Item::Item(): BaseClass (), 
  _parent      ( 0x0 ),
  _flags       ( Item::ENABLED )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Copy constructor.
//
///////////////////////////////////////////////////////////////////////////////

Item::Item ( const Item &i ) : BaseClass ( i ),
  _parent      ( i._parent ),
  _flags       ( i._flags )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Item::~Item()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Assignment.
//
///////////////////////////////////////////////////////////////////////////////

Item& Item::operator = ( const Item &i )
{
  BaseClass::operator = (i);
  _parent      = i._parent;
  _flags       = i._flags;

  return (*this);
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the flags.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Item::flags() const
{
  return _flags;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the flags.
//
///////////////////////////////////////////////////////////////////////////////

void Item::flags ( unsigned int f )
{
  _flags = f;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the enabled state.
//
///////////////////////////////////////////////////////////////////////////////

bool Item::enabled() const
{
  return Usul::Bits::has <unsigned int, unsigned int> ( this->flags(), Item::ENABLED );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the enabled state.
//
///////////////////////////////////////////////////////////////////////////////

void Item::enabled ( bool e )
{
  if ( e )
    this->flags ( Usul::Bits::add    <unsigned int, unsigned int> ( this->flags(), Item::ENABLED ) );
  else
    this->flags ( Usul::Bits::remove <unsigned int, unsigned int> ( this->flags(), Item::ENABLED ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the expansion state.
//
///////////////////////////////////////////////////////////////////////////////

bool Item::expanded() const
{
  return Usul::Bits::has <unsigned int, unsigned int> ( this->flags(), Item::EXPANDED );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the expansion state.
//
///////////////////////////////////////////////////////////////////////////////

void Item::expanded ( bool e )
{
  if ( e )
    this->flags ( Usul::Bits::add    <unsigned int, unsigned int> ( this->flags(), Item::EXPANDED ) );
  else
    this->flags ( Usul::Bits::remove <unsigned int, unsigned int> ( this->flags(), Item::EXPANDED ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the marked state.
//
///////////////////////////////////////////////////////////////////////////////

bool Item::marked() const
{
  return Usul::Bits::has <unsigned int, unsigned int> ( this->flags(), Item::MARKED );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the marked state.
//
///////////////////////////////////////////////////////////////////////////////

void Item::marked ( bool e )
{
  if ( e )
    this->flags ( Usul::Bits::add    <unsigned int, unsigned int> ( this->flags(), Item::MARKED ) );
  else
    this->flags ( Usul::Bits::remove <unsigned int, unsigned int> ( this->flags(), Item::MARKED ) );
}
