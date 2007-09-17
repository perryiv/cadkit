
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, John K. Grant.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _menukit_osg_themeskin_h_
#define _menukit_osg_themeskin_h_

#include "MenuKit/Menu.h"     // for request support
#include "MenuKit/Button.h"   // for request support
#include "MenuKit/OSG/TileFunctor.h" // for map typedef

#include "Usul/Base/Object.h"

#include "osgText/Font"

namespace osg
{
  class Node;
};

namespace MenuKit
{

  namespace OSG
  {
    /** @class ThemSkin
      * 
      * This is an abstract base class for implementing classes
      * that want to take advantage of a Tile's 'DisplayMode' member.
      *
      * Imlementing classes will need to manufacture graphic
      * objects.  Derived classes will most likey use the
      * MapType member for color designation.
      */
    template<typename ThemeType>
    class ThemeSkin : public Usul::Base::Object
    {
      /**\todo TODO: have skins support their own theme maps.
        * thus taking the pressure off of a Tile to maintain
        * a theme and making the Mason configure such a theme.
        * Also, it just makes more sense to have themes be a
        * feature of a skin.
        * The Mode concept found in Tiles should be inherit
        * to the Skin's base class which will define an
        * interface with which the Tile will operate.
        * Therefore, this class should inherit from a Skin class.
        * This is a medium priority TODO.
        */
    public:
      typedef Usul::Base::Object base_class;

      typedef ThemeType theme_type;
      typedef ThemeSkin<theme_type> thisclass;
      
      USUL_DECLARE_REF_POINTERS ( thisclass );

      typedef Tile tile_type;
      typedef std::map<tile_type::DisplayMode,theme_type> ModeMap;

      ThemeSkin(ModeMap m=ModeMap()): base_class(), _font(new osgText::Font()), _modemap(m), _mode(Tile::NORMAL), _letter_height(0.75f), _separator_height(0.2f), _graphic_width(0.0f) {}

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

      void font(osgText::Font* f)       { _font = f; }
      osgText::Font* font()             { return _font.get(); }
      const osgText::Font* font() const { return _font.get(); }

      void graphic_width(float w) { _graphic_width = w; }
      float graphic_width() const { return _graphic_width; }

      void letter_height(float h) { _letter_height = h; }
      float letter_height() const { return _letter_height; }

      void separator_height(float w) { _separator_height = w; }
      float separator_height() const { return _separator_height; }

      void mode_map(const ModeMap& t) { _modemap = t; }
      const ModeMap& mode_map() const { return _modemap; }
      ModeMap& mode_map() { return _modemap; }

      void mode(Tile::DisplayMode m) { _mode=m; }
      Tile::DisplayMode mode() const { return _mode; }

    protected:
      virtual ~ThemeSkin() {}

    private:
      ///\todo TODO: evaluate if this class is copyable

      // not implemented by design
      ThemeSkin(const ThemeSkin& ts);
      ThemeSkin& operator =(const ThemeSkin& ts);

      osg::ref_ptr<osgText::Font> _font;
      ModeMap _modemap;
      Tile::DisplayMode _mode;  // current mode state
      float _letter_height,     // height of the printed-type
            _separator_height,  // height of the separator
            _graphic_width;     // horizontal distance to be honored when rendering
    };

  };

};

#endif
