
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_INTERFACES_IPOINT_LAYER_H__
#define __MINERVA_INTERFACES_IPOINT_LAYER_H__

#include "Usul/Interfaces/IUnknown.h"

namespace Minerva {
namespace Interfaces {


struct IPointLayer : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IPointLayer );

  /// Id for this interface.
  enum { IID = 3055369416u };

  enum PrimitiveTypes
  {
    POINT = 1,
    SPHERE,
    CONE,
    DISK,
    CUBE,
    INVERTED_CONE,
    CYLINDER
  };

  /// Get/Set primitive id.
  virtual void                    primitiveID( Usul::Types::Uint32 ) = 0;
  virtual Usul::Types::Uint32     primitiveID() const = 0;

}; // struct IPointLayer


} // end namespace Interfaces
} // end namespace Minerva


#endif /* __MINERVA_INTERFACES_IPOINT_LAYER_H__ */

