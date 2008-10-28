
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "VRV/Core/BaseApplication.h"
#include "VRV/Core/JugglerFunctors.h"
#include "VRV/Core/Exceptions.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/App/Application.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Threads/Safe.h"
#include "Usul/Trace/Trace.h"
#include "Usul/Strings/Format.h"
#include "Usul/User/Directory.h"

#include "vrj/Kernel/Kernel.h"

using namespace VRV::Core;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

BaseApplication::BaseApplication() : 
  BaseClass ( vrj::Kernel::instance() ),
  _mutex(),
  _refCount          ( 0 ),
  _buttons           ( new VRV::Devices::ButtonGroup ),
  _analogs           (),
  _tracker           ( new VRV::Devices::TrackerDevice ( "VJWand" ) ),
  _navigationMatrix  (),
  _navigator         ( 0x0 )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

BaseApplication::~BaseApplication()
{
  USUL_TRACE_SCOPE;

  // Make sure we don't have any references hanging around.
  USUL_ASSERT ( 0 == _refCount );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown* BaseApplication::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::INavigationFunctor::IID:
    return static_cast<Usul::Interfaces::INavigationFunctor*> ( this );
  default:
    return 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Reference.  Keep track for debugging.
//
///////////////////////////////////////////////////////////////////////////////

void BaseApplication::ref()
{
  ++_refCount;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Unreference.  Keep track for debugging..
//
///////////////////////////////////////////////////////////////////////////////

void BaseApplication::unref ( bool )
{
  --_refCount;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Run the application.
//
///////////////////////////////////////////////////////////////////////////////

void BaseApplication::run()
{
  USUL_TRACE_SCOPE;

  // Get the kernel.
  vrj::Kernel* kernel ( vrj::Kernel::instance() );

  // Tell the kernel that we are the app.
  kernel->setApplication ( this );

  // Start the kernel.
  kernel->start();

  // Wait for events.
  kernel->waitForKernelStop();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Quit the application.
//
///////////////////////////////////////////////////////////////////////////////

void BaseApplication::quit()
{
  USUL_TRACE_SCOPE;
  vrj::Kernel::instance()->stop();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize this instance.
//
///////////////////////////////////////////////////////////////////////////////

void BaseApplication::init()
{
  bool stop ( false );

  try
  {
    this->_init();
  }

  catch ( const std::exception &e )
  {
    std::cout << "Error 1082603967: "
              << "Exception generated while initializing."
              << "\n\tWhat: " << e.what()
              << std::endl;
    stop = true;
  }
  catch ( ... )
  {
    std::cout << "Error 1082603859: "
              << "Unknown exception generated while initializing." 
              << std::endl;
    stop = true;
  }

  // Are you supposed to stop?
  if ( stop )
    vrj::Kernel::instance()->stop();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize the application.
//
///////////////////////////////////////////////////////////////////////////////

void BaseApplication::_init()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize shared data.
//
///////////////////////////////////////////////////////////////////////////////

void BaseApplication::_initializeSharedData ( const std::string& hostname )
{
  USUL_TRACE_SCOPE;

  // Initialize the shared navigation matrix.
  {
    vpr::GUID guid ( "FEFB5D44-9EC3-4fe3-B2C7-43C394A49848" );
    _navigationMatrix.init ( guid, hostname );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called before the frame.
//
///////////////////////////////////////////////////////////////////////////////

void BaseApplication::preFrame()
{
  Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( this, &BaseApplication::_preFrame ), "3669056808" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called before the frame.
//
///////////////////////////////////////////////////////////////////////////////

void BaseApplication::_preFrame()
{
  USUL_TRACE_SCOPE;

  // Update these input devices.
  {
    Guard guard ( this->mutex() );
    if ( true == _buttons.valid() )
      _buttons->notify();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called after preFrame, but before the frame.
//
///////////////////////////////////////////////////////////////////////////////

void BaseApplication::latePreFrame()
{
  Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( this, &BaseApplication::_latePreFrame ), "1897051329" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called after preFrame, but before the frame.
//
///////////////////////////////////////////////////////////////////////////////

void BaseApplication::_latePreFrame()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the frame is done.
//
///////////////////////////////////////////////////////////////////////////////

void BaseApplication::postFrame()
{
  Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( this, &BaseApplication::_postFrame ), "2751540437" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the frame is done.
//
///////////////////////////////////////////////////////////////////////////////

void BaseApplication::_postFrame()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the default configuration filename path.
//
///////////////////////////////////////////////////////////////////////////////

void BaseApplication::_setDefaultPath ( std::string& filename, const std::string& name )
{
  // Get the vendor and the program.
  const std::string vendor  ( Usul::App::Application::instance().vendor()  );
  const std::string program ( Usul::App::Application::instance().program() );

  // Set default file path.
  filename = Usul::Strings::format ( Usul::User::Directory::program ( vendor, program ), "/", name, ".xml" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Load all the config files.
//
///////////////////////////////////////////////////////////////////////////////

void BaseApplication::_loadConfigFiles ( const std::vector < std::string > &configs )
{
  USUL_TRACE_SCOPE;

  // See if there is at least one config file. Do not use the ErrorChecker.
  if ( configs.empty() )
  {
    std::cout << "No VRJuggler config-files specified."
              << "\n\tAttempting to use a sim-mode configuration."
              << std::endl;
    this->_loadSimConfigs();
    return;
  }

  // Load the config files.
  std::for_each ( configs.begin(), configs.end(), VRV::Core::Detail::LoadConfigFile() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Load the default config files.
//
///////////////////////////////////////////////////////////////////////////////

void BaseApplication::_loadSimConfigs()
{
  USUL_TRACE_SCOPE;

  std::cout << "Looking for environment variable VJ_CONFIG_DIR" << std::endl;

  // Try this environment variable.
  const char *dir = ::getenv ( "VJ_CONFIG_DIR" );
  if ( dir )
  {
    // Load default config files from this directory.
    this->_loadSimConfigs ( dir );
    return;
  }

  std::cout << "Environment variable VJ_CONFIG_DIR not found." << std::endl;
  std::cout << "Looking for environment variable VJ_BASE_DIR" << std::endl;

  // Try this environment variable.
  dir = ::getenv ( "VJ_BASE_DIR" );
  if ( dir )
  {
    // Make sure there is a slash.
    std::string d ( dir );
    std::string::size_type last ( d.size() - 1 );
    if ( '/' != d[last] || '\\' != d[last] )
      d += '/';

    // Add the sub-directory.
    d += "share/vrjuggler/data/configFiles";

    // Load default config files from this directory.
    this->_loadSimConfigs ( d );
    return;
  }

  std::cout << "Environment variable VJ_BASE_DIR not found." << std::endl;

  // If we get this far then we failed.
  Usul::Exceptions::Thrower < VRV::Core::Exceptions::UserInput > 
    ( "No default VRJuggler config-files found." );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Load the default config files from the given directory.
//
///////////////////////////////////////////////////////////////////////////////

void BaseApplication::_loadSimConfigs ( const std::string& d )
{
  USUL_TRACE_SCOPE;

  // Return if string is empty.
  if ( d.empty() )
    return;

  // Make a copy.
  std::string dir ( d );

  // Make sure there is a slash.
  std::string::size_type last ( dir.size() - 1 );
  if ( '/' != dir[last] || '\\' != dir[last] )
    dir += '/';

  // The config-file loader.
  VRV::Core::Detail::LoadConfigFile loader;

  // Load the config files.
  loader ( dir + "sim.base.jconf" );
  loader ( dir + "sim.wand.mixin.jconf" );
  loader ( dir + "sim.analog.mixin.jconf" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the buttons.
//
///////////////////////////////////////////////////////////////////////////////

void BaseApplication::buttons ( ButtonGroup* buttons )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  _buttons = buttons;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the buttons.
//
///////////////////////////////////////////////////////////////////////////////

VRV::Devices::ButtonGroup* BaseApplication::buttons()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _buttons;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the buttons.
//
///////////////////////////////////////////////////////////////////////////////

const VRV::Devices::ButtonGroup* BaseApplication::buttons() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _buttons;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add the listener.
//
///////////////////////////////////////////////////////////////////////////////

void BaseApplication::_addButtonPressListener ( Usul::Interfaces::IUnknown * caller )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  for ( ButtonGroup::iterator iter = _buttons->begin(); iter != _buttons->end(); ++iter )
  {
    (*iter)->addButtonPressListener ( caller );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove all listeners.
//
///////////////////////////////////////////////////////////////////////////////

void BaseApplication::_clearButtonPressListeners()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  for ( ButtonGroup::iterator iter = _buttons->begin(); iter != _buttons->end(); ++iter )
  {
    (*iter)->clearButtonPressListeners();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove the listener.
//
///////////////////////////////////////////////////////////////////////////////

void BaseApplication::_removeButtonPressListener ( Usul::Interfaces::IUnknown * caller )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  for ( ButtonGroup::iterator iter = _buttons->begin(); iter != _buttons->end(); ++iter )
  {
    (*iter)->removeButtonPressListener ( caller );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add the listener.
//
///////////////////////////////////////////////////////////////////////////////

void BaseApplication::_addButtonReleaseListener ( Usul::Interfaces::IUnknown * caller )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  for ( ButtonGroup::iterator iter = _buttons->begin(); iter != _buttons->end(); ++iter )
  {
    (*iter)->addButtonReleaseListener ( caller );
  }
  
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove all listeners.
//
///////////////////////////////////////////////////////////////////////////////

void BaseApplication::_clearButtonReleaseListeners()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  for ( ButtonGroup::iterator iter = _buttons->begin(); iter != _buttons->end(); ++iter )
  {
    (*iter)->clearButtonReleaseListeners();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove the listener.
//
///////////////////////////////////////////////////////////////////////////////

void BaseApplication::_removeButtonReleaseListener ( Usul::Interfaces::IUnknown *caller  )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  for ( ButtonGroup::iterator iter = _buttons->begin(); iter != _buttons->end(); ++iter )
  {
    (*iter)->removeButtonReleaseListener ( caller );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get a copy of the analogs.
//
///////////////////////////////////////////////////////////////////////////////

BaseApplication::Analogs BaseApplication::analogs() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _analogs;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add an analog.
//
///////////////////////////////////////////////////////////////////////////////

void BaseApplication::analogAdd ( JoystickPtr analog )
{
  USUL_TRACE_SCOPE;

  if ( true == analog.valid() )
  {
    Guard guard ( this->mutex() );
    _analogs[analog->name()] = analog;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Find an analog.
//
///////////////////////////////////////////////////////////////////////////////

BaseApplication::JoystickPtr BaseApplication::analogFind ( const std::string& key )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  Analogs::iterator iter ( _analogs.find ( key ) );
  return ( iter != _analogs.end() ? iter->second : 0x0 );
}



///////////////////////////////////////////////////////////////////////////////
//
//  Set the trim. This assumes the user is not tilting the joystick one way 
//  or the other. It records the value at the neutral position. If the value 
//  is 0.5 (like it should be) then the "trim" will be zero.
//
///////////////////////////////////////////////////////////////////////////////

void BaseApplication::analogsCalibrate()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  for( Analogs::iterator iter = _analogs.begin(); iter != _analogs.end(); ++iter )
  {
		Joystick::RefPtr joystick ( iter->second );

		if ( joystick.valid() )
		{
			float x ( 0.5f - joystick->horizontal() );
			float y ( 0.5f - joystick->vertical() );
	  
			joystick->analogTrim ( x, y );
		}
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update all analogs.
//
///////////////////////////////////////////////////////////////////////////////

void BaseApplication::analogsUpdate()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  for( Analogs::iterator iter = _analogs.begin(); iter != _analogs.end(); ++iter )
  {
    Joystick::RefPtr joystick ( iter->second );

    if ( joystick.valid() )
    {
      // update all the joystick analog inputs
      joystick->update();

      // Send any notifications to all joystick analog inputs.
      joystick->notify();
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the tracker.
//
///////////////////////////////////////////////////////////////////////////////

void BaseApplication::tracker ( TrackerDevice* tracker )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  _tracker = tracker;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the tracker.
//
///////////////////////////////////////////////////////////////////////////////

BaseApplication::TrackerDevice* BaseApplication::tracker()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _tracker.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the tracker.
//
///////////////////////////////////////////////////////////////////////////////

const BaseApplication::TrackerDevice* BaseApplication::tracker() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _tracker.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the tracker's position.
//
///////////////////////////////////////////////////////////////////////////////

void BaseApplication::trackerPosition ( Usul::Math::Vec3d& position ) const
{
  USUL_TRACE_SCOPE;
  
  // Get the tracker.
  TrackerPtr tracker ( Usul::Threads::Safe::get ( this->mutex(), _tracker ) );

  // Set the position.
  if ( tracker.valid() )
  {
    position.set ( tracker->x(), tracker->y(), tracker->z() );
  }
  // Set to zero.
  else
  {
    position.set ( 0.0, 0.0, 0.0 );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the trackers's matrix.
//
///////////////////////////////////////////////////////////////////////////////

void BaseApplication::trackerMatrix ( Matrix& W ) const
{
  USUL_TRACE_SCOPE;
  
  // Get the tracker.
  TrackerPtr tracker ( Usul::Threads::Safe::get ( this->mutex(), _tracker ) );

  // Set the given matrix from the wand's matrix.
  if ( true == tracker.valid() )
    W.set ( tracker->matrix().getData() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the tracker.
//
///////////////////////////////////////////////////////////////////////////////

void BaseApplication::trackerUpdate()
{
  TrackerPtr tracker ( Usul::Threads::Safe::get ( this->mutex(), _tracker ) );
  if ( tracker.valid() )
    tracker->update();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the navigator.
//
///////////////////////////////////////////////////////////////////////////////

void BaseApplication::navigator ( Navigator * navigator )
{
  USUL_TRACE_SCOPE;

  // Save the old navigator.
  Navigator::RefPtr old ( this->navigator() );
  
  {
    Guard guard ( this->mutex () );
    _navigator = navigator;
  }

  this->_navigatorChanged ( navigator, old );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the navigator.
//
///////////////////////////////////////////////////////////////////////////////

BaseApplication::Navigator* BaseApplication::navigator ()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex () );
  return _navigator;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the navigator.
//
///////////////////////////////////////////////////////////////////////////////

const BaseApplication::Navigator* BaseApplication::navigator () const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex () );
  return _navigator;
}


///////////////////////////////////////////////////////////////////////////////
//
//  The navigator has changed.
//
///////////////////////////////////////////////////////////////////////////////

void BaseApplication::_navigatorChanged ( Navigator::RefPtr newNavigator, Navigator::RefPtr oldNavigator )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Post-multiply the navigation matrix by the given matrix.
//
///////////////////////////////////////////////////////////////////////////////

void BaseApplication::navigationPostMultiply ( const Matrix &M )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex () );
  Matrix matrix ( _navigationMatrix->data() );
  matrix.postMultiply ( M );
  _navigationMatrix->data ( matrix );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Pre-multiply the navigation matrix by the given matrix.
//
///////////////////////////////////////////////////////////////////////////////

void BaseApplication::navigationPreMultiply ( const Matrix &M )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex () );
  Matrix matrix ( _navigationMatrix->data() );
  matrix.preMultiply ( M );
  _navigationMatrix->data ( matrix );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the navigation matrix.
//
///////////////////////////////////////////////////////////////////////////////

void BaseApplication::_navigationMatrixSet ( const Matrix& m )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex () );
  _navigationMatrix->data ( m );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the navigation matrix.
//
///////////////////////////////////////////////////////////////////////////////

BaseApplication::Matrix BaseApplication::_navigationMatrixGet()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex () );
  return _navigationMatrix->data();
}

