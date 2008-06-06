
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for getting the cull visitor.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_CULL_SCENE_VISITOR_H_
#define _USUL_INTERFACE_CULL_SCENE_VISITOR_H_

#include "Usul/Interfaces/IUnknown.h"

namespace osgUtil { class CullVisitor; }


namespace Usul {
namespace Interfaces {


struct ICullSceneVisitor : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( ICullSceneVisitor );

  /// Id for this interface.
  enum { IID = 4016842226u };

  // Get the cull visitor.
  virtual osgUtil::CullVisitor *getCullSceneVisitor ( Usul::Interfaces::IUnknown *caller ) = 0;
};


} // namespace Interfaces
} // namespace Usul


#endif // _USUL_INTERFACE_CULL_SCENE_VISITOR_H_
