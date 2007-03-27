
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Aashish Chaudhary
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_PLAY_MOVIE_H__
#define __USUL_INTERFACES_PLAY_MOVIE_H__

#include "Usul/Interfaces/IUnknown.h"

namespace osg { class Vec3f; class Node; }

#include <vector>

namespace Usul {
namespace Interfaces {

struct IPlayMovie : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IPlayMovie );

  /// Id for this interface.
  enum { IID = 2131089585 };
  
  virtual osg::Node* playMovie( const osg::Vec3f& lowerLeft, const osg::Vec3f& upperRight, const std::string& fileName ) = 0;

};

}
}

#endif // __USUL_INTERFACES_PLAY_MOVIE_H__
