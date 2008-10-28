
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

#include "VRV/Core/SharedData.h"

#include "VRV/Devices/JoystickDevice.h"
#include "VRV/Devices/ButtonGroup.h"
#include "VRV/Devices/TrackerDevice.h"

#include "Usul/Functors/Interaction/Common/BaseFunctor.h"
#include "Usul/Interfaces/INavigationFunctor.h"
#include "Usul/Math/Matrix44.h"
#include "Usul/Threads/RecursiveMutex.h"
#include "Usul/Threads/Guard.h"

#include "vrj/Draw/OGL/GlApp.h"
#include "plugins/ApplicationDataManager/UserData.h"

#include <string>

namespace VRV {
namespace Core {


class VRV_EXPORT BaseApplication : public vrj::GlApp,
                                   public Usul::Interfaces::INavigationFunctor
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
  typedef Usul::Functors::Interaction::Common::BaseFunctor   Navigator;

  USUL_DECLARE_IUNKNOWN_MEMBERS;

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

  /// Get/Set the navigator.
  void                          navigator ( Navigator * );
  Navigator *                   navigator();
  const Navigator *             navigator() const;

  /// Quit the application.
  void                          quit();

  /// Run the application.
  void                          run();

  /// Post-multiply the navigation matrix by the given matrix.
  void                          navigationPostMultiply ( const Matrix &M );

  /// Pre-multiply the navigation matrix by the given matrix.
  void                          navigationPreMultiply ( const Matrix &M );

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

  // Initialize shared data.
  virtual void                  _initializeSharedData ( const std::string& hostname );

  // Load VR Juggler config files.
  void                          _loadConfigFiles ( const std::vector < std::string > &configs );
  void                          _loadSimConfigs  ( const std::string& dir );
  void                          _loadSimConfigs();

  // The navigator has changed.
  virtual void                  _navigatorChanged ( Navigator::RefPtr newNavigator, Navigator::RefPtr oldNavigator );

  // Set/Get the navigation matrix.
  void                          _navigationMatrixSet ( const Matrix& m );
  Matrix                        _navigationMatrixGet();

  // Remove the listener.
  void                          _removeButtonPressListener ( Usul::Interfaces::IUnknown * );
  void                          _removeButtonReleaseListener ( Usul::Interfaces::IUnknown * );

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

  // Typedef.
  typedef VRV::Core::SharedData<Matrix>              SharedMatrix;

  // Data Members.
  mutable Mutex                          _mutex;
  unsigned int                           _refCount;
  ButtonsPtr                             _buttons;
  Analogs                                _analogs;
  TrackerPtr                             _tracker;
  cluster::UserData<SharedMatrix>        _navigationMatrix;
  Navigator::RefPtr                      _navigator;

  /// No copying.
  BaseApplication ( const BaseApplication& );
  BaseApplication& operator = ( const BaseApplication&) ;
};


}
}

#endif // __VRV_CORE_BASE_APPLICATION_H__
