
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_INTERFACES_IDIRTY_SCENE_H__
#define __MINERVA_INTERFACES_IDIRTY_SCENE_H__

#include "Usul/Interfaces/IUnknown.h"

namespace Minerva {
namespace Interfaces {

struct IDirtyScene : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IDirtyScene );

  /// Id for this interface.
  enum { IID = 1789897599u };

  /// Dirty the scene.
  virtual void            dirtyScene () = 0;

}; // struct IDirtyScene

} // end namespace Interfaces
} // end namespace Minerva


#endif // __MINERVA_INTERFACES_IDIRTY_SCENE_H__ 
