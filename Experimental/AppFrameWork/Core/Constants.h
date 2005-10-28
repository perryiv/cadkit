
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
//  Constant definitions.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef _APP_FRAME_WORK_CONSTANTS_H_
#define _APP_FRAME_WORK_CONSTANTS_H_

#include <string>


namespace AFW
{
  namespace Registry
  {
    namespace Sections
    {
      // Registry sections.
      const std::string MAIN_WINDOW               = "main_window";
    };

    namespace Keys
    {
      // Geometry.
      const std::string X                         = "x";
      const std::string Y                         = "y";
      const std::string WIDTH                     = "width";
      const std::string HEIGHT                    = "height";
      const std::string MAXIMIZED                 = "maximized";
    };
  };
};


#endif // _APP_FRAME_WORK_CONSTANTS_H_
