
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for getting the fox image
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_FOX_IMAGE_H_
#define _USUL_INTERFACE_FOX_IMAGE_H_

#include "Usul/Interfaces/IUnknown.h"

namespace FX { class FXImage; }

namespace Usul {
namespace Interfaces {


struct IFoxImage : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IFoxImage );

  /// Id for this interface.
  enum { IID = 1801892303u };

  virtual FX::FXImage* getFoxImage() = 0;

}; // struct IFoxImage


}; // namespace Interfaces
}; // namespace Usul


#endif // _USUL_INTERFACE_FOX_IMAGE_H_
