
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_INTERFACES_ILOOK_AT_LAYER_H__
#define __MINERVA_INTERFACES_ILOOK_AT_LAYER_H__

#include "Usul/Interfaces/IUnknown.h"

namespace Minerva {
namespace Interfaces {

struct ILookAtLayer : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( ILookAtLayer );

  /// Id for this interface.
  enum { IID = 4069381298u };

  /// Add a layer.
  virtual void                  lookAtLayer ( Usul::Interfaces::IUnknown* layer ) = 0;

  virtual void                  lookAtPoint ( const Usul::Math::Vec2d& point ) = 0;
  
}; // struct ILookAtLayer

} // end namespace Interfaces
} // end namespace Minerva


#endif // __MINERVA_INTERFACES_ILOOK_AT_LAYER_H__ 
