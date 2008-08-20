
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_INTERFACES_IADD_LAYER_H__
#define __MINERVA_INTERFACES_IADD_LAYER_H__

#include "Usul/Interfaces/IUnknown.h"

namespace Minerva {
namespace Interfaces {

struct IAddLayer : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IAddLayer );

  /// Id for this interface.
  enum { IID = 3428752783u };

  /// Add a layer.
  virtual void                  addLayer ( Usul::Interfaces::IUnknown* layer ) = 0;

}; // struct IAddLayer

} // end namespace Interfaces
} // end namespace Minerva


#endif // __MINERVA_INTERFACES_IADD_LAYER_H__ 
