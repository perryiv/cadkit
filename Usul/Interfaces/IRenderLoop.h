
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_IRENDER_LOOP_H__
#define __USUL_INTERFACES_IRENDER_LOOP_H__

#include "Usul/Interfaces/IUnknown.h"

namespace Usul {
namespace Interfaces {


struct IRenderLoop : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IRenderLoop );

  /// Id for this interface.
  enum { IID = 2693603736u };

  /// Get/Set render loop flag.
  virtual void                     renderLoop ( bool b ) = 0;
  virtual bool                     renderLoop () const = 0;

}; // struct IRenderLoop


} // End namespace Interfaces
} // End namespace Usul


#endif // __USUL_INTERFACES_IRENDER_LOOP_H__
