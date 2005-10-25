
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

  // Run the application.
  virtual void                  runApplication() = 0;

  // Update the application.
  virtual void                  updateApplication() = 0;
};


} // namespace Interfaces
} // namespace Usul


#endif // _USUL_INTERFACE_GUI_SERVER_H_
