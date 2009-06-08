
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface to get OpenSceneGraph's RenderInfo.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_RENDER_INFO_OSG_H_
#define _USUL_INTERFACE_RENDER_INFO_OSG_H_

#include "Usul/Interfaces/IUnknown.h"

namespace osg { class RenderInfo; }

namespace Usul {
namespace Interfaces {


struct IRenderInfoOSG : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IRenderInfoOSG );

  /// Id for this interface.
  enum { IID = 4114538498u };

  // Get the render info.
  virtual osg::RenderInfo              getRenderInfo() const = 0;
};


} // namespace Interfaces
} // namespace Usul


#endif // _USUL_INTERFACE_RENDER_INFO_OSG_H_
