
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __DELETE_GEOMETRY_H__
#define __DELETE_GEOMETRY_H__

#include "Usul/Base/Referenced.h"
#include "Usul/Interfaces/IFoxEvent.h"
#include "Usul/Interfaces/ISetCursor.h"

namespace FX { class FXCursor; }
namespace osgUtil { class Hit; }

namespace Helios {
namespace Plugins {
namespace EditPolygons {

class Geometry : public Usul::Base::Referenced,
                  public Usul::Interfaces::IFoxEvent,
                  public Usul::Interfaces::ISetCursor
{
public:
  /// Typedefs.
  typedef Usul::Base::Referenced BaseClass;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Geometry );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  Geometry ( );

  //  Usul::Interfaces::IFoxEvent
  virtual bool            execute ( Usul::Interfaces::IUnknown *caller,  bool left, bool middle, bool right, bool motion, float x, float y, float z );

  //  Usul::Interfaces::IGetCursor
  virtual FX::FXCursor *  getCursor();

protected:

  virtual ~Geometry();

  void                    _deleteAllConnected ( Usul::Interfaces::IUnknown *caller, const osgUtil::Hit &hit );

private:

  FX::FXCursor *_cursor;
};

} //namespace EditPolygon
} //namespace Plugin
} //namespace Helios

#endif //__DELETE_GEOMETRY_H__

