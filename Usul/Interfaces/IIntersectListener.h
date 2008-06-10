
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for intersect listeners.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACES_INTERSECT_LISTENER_H_
#define _USUL_INTERFACES_INTERSECT_LISTENER_H_

#include "Usul/Interfaces/IUnknown.h"

#include "osgUtil/LineSegmentIntersector"

namespace Usul {
namespace Interfaces {


struct IIntersectListener : public Usul::Interfaces::IUnknown
{
  // Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IIntersectListener );

  // Id for this interface.
  enum { IID = 1909417664u };

  // Notify the observer of the intersection.
  virtual void      intersectNotify ( float x, float y, const osgUtil::LineSegmentIntersector::Intersection &hit, Usul::Interfaces::IUnknown *caller ) = 0;
};


} // End namespace Interfaces
} // End namespace Usul


#endif // _USUL_INTERFACES_INTERSECT_LISTENER_H_
