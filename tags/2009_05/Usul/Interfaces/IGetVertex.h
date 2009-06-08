
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_GET_VERTEX_H__
#define __USUL_INTERFACES_GET_VERTEX_H__

#include "Usul/Interfaces/IUnknown.h"

namespace osg { class Vec3f; }

namespace Usul {
namespace Interfaces {


struct IGetVertex : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IGetVertex );

  /// Id for this interface.
  enum { IID = 1862774388u };

  virtual const osg::Vec3f& getVertex( unsigned int index ) const = 0;


}; //struct IGetVertex



}
}

#endif // __USUL_INTERFACES_GET_VERTEX_H__
