
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Constant values.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_HELIOS_QT_CORE_CONSTANTS_H_
#define _CADKIT_HELIOS_QT_CORE_CONSTANTS_H_

#include "Helios/Qt/Core/Export.h"

#include "QtCore/QRect"

#include <string>


namespace CadKit
{
  namespace Helios
  {
    namespace Core
    {
      namespace Registry
      {
        namespace Sections
        {
          // Registry sections.
          const std::string MAIN_WINDOW               ( "main_window" );
          const std::string TEXT_WINDOW               ( "text_window" );
        }

        namespace Keys
        {
          // Geometry.
          const std::string GEOMETRY                  ( "geometry" );
          const std::string X                         ( "x" );
          const std::string Y                         ( "y" );
          const std::string WIDTH                     ( "width" );
          const std::string HEIGHT                    ( "height" );
          const std::string MAXIMIZED                 ( "maximized" );

          // Dock window positions.
          const std::string DOCK_WINDOW_POSITIONS     ( "dock_window_positions" );

          // Recent files.
          const std::string RECENT_FILES              ( "recent_files" );

          // Misc.
          const std::string MAXIMUM_BLOCK_COUNT       ( "maximum_block_count" );
          const std::string MAXIMUM_ITERATIONS        ( "maximum_iterations" );
        }

        namespace Defaults
        {
          // Geometry.
          const QRect GEOMETRY                        ( 100, 100, 800, 600 );
          const int X                                 ( 100 );
          const int Y                                 ( 100 );
          const int WIDTH                             ( 800 );
          const int HEIGHT                            ( 600 );
        }
      }

      namespace Constants
      {
        const unsigned int VERSION                    ( 1 );
      }
    }
  }
}


#endif //_CADKIT_HELIOS_QT_CORE_CONSTANTS_H_
