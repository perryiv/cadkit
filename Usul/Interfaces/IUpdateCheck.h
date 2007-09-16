
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_IUPDATE_CHECK_H__
#define __USUL_INTERFACES_IUPDATE_CHECK_H__

#include "Usul/Interfaces/IUnknown.h"

namespace Usul {
namespace Interfaces {


struct IUpdateCheck : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IUpdateCheck );

  /// Id for this interface.
  enum { IID = 1480067193u };

  virtual bool               updateCheck () const = 0;

}; // struct IUpdateCheck


} // End namespace Interfaces
} // End namespace Usul


#endif // __USUL_INTERFACES_IUPDATE_CHECK_H__
