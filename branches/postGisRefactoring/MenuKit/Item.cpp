
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
#include "Bits.h"
#include "Errors.h"

///\todo TODO: fix this! //#include "boost/mpl/assert_is_same.hpp"

#include <algorithm>
#include <sstream>
#include <stdexcept>

using namespace MenuKit;


///////////////////////////////////////////////////////////////////////////////
//
//  Default constructor.
//
///////////////////////////////////////////////////////////////////////////////

Item::Item(): Referenced(), 
  _callbacks   (),
  _text        (),
  _info        (),
  _iconFile    (),
  _parent      ( 0x0 ),
  _flags       ( Item::ENABLED )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Item::Item ( const std::string &text, const std::string &iconFile, Callback *cb, MenuKit::Message m ) :
  _callbacks   (),
  _text        ( text ),
  _info        (),
  _iconFile    ( iconFile ),
  _parent      ( 0x0 ),
  _flags       ( Item::ENABLED )
{
  // Set the callback.
  this->callback ( m, cb );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Item::Item ( const std::string &text, const std::string &iconFile ) :
  _callbacks   (),
  _text        ( text ),
  _info        (),
  _iconFile    ( iconFile ),
  _parent      ( 0x0 ),
  _flags       ( Item::ENABLED )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Copy constructor.
//
///////////////////////////////////////////////////////////////////////////////

Item::Item ( const Item &i ) : Referenced ( i ),
  _callbacks   ( i._callbacks ), 
  _text        ( i._text ), 
  _info        ( i._info ),
  _iconFile    ( i._iconFile ),
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
  _callbacks   = i._callbacks;
  _iconFile    = i._iconFile;
  _text        = i._text;
  _info        = i._info;
  _parent      = i._parent;
  _flags       = i._flags;

  return (*this);
}


///////////////////////////////////////////////////////////////////////////////
//
//  Send the message to the client.
//
///////////////////////////////////////////////////////////////////////////////

void Item::sendMessage ( MenuKit::Message m )
{
  Callback *cb = this->callback ( m );
  if ( cb )
  {
    (*cb) ( m, this );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the callback.
//
///////////////////////////////////////////////////////////////////////////////

void Item::callback ( MenuKit::Message m, Callback::Ptr c )
{
  _callbacks[m] = c;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the callback.
//
///////////////////////////////////////////////////////////////////////////////

Callback *Item::callback ( MenuKit::Message m )
{
  Callbacks::iterator i = _callbacks.find ( m );
  return ( _callbacks.end() == i ) ? 0x0 : i->second.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Predicate to compare second element in a pair.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  template < class SecondType > struct IsSameAsSecond
  {
    IsSameAsSecond ( const SecondType &t ) : _t ( t ){}
    template < class FirstType >
    bool operator () ( const std::pair<FirstType,SecondType> &p )
    {
      return ( p.second == _t );
    }
  private:
    const SecondType &_t;
  };
};


///////////////////////////////////////////////////////////////////////////////
//
//  Remove all occurance of the callback.
//
///////////////////////////////////////////////////////////////////////////////

void Item::removeAll ( Callback::Ptr c )
{
  ///\todo TODO: this call was not in boost version 1_32: //BOOST_MPL_ASSERT_IS_SAME ( Callbacks::mapped_type, Callback::Ptr );

  // Initialize.
  bool loop ( true );
  Detail::IsSameAsSecond<Callbacks::mapped_type> pred ( c );

  // Loop until we are told to stop.
  while ( loop )
  {
    // Find the first occurance.
    Callbacks::iterator i = std::find_if ( _callbacks.begin(), _callbacks.end(), pred );

    // If we did not find it then stop.
    if ( _callbacks.end() == i )
      loop = false;

    // Otherwise, erase it.
    else
      _callbacks.erase ( i );
  }
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
//  Get the checked state.
//
///////////////////////////////////////////////////////////////////////////////

bool Item::checked() const
{
  return Bits<unsigned int>::has ( this->flags(), Item::CHECKED );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the checked state.
//
///////////////////////////////////////////////////////////////////////////////

void Item::checked ( bool e )
{
  typedef MenuKit::Bits<unsigned int> Bits;
  if ( e )
    this->flags ( Bits::add    ( this->flags(), Item::CHECKED ) );
  else
    this->flags ( Bits::remove ( this->flags(), Item::CHECKED ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the toggle state.
//
///////////////////////////////////////////////////////////////////////////////

bool Item::toggle() const
{
  return Bits<unsigned int>::has ( this->flags(), Item::TOGGLE );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the toggle state.
//
///////////////////////////////////////////////////////////////////////////////

void Item::toggle ( bool e )
{
  typedef MenuKit::Bits<unsigned int> Bits;
  if ( e )
    this->flags ( Bits::add    ( this->flags(), Item::TOGGLE ) );
  else
    this->flags ( Bits::remove ( this->flags(), Item::TOGGLE ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the radio state.
//
///////////////////////////////////////////////////////////////////////////////

bool Item::radio() const
{
  return Bits<unsigned int>::has ( this->flags(), Item::RADIO );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the radio state.
//
///////////////////////////////////////////////////////////////////////////////

void Item::radio ( bool e )
{
  typedef MenuKit::Bits<unsigned int> Bits;
  if ( e )
    this->flags ( Bits::add    ( this->flags(), Item::RADIO ) );
  else
    this->flags ( Bits::remove ( this->flags(), Item::RADIO ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the enabled state.
//
///////////////////////////////////////////////////////////////////////////////

bool Item::enabled() const
{
  return Bits<unsigned int>::has ( this->flags(), Item::ENABLED );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the enabled state.
//
///////////////////////////////////////////////////////////////////////////////

void Item::enabled ( bool e )
{
  typedef MenuKit::Bits<unsigned int> Bits;
  if ( e )
    this->flags ( Bits::add    ( this->flags(), Item::ENABLED ) );
  else
    this->flags ( Bits::remove ( this->flags(), Item::ENABLED ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the separator state.
//
///////////////////////////////////////////////////////////////////////////////

bool Item::separator() const
{
  return Bits<unsigned int>::has ( this->flags(), Item::SEPARATOR );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the separator state.
//
///////////////////////////////////////////////////////////////////////////////

void Item::separator ( bool e )
{
  typedef MenuKit::Bits<unsigned int> Bits;
  if ( e )
    this->flags ( Bits::add    ( this->flags(), Item::SEPARATOR ) );
  else
    this->flags ( Bits::remove ( this->flags(), Item::SEPARATOR ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the expansion state.
//
///////////////////////////////////////////////////////////////////////////////

bool Item::expanded() const
{
  return Bits<unsigned int>::has ( this->flags(), Item::EXPANDED );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the expansion state.
//
///////////////////////////////////////////////////////////////////////////////

void Item::expanded ( bool e )
{
  typedef MenuKit::Bits<unsigned int> Bits;
  if ( e )
    this->flags ( Bits::add    ( this->flags(), Item::EXPANDED ) );
  else
    this->flags ( Bits::remove ( this->flags(), Item::EXPANDED ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the marked state.
//
///////////////////////////////////////////////////////////////////////////////

bool Item::marked() const
{
  return Bits<unsigned int>::has ( this->flags(), Item::MARKED );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the marked state.
//
///////////////////////////////////////////////////////////////////////////////

void Item::marked ( bool e )
{
  typedef MenuKit::Bits<unsigned int> Bits;
  if ( e )
    this->flags ( Bits::add    ( this->flags(), Item::MARKED ) );
  else
    this->flags ( Bits::remove ( this->flags(), Item::MARKED ) );
}