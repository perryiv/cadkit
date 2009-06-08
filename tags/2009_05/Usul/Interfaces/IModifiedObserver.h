
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_IMODIFIEDOBSERVER_H__
#define __USUL_INTERFACES_IMODIFIEDOBSERVER_H__

#include "Usul/Interfaces/IUnknown.h"

namespace Usul {
namespace Interfaces {


struct IModifiedObserver : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IModifiedObserver );

  /// Id for this interface.
  enum { IID = 1864009502u };

  virtual void subjectModified ( Usul::Interfaces::IUnknown *caller = 0x0 ) = 0;

}; // struct IModifiedObserver


} // end namespace Interfaces
} // end namespace Usul


#endif /* __USUL_INTERFACES_IMODIFIEDOBSERVER_H__ */
