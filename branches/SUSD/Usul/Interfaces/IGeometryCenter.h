
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_IGEOMETRY_CENTER_H__
#define __USUL_INTERFACES_IGEOMETRY_CENTER_H__

#include "Usul/Interfaces/IUnknown.h"

namespace osg { class Vec3f; }

namespace Usul {
namespace Interfaces {


struct IGeometryCenter : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IGeometryCenter );

  /// Id for this interface.
  enum { IID = 1213513200u };

  virtual osg::Vec3f geometryCenter ( ) = 0;
  virtual osg::Vec3f geometryCenter ( const osg::Vec3f& offset ) = 0;

}; // struct IGeometryCenter


} // end namespace Interfaces
} // end namespace Usul


#endif /* __USUL_INTERFACES_IGEOMETRY_CENTER_H__ */

