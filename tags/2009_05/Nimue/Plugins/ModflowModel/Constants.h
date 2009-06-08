
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Constants and definitions.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _MODFLOW_MODEL_CONSTANTS_H_
#define _MODFLOW_MODEL_CONSTANTS_H_

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
    const std::string CELL_BOUNDARY         ( "Cell Boundary"         );
    const std::string STARTING_HEAD         ( "Starting Head"         );
    const std::string HEAD_LEVELS           ( "Head Levels"           );
    const std::string HEAD_SURFACE          ( "Head Surface"          );
    const std::string HEAD_SURFACE_COARSE   ( "Head Surface Coarse"   );
    const std::string HEAD_SURFACE_SMOOTH   ( "Head Surface Smooth"   );
    const std::string MAX_ELEVATION         ( "Max Elevation"         );
    const std::string MIN_ELEVATION         ( "Min Elevation"         );
    const std::string MAX_HEAD_LEVEL        ( "Max Head Level"        );
    const std::string MIN_HEAD_LEVEL        ( "Min Head Level"        );
    const std::string RECHARGE              ( "Recharge"              );
    const std::string PUMPING               ( "Pumping"               );
  }

  // Readers.
  namespace Readers
  {
    const std::string DISCRETIZATION      ( "discretization"      );
    const std::string BASIC_PACKAGE       ( "basic_package"       );
    const std::string BLOCK_CENTERED_FLOW ( "block_centered_flow" );
    const std::string RECHARGE            ( "recharge"            );
    const std::string PUMPING             ( "pumping"             );
    const std::string HEAD_LEVEL_OUTPUT   ( "head_level_output"   );
    const std::string AUXILIARY_DATA      ( "auxiliary_data"      );
  }

  // File types.
  namespace FileTypes
  {
    const std::string DISCRETIZATION      ( "Discretization"      );
    const std::string BASIC_PACKAGE       ( "Basic Package"       );
    const std::string BLOCK_CENTERED_FLOW ( "Block Centered Flow" );
    const std::string RECHARGE            ( "Recharge"            );
    const std::string PUMPING             ( "Pumping"             );
    const std::string HEAD_LEVEL_OUTPUT   ( "Head Level Output"   );
    const std::string AUXILIARY_DATA      ( "Auxiliary Data"      );
  }

  // Named attributes.
  namespace Attributes
  {
    const std::string UNIT_CONVERSION  ( "unit_conversion"  );
    const std::string CELL_GRID_ORIGIN ( "cell_grid_origin" );
    const std::string SORT_TRIANGLES   ( "sort_triangles"   );
    const std::string COLOR            ( "color"            );
    const std::string ROTATE           ( "rotate"           );
    const std::string TRANSLATE        ( "translate"        );
    const std::string LABEL            ( "label"            );
    const std::string TEXTURE_IMAGES   ( "texture_images"   );
    const std::string WELL_KNOWN_TEXT  ( "well_known_text"  );
    const std::string HEIGHT_SCALE     ( "height_scale"     );
    const std::string RADIUS_SCALE     ( "radius_scale"     );
    const std::string POINT_SIZE       ( "point_size"       );
    const std::string LINE_WIDTH       ( "line_width"       );
  }

  // Flags for state.
  namespace Flags
  {
    const unsigned int VISIBLE   ( 0x00000100 );
    const unsigned int DIRTY     ( 0x00000200 );
  };

  // Enumerations identifying geometry.
  struct Geometry
  {
    enum ID
    {
      CELL_TOP, 
      CELL_BOTTOM, 
      CELL_EAST, 
      CELL_WEST, 
      CELL_NORTH, 
      CELL_SOUTH,
      CELL_HEAD_LEVEL
    };
  };

  // Misc.
  namespace Constants
  {
    // When converting the "no data" string to a double, there is sometimes a 
    // small round-off error in the last decimal. We are handeling this by 
    // doing integer comparisons instead.
    const unsigned long       NO_DATA_MULTIPLIER ( 1000 );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Definitions
//
///////////////////////////////////////////////////////////////////////////////

#define INVALID_MIN std::numeric_limits<double>::max()
#define INVALID_MAX std::numeric_limits<double>::min()


#endif // _MODFLOW_MODEL_CONSTANTS_H_
