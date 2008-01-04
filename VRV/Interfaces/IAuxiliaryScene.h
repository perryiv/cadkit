
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant and Perry L. Miller IV.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Access to the auxiliary scene interface.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _VRV_INTERFACE_AUXILIARY_SCENE_H_
#define _VRV_INTERFACE_AUXILIARY_SCENE_H_

#include "Usul/Interfaces/IUnknown.h"

namespace osg { class Group; };


namespace VRV {
namespace Interfaces {


struct IAuxiliaryScene : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IAuxiliaryScene );

  /// Id for this interface.
  enum { IID = 1075924582 };

  // Get the auxiliary scene.
  virtual const osg::Group *    auxiliaryScene() const = 0;
  virtual osg::Group *          auxiliaryScene() = 0;
};


}; // namespace Interfaces
}; // namespace VRV


#endif // _CV_INTERFACE_AUXILIARY_SCENE_H_
