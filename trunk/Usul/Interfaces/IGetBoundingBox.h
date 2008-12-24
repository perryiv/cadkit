
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
///
///  Interface for getting the bounding box.
///
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_GET_BOUNDING_BOX_H_
#define _USUL_INTERFACE_GET_BOUNDING_BOX_H_

#include "Usul/Interfaces/IUnknown.h"

#include "osg/BoundingBox"

namespace Usul {
namespace Interfaces {


struct IGetBoundingBox : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IGetBoundingBox );

  /// Id for this interface.
  enum { IID = 3115299299u };

  /// Get the bounding-box.
  virtual osg::BoundingBox getBoundingBox() const = 0;

}; // class IGetBoundingBox


} // namespace Interfaces
} // namespace Usul


#endif // _USUL_INTERFACE_GET_BOUNDING_BOX_H_
