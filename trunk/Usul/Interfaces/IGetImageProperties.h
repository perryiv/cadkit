
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for getting image properties.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_GET_IMAGE_PROPERTIES_H_
#define _USUL_INTERFACE_GET_IMAGE_PROPERTIES_H_

#include "Usul/Interfaces/IUnknown.h"


namespace Usul {
namespace Interfaces {


struct IGetImageProperties : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IGetImageProperties );

  /// Id for this interface.
  enum { IID = 2372920170u };

  // Get the number of bytes per value.
  virtual unsigned int          getNumBytesPerValue() const = 0;

  // See if the pixel format is integer.
  virtual bool                  isValueInteger() const = 0;
};


} // namespace Interfaces
} // namespace Usul


#endif // _USUL_INTERFACE_GET_IMAGE_PROPERTIES_H_
