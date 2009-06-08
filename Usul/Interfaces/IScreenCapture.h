
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_ISCREENCAPTURE_H__
#define __USUL_INTERFACES_ISCREENCAPTURE_H__

#include "Usul/Interfaces/IUnknown.h"

namespace osg { class Image; class Vec3d; class Quat; }

namespace Usul {
namespace Interfaces {


struct IScreenCapture : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IScreenCapture );

  /// Id for this interface.
  enum { IID = 1275760u };

  virtual osg::Image* screenCapture ( const osg::Vec3d& center, double distance, const osg::Quat& rotation, unsigned int height, unsigned int width ) const = 0;
  virtual osg::Image* screenCapture ( unsigned int height, unsigned int width ) const = 0;

}; // struct IScreenCapture


} // end namespace Interfaces
} // end namespace Usul


#endif /* __USUL_INTERFACES_ISCREENCAPTURE_H__ */
