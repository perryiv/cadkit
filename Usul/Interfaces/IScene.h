
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for setting the scene
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_SCENE_H_
#define _USUL_INTERFACE_SCENE_H_

#include "Usul/Interfaces/IUnknown.h"

namespace osg { class Node; }

namespace Usul {
namespace Interfaces {


struct IScene : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IScene );

  /// Id for this interface.
  enum { IID = 2543180277u };

  virtual void setScene( osg::Node* ) = 0;
}; // class IScene


}; // namespace Interfaces
}; // namespace Usul


#endif // _USUL_INTERFACE_SCENE_H_
