
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_IRAW_POINTER_H__
#define __USUL_INTERFACES_IRAW_POINTER_H__

#include "Usul/Interfaces/IUnknown.h"


namespace Usul {
namespace Interfaces {

template < class T, unsigned long iid  >
struct IRawPointer : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IRawPointer );

  /// Id for this interface.
  enum { IID = iid };

  typedef T PointerType;

  virtual PointerType*                   getRawPointer() = 0;
  virtual const PointerType*             getRawPointer() const = 0;

}; // struct IRawPointer


} // end namespace Interfaces
} // end namespace Usul


#endif /* __USUL_INTERFACES_IRAW_POINTER_H__ */

