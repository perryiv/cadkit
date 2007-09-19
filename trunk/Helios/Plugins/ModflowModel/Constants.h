
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Constant values.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _MODFLOW_MODEL_CONSTANTS_H_
#define _MODFLOW_MODEL_CONSTANTS_H_

#include "CompileGuard.h"

#include <string>


namespace Modflow
{
  // String names.
  namespace Names
  {
    const std::string BOUNDS ( "Bounds" );
    const std::string HEAD   ( "head" );
  }

  namespace Flags
  {
    // What to draw.
    const unsigned int TOP       ( 0x00000001 );
    const unsigned int BOTTOM    ( 0x00000002 );
    const unsigned int EAST      ( 0x00000004 );
    const unsigned int WEST      ( 0x00000008 );
    const unsigned int NORTH     ( 0x00000010 );
    const unsigned int SOUTH     ( 0x00000020 );
    const unsigned int SIDES     ( EAST | WEST | NORTH | SOUTH );
    const unsigned int CUBE      ( SIDES | TOP | BOTTOM );
    const unsigned int BOUNDS    ( 0x00000040 );
    const unsigned int HEADS     ( 0x00000080 );

    // Other flags.
    const unsigned int VISIBLE   ( 0x00000100 );
    const unsigned int DIRTY     ( 0x00000200 );
  };
}


#endif // _MODFLOW_MODEL_CONSTANTS_H_
