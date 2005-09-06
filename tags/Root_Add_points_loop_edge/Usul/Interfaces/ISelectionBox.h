
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for drawing and removing a selection box
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_SELECTION_BOX_H_
#define _USUL_INTERFACE_SELECTION_BOX_H_

#include "Usul/Interfaces/IUnknown.h"

namespace osg { class Vec3f; typedef Vec3f Vec3; }

namespace Usul {
namespace Interfaces {


struct ISelectionBox : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( ISelectionBox );

  /// Id for this interface.
  enum { IID = 1104766252u };

  virtual void drawSelectionBox ( const osg::Vec3&, const osg::Vec3& ) = 0;
  virtual void removeSelectionBox() = 0;

}; // class ISelectionBox


}; // namespace Interfaces
}; // namespace Usul


#endif // _USUL_INTERFACE_SELECTION_BOX_H_
