
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_ILAYER_EXTENTS_H__
#define __USUL_INTERFACES_ILAYER_EXTENTS_H__

#include "Usul/Interfaces/IUnknown.h"

namespace Usul {
namespace Interfaces {


struct ILayerExtents : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( ILayerExtents );

  /// Id for this interface.
  enum { IID = 3529785747u };

  /// Get the extents.
  virtual void               layerExtents ( double &lat, double &lon, double& height ) const = 0;

}; // struct ILayerExtents


} // end namespace Interfaces
} // end namespace Usul


#endif /* __USUL_INTERFACES_ILAYER_EXTENTS_H__ */

