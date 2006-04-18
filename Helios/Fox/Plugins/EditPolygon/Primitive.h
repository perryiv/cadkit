
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __DELETE_PRIMITIVE_H__
#define __DELETE_PRIMITIVE_H__

#include "Usul/Base/Referenced.h"
#include "Usul/Interfaces/IFoxEvent.h"
#include "Usul/Interfaces/ISetCursor.h"

namespace FX { class FXCursor; }

namespace osgUtil { class Hit; }

class Primitive : public Usul::Base::Referenced,
                  public Usul::Interfaces::IFoxEvent,
                  public Usul::Interfaces::ISetCursor
{
public:
  /// Typedefs.
  typedef Usul::Base::Referenced BaseClass;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Primitive );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  Primitive( );

  /////////////////////////////////////////////////////////////////////////////
  //
  //  Usul::Interfaces::IFoxEvent
  //
  /////////////////////////////////////////////////////////////////////////////
  virtual bool            execute ( Usul::Interfaces::IUnknown *caller, bool left, bool middle, bool right, bool motion, float x, float y, float z );

  /////////////////////////////////////////////////////////////////////////////
  //
  //  Usul::Interfaces::ISetCursor
  //
  /////////////////////////////////////////////////////////////////////////////
  virtual FX::FXCursor*   getCursor();

protected:

  virtual ~Primitive();

  void     _deleteUsingDocument( Usul::Interfaces::IUnknown *caller, osgUtil::Hit& );
  void     _deleteUsingScene   ( Usul::Interfaces::IUnknown *caller, osgUtil::Hit& );

private:

  FX::FXCursor *_cursor; // Raw pointer allows forward declaration above.

}; // class Primitive


#endif //__DELETE_PRIMITIVE_H__


