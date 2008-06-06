
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry Miller
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_GET_TRIANGLE_NORMAL_H__
#define __USUL_INTERFACES_GET_TRIANGLE_NORMAL_H__

#include "Usul/Interfaces/IUnknown.h"

namespace osg { class Vec3f; }

namespace Usul {
namespace Interfaces {


struct IGetTriangleNormal : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IGetTriangleNormal );

  /// Id for this interface.
  enum { IID = 1804974857u };

  /// Get normal for the i'th triangle.
  virtual const osg::Vec3f& getTriangleNormal ( unsigned int index ) const = 0;
};


}
}


#endif // __USUL_INTERFACES_GET_TRIANGLE_NORMAL_H__
