
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __DEVICES_IDEVICE_H__
#define __DEVICES_IDEVICE_H__

#include "Usul/Interfaces/IUnknown.h"

namespace osg { class Node; }

namespace Usul {
namespace Devices {

struct IButtonListener;
struct IMotionListener;

struct IDevice : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IDevice );

  /// Id for this interface.
  enum { IID = 1962936564u };

  virtual void navigating( bool ) = 0;
  virtual bool navigating() = 0;

  virtual void setNode ( osg::Node* ) = 0;

  virtual void addButtonListener( unsigned int button, IButtonListener* ) = 0;
  virtual void addMotionListener( IMotionListener* ) = 0;

  virtual void clear() = 0;

#if 0
  
  virtual void addJoystickListener( unsigned int number, IJoystickListener* ) = 0;
  virtual void addDigitalPadListener( IDigitalPadListener* ) = 0;

  virtual void removeButtonListener ( unsigned int button, IButtonListener* ) = 0;
  virtual void removeMotionListener ( IMotionListener* ) = 0;
  virtual void removeJoystickListener( unsigned int number, IJoystickListener* ) = 0;
  virtual void removeDigitalPadListener( IDigitalPadListener* ) = 0;


  enum Type
  {
    MOUSE,
    JOYSTICK,
    TRACKED_JOYSTICK
  };

  virtual unsigned int type() = 0;
  unsigned int numButtons() = 0;
#endif
};


} // namespace Usul
} // namespace Devices


#endif // __DEVICES_IDEVICE_H__
