
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for getting trackball information
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_TRACKBALL_H_
#define _USUL_INTERFACE_TRACKBALL_H_

#include "Usul/Interfaces/IUnknown.h"

namespace osg { class Quat; class Vec3f; typedef Vec3f Vec3; }

namespace Usul {
namespace Interfaces {

struct ITrackball : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( ITrackball );

  /// Id for this interface.
  enum { IID = 1102018737u };

  virtual osg::Vec3  getCenter() = 0;
  virtual void       setCenter( const osg::Vec3& ) = 0;
  virtual float      getDistance() = 0;
  virtual void       setDistance( float ) = 0;
  virtual osg::Quat  getRotation() = 0;
  virtual void       setRotation ( const osg::Quat& ) = 0;

  virtual void setTrackball ( const osg::Vec3&, float distance, const osg::Quat&, bool makeTrackball, bool setViewerToo ) = 0;

}; // class ITrackball


} // namespace Interfaces
} // namespace Usul


#endif // _USUL_INTERFACE_TRACKBALL_H_
