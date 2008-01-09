
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for getting the update visitor.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_UPDATE_SCENE_VISITOR_H_
#define _USUL_INTERFACE_UPDATE_SCENE_VISITOR_H_

#include "Usul/Interfaces/IUnknown.h"

namespace osg { class NodeVisitor; }


namespace Usul {
namespace Interfaces {


struct IUpdateSceneVisitor : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IUpdateSceneVisitor );

  /// Id for this interface.
  enum { IID = 2847158255u };

  // Get the update visitor.
  virtual osg::NodeVisitor *getUpdateSceneVisitor ( Usul::Interfaces::IUnknown *caller ) = 0;
};


} // namespace Interfaces
} // namespace Usul


#endif // _USUL_INTERFACE_UPDATE_SCENE_VISITOR_H_
