
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////


#ifndef __DEVICES_BASE_DEVICE_H__
#define __DEVICES_BASE_DEVICE_H__

#include "Usul/Export/Export.h"
#include "IDevice.h"
#include "IButtonListener.h"
#include "IMotionListener.h"

#include "Usul/Base/Referenced.h"

#include <map>
#include <vector>

namespace Usul {
namespace Devices {

class USUL_EXPORT BaseDevice : public Usul::Base::Referenced, 
                               public IDevice
{
public:

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( BaseDevice );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  virtual void navigating( bool b ) { _navigating = b; }
  virtual bool navigating()         { return _navigating; }

  virtual void addButtonListener( unsigned int button, IButtonListener* );
  virtual void addMotionListener( IMotionListener* );

  virtual void clear();

protected:

  BaseDevice ( );
  virtual ~BaseDevice();

  void _notifyButtonPressed  ( unsigned int button, const State& state );
  void _notifyButtonReleased ( unsigned int button, const State& state );

  void _notifyMotionListeners ( const State& state );
  
private:
  typedef std::vector < IButtonListener::ValidRefPtr > ButtonListeners;
  typedef std::map< unsigned int, ButtonListeners >    ButtonListenersMap;
  typedef std::vector < IMotionListener::ValidRefPtr > MotionListeners;

  bool _navigating;
  ButtonListenersMap _buttonListeners;
  MotionListeners    _motionListeners;
};

}
}

#endif // __DEVICES_BASE_DEVICE_H__

