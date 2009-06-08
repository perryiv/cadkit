
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for reading images.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_READ_IMAGE_FILES_H_
#define _USUL_INTERFACE_READ_IMAGE_FILES_H_

#include "Usul/Interfaces/IUnknown.h"


namespace Usul {
namespace Interfaces {


struct IReadImage : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IReadImage );

  /// Id for this interface.
  enum { IID = 3688317255u };
};


} // namespace Interfaces
} // namespace Usul


#endif // _USUL_INTERFACE_READ_IMAGE_FILES_H_
