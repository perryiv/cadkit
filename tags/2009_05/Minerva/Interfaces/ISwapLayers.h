
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_INTERFACES_ISWAP_LAYERS_H__
#define __MINERVA_INTERFACES_ISWAP_LAYERS_H__

#include "Usul/Interfaces/IUnknown.h"

namespace Minerva {
namespace Interfaces {
    
struct ISwapLayers : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( ISwapLayers );
      
  /// Id for this interface.
  enum { IID = 2415493090u };
      
  /// Swap two layers.
  virtual void swapLayers ( Usul::Interfaces::IUnknown* layer0, Usul::Interfaces::IUnknown* layer1 ) = 0;
      
}; // struct ISwapLayers
    
} // end namespace Interfaces
} // end namespace Minerva


#endif // __MINERVA_INTERFACES_IADD_LAYER_H__ 
