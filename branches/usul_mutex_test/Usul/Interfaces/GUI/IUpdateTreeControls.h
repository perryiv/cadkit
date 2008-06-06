
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_UPDATE_TREE_CONTROLS_H_
#define _USUL_INTERFACE_UPDATE_TREE_CONTROLS_H_

#include "Usul/Interfaces/IUnknown.h"

namespace osg { class Node; }

namespace Usul {
namespace Interfaces {

struct IUpdateTreeControls : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IUpdateTreeControls );

  /// Id for this interface.
  enum { IID = 2226012704u };

  virtual void updateTreeControls( osg::Node *scene ) = 0;

}; // class IUpdateTreeControls


} // namespace Interfaces
} // namespace Usul


#endif // _USUL_INTERFACE_UPDATE_TREE_CONTROLS_H_
