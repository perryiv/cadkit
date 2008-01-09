
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant and Perry L. Miller IV.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Access to the navigation scene interface.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _VRV_INTERFACE_NAVIGATION_SCENE_H_
#define _VRV_INTERFACE_NAVIGATION_SCENE_H_

#include "Usul/Interfaces/IUnknown.h"

namespace osg { class Group; };


namespace VRV {
namespace Interfaces {


struct INavigationScene : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( INavigationScene );

  /// Id for this interface.
  enum { IID = 1076038111u };

  // Get the navigation scene.
  virtual const osg::Group *    navigationScene() const = 0;
  virtual osg::Group *          navigationScene() = 0;
};


} // namespace Interfaces
} // namespace VRV


#endif // _VRV_INTERFACE_NAVIGATION_SCENE_H_
