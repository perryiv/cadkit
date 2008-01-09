
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Jeff Conner
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_IMEMORYPOOL_H__
#define __USUL_INTERFACES_IMEMORYPOOL_H__

#include "Usul/Interfaces/IUnknown.h"

//namespace osg { class Vec4; }

namespace Usul {
namespace Interfaces {


struct IMemoryPool : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IMemoryPool );

  /// Id for this interface.
  enum { IID = 3516280464u };

  // Use shared memory pool
  virtual void                usePool( bool use ) = 0;

}; //struct IVertices



}
}

#endif // __USUL_INTERFACES_ICOLORSPERVERTEX_H__
