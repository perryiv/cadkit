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
    class ThemeSkin : public Referenced
    {
    public:
      typedef Referenced base_class;
      typedef MapType theme_type;
      typedef ThemeSkin<theme_type> thisclass;
      MENUKIT_DECLARE_POINTER ( thisclass );  ///\todo TODO: correct syntax?  needs 'typename' in front?

      ThemeSkin(): base_class(), _box(1.0,1.0), _separator(0.1,1.0), _theme() {}

      virtual osg::Node* operator ()(const Menu& menu)=0;
      virtual osg::Node* operator ()(const Button& butn)=0;
      // TODO: virtual osg::Node* operator ()(const Item& item)=0;

      virtual float height(const Menu& menu)=0;
      virtual float height(const Button& butn)=0;
      virtual float height(const Item* item)=0;

      virtual float width(const Menu& menu)=0;
      virtual float width(const Button& butn)=0;
      virtual float width(const Item* item)=0;
      // TODO: virtual float width(const Item& menu);

      void box(const Detail::Box& b) { _box = b; }
      const Detail::Box& box() const { return _box; }
      Detail::Box& box() { return _box; }

      void separator(const Detail::Box& b) { _separator = b; }
      const Detail::Box& separator() const { return _separator; }
      Detail::Box& separator() { return _separator; }

      void theme(const theme_type& t) { _theme = t; }
      const theme_type& theme() const { return _theme; }
      theme_type& theme() { return _theme; }

    protected:
      virtual ~ThemeSkin() {}

    private:
      ///\todo TODO: evaluate if this class is copyable
      // not implemented by design
      ThemeSkin(const ThemeSkin& ts);//: base_class(ts)
      //  _box(ts._box), _separator(ts._separator), _theme(ts._theme)
      //{}

      ThemeSkin& operator =(const ThemeSkin& ts);
      //{
      //  base_class::operator =(ts);
      //  _box = ts._box;
      //  _separator = ts._separator;
      //  _theme = ts._theme;
      //  return( *this );
      //}

      Detail::Box _box, _separator;  // 2 boxes that define the size of the total graphic
      theme_type _theme;
    };

  };

};

#endif
