
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for adding a shared vertex.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_ADD_SHARED_VERTEX_H__
#define __USUL_INTERFACES_ADD_SHARED_VERTEX_H__

#include "Usul/Interfaces/IUnknown.h"

namespace OsgTools { namespace Triangles { class SharedVertex; } }
namespace osg { class Vec3f; }

namespace Usul {
namespace Interfaces {


struct IAddSharedVertex : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IAddSharedVertex );

  /// Id for this interface.
  enum { IID = 3984837821u };

  virtual OsgTools::Triangles::SharedVertex*       addSharedVertex ( const osg::Vec3f &v, bool look = true ) = 0;

}; //struct IAddSharedVertex



}
}

#endif // __USUL_INTERFACES_ADD_SHARED_VERTEX_H__
