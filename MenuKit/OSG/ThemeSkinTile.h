//
// Author: John K. Grant
// All rights reserved.
//

#ifndef _menukit_osg_skintile_h_
#define _menukit_osg_skintile_h_

#include "MenuKit/OSG/ThemeTile.h"      // base class

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
    template<typename SkinType>
    class ThemeSkinTile : public ThemeTile<typename SkinType::theme_type>
    {
    public:
      typedef ThemeTile<typename SkinType::theme_type> base_class;
      typedef SkinType skin_type;
      typedef typename skin_type::Ptr skin_ptr;
      typedef ThemeSkinTile<SkinType> thisclass;
      MENUKIT_DECLARE_POINTER ( thisclass );

      ThemeSkinTile(): base_class(), _skin(0x0) {}

      virtual float height(const Menu&);
      virtual float height(const Button&);
      virtual float height(const Item*);

      virtual float width(const Menu&);
      virtual float width(const Button&);
      virtual float width(const Item* itm);
      // TODO: virtual float width(const Item&);

      virtual osg::Node* operator() (const Menu&);
      virtual osg::Node* operator() (const Button&);
      // TODO: virtual osg::Node* operator() (const Item&);

      void skin(skin_type* s)       { _skin = s; }
      skin_type* skin()             { return _skin.get(); }
      const skin_type* skin() const { return _skin.get(); }

    protected:
      virtual ~ThemeSkinTile() {}

    private:
      ///\todo TODO: evaluate if this class is copyable
      // not implemented by design
      ThemeSkinTile(const ThemeSkinTile& tt);//:
      //  base_class(tt), _skin(tt._skin)
      //{}

      ThemeSkinTile& operator =(const ThemeSkinTile& tt);
      //{
      //  base_class::operator=(tt);
      //  _skin = tt._skin;
      //  return( *this );
      //}

      skin_ptr _skin;
    };

  };

};

using namespace MenuKit;
using namespace OSG;

template<typename T>
osg::Node* ThemeSkinTile<T>::operator ()(const Menu& m)
{
  // configure the skin
  _skin->theme( _proper_theme() );
  _skin->box( this->box() );

  // produce the graphic
  return( _skin->operator ()(m) );
}

template<typename T>
osg::Node* ThemeSkinTile<T>::operator ()(const Button& b)
{
  // configure the skin
  _skin->theme( _proper_theme() );
  _skin->box( this->box() );

  // produce the graphic
  return( _skin->operator ()(b) );
}

template<typename T>
float ThemeSkinTile<T>::height(const Menu& m)
{
  return( _skin->height(m) );
}

template<typename T>
float ThemeSkinTile<T>::height(const Button& b)
{
  return( _skin->height(b) );
}

template<typename T>
float ThemeSkinTile<T>::height(const Item* i)
{
  return( _skin->height(i) );
}

template<typename T>
float ThemeSkinTile<T>::width(const Menu& m)
{
  _skin->box().height( this->box().height() );
  return( _skin->width(m) );
}

template<typename T>
float ThemeSkinTile<T>::width(const Button& b)
{
  _skin->box().height( this->box().height() );
  return( _skin->width(b) );
}

template<typename T>
float ThemeSkinTile<T>::width(const Item* i)
{
  _skin->box().height( this->box().height() );
  return( _skin->width(i) );
}

#endif
