
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for swapping frame buffers.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_SWAP_BUFFERS_H_
#define _USUL_INTERFACE_SWAP_BUFFERS_H_

#include "Usul/Interfaces/IUnknown.h"

namespace Usul {
namespace Interfaces {


struct ISwapBuffers : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( ISwapBuffers );

  /// Id for this interface.
  enum { IID = 1840009557u };

  /// Swap the frame buffers.
  virtual void                    swapBuffers ( IUnknown *caller ) = 0;
};


}; // namespace Interfaces
}; // namespace Usul


#endif // _USUL_INTERFACE_SWAP_BUFFERS_H_
