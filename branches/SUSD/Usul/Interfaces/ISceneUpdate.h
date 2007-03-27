
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface to update the scene.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_ISCENEUPDATE_H__
#define __USUL_INTERFACES_ISCENEUPDATE_H__

#include "Usul/Interfaces/IUnknown.h"

namespace Usul {
namespace Interfaces {


struct ISceneUpdate : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( ISceneUpdate );

  /// Id for this interface.
  enum { IID = 3574234282u };

  virtual void sceneUpdate( ) = 0;

}; // struct ISceneUpdate


} // end namespace Interfaces
} // end namespace Usul


#endif /* __USUL_INTERFACES_ISCENEUPDATE_H__ */
