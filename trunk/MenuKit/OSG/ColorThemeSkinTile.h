//
// Author: John K. Grant
// All rights reserved.
//

#ifndef _menukit_osg_skintile_h_
#define _menukit_osg_skintile_h_

#include "TileFunctor.h"      // base class
#include "ColorTheme.h"       // for typedef
#include "ColorThemeSkin.h"   // for typedef

#include "osg/Node"

namespace MenuKit
{

  namespace OSG
  {

    /** ColorThemeSkinTile
      * This class use's a policy member of type SkinType
      * to help it provide correct data.  A "Tile" needs
      * to supply a "Mason" with size information.  This
      * particular tile accomplishes that with its 'skin' member.
      * This class also uses the skin for the important job
      * of manufacturing a graphic object for a Menu Item.
      */
    template<class ThemeType, class SkinType>
    class ColorThemeSkinTile : public TileFunctor
    {
    public:
      typedef TileFunctor BaseClass;

      ColorThemeSkinTile(): BaseClass(), _skin(),
        _disabled(), _highlight(), _normal()
      {}

      ColorThemeSkinTile(const ColorThemeSkinTile& tt):
        BaseClass(tt), _skin(tt._skin),
        _disabled(tt._disabled), _highlight(tt._highlight),
        _normal(tt._normal)
      {}

      ColorThemeSkinTile& operator =(const ColorThemeSkinTile& tt)
      {
        BaseClass::operator=(tt);
        _skin = tt._skin;
        _disabled = tt._disabled;
        _highlight = tt._highlight;
        _normal = tt._normal;
        return( *this );
      }

      ~ColorThemeSkinTile() {}

      // asks the skin to compute the width
      // of the resulting graphic
      virtual float width(const Menu&);
      virtual float width(const Button&);
      // TODO: virtual float width(const Item&);

      // asks the skin to generate the graphic
      virtual osg::Node* operator() (const Menu&);
      virtual osg::Node* operator() (const Button&);
      // TODO: virtual osg::Node* operator() (const Item&);

      void skin(const SkinType& s) { _skin = s; }
      const SkinType& skin() const { return _skin; }
      SkinType& skin() { return _skin; }

      // theme information
      const ThemeType& proper_theme();

      void disabled(const ThemeType& n) { _disabled = n; }
      const ThemeType& disabled() const { return _disabled; }
      ThemeType& disabled() { return _disabled; }

      void highlight(const ThemeType& n) { _highlight = n; }
      const ThemeType& highlight() const { return _highlight; }
      ThemeType& highlight() { return _highlight; }

      void normal(const ThemeType& n) { _normal = n; }
      const ThemeType& normal() const { return _normal; }
      ThemeType& normal() { return _normal; }

    protected:

    private:
      SkinType _skin;
      ThemeType _disabled, _highlight, _normal;
    };

    typedef ColorThemeSkinTile<osgColor,osgSkin> osgTile;
  };

};

using namespace MenuKit;
using namespace OSG;

template<class ThemeType, class SkinType>
const ThemeType& ColorThemeSkinTile<ThemeType,SkinType>::proper_theme()
{
  switch( this->mode() )
  {
  case DISABLED:  return(_disabled); break;
  case HIGHLIGHT: return(_highlight); break;
  case NORMAL:    return(_normal); break;
  default:        return(_normal); break;
  }
}

template<class ThemeType, class SkinType>
osg::Node* ColorThemeSkinTile<ThemeType,SkinType>::operator ()(const Menu& m)
{
  // configure the skin
  _skin.theme( this->proper_theme() );
  _skin.box( this->box() );

  // produce the graphic
  osg::ref_ptr<osg::Node> node = _skin(m);
  return node.release();
}

template<class ThemeType, class SkinType>
osg::Node* ColorThemeSkinTile<ThemeType,SkinType>::operator ()(const Button& b)
{
  // configure the skin
  _skin.theme( proper_theme() );
  _skin.box( this->box() );

  // produce the graphic
  osg::ref_ptr<osg::Node> node = _skin(b);
  return node.release();
}

template<class ThemeType, class SkinType>
float ColorThemeSkinTile<ThemeType,SkinType>::width(const Menu& m)
{
  _skin.box().height( this->box().height() );
  return( _skin.width(m) );
}

template<class ThemeType, class SkinType>
float ColorThemeSkinTile<ThemeType,SkinType>::width(const Button& b)
{
  _skin.box().height( this->box().height() );
  return( _skin.width(b) );
}

#endif
