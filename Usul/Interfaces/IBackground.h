
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for changing the background of a view
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_BACKGROUND_H_
#define _USUL_INTERFACE_BACKGROUND_H_

#include "Usul/Interfaces/IUnknown.h"

namespace Usul {
namespace Interfaces {


struct IBackground : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IBackground );

  /// Id for this interface.
  enum { IID = 1910338179u };

  // Edit the background color.
  virtual void            editBackground() = 0;

  // Set the background color to the default
  virtual void            defaultBackground() = 0;


}; // class IBackground


} // namespace Interfaces
} // namespace Usul


#endif // _USUL_INTERFACE_BACKGROUND_H_
