
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

#ifndef _USUL_BINDING_CONSTANTS_H_
#define _USUL_BINDING_CONSTANTS_H_

#include <string>


namespace Usul
{
  namespace Registry
  {
    namespace Sections
    {
      // Registry sections.
      const std::string MAIN_WINDOW               ( "main_window" );
      const std::string OPEN_GL_CANVAS            ( "open_gl_canvas" );
      const std::string TAB_BOOK                  ( "tab_book" );
      const std::string HORIZONTAL_SPLITTER       ( "horizontal_splitter" );
      const std::string FILE_DIALOG_OPEN          ( "file_dialog_open" );
      const std::string FILE_DIALOG_SAVE          ( "file_dialog_save" );
      const std::string VIEWER_SETTINGS           ( "viewer_settings" );
      const std::string DOCUMENT_SETTINGS         ( "document_settings" );
      const std::string PREFERENCES               ( "preferences" );
      const std::string IMAGE_EXPORT_SIZE         ( "image_export_size" );
      const std::string PATH_ANIMATION            ( "path_animation" );
    };

    namespace Keys
    {
      // Geometry.
      const std::string X                         ( "x" );
      const std::string Y                         ( "y" );
      const std::string WIDTH                     ( "width" );
      const std::string HEIGHT                    ( "height" );
      const std::string MAXIMIZED                 ( "maximized" );
      const std::string SPLIT                     ( "split" );

      // Preference values.
      const std::string ANIMATION_TIME            ( "animation_time" );
      const std::string WARN_NO_UNDO              ( "warn_no_undo" );
      const std::string VIEW_ALL_ON_LOAD          ( "view_all_on_load" );
      const std::string ALLOW_SPIN                ( "allow_spin" );
      const std::string DISPLAY_LISTS             ( "display_lists" );
      const std::string LOW_LODS                  ( "low_lods" );
      const std::string HIGH_LODS                 ( "high_lods" );
      const std::string FOV                       ( "fov" );
      const std::string PERSPECTIVE               ( "perspective" );
      const std::string COR_NAME                  ( "center_of_rotation" );

      // Misc.
      const std::string BACKGROUND_COLOR          ( "background_color" );
      const std::string FRAME_DUMP_DIRECTORY      ( "frame_dump_directory" );
      const std::string RENDER_LOOP               ( "render_loop" );
      const std::string TRACKBALL                 ( "trackball" );
      const std::string SEEK_NUM_STEPS            ( "seek_num_steps" );
      const std::string FRAME_BUFFER_OBJECT_SIZE  ( "frame_buffer_object_size" );
    };
  };
};


#endif // _USUL_BINDING_CONSTANTS_H_
