
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __FLIP_NORMAL_H__
#define __FLIP_NORMAL_H__

#include "Usul/Base/Referenced.h"
#include "Usul/Interfaces/IFoxEvent.h"
#include "Usul/Interfaces/ISetCursor.h"

namespace FX { class FXCursor; }

class FlipNormal : public Usul::Base::Referenced,
                   public Usul::Interfaces::IFoxEvent,
                   public Usul::Interfaces::ISetCursor
{
public:
  /// Typedefs.
  typedef Usul::Base::Referenced BaseClass;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( FlipNormal );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  FlipNormal( );

protected:

  virtual ~FlipNormal();

  ///  Usul::Interfaces::IFoxEvent
  virtual bool            execute ( Usul::Interfaces::IUnknown *caller,  bool left, bool middle, bool right, bool motion, float x, float y, float z );

  ///  Usul::Interfaces::ISetCursor
  virtual FX::FXCursor*   getCursor();

private:

  FX::FXCursor *_cursor; // Raw pointer allows forward declaration above.

}; // class FlipNormal


#endif //__FLIP_NORMAL_H__

