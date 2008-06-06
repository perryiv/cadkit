
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_IRENDERING_PASSES_H__
#define __USUL_INTERFACES_IRENDERING_PASSES_H__

#include "Usul/Interfaces/IUnknown.h"

namespace Usul {
namespace Interfaces {


struct IRenderingPasses : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IRenderingPasses );

  /// Id for this interface.
  enum { IID = 2393602489u };

  /// Get/Set the number of rendering passes.
  virtual void                       renderingPasses ( unsigned int number ) = 0;
  virtual unsigned int               renderingPasses () const = 0;

}; // struct IRenderingPasses


} // End namespace Interfaces
} // End namespace Usul


#endif // __USUL_INTERFACES_IRENDERING_PASSES_H__
