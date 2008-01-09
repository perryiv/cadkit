
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for button id.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACES_BUTTON_ID_H_
#define _USUL_INTERFACES_BUTTON_ID_H_

#include "Usul/Interfaces/IUnknown.h"


namespace Usul {
namespace Interfaces {


struct IButtonID : public Usul::Interfaces::IUnknown
{
  // Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IButtonID );

  // Id for this interface.
  enum { IID = 2135706984u };

  // Get the button id.
  virtual unsigned long           buttonID ( ) const = 0;
};


} // namespace Interfaces
} // namespace Usul


#endif // _USUL_INTERFACES_BUTTON_ID_H_
