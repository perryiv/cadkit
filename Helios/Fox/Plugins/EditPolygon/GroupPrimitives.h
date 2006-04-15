
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _EDIT_POLYGONS_GROUP_PRIMITIVES_H_
#define _EDIT_POLYGONS_GROUP_PRIMITIVES_H_

#include "FoxTools/Headers/Object.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Pointers/Pointers.h"
#include "Usul/Interfaces/IUnknown.h"

namespace Helios {
namespace Plugins {
namespace EditPolygons {

class GroupPrimitives : public Usul::Base::Referenced
{
public:

  /// Typedefs.
  typedef Usul::Base::Referenced BaseClass;
  typedef Usul::Interfaces::IUnknown Unknown;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( GroupPrimitives );

  GroupPrimitives ( Usul::Interfaces::IUnknown *mainWindow );

  virtual void execute ( Usul::Interfaces::IUnknown *caller );

 
protected:
  //For fox do not use
  GroupPrimitives () { }

  // Use ref counting
  virtual ~GroupPrimitives();

  Usul::Interfaces::IUnknown::ValidAccessQueryPtr _mainWindow;


};


} //namespace EditPolygons
} //namespace Plugins
} //namespace Helios


#endif // _EDIT_POLYGONS_GROUP_PRIMITIVES_H_
