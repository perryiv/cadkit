
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for intersecting a scene
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_SCENE_INTERSECT_H_
#define _USUL_INTERFACE_SCENE_INTERSECT_H_

#include "Usul/Interfaces/IUnknown.h"

namespace osg { class Node; };
namespace osgUtil { class Hit; };
namespace FX { struct FXEvent; };

namespace Usul {
namespace Interfaces {


struct ISceneIntersect : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( ISceneIntersect );

  /// Id for this interface.
  enum { IID = 1101086294u };

  virtual bool intersect ( const FX::FXEvent &event, osg::Node *scene, osgUtil::Hit &hit ) = 0;
}; // class ISceneIntersect


}; // namespace Interfaces
}; // namespace Usul


#endif // _USUL_INTERFACE_SCENE_INTERSECT_H_
