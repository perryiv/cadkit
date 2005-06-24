
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for toggling the bounding sphere
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_BOUNDING_SPHERE_H_
#define _USUL_INTERFACE_BOUNDING_SPHERE_H_

#include "Usul/Interfaces/IUnknown.h"

namespace Usul {
namespace Interfaces {


struct IBoundingSphere : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IBoundingSphere );

  /// Id for this interface.
  enum { IID = 1770828539u };

  // Set/get the bounding-box state.
  virtual void                  boundingSphere ( bool show ) = 0;
  virtual bool                  boundingSphere() const = 0;

}; // class IBoundingSphere


} // namespace Interfaces
} // namespace Usul


#endif // _USUL_INTERFACE_BOUNDING_SPHERE_H_
