
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

#include "Usul/Types/Types.h"

#include <string>


namespace AFW
{
  namespace Registry
  {
    namespace Sections
    {
      // Registry sections.
      const std::string MAIN_WINDOW               ( "main_window" );
    };

    namespace Keys
    {
      // Geometry.
      const std::string X                         ( "x" );
      const std::string Y                         ( "y" );
      const std::string WIDTH                     ( "width" );
      const std::string HEIGHT                    ( "height" );
      const std::string MAXIMIZED                 ( "maximized" );
    };
  };

  namespace Core
  {
    namespace State
    {
      typedef Usul::Types::Uint64 Type;
      const Type  DIRTY     ( 1 << 0 );
      const Type  DOCKABLE  ( 1 << 1 );
      const Type  DOCKED    ( 1 << 2 );
    }
  }
};


#endif // _APP_FRAME_WORK_CONSTANTS_H_
