
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for toggling the bounding box
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_BOUNDING_BOX_H_
#define _USUL_INTERFACE_BOUNDING_BOX_H_

#include "Usul/Interfaces/IUnknown.h"

namespace Usul {
namespace Interfaces {


struct IBoundingBox : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IBoundingBox );

  /// Id for this interface.
  enum { IID = 2633628806u };

  // Set/get the bounding-box state.
  virtual void                  boundingBoxVisible ( bool show ) = 0;
  virtual bool                  boundingBoxVisible() const = 0;

}; // class IBoundingBox


} // namespace Interfaces
} // namespace Usul


#endif // _USUL_INTERFACE_BOUNDING_BOX_H_
