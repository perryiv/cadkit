#ifndef _menukit_osg_themeskin_h_
#define _menukit_osg_themeskin_h_

#include "MenuKit/Menu.h"
#include "MenuKit/Button.h"

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
      MENUKIT_DECLARE_POINTER ( thisclass );

      ThemeSkin(): base_class(), _theme(), _letter_height(0.7f), _separator_height(0.2f), _graphic_width(0.0f) {}

      virtual osg::Node* operator ()(const Menu& menu)=0;
      virtual osg::Node* operator ()(const Button& butn)=0;
      // TODO: virtual osg::Node* operator ()(const Item& item)=0;

      virtual float height(const Menu& menu) const=0;
      virtual float height(const Button& butn) const=0;
      virtual float height(const Item* item) const=0;

      virtual float width(const Menu& menu) const=0;
      virtual float width(const Button& butn) const=0;
      virtual float width(const Item* item) const=0;
      // TODO: virtual float width(const Item& menu);

      void graphic_width(float w) { _graphic_width = w; }
      float graphic_width() const { return _graphic_width; }

      void letter_height(float h) { _letter_height = h; }
      float letter_height() const { return _letter_height; }

      void separator_height(float w) { _separator_height = w; }
      float separator_height() const { return _separator_height; }

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

      //Detail::Box _box, _separator;  // 2 boxes that define the size of the total graphic
      theme_type _theme;
      float _letter_height,     // height of the printed-type
            _separator_height,  // height of the separator
            _graphic_width;     // horizontal distance to be honored when rendering
    };

  };

};

#endif
