
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface to toggling the scene stage.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_ISCENESTAGE_H__
#define __USUL_INTERFACES_ISCENESTAGE_H__

#include "Usul/Interfaces/IUnknown.h"

namespace Usul {
namespace Interfaces {


struct ISceneStage : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( ISceneStage );

  /// Id for this interface.
  enum { IID = 1142049150u };

  virtual bool sceneStage() const = 0;
  virtual void sceneStage( bool b ) = 0;

}; // struct ISceneStage


} // end namespace Interfaces
} // end namespace Usul


#endif /* __USUL_INTERFACES_ISCENESTAGE_H__ */
