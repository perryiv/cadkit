
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for updating the text window.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_UPDATE_TEXT_WINDOW_H_
#define _USUL_INTERFACE_UPDATE_TEXT_WINDOW_H_

#include "Usul/Interfaces/IUnknown.h"


namespace Usul {
namespace Interfaces {


struct IUpdateTextWindow : public Usul::Interfaces::IUnknown
{
  // Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IUpdateTextWindow );

  // Id for this interface.
  enum { IID = 1831640466u };

  // Update the text window.
  virtual void updateTextWindow ( bool force ) = 0;
};


} // namespace Interfaces
} // namespace Usul


#endif // _USUL_INTERFACE_UPDATE_TEXT_WINDOW_H_
