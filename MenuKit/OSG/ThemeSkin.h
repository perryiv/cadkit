#ifndef _menukit_osg_themeskin_h_
#define _menukit_osg_themeskin_h_

#include "MenuKit/Menu.h"
#include "MenuKit/Button.h"

#include "osgText/Font"
#include "MenuKit/Box.h"

namespace osg
{
  class Node;
};

namespace MenuKit
{

  namespace OSG
  {
    /** ThemeSkin
      * 
      * This is an abstract base class for implementing classes
      * that want to take advantage of a "theme" member.  The
      * Box and MapType members are template requirements for
      * the ColorThemeSkinTile class.
      *
      * Imlementing classes will need to manufacture graphic
      * objects.  Derived classes will most likey use the
      * MapType member for color designation.
      */
    template<typename MapType>
    class ThemeSkin
    {
    public:
      ThemeSkin():
          _box(1.0,0.0), _separator(0.1,1.0), _font(), _theme()
      {}

      ThemeSkin(const ThemeSkin& ts):
        _box(ts._box), _separator(ts._separator), _font(ts._font), _theme(ts._theme)
      {}

      ThemeSkin& operator =(const ThemeSkin& ts)
      {
        _box = ts._box;
        _separator = ts._separator;
        _font = ts._font;
        _theme = ts._theme;
        return( *this );
      }

      ~ThemeSkin()
      {}

      virtual osg::Node* operator ()(const Menu& menu)=0;
      virtual osg::Node* operator ()(const Button& butn)=0;
      // TODO: virtual osg::Node* operator ()(const Item& item)=0;

      virtual float height(const Menu& menu)=0;
      virtual float height(const Button& butn)=0;

      virtual float width(const Menu& menu)=0;
      virtual float width(const Button& butn)=0;
      virtual float width(const Item* item)=0;
      // TODO: virtual float width(const Item& menu);

      void font(osgText::Font* f) { _font = f; }
      const osgText::Font* font() const { return _font.get(); }
      osgText::Font* font() { return _font.get(); }

      void box(const Detail::Box& b) { _box = b; }
      const Detail::Box& box() const { return _box; }
      Detail::Box& box() { return _box; }

      void separator(const Detail::Box& b) { _separator = b; }
      const Detail::Box& separator() const { return _separator; }
      Detail::Box& separator() { return _separator; }

      void theme(const MapType& t) { _theme = t; }
      const MapType& theme() const { return _theme; }
      MapType& theme() { return _theme; }

    private:
      Detail::Box _box, _separator;
      osg::ref_ptr<osgText::Font> _font;
      MapType _theme;
    };

  };

};

#endif
