
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for flipping normals
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_FLIP_NORMALS_H_
#define _USUL_INTERFACE_FLIP_NORMALS_H_

#include "Usul/Interfaces/IUnknown.h"

namespace Usul {
namespace Interfaces {


struct IFlipNormals : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IFlipNormals );

  /// Id for this interface.
  enum { IID = 1873241803u };

  /// Flip the normals.
  virtual void                flipNormalVectors() = 0;
};


} // namespace Interfaces
} // namespace Usul


#endif // _USUL_INTERFACE_FLIP_NORMALS_H_
