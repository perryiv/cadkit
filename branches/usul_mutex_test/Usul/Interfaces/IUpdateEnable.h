
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_IUPDATE_ENABLE_H__
#define __USUL_INTERFACES_IUPDATE_ENABLE_H__

#include "Usul/Interfaces/IUnknown.h"

namespace Usul {
namespace Interfaces {


struct IUpdateEnable : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IUpdateEnable );

  /// Id for this interface.
  enum { IID = 1207541778u };

  virtual bool               updateEnable () const = 0;

}; // struct IUpdateEnable


} // End namespace Interfaces
} // End namespace Usul


#endif // __USUL_INTERFACES_IUPDATE_ENABLE_H__
