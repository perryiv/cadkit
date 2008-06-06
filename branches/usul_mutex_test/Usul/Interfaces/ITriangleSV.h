
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_TRIANGLE_SV_H__
#define __USUL_INTERFACES_TRIANGLE_SV_H__

#include "Usul/Interfaces/IUnknown.h"

namespace OsgTools { namespace Triangles { class SharedVertex; } }
namespace osg { class Drawable; }

namespace Usul {
namespace Interfaces {


struct ITriangleSV : public Usul::Interfaces::IUnknown
{
  // Typedefs.
  typedef OsgTools::Triangles::SharedVertex SharedVertex;

  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( ITriangleSV );

  /// Id for this interface.
  enum { IID = 4260819650u };

  // Get the shared vertices of the i'th triangle.
  virtual const SharedVertex*         sharedVertex0 ( const osg::Geode *g, const osg::Drawable*, unsigned int i ) const = 0;
  virtual const SharedVertex*         sharedVertex1 ( const osg::Geode *g, const osg::Drawable*, unsigned int i ) const = 0;
  virtual const SharedVertex*         sharedVertex2 ( const osg::Geode *g, const osg::Drawable*, unsigned int i ) const = 0;
  virtual SharedVertex*               sharedVertex0 ( const osg::Geode *g, const osg::Drawable*, unsigned int i ) = 0;
  virtual SharedVertex*               sharedVertex1 ( const osg::Geode *g, const osg::Drawable*, unsigned int i ) = 0;
  virtual SharedVertex*               sharedVertex2 ( const osg::Geode *g, const osg::Drawable*, unsigned int i ) = 0;
};


}
}

#endif // __USUL_INTERFACES_TRIANGLE_SV_H__
