
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_IASSEMBLY_H__
#define __USUL_INTERFACES_IASSEMBLY_H__

#include "Usul/Interfaces/IUnknown.h"


namespace Usul {
namespace Interfaces {

  struct ISceneElement;

struct IAssembly : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IAssembly );

  /// Id for this interface.
  enum { IID = 2049500u };

  virtual ISceneElement*  at ( unsigned int i ) = 0;

  virtual unsigned int    size () const = 0;

}; // struct IAssembly


} // end namespace Interfaces
} // end namespace Usul


#endif /* __USUL_INTERFACES_IASSEMBLY_H__ */
