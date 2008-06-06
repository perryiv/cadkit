
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Jeff Conner
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_IVERTICES_H__
#define __USUL_INTERFACES_IVERTICES_H__

#include "Usul/Interfaces/IUnknown.h"

//namespace osg { class Vec3Array; }

namespace Usul {
namespace Interfaces {


struct IVertices : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IVertices );

  /// Id for this interface.
  enum { IID = 1885241013u };

  // Get vertex pool. Use with caution.
  virtual osg::Vec3Array *      vertices() = 0;
  


}; //struct IVertices



}
}

#endif // __USUL_INTERFACES_IVERTICES_H__
