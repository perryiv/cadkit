#ifndef _menukit_osg_osg_types_h_
#define _menukit_osg_osg_types_h_

#include "MenuKit/OSG/ColorTheme.h"

#include "MenuKit/OSG/ThemeSkin.h"
#include "MenuKit/OSG/ThemeSkinTile.h"
#include "MenuKit/OSG/TileMason.h"

namespace MenuKit
{
  namespace OSG
  {
    typedef ColorTheme<osg::Vec4> osg_color_theme;
    typedef osg_color_theme::color_map osg_color_map;

    typedef ThemeSkin<osg_color_map> osgThemeSkin;
    typedef ThemeSkinTile<osgThemeSkin> osgThemeSkinTile;
    typedef Mason<osgThemeSkinTile> osgMason;
  };

};

#endif
