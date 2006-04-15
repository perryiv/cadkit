
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for getting the image view
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_IMAGE_VIEW_H_
#define _USUL_INTERFACE_IMAGE_VIEW_H_

#include "Usul/Interfaces/IUnknown.h"

namespace Helios { namespace Views { class ImageView; }  }

namespace Usul {
namespace Interfaces {


struct IImageView : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IImageView );

  /// Id for this interface.
  enum { IID = 2325881488u };

  virtual Helios::Views::ImageView*       imageView() = 0;
  virtual const Helios::Views::ImageView* imageView() const = 0;

}; // struct IImageView


}; // namespace Interfaces
}; // namespace Usul


#endif // _USUL_INTERFACE_IMAGE_VIEW_H_
