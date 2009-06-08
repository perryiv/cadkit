
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for intersecting a scene
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_SCENE_INTERSECT_H_
#define _USUL_INTERFACE_SCENE_INTERSECT_H_

#include "Usul/Interfaces/IUnknown.h"

#include "osgUtil/LineSegmentIntersector"
#include "osgUtil/PolytopeIntersector"


namespace Usul {
namespace Interfaces {


struct ISceneIntersect : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( ISceneIntersect );

  /// Id for this interface.
  enum { IID = 1101086294u };

  virtual bool intersect ( float x, float y, osgUtil::LineSegmentIntersector::Intersection &  ) = 0;
  virtual bool intersect ( float x, float y, osgUtil::LineSegmentIntersector::Intersections & ) = 0;

  virtual bool intersect ( double x, double y, osgUtil::PolytopeIntersector::Intersection &,  double width = 0.05, double height = 0.05, unsigned int dimensionMask = osgUtil::PolytopeIntersector::AllDims ) = 0;
  virtual bool intersect ( double x, double y, osgUtil::PolytopeIntersector::Intersections &, double width = 0.05, double height = 0.05, unsigned int dimensionMask = osgUtil::PolytopeIntersector::AllDims ) = 0;
  
}; // class ISceneIntersect


} // namespace Interfaces
} // namespace Usul


#endif // _USUL_INTERFACE_SCENE_INTERSECT_H_
