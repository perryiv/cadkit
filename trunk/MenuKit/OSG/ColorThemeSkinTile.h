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

    template<class ThemeType,class SkinType>
    class SkinTile : public TileFunctor
    {
    public:
      typedef TileFunctor BaseClass;

      enum DisplayMode { DISABLED, HIGHLIGHT, NORMAL };

      SkinTile(): BaseClass(), _skin(),
        _disabled(), _highlight(), _normal(), _mode(NORMAL)
      {}

      SkinTile(const SkinTile& tt):
        BaseClass(tt), _skin(tt._skin),
        _disabled(tt._disabled), _highlight(tt._highlight),
        _normal(tt._normal), _mode(tt._mode)
      {}

      SkinTile& operator =(const SkinTile& tt)
      {
        BaseClass::operator=(tt);
        _skin = tt._skin;
        _disabled = tt._disabled;
        _highlight = tt._highlight;
        _normal = tt._normal;
        _mode = tt._mode;
        return( *this );
      }

      ~SkinTile() {}

      // asks the skin to compute the width
      // of the resulting graphic
      virtual float width(const Menu&);
      virtual float width(const Button&);
      // TODO: virtual float width(const Item&);

      // asks the skin to generate the graphic
      virtual osg::Node* operator() (const Menu&);
      virtual osg::Node* operator() (const Button&);
      // TODO: virtual osg::Node* operator() (const Item&);

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

      void skin(const SkinType& s) { _skin = s; }
      const SkinType& skin() const { return _skin; }
      SkinType& skin() { return _skin; }

    protected:

    private:
      SkinType _skin;
      ThemeType _disabled, _highlight, _normal;
      DisplayMode _mode;
    };

    typedef SkinTile<osgColor,osgSkin> osgTile;
  };

};

using namespace MenuKit;
using namespace OSG;

template<class ThemeType, class SkinType>
const ThemeType& SkinTile<ThemeType,SkinType>::proper_theme()
{
  switch( _mode )
  {
  case DISABLED:  return(_disabled); break;
  case HIGHLIGHT: return(_highlight); break;
  case NORMAL:    return(_normal); break;
  default:        return(_normal); break;
  }
}

template<class ThemeType, class SkinType>
osg::Node* SkinTile<ThemeType,SkinType>::operator ()(const Menu& m)
{
  // configure the skin
  _skin.theme( this->proper_theme() );
  _skin.box().height( this->box().height() );
  _skin.box().width( this->box().width() );

  // produce the graphic
  osg::ref_ptr<osg::Node> node = _skin(m);
  return node.release();
}

template<class ThemeType, class SkinType>
osg::Node* SkinTile<ThemeType,SkinType>::operator ()(const Button& b)
{
  // configure the skin
  _skin.theme( proper_theme() );
  _skin.box().height( this->box().height() );
  _skin.box().width( this->box().width() );

  // produce the graphic
  osg::ref_ptr<osg::Node> node = _skin(b);
  return node.release();
}

template<class ThemeType, class SkinType>
float SkinTile<ThemeType,SkinType>::width(const Menu& m)
{
  _skin.box().height( this->box().height() );
  return( _skin.width(m) );
}

template<class ThemeType, class SkinType>
float SkinTile<ThemeType,SkinType>::width(const Button& b)
{
  _skin.box().height( this->box().height() );
  return( _skin.width(b) );
}

#endif
