#ifndef _menukit_osg_themeskin_h_
#define _menukit_osg_themeskin_h_

#include "osgText/Font"

namespace osg
{
  class Node;
};

namespace MenuKit
{

  namespace OSG
  {

    /** ThemeSkin
      * Abstract base class for Skin's using a theme
      * that can be used with Tile's that use a theme
      */
    template<class ThemeType>
    class ThemeSkin
    {
    public:
      ThemeSkin(): _box(), _font(0x0), _theme() {}

      ThemeSkin(const Detail::Box& b, osgText::Font* f, ThemeType t):
        _box(b), _font(f), _theme(t) {}

      ThemeSkin(const ThemeSkin& ts): _box(ts._box), _font(ts._font), _theme(ts._theme) {}

      ThemeSkin& operator =(const ThemeSkin& ts)
      {
        _box = ts._box;
        _font = ts._font;
        _theme = ts._theme;
      }

      ~ThemeSkin() {}

      virtual osg::Node* operator ()(const Menu& menu)=0;
      virtual osg::Node* operator ()(const Button& butn)=0;
      // TODO: virtual osg::Node* operator ()(const Item& item)=0;

      virtual float width(const Menu& menu)=0;
      virtual float width(const Button& butn)=0;
      // TODO: virtual float width(const Item& menu);

      void box(const Detail::Box& b) { _box = b; }
      const Detail::Box& box() const { return _box; }
      Detail::Box& box() { return _box; }

      void font(osgText::Font* f) { _font = f; }
      const osgText::Font* font() const { return _font.get(); }
      osgText::Font* font() { return _font.get(); }

      void theme(const ThemeType& t) { _theme = t; }
      const ThemeType& theme() const { return _theme; }

    private:
      Detail::Box _box;
      osg::ref_ptr<osgText::Font> _font;
      ThemeType _theme;
    };

  };

};

#endif
