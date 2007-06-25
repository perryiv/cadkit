
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Wrapper class for a multiple button device.
//
///////////////////////////////////////////////////////////////////////////////

#include "ButtonGroup.h"

#include <sstream>
#include <stdexcept>

using namespace VRV::Devices;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

ButtonGroup::ButtonGroup() : BaseClass(),
  _buttons  (),
  _down     ( 0 ),
  _pressed  ( 0 ),
  _released ( 0 )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

ButtonGroup::~ButtonGroup()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the internal state.
//
///////////////////////////////////////////////////////////////////////////////

void ButtonGroup::update()
{
  // Initialize.
  _down = _pressed = _released = 0;

  // Loop though the buttons.
  for ( Buttons::size_type i = 0; i < _buttons.size(); ++i )
  {
    // Get the digital interface.
    Buttons::value_type &button = _buttons.at ( i );

    // Query the button's state.
    switch ( button->state() )
    {
      case gadget::Digital::ON:
        _down |= button->mask();
        break;
      case gadget::Digital::OFF:
        break;
      case gadget::Digital::TOGGLE_ON:
        _pressed |= button->mask();
        break;
      case gadget::Digital::TOGGLE_OFF:
        _released |= button->mask();
        break;
      default:
      {
        std::ostringstream message;
        message << "Error 2194534052: unknown state of digital interface: " << button->state();
        throw std::logic_error ( message.str() );
      }
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Small predicate that returns true if the masks are the same.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  struct SameMask : public std::unary_function < ButtonDevice::RefPtr *, bool >
  {
    SameMask ( unsigned long mask ) : _mask ( mask ){}

    template < class T >
    bool operator () ( const T &b ) const
    {
      return ( ( 0x0 != b.get() ) && ( _mask == b->mask() ) );
    }
  private:
    unsigned long _mask;
  };
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a device.
//
///////////////////////////////////////////////////////////////////////////////

void ButtonGroup::add ( ButtonDevice *b )
{
  // Only valid pointers.
  if ( 0x0 == b )
    throw std::invalid_argument ( "Error 3629364095: Null pointer." );

  // See if another button with the same mask is already in the group.
  if ( _buttons.end() != std::find_if ( _buttons.begin(), _buttons.end(), Detail::SameMask ( b->mask() ) ) )
  {
    std::ostringstream message;
    message << "Error 1927963521: Button mask value of " 
            << b->mask()
            << " is already in the button group.";
    throw std::runtime_error ( message.str() );
  }

  // Add the new button device.
  _buttons.push_back ( b );
}
