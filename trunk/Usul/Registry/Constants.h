
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
      const std::string MAIN_WINDOW               = "main_window";
      const std::string OPEN_GL_CANVAS            = "open_gl_canvas";
      const std::string TAB_BOOK                  = "tab_book";
      const std::string HORIZONTAL_SPLITTER       = "horizontal_splitter";
      const std::string FILE_DIALOG_OPEN          = "file_dialog_open";
      const std::string FILE_DIALOG_SAVE          = "file_dialog_save";
      const std::string VIEWER_SETTINGS           = "viewer_settings";
      const std::string PREFERENCES               = "preferences";
    };

    namespace Keys
    {
      // Geometry.
      const std::string X                         = "x";
      const std::string Y                         = "y";
      const std::string WIDTH                     = "width";
      const std::string HEIGHT                    = "height";
      const std::string MAXIMIZED                 = "maximized";
      const std::string SPLIT                     = "split";

      // Child window background colors.
      const std::string CLEAR_COLOR               = "clear_color";

      // Preference values.
      const std::string ANIMATION_TIME            = "animation_time";
      const std::string WARN_NO_UNDO              = "warn_no_undo";
      const std::string VIEW_ALL_ON_LOAD          = "view_all_on_load";
      const std::string ALLOW_SPIN                = "allow_spin";
      const std::string DISPLAY_LISTS             = "display_lists";
      const std::string LOW_LODS                  = "low_lods";
      const std::string HIGH_LODS                 = "high_lods";
    };
  };
};


#endif // _USUL_BINDING_CONSTANTS_H_
