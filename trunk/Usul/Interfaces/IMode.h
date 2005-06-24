
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for getting the view
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_MODE_H_
#define _USUL_INTERFACE_MODE_H_

#include "Usul/Interfaces/IUnknown.h"

namespace Usul {
namespace Interfaces {


struct IMode : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IMode );

  /// Id for this interface.
  enum { IID = 3486164218u };

  virtual bool                navigating() const = 0;
  virtual void                navigating( bool ) = 0;

  virtual bool                picking() const = 0;
  virtual void                picking( bool ) = 0;

  virtual bool                tool() const = 0;

}; // class IMode


} // namespace Interfaces
} // namespace Usul


#endif // _USUL_INTERFACE_MODE_H_
