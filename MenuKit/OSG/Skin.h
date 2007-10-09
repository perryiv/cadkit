
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, John K. Grant.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _menukit_osg_skin_
#define _menukit_osg_skin_

#include "MenuKit/OSG/Export.h"
#include "MenuKit/Menu.h"

#include "Usul/Base/Object.h"

#include "osg/Vec4"
#include "osgText/Font"
#include "osg/PrimitiveSet"

namespace osg
{
  class Node;
};

namespace MenuKit
{
  class Item;
  class Button;
  class Menu;

  namespace OSG
  {

    // abstract base class for all skins
    class MENUKIT_OSG_EXPORT Skin : public Usul::Base::Object
    {
    public:
      typedef Usul::Base::Object BaseClass;
      USUL_DECLARE_REF_POINTERS ( Skin );

      // Mode.
      enum Mode { HIGHLIGHT, NORMAL, DISABLED };

      // background text block drawing mode
      enum BGDM
      {
        BGDM_OPAQUE = osg::PrimitiveSet::QUADS,
        BGDM_TRANSPARENT = osg::PrimitiveSet::LINE_LOOP
      };

      Skin ( osgText::Font* f,
             const osg::Vec4& tcn=osg::Vec4(1,0,0,1),
             const osg::Vec4& tca=osg::Vec4(0,1,0,1),
             const osg::Vec4& tcd=osg::Vec4(0,1,0,1),
             const osg::Vec4& bcn=osg::Vec4(0,0,0,1), 
             const osg::Vec4& bca=osg::Vec4(1,1,1,1), 
             const osg::Vec4& bcd=osg::Vec4(0.5,0.5,0.5,1), 
             float h=1, float w=0, float m=0.4f, float ics=1.0f,
             BGDM bgdm=BGDM_OPAQUE);
      Skin(const Skin&);
      Skin& operator= (const Skin&);

      

      virtual osg::Node* create(const MenuKit::Item*);
      virtual osg::Node* create_button(const MenuKit::Button*);
      virtual osg::Node* create_menu(const MenuKit::Menu*);
      virtual osg::Node* create_separator(const MenuKit::Item*);

      // returns the graphical height/width for the skin of the Item
      float graphic_height(const Item*);
      float graphic_width(const Item*);

      template<class ItemType>
      class CompareItemsPredicate : public std::binary_function<ItemType,ItemType,bool>
      {
      public:
        CompareItemsPredicate(Skin* s): _mskin(s) {}
        bool operator () ( const ItemType &item1, const ItemType &item2 ) const
        {
          return ( _mskin->graphic_width(item1.get()) < _mskin->graphic_width(item2.get()) );
        }

        Skin* _mskin;
      };

      float find_max_width(const Menu::Items& items);

      void height(float h) { _height = h; }
      virtual float height() const { return _height; }

      void height_control(float h) { _height=h;  _icon_space=h; _margin=0.4*h; }

      void margin(float m) { _margin = m; }
      virtual float margin() const { return _margin; }

      virtual void width(float w) { _width = w; }
      float width() const { return _width; }

      void bg_color_disabled(const osg::Vec4& c) { _bg_color_disabled = c; }
      const osg::Vec4& bg_color_disabled() const { return _bg_color_disabled; }

      void bg_color_highlight(const osg::Vec4& c) { _bg_color_highlight = c; }
      const osg::Vec4& bg_color_highlight() const { return _bg_color_highlight; }

      void bg_color_normal(const osg::Vec4& c) { _bg_color_normal = c; }
      const osg::Vec4& bg_color_normal() const { return _bg_color_normal; }

      void bg_draw_mode(BGDM m) { _bg_draw_mode = m; }
      BGDM bg_draw_mode() const { return _bg_draw_mode; }

      void text_color_disabled(const osg::Vec4& c) { _text_color_disabled = c; }
      const osg::Vec4& text_color_disabled() const { return _text_color_disabled; }

      void text_color_highlight(const osg::Vec4& c) { _text_color_highlight = c; }
      const osg::Vec4& text_color_highlight() const { return _text_color_highlight; }

      void text_color_normal(const osg::Vec4& c) { _text_color_normal = c; }
      const osg::Vec4& text_color_normal() const { return _text_color_normal; }

      void icon_space(float t) { _icon_space = t; }
      float icon_space() const { return _icon_space; }

      void font ( osgText::Font *f ) { _font = f; }
      const osgText::Font *font() const { return _font.get(); }

      // Set/get the mode.
      Mode mode() const { return _mode; }
      void mode ( Mode m ) { _mode = m; }

    protected:
      Skin();
      virtual ~Skin() {}
      const osg::Vec4 &_properTextColor();
      const osg::Vec4 &_properBackgroundColor();

    private:
      osg::ref_ptr<osgText::Font> _font;
      osg::Vec4 _text_color_normal;
      osg::Vec4 _text_color_highlight;
      osg::Vec4 _text_color_disabled;
      osg::Vec4 _bg_color_normal;
      osg::Vec4 _bg_color_highlight;
      osg::Vec4 _bg_color_disabled;
      float _height;
      float _width;
      float _margin;
      float _icon_space;
      Mode _mode;
      BGDM _bg_draw_mode;
    };

  };

};

#endif
