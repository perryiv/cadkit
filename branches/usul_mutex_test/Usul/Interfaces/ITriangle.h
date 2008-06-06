
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_TRIANGLE_H__
#define __USUL_INTERFACES_TRIANGLE_H__

#include "Usul/Interfaces/IUnknown.h"

namespace osg { class Vec3f; class Drawable; }

namespace Usul {
namespace Interfaces {


struct ITriangle : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( ITriangle );

  /// Id for this interface.
  enum { IID = 1203224138u };

  // Get the vertices of the i'th triangle.
  virtual const osg::Vec3f &          vertex0 ( const osg::Geode *g, const osg::Drawable *, unsigned int i ) const = 0;
  virtual const osg::Vec3f &          vertex1 ( const osg::Geode *g, const osg::Drawable *, unsigned int i ) const = 0;
  virtual const osg::Vec3f &          vertex2 ( const osg::Geode *g, const osg::Drawable *, unsigned int i ) const = 0;


}; //struct ITriangle



}
}

#endif // __USUL_INTERFACES_GET_VERTEX_H__
