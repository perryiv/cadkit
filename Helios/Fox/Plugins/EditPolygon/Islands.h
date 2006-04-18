
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __DELETION_DELETE_ALL_ISLANDS_H__
#define __DELETION_DELETE_ALL_ISLANDS_H__

#include "Usul/Base/Referenced.h"
#include "Usul/Interfaces/IFoxEvent.h"
#include "Usul/Interfaces/ISetCursor.h"

namespace FX { class FXCursor; }
namespace osgUtil { class Hit; }


class Islands : public Usul::Base::Referenced,
                public Usul::Interfaces::IFoxEvent,
                public Usul::Interfaces::ISetCursor
{
public:

  /// Typedefs.
  typedef Usul::Base::Referenced BaseClass;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Islands );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  Islands();

  ///  Usul::Interfaces::IFoxEvent
  virtual bool            execute ( Usul::Interfaces::IUnknown *caller,  bool left, bool middle, bool right, bool motion, float x, float y, float z );

  /// Usul::Interfaces::ISetCursor
  virtual FX::FXCursor *  getCursor();

protected:

  virtual ~Islands();

  void                    _keepAllConnected ( Usul::Interfaces::IUnknown* caller, const osgUtil::Hit &hit );

private:

  FX::FXCursor *_cursor;
};


#endif // __DELETION_DELETE_ALL_ISLANDS_H__

