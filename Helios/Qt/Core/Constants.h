
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
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
          const std::string MAIN_WINDOW               = "main_window";
        }

        namespace Keys
        {
          // Geometry.
          const std::string GEOMETRY                  = "geometry";
          const std::string X                         = "x";
          const std::string Y                         = "y";
          const std::string WIDTH                     = "width";
          const std::string HEIGHT                    = "height";
          const std::string MAXIMIZED                 = "maximized";
        }

        namespace Defaults
        {
          const QRect GEOMETRY                        ( 100, 100, 800, 600 );
          const int X                                 = 100;
          const int Y                                 = 100;
          const int WIDTH                             = 800;
          const int HEIGHT                            = 600;
        }
      }
    }
  }
}


#endif //_CADKIT_HELIOS_QT_CORE_CONSTANTS_H_
