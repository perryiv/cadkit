
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_IOFFSET_H__
#define __USUL_INTERFACES_IOFFSET_H__

#include "Usul/Interfaces/IUnknown.h"

namespace osg { class Vec3f; }

namespace Usul {
namespace Interfaces {


struct IOffset : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IOffset );

  /// Id for this interface.
  enum { IID = 2942228631u };

  virtual const osg::Vec3f&   spatialOffset () const  = 0;
  virtual void                spatialOffset ( const osg::Vec3f& ) = 0;

}; // struct IOffset


} // end namespace Interfaces
} // end namespace Usul


#endif /* __USUL_INTERFACES_IOFFSET_H__ */

