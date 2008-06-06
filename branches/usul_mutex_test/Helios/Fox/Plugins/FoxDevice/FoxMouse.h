
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __DEVICES_FOX_MOUSE_H__
#define __DEVICES_FOX_MOUSE_H__

#include "Usul/Devices/BaseDevice.h"
#include "Usul/Devices/IKeyboardDevice.h"
#include "Usul/Devices/IKeyListener.h"

#include "FoxTools/Headers/Object.h"

#include "osg/ref_ptr"

namespace FX { class FXWindow; struct FXEvent; }


class FoxMouse : public FX::FXObject,
                                public Usul::Devices::BaseDevice,
                                public Usul::Devices::IKeyboardDevice
{
public:

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( FoxMouse );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  FoxMouse ( FX::FXWindow *view, Usul::Interfaces::IUnknown *caller );
  virtual ~FoxMouse();

  long                  onLeftBtnPress       ( FX::FXObject *object, FX::FXSelector id, void *data );
  long                  onLeftBtnRelease     ( FX::FXObject *object, FX::FXSelector id, void *data );
  long                  onMiddleBtnPress     ( FX::FXObject *object, FX::FXSelector id, void *data );
  long                  onMiddleBtnRelease   ( FX::FXObject *object, FX::FXSelector id, void *data );
  long                  onRightBtnPress      ( FX::FXObject *object, FX::FXSelector id, void *data );
  long                  onRightBtnRelease    ( FX::FXObject *object, FX::FXSelector id, void *data );
  long                  onMotion             ( FX::FXObject *object, FX::FXSelector id, void *data );
  long                  onCommandMouseWheel  ( FX::FXObject *object, FX::FXSelector id, void *data );

  virtual void          addKeyListener    ( Usul::Devices::IKeyListener* );
  virtual void          removeKeyListener ( Usul::Devices::IKeyListener* );

  long                  onKeyPress        ( FX::FXObject *object, FX::FXSelector id, void *data );
  long                  onKeyRelease      ( FX::FXObject *object, FX::FXSelector id, void *data );

protected:

  void                  _getXY ( const FX::FXEvent& event, float &x, float &y );
  void                  _setState ( Usul::Devices::State& state, const FX::FXEvent& event );

private:
  // For FOX
  FoxMouse() { }

  typedef Usul::Devices::IKeyListener::ValidRefPtr KeyListenerPtr;
  typedef std::vector < KeyListenerPtr > KeyListeners;

  FX::FXWindow *_view;

  KeyListeners   _keyListeners;

  FXDECLARE( FoxMouse )
};



#endif // __DEVICES_FOX_MOUSE_H__
