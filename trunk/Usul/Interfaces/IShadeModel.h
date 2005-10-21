
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for getting and setting the shading model.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_SHADING_MODEL_H_
#define _USUL_INTERFACE_SHADING_MODEL_H_

#include "Usul/Interfaces/IUnknown.h"

namespace Usul {
namespace Interfaces {


struct IShadeModel : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IShadeModel );

  /// Id for this interface.
  enum { IID = 3260613509u };

  /// Possible modes.
  enum Mode { NONE, FLAT, SMOOTH };

  /// Set/get the shade model.
  virtual void                    shadeModel ( Mode mode ) = 0;
  virtual Mode                    shadeModel() const = 0;

}; // class IShadeModel


} // namespace Interfaces
} // namespace Usul


#endif // _USUL_INTERFACE_SHADING_MODEL_H_
