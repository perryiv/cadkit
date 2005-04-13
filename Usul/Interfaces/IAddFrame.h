
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_ADD_FRAME_H__
#define __USUL_INTERFACES_ADD_FRAME_H__

#include "Usul/Interfaces/IUnknown.h"

#include <string>

namespace osg { class Quat; class Vec3f; typedef Vec3f Vec3; };

namespace Usul {
namespace Interfaces {

struct IAddFrame : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IAddFrame );

  /// Id for this interface.
  enum { IID = 2362918641u };

  virtual void addFrame( const std::string& groupName, const osg::Vec3& center, float distance, const osg::Quat& rotation ) = 0;

}; 

}

}


#endif // __USUL_INTERFACES_ADD_FRAME_H__

