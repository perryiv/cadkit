
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for smoothing the model the implementor contains.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_SMOOTH_MODEL_H_
#define _USUL_INTERFACE_SMOOTH_MODEL_H_

#include "Usul/Interfaces/IUnknown.h"

namespace Usul {
namespace Interfaces {


struct ISmoothModel : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( ISmoothModel );

  /// Id for this interface.
  enum { IID = 1204646586u };

  virtual void smoothModel ( ) = 0;

}; // class ISmoothModel


} // namespace Interfaces
} // namespace Usul


#endif // _USUL_INTERFACE_SMOOTH_MODEL_H_
