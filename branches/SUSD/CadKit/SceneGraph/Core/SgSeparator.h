
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
//  SgSeparator.h: A group node that isolates properties.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_SCENEGRAPH_CORE_LIBRARY_SEPARATOR_H_
#define _CADKIT_SCENEGRAPH_CORE_LIBRARY_SEPARATOR_H_

#include "SgGroup.h"

namespace CadKit
{
class SG_API SgSeparator : public SgGroup
{
public:

  enum
  {
    PROJECTION    = 0x00000001, // Save/restore the projection matrix stack.
    MODELVIEW     = 0x00000002, // Save/restore the modelview matrix stack.
    NAMES         = 0x00000004, // Save/restore the names stack.
    COORDINATES   = 0x00000008, // Save/restore the coordinates.
    NORMALS       = 0x00000010, // Save/restore the normals.
    FONT          = 0x00000020, // Save/restore the font.
    MATERIAL      = 0x00000040, // Save/restore the attribute stack.
    BASE_COLOR    = 0x00000080, // Save/restore the attribute stack.
    DRAW_STYLE    = 0x00000100, // Save/restore the attribute stack.
    ALL           = 0xFFFFFFFF  // Save/restore everything (default).
  };

  unsigned int _pushPopFlags;

  SgSeparator();

protected:

  virtual ~SgSeparator();

  SG_DECLARE_DYNAMIC_NODE ( SgSeparator, 1033948672 );
  SL_DECLARE_BITMASK_FUNCTIONS ( PushPopFlags, unsigned int, _pushPopFlags );
};
};

#endif
