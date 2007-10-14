
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Constants and definitions.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _MODFLOW_MODEL_CONSTANTS_H_
#define _MODFLOW_MODEL_CONSTANTS_H_

#include "Helios/Plugins/ModflowModel/CompileGuard.h"

#include <string>


///////////////////////////////////////////////////////////////////////////////
//
//  Constants
//
///////////////////////////////////////////////////////////////////////////////

namespace Modflow
{
  // String names.
  namespace Names
  {
    const std::string CELL_BOUNDARY ( "Cell Boundary" );
    const std::string STARTING_HEAD ( "Starting Head" );
    const std::string HEAD_LEVELS   ( "Head Levels"   );
    const std::string MAX_ELEVATION ( "Max Elevation" );
    const std::string MIN_ELEVATION ( "Min Elevation" );
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


///////////////////////////////////////////////////////////////////////////////
//
//  Definitions
//
///////////////////////////////////////////////////////////////////////////////

#define INVALID_MIN std::numeric_limits<double>::max()
#define INVALID_MAX std::numeric_limits<double>::min()


#endif // _MODFLOW_MODEL_CONSTANTS_H_
