
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

#include "Usul/Math/Vector3.h"
#include "Usul/Math/Vector4.h"

namespace VRV
{
  namespace Constants
  {
    namespace Sections
    {
      const std::string VRV             ( "VRV" );
      const std::string PREFERENCES     ( "preferences" );
    }

    namespace Keys
    {
      // Keys for registry.
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

      // Keys for settings.
      const std::string CLIPPING_PLANE     ( "clipping_plane" );
      const std::string NEAR_PLANE         ( "near" );
      const std::string FAR_MULTIPLIER     ( "far_multiplier" );
      const std::string VIEW_ALL           ( "view_all" );
      const std::string SCALE              ( "scale" );
      const std::string TRANSLATE          ( "translate" );
      const std::string ROTATE             ( "rotate" );
      const std::string LIGHT              ( "light" );
      const std::string AMBIENT            ( "ambient" );
      const std::string DIFFUSE            ( "diffuse" );
      const std::string SPECULAR           ( "specular" );
      const std::string DIRECTION          ( "direction" );
      const std::string POSITION           ( "position" );
      const std::string BACKGROUND         ( "background" );
      const std::string COLOR              ( "color" );
      const std::string MACHINE            ( "machine" );
      const std::string WRITER             ( "writer" );
      const std::string HEAD               ( "head" );
      const std::string NORMALIZE          ( "normalize" );
      const std::string GLOBAL             ( "global" );
      const std::string MODELS             ( "models" );
      const std::string MENU               ( "menu" );
      const std::string TEXT_KEY           ( "text" );
      const std::string NORMAL             ( "normal" );
      const std::string HIGHLIGHT          ( "hightlight" );
      const std::string DISABLED           ( "disabled" );
      const std::string STATUS_BAR         ( "status_bar" );
      const std::string VISIBLE            ( "visible" );
      const std::string SPEED              ( "speed" );
      const std::string IMAGE              ( "image" );
      const std::string FRAME_SCALE        ( "frame_scale" );
      const std::string EXTENSION          ( "ext" );
      const std::string DIRECTORY          ( "directory" );
      const std::string WAND_OFFSET        ( "wand_offset" );
    }

    namespace Defaults
    {
      const float NEAR_CLIPPING_PLANE           ( 0.1f );
      const float FAR_MULTIPLIER                ( 1.0f );
      const float VIEW_ALL_SCALE                ( 2.0f );
      const Usul::Math::Vec4f LIGHT_AMBIENT     ( 0.2f, 0.2f, 0.2f, 1.0f );
      const Usul::Math::Vec4f LIGHT_DIFFUSE     ( 1.0f, 1.0f, 1.0f, 1.0f );
      const Usul::Math::Vec4f LIGHT_SPECULAR    ( 0.7f, 0.7f, 0.7f, 1.0f );
      const Usul::Math::Vec3f LIGHT_DIRECTION   ( 0.0f, 0.0f, -1.0f );
      const Usul::Math::Vec4f LIGHT_POSITION    ( 0.0f, 0.0f, 0.0f, 1.0f );
      const Usul::Math::Vec4f BACKGROUND_COLOR  ( 0.0f, 0.0f, 0.0f, 1.0f );
    }
  }
}

#endif // __VRV_CONSTANTS_H__
