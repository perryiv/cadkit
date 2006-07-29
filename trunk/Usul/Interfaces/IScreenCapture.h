#ifndef __USUL_INTERFACES_ISCREENCAPTURE_H__
#define __USUL_INTERFACES_ISCREENCAPTURE_H__

#include "Usul/Interfaces/IUnknown.h"

namespace osg { class Image; class Vec3f; class Quat; }

namespace Usul {
namespace Interfaces {


struct IScreenCapture : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IScreenCapture );

  /// Id for this interface.
  enum { IID = 1275760u };

  virtual osg::Image* screenCapture ( const osg::Vec3f& center, float distance, const osg::Quat& rotation, unsigned int height, unsigned int width ) const = 0;
  virtual osg::Image* screenCapture ( unsigned int height, unsigned int width ) const = 0;

}; // struct IScreenCapture


} // end namespace Interfaces
} // end namespace Usul


#endif /* __USUL_INTERFACES_ISCREENCAPTURE_H__ */
