
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __VRV_CORE_BASE_APPLICATION_H__
#define __VRV_CORE_BASE_APPLICATION_H__

#include "VRV/Export.h"

#include "VRV/Devices/JoystickDevice.h"
#include "VRV/Devices/ButtonGroup.h"
#include "VRV/Devices/TrackerDevice.h"

#include "Usul/Math/Matrix44.h"
#include "Usul/Threads/RecursiveMutex.h"
#include "Usul/Threads/Guard.h"

#include "vrj/Draw/OGL/GlApp.h"

#include <string>

namespace VRV {
namespace Core {


class VRV_EXPORT BaseApplication : public vrj::GlApp
{
public:

  // Typedefs.
  typedef vrj::GlApp                           BaseClass;
  typedef Usul::Threads::RecursiveMutex        Mutex;
  typedef Usul::Threads::Guard<Mutex>          Guard;
  typedef VRV::Devices::ButtonGroup            ButtonGroup;
  typedef ButtonGroup::ValidRefPtr             ButtonsPtr;
  typedef VRV::Devices::TrackerDevice          TrackerDevice;
  typedef TrackerDevice::ValidRefPtr           TrackerPtr;
  typedef VRV::Devices::JoystickDevice         Joystick;
  typedef Joystick::RefPtr                     JoystickPtr;
  typedef std::map<std::string, JoystickPtr>   Analogs;
  typedef Usul::Math::Matrix44d                Matrix;

  // Constructors.
  BaseApplication();

  // Destructor.
  virtual ~BaseApplication();

  /// Get a copy of the analogs.
  Analogs                       analogs() const;

  /// Add an analog.
  void                          analogAdd ( JoystickPtr analog );

  /// Calibrate the analogs.
  void                          analogsCalibrate();

  /// Find an analog.
  JoystickPtr                   analogFind ( const std::string& key );

  /// Update all analogs.
  void                          analogsUpdate();

  /// Set/get the buttons.
  void                          buttons ( ButtonGroup* buttons );
  ButtonGroup *                 buttons();
  const ButtonGroup *           buttons() const;

  /// Get the mutex.
  Mutex&                        mutex() const { return _mutex; }

  void                          quit();
  void                          run();

  /// Set/get the tracker.
  void                          tracker ( TrackerDevice* tracker );
  TrackerDevice*                tracker();
  const TrackerDevice*          tracker() const;

  /// Get the tracker's position.
  void                          trackerPosition ( Usul::Math::Vec3d& ) const;

  /// Get the trackers's matrix.
  void                          trackerMatrix ( Matrix& W ) const;

  /// Update the tracker.
  void                          trackerUpdate();

protected:

  virtual void                  _init();
  virtual void                  _preFrame();
  virtual void                  _latePreFrame();
  virtual void                  _postFrame();

  // Add the listener.
  void                          _addButtonPressListener ( Usul::Interfaces::IUnknown * );
  void                          _addButtonReleaseListener ( Usul::Interfaces::IUnknown * );

  // Remove all listeners.
  void                          _clearButtonPressListeners();
  void                          _clearButtonReleaseListeners();

  // Remove the listener.
  void                          _removeButtonPressListener ( Usul::Interfaces::IUnknown * );
  void                          _removeButtonReleaseListener ( Usul::Interfaces::IUnknown * );

  // Load VR Juggler config files.
  void                          _loadConfigFiles ( const std::vector < std::string > &configs );
  void                          _loadSimConfigs  ( const std::string& dir );
  void                          _loadSimConfigs();

  // Set the default configuration filename path.
  void                          _setDefaultPath ( std::string& filename, const std::string& name );

private:
  
  // Don't allow derived classes to implement these VR Juggler functions.
  // Implement the _function instead.  
  // This is to ensure that the functions are wrapped in a try/catch.

  virtual void            init();
  virtual void            preFrame();
  virtual void            latePreFrame();
  virtual void            postFrame();

  // Data Members.
  mutable Mutex                          _mutex;
  ButtonsPtr                             _buttons;
  Analogs                                _analogs;
  TrackerPtr                             _tracker;

  /// No copying.
  BaseApplication ( const BaseApplication& );
  BaseApplication& operator = ( const BaseApplication&) ;
};


}
}

#endif // __VRV_CORE_BASE_APPLICATION_H__
