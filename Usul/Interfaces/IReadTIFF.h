
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for reading a tiff.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_READ_TIFF_IMAGE_H_
#define _USUL_INTERFACE_READ_TIFF_IMAGE_H_

#include "Usul/Interfaces/IUnknown.h"


namespace Usul {
namespace Interfaces {


struct IReadTIFF : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IReadTIFF );

  /// Id for this interface.
  enum { IID = 3572826976u };
};


} // namespace Interfaces
} // namespace Usul


#endif // _USUL_INTERFACE_READ_TIFF_IMAGE_H_
