
///////////////////////////////////////////////////////////////////////////////
//
//  BSD License
//  http://www.opensource.org/licenses/bsd-license.html
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without 
//  modification, are permitted provided that the following conditions are met:
//
//  - Redistributions of source code must retain the above copyright notice, 
//    this list of conditions and the following disclaimer. 
//
//  - Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution. 
//
//  - Neither the name of the CAD Toolkit nor the names of its contributors may
//    be used to endorse or promote products derived from this software without
//    specific prior written permission. 
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
//  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
//  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
//  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
//  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
//  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
//  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
//  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
//  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
//  POSSIBILITY OF SUCH DAMAGE.
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
    PROJECTION    = (1L<<0),    // Save/restore the projection matrix stack.
    MODELVIEW     = (1L<<1),    // Save/restore the modelview matrix stack.
    NAMES         = (1L<<2),    // Save/restore the names stack.
    COORDINATES   = (1L<<3),    // Save/restore the coordinates.
    NORMALS       = (1L<<4),    // Save/restore the normals.
    FONT          = (1L<<5),    // Save/restore the font.
    MATERIAL      = (1L<<6),    // Save/restore the attribute stack.
    BASE_COLOR    = (1L<<7),    // Save/restore the attribute stack.
    DRAW_STYLE    = (1L<<8),    // Save/restore the attribute stack.
    ALL           = 0xFFFFFFFF  // Save/restore everything.
  };

  unsigned int _pushPopFlags;  // Default is ALL.

  SgSeparator();

protected:

  virtual ~SgSeparator();

  SG_DECLARE_DYNAMIC_NODE ( SgSeparator, 0x0000102A );
  SL_DECLARE_BITMASK_FUNCTIONS ( PushPopFlags, unsigned int, _pushPopFlags );
};
};

#endif
