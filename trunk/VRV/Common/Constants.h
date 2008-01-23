
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __VRV_CONSTANTS_H__
#define __VRV_CONSTANTS_H__

namespace VRV
{
  namespace Constants
  {
    namespace Sections
    {
      const std::string VRV             ( "VRV" );
    }

    namespace Keys
    {
      const std::string HOME_POSITION      ( "home_position" );
      const std::string AUTO_NEAR_FAR      ( "auto_near_far" );
      const std::string MENU_SHOW_HIDE     ( "menu_show_hide" );
      const std::string TRANSLATION_SPEED  ( "translation_speed" );
      const std::string NAVIGATION_FUNCTOR ( "navigation_functor" );
      const std::string SEEK_STEP_SIZE     ( "seek_step_size" );
      const std::string BACKGROUND_COLOR   ( "background_color" );
      const std::string BACKGROUND_CORNERS ( "background_corners" );
      const std::string TOP_LEFT           ( "top_left" );
      const std::string TOP_RIGHT          ( "top_right" );
      const std::string BOTTOM_LEFT        ( "bottom_left" );
      const std::string BOTTOM_RIGHT       ( "bottom_right" );
    }
  }
}

#endif // __VRV_CONSTANTS_H__
