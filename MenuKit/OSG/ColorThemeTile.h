//
// Author: John K. Grant
// All rights reserved.
//

#ifndef _menukit_osg_colorthemetile_h_
#define _menukit_osg_colorthemetile_h_

#include "BaseTileFunctor.h"
#include "ColorTheme.h"

#include "osg/Node"

namespace Detail
{
  float CalculateTextWidth(const MenuKit::Menu& m) { return 1.0; }
}

namespace MenuKit
{

  namespace OSG
  {
    template<class ThemeType>
    class ColorThemeTile : public BaseTileFunctor
    {
    public:
      typedef BaseTileFunctor BaseClass;
      ColorThemeTile(): BaseClass(), _theme() {}
      ColorThemeTile(const ColorThemeTile& tt): BaseClass(tt), _theme() {}

      ThemeType& theme() { return _theme; }

      ColorThemeTile& operator= (const ColorThemeTile<ThemeType>& tt)
      {
        BaseClass::operator=(tt);
        _theme = tt._theme;
      }

      virtual float width(const Menu&);// { return 1.0; }
      virtual float width(const Button&);// { return 1.0; }
      // TODO: virtual float calculate_width(Item&);

      virtual osg::Node* operator() (const Menu&);// { return (new osg::Node()); }
      virtual osg::Node* operator() (const Button&);// { return (new osg::Node()); }
      // TODO: virtual osg::Node* operator() (Item&);

    private:
      ThemeType _theme;
    };

    typedef ColorThemeTile<ColorTheme> ColorTile;
  };

};

using namespace MenuKit;

template<class ThemeType>
osg::Node* OSG::ColorThemeTile<ThemeType>::operator ()(const Menu& m)
{
  osg::ref_ptr<osg::Node> node = new osg::Node();
  return node.release();
}

template<class ThemeType>
osg::Node* OSG::ColorThemeTile<ThemeType>::operator ()(const Button& m)
{
  osg::ref_ptr<osg::Node> node = new osg::Node();
  return node.release();
}

template<class ThemeType>
float OSG::ColorThemeTile<ThemeType>::width(const Menu& m)
{
  return 1.0;
}

template<class ThemeType>
float OSG::ColorThemeTile<ThemeType>::width(const Button& b)
{
  return 1.0;
}

#endif
