//
// Author: John K. Grant
// All rights reserved.
//

#ifndef _menukit_sample_menu_
#define _menukit_sample_menu_

#include "MenuKit/Export.h"

namespace MenuKit
{
  class Menu;

  struct MENUKIT_EXPORT SampleMenu
  {
    Menu* operator ()();
  };
};

#endif
