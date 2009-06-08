
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_CLIPPING_PLANES_H__
#define __USUL_INTERFACES_CLIPPING_PLANES_H__

#include "Usul/Interfaces/IUnknown.h"

#include "osg/BoundingBox"

namespace osg { class Plane; }

namespace Usul {
namespace Interfaces {

struct IClippingPlanes : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IClippingPlanes );

  /// Id for this interface.
  enum { IID = 2541716112u };

  // Add/Remove clipping plane.
  virtual unsigned int          addClippingPlane ( ) = 0;
  virtual unsigned int          addClippingPlane ( const osg::Plane& plane ) = 0;
  virtual void                  removeClippingPlane ( unsigned int ) = 0;
  virtual void                  removeClippingPlanes (  ) = 0;

  // Get/Set the clipping plane.
  virtual osg::Plane            getClippingPlane ( unsigned int ) = 0;
  virtual void                  setClippingPlane ( unsigned int, const osg::Plane& ) = 0;

  // Get the number of clipping planes in the scene.
  virtual unsigned int          numClippingPlanes () = 0;

}; // struct IClippingPlanes

}
}

#endif // __USUL_INTERFACES_CLIPPING_PLANES_H__

