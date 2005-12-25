
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////


#ifndef __USUL_INTERFACES_ANIMATE_H__
#define __USUL_INTERFACES_ANIMATE_H__

#include "Usul/Interfaces/IUnknown.h"

namespace osg { class Quat; class Vec3f; typedef Vec3f Vec3; };

namespace Usul {
namespace Interfaces {

struct IAnimate : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IAnimate );

  /// Id for this interface.
  enum { IID = 3673709138u };

  //Animate from current position to given position
  virtual void animate ( const osg::Vec3& center, float distance, const osg::Quat& rotation ) = 0;
  
  //Animate from one position and orientation to another.
  virtual void animate ( const osg::Vec3 &t1, const osg::Quat &r1, const osg::Vec3 &t2, const osg::Quat &r2, double duration ) = 0;

  //Animate from one orientation to another.
  virtual void animate ( const osg::Quat &r1, const osg::Quat &r2, double duration ) = 0;

}; 

}

}


#endif // __USUL_INTERFACES_ANIMATE_H__

