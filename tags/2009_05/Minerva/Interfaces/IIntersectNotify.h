
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _MINERVA_INTERFACES_INTERSECT_NOTIFY_LAYER_H_
#define _MINERVA_INTERFACES_INTERSECT_NOTIFY_LAYER_H_

#include "Usul/Interfaces/IUnknown.h"

#include <vector>


namespace Minerva {
namespace Interfaces {


struct IIntersectNotify : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IIntersectNotify );

  /// Id for this interface.
  enum { IID = 2851728485u };

  /// Typedefs for this interface.
  typedef std::vector<Usul::Interfaces::IUnknown::RefPtr> Path;
  typedef std::vector<double> Point;
  typedef std::pair<Point,double> PointAndDistance;
  typedef std::pair<Path,PointAndDistance> Closest;

  /// Call to notify about the intersection.
  virtual void                  intersectNotify ( double x, double y, double z, 
                                                  double lon, double lat, double elev, 
                                                  IUnknown::RefPtr tile, 
                                                  IUnknown::RefPtr body, 
                                                  IUnknown::RefPtr caller,
                                                  Closest & ) = 0;
};


} // end namespace Interfaces
} // end namespace Minerva


#endif // _MINERVA_INTERFACES_INTERSECT_NOTIFY_LAYER_H_
