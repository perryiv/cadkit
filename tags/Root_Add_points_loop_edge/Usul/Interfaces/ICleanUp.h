
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for cleaning up.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACE_CLEAN_UP_H__
#define __USUL_INTERFACE_CLEAN_UP_H__

#include "Usul/Interfaces/IUnknown.h"

namespace Usul {
namespace Interfaces  {
    
struct ICleanUp : public Usul::Interfaces::IUnknown
{
  // Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( ICleanUp );

  enum { IID = 1104796696u };

  virtual void cleanUp( Usul::Interfaces::IUnknown* caller ) = 0;

};// ICleanUp

} //namespace Interfaces
}//namespace Usul

#endif //__USUL_INTERFACE_CLEAN_UP_H__

