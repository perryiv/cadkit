//
// Author: John K. Grant
// All rights reserved.
//

#ifndef _menukit_osg_skintile_h_
#define _menukit_osg_skintile_h_

#include "MenuKit/OSG/ThemeTile.h"      // base class
#include "MenuKit/OSG/ColorTheme.h"     // for typedef
#include "MenuKit/OSG/VisualThemeSkin.h"      // for typedef

#include "osg/Node"

namespace MenuKit
{

  namespace OSG
  {

    /** ThemeSkinTile
      * The ThemeSkinTile class uses a policy member of type SkinType
      * to help it provide correct data.  A "Tile" needs
      * to supply a "Mason" with size information.  This
      * particular TileFunctor accomplishes that with its
      * 'skin' member. This class also uses the skin for
      * the important job of manufacturing a graphic
      * object for a Menu Item.
      */
    template<class ThemeType, class SkinType>
    class ThemeSkinTile : public ThemeTile<ThemeType>
    {
    public:
      typedef ThemeTile<ThemeType> BaseClass;

      ThemeSkinTile(): BaseClass(), _skin()
      {}

      ThemeSkinTile(const ThemeSkinTile& tt):
        BaseClass(tt), _skin(tt._skin)
      {}

      ThemeSkinTile& operator =(const ThemeSkinTile& tt)
      {
        BaseClass::operator=(tt);
        _skin = tt._skin;
        return( *this );
      }

      virtual ~ThemeSkinTile() {}

      virtual float height(const Menu&);
      virtual float height(const Button&);

      virtual float width(const Menu&);
      virtual float width(const Button&);
      virtual float width(const Item* itm);
      // TODO: virtual float width(const Item&);

      virtual osg::Node* operator() (const Menu&);
      virtual osg::Node* operator() (const Button&);
      // TODO: virtual osg::Node* operator() (const Item&);

      void skin(const SkinType& s) { _skin = s; }
      const SkinType& skin() const { return _skin; }
      SkinType& skin() { return _skin; }

    private:
      SkinType _skin;
    };

    typedef ThemeSkinTile<osgColorTheme,osgVisualSkin> osgVisualSkinTile;
  };

};

using namespace MenuKit;
using namespace OSG;

template<class ThemeType, class SkinType>
osg::Node* ThemeSkinTile<ThemeType,SkinType>::operator ()(const Menu& m)
{
  // configure the skin
  _skin.theme( this->proper_theme() );
  _skin.box( this->box() );

  // TODO: make the graphic NULL, _skin.icon(NULL);

  // produce the graphic
  osg::ref_ptr<osg::Node> node = _skin(m);
  return node.release();
}

template<class ThemeType, class SkinType>
osg::Node* ThemeSkinTile<ThemeType,SkinType>::operator ()(const Button& b)
{
  // configure the skin
  _skin.theme( proper_theme() );
  _skin.box( this->box() );

  // TODO: load a graphic, _skin.load_icon( b.icon_file() );

  // produce the graphic
  osg::ref_ptr<osg::Node> node = _skin(b);
  return node.release();
}

template<class ThemeType, class SkinType>
float ThemeSkinTile<ThemeType,SkinType>::height(const Menu& m)
{
  return( _skin.height(m) );
}

template<class ThemeType, class SkinType>
float ThemeSkinTile<ThemeType,SkinType>::height(const Button& b)
{
  return( _skin.height(b) );
}

template<class ThemeType, class SkinType>
float ThemeSkinTile<ThemeType,SkinType>::width(const Menu& m)
{
  _skin.box().height( this->box().height() );
  return( _skin.width(m) );
}

template<class ThemeType, class SkinType>
float ThemeSkinTile<ThemeType,SkinType>::width(const Button& b)
{
  _skin.box().height( this->box().height() );
  return( _skin.width(b) );
}

template<class ThemeType, class SkinType>
float ThemeSkinTile<ThemeType,SkinType>::width(const Item* i)
{
  _skin.box().height( this->box().height() );
  return( _skin.width(i) );
}

#endif
