
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for toggling axes
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_AXES_H_
#define _USUL_INTERFACE_AXES_H_

#include "Usul/Interfaces/IUnknown.h"

namespace Usul {
namespace Interfaces {


struct IAxes : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IAxes );

  /// Id for this interface.
  enum { IID = 2335136600u };

  virtual void               setAxes ( bool ) = 0;
  virtual bool               hasAxes ( ) const = 0;

}; // class IAxes


} // namespace Interfaces
} // namespace Usul


#endif // _USUL_INTERFACE_SPIN_H_
