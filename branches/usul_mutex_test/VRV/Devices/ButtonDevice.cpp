
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Wrapper class for a single gadget::DigitalInterface.
//
///////////////////////////////////////////////////////////////////////////////

#include "VRV/Devices/ButtonDevice.h"

#include "Usul/Trace/Trace.h"

#include "gadget/Type/Digital.h"

using namespace VRV::Devices;

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( ButtonDevice, ButtonDevice::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

ButtonDevice::ButtonDevice ( unsigned long mask, const std::string &name ) : 
  BaseClass(),
  _di(),
  _mask ( mask ),
  _pressed(),
  _released(),
	_buttonName ( name )
{
  USUL_TRACE_SCOPE;

  // Initialize.
  _di.init ( name );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

ButtonDevice::ButtonDevice ( unsigned long mask, const std::string &vrj_name, const std::string &name ) : 
  BaseClass(),
  _di(),
  _mask ( mask ),
  _pressed(),
  _released(),
  _buttonName ( name )
{
  USUL_TRACE_SCOPE;

  // Initialize.
  _di.init ( vrj_name );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

ButtonDevice::~ButtonDevice()
{
  USUL_TRACE_SCOPE;
  this->clearButtonPressListeners();
  this->clearButtonReleaseListeners();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the device mask.
//
///////////////////////////////////////////////////////////////////////////////

unsigned long ButtonDevice::mask () const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _mask;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the device state.
//
///////////////////////////////////////////////////////////////////////////////

unsigned long ButtonDevice::state() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  // Jump through these hoops because getData() is not const.
  DI &di = const_cast < DI & > ( _di );
  return static_cast < unsigned long > ( di->getData() );
}

///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown* ButtonDevice::queryInterface ( unsigned long iid )
{
  USUL_TRACE_SCOPE;

  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::IButtonPressSubject::IID:
    return static_cast < Usul::Interfaces::IButtonPressSubject* > ( this );
  case Usul::Interfaces::IButtonReleaseSubject::IID:
    return static_cast < Usul::Interfaces::IButtonReleaseSubject* > ( this );
  case Usul::Interfaces::IButtonID::IID:
    return static_cast < Usul::Interfaces::IButtonID * > ( this );
  default:
    return 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Notify listeners if state changed.
//
///////////////////////////////////////////////////////////////////////////////

void ButtonDevice::notify()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  // Get state.
  unsigned long state ( static_cast < unsigned long > ( _di->getData() ) );

  // Notify pressed listeners.
  if ( gadget::Digital::TOGGLE_ON == state )
    this->_notifyPressed();

  // Notify released listeners.
  if ( gadget::Digital::TOGGLE_OFF == state )
    this->_notifyReleased();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Notify of button press.
//
///////////////////////////////////////////////////////////////////////////////

void ButtonDevice::_notifyPressed()
{
  USUL_TRACE_SCOPE;

  ButtonPressListeners pressed;
  {
    Guard guard ( this->mutex() );
    pressed = _pressed;
  }

  Usul::Interfaces::IUnknown::QueryPtr unknown ( this->queryInterface ( Usul::Interfaces::IUnknown::IID ) );
  for ( ButtonPressListeners::iterator iter = pressed.begin(); iter != pressed.end(); ++iter )
  {
    // Break if we aren't suppose to continue.
    if ( false == (*iter)->buttonPressNotify ( unknown.get() ) )
      break;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Notify of button release.
//
///////////////////////////////////////////////////////////////////////////////

void ButtonDevice::_notifyReleased()
{
  USUL_TRACE_SCOPE;

  ButtonReleaseListeners released;
  {
    Guard guard ( this->mutex() );
    released = _released;
  }

  Usul::Interfaces::IUnknown::QueryPtr unknown ( this->queryInterface ( Usul::Interfaces::IUnknown::IID ) );
  for ( ButtonReleaseListeners::iterator iter = released.begin(); iter != released.end(); ++iter )
  {
    // Break if we aren't suppose to continue.
    if ( false == (*iter)->buttonReleaseNotify ( unknown.get() ) )
      break;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add the listener.
//
///////////////////////////////////////////////////////////////////////////////

void ButtonDevice::addButtonPressListener ( IUnknown *caller )
{
  USUL_TRACE_SCOPE;

  // Don't add twice.
  this->removeButtonPressListener ( caller );

  IButtonPressListener::QueryPtr listener ( caller );
  if ( true == listener.valid() )
  {
    
    Guard guard ( this->mutex() );
    _pressed.push_back ( IButtonPressListener::RefPtr ( listener.get() ) );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add the listener.
//
///////////////////////////////////////////////////////////////////////////////

void ButtonDevice::addButtonReleaseListener ( IUnknown *caller )
{
  USUL_TRACE_SCOPE;

  // Don't add twice.
  this->removeButtonReleaseListener ( caller );

  IButtonReleaseListener::QueryPtr listener ( caller );
  if ( true == listener.valid() )
  {
    Guard guard ( this->mutex() );
    _released.push_back ( IButtonReleaseListener::RefPtr ( listener.get() ) );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove all render listeners.
//
///////////////////////////////////////////////////////////////////////////////

void ButtonDevice::clearButtonPressListeners()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  _pressed.clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove all render listeners.
//
///////////////////////////////////////////////////////////////////////////////

void ButtonDevice::clearButtonReleaseListeners()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  _released.clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove the listener.
//
///////////////////////////////////////////////////////////////////////////////

void ButtonDevice::removeButtonPressListener ( IUnknown *caller )
{
  USUL_TRACE_SCOPE;

  IButtonPressListener::QueryPtr listener ( caller );
  if ( true == listener.valid() )
  {
    Guard guard ( this->mutex() );
    IButtonPressListener::RefPtr value ( listener.get() );
    ButtonPressListeners::iterator end ( std::remove ( _pressed.begin(), _pressed.end(), value ) );
    _pressed.erase ( end, _pressed.end() );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove the listener.
//
///////////////////////////////////////////////////////////////////////////////

void ButtonDevice::removeButtonReleaseListener ( IUnknown *caller )
{
  USUL_TRACE_SCOPE;

  IButtonReleaseListener::QueryPtr listener ( caller );
  if ( true == listener.valid() )
  {
    Guard guard ( this->mutex() );
    IButtonReleaseListener::RefPtr value ( listener.get() );
    ButtonReleaseListeners::iterator end ( std::remove ( _released.begin(), _released.end(), value ) );
    _released.erase ( end, _released.end() );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the button id.
//
///////////////////////////////////////////////////////////////////////////////

unsigned long ButtonDevice::buttonID () const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex () );
  return _mask;
}

///////////////////////////////////////////////////////////////////////////////
//
//  Get the button name
//
///////////////////////////////////////////////////////////////////////////////

std::string ButtonDevice::getButtonName() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex () );

  return _buttonName;
}
