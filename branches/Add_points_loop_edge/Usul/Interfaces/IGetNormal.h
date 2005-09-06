
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_GET_NORMAL_H__
#define __USUL_INTERFACES_GET_NORMAL_H__

#include "Usul/Interfaces/IUnknown.h"

namespace osg { class Vec3f; }

namespace Usul {
namespace Interfaces {


struct IGetNormal : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IGetNormal );

  /// Id for this interface.
  enum { IID = 3661522526u };

  // Get normal for the i'th vertex.
  virtual const osg::Vec3f& getNormal( unsigned int index ) const = 0;


}; //struct __USUL_INTERFACES_GET_NORMAL_H__



}
}

#endif // __USUL_INTERFACES_GET_VERTEX_H__
