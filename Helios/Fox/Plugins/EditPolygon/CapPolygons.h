
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __EDIT_POLYGONS_CAP_POLYGONS_H__
#define __EDIT_POLYGONS_CAP_POLYGONS_H__

#include "FoxTools/Headers/Object.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Pointers/Pointers.h"
#include "Usul/Interfaces/IUnknown.h"

namespace Helios {
namespace Plugins {
namespace EditPolygons {

class CapPolygons : public Usul::Base::Referenced
{
public:

  /// Typedefs.
  typedef Usul::Base::Referenced BaseClass;
  typedef Usul::Interfaces::IUnknown Unknown;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( CapPolygons );

  CapPolygons ( Usul::Interfaces::IUnknown *mainWindow );

  virtual void execute ( Usul::Interfaces::IUnknown *caller );

 
protected:

  

  
  //For fox do not use
  CapPolygons () { }

  // Use ref counting
  virtual ~CapPolygons();

  Usul::Interfaces::IUnknown::ValidAccessQueryPtr _mainWindow;


};


} //namespace EditPolygons
} //namespace Plugins
} //namespace Helios


#endif // __EDIT_POLYGONS_CAP_POLYGONS_H__
