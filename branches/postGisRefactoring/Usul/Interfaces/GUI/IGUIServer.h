
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
///
///  Interface for a GUI server
///
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_GUI_SERVER_H_
#define _USUL_INTERFACE_GUI_SERVER_H_

#include "Usul/Interfaces/IUnknown.h"

#include <string>

namespace AFW { namespace Core { class Window; } }


namespace Usul {
namespace Interfaces {


struct IGUIServer : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IGUIServer );

  /// Id for this interface.
  enum { IID = 1696628448u };

  // Build the application.
  virtual void                  buildApplication() = 0;

  // Destroy the application.
  virtual void                  destroyApplication() = 0;

  // Notification that the object is being destroyed.
  virtual void                  destroyNotify ( AFW::Core::Window * ) = 0;

  // Enable/disable the window.
  virtual void                  enableWindow ( bool state, AFW::Core::Window *window ) = 0;

  // Return the enabled/disabled state.
  virtual bool                  isWindowEnabled ( const AFW::Core::Window *window ) = 0;

  // Notification that the object is being removed from the scene.
  virtual void                  removeNotify ( AFW::Core::Window * ) = 0;

  // Run the application.
  virtual void                  runApplication() = 0;
  virtual void                  runWhileEvents() = 0;

  // Quit the application.
  virtual void                  quitApplication() = 0;

  // Scroll the window.
  virtual void                  scrollWindowToEnd ( AFW::Core::Window *window ) = 0;

  // Update the window's text.
  virtual void                  windowTextAppend ( AFW::Core::Window *window, const std::string & ) = 0;
  virtual void                  windowTextAppend ( AFW::Core::Window *window, const char *, unsigned int length ) = 0;
  virtual void                  windowTextGet    ( const AFW::Core::Window *window, std::string & ) = 0;
  virtual void                  windowTextSet    ( AFW::Core::Window *window, const std::string & ) = 0;
  virtual void                  windowTextSet    ( AFW::Core::Window *window, const char *, unsigned int length ) = 0;
};


} // namespace Interfaces
} // namespace Usul


#endif // _USUL_INTERFACE_GUI_SERVER_H_
