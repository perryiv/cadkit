
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
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

#include "Usul/Types/Types.h"

#include <string>


namespace AFW
{
  namespace Registry
  {
    namespace Keys
    {
      // Geometry.
      const std::string X                         ( "x" );
      const std::string Y                         ( "y" );
      const std::string WIDTH                     ( "width" );
      const std::string HEIGHT                    ( "height" );
      const std::string MAXIMIZED                 ( "maximized" );
      const std::string SPLIT_DISTANCE            ( "split_distance" );
    }
  }

  namespace Core
  {
    namespace State
    {
      typedef Usul::Types::Uint64 Type;
      const Type DIRTY    ( 1 << 0 );
      const Type MODIFIED ( 1 << 1 );
      const Type VISIBLE  ( 1 << 2 );
      const Type ENABLED  ( 1 << 3 );
      const Type CHECKED  ( 1 << 4 );
    }

    namespace DockSite
    {
      enum Type
      {
        LEFT, RIGHT, TOP, BOTTOM, NONE
      };
    }
  }
}


#endif // _APP_FRAME_WORK_CONSTANTS_H_
