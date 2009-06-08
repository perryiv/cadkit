
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, John K. Grant.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _menukit_osg_skintile_h_
#define _menukit_osg_skintile_h_

#include "MenuKit/OSG/TileFunctor.h"      // base class

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
    class SkinTile : public Tile
    {
    public:
      typedef Tile base_class;
      typedef SkinType skin_type;
      typedef typename skin_type::RefPtr skin_ptr;
      typedef SkinTile<skin_type> thisclass;
      MENUKIT_DECLARE_POINTER ( thisclass );

      ///\todo require skin_type::node_type

      SkinTile(): base_class(), _skin(0x0) {}

      virtual float height(const Menu&) const;
      virtual float height(const Button&) const;
      virtual float height(const Item*) const;

      virtual float width(const Menu&) const;
      virtual float width(const Button&) const;
      virtual float width(const Item* itm) const;
      // TODO: virtual float width(const Item&);

      virtual osg::Node* operator() (const Menu&);
      virtual osg::Node* operator() (const Button&);
      // TODO: virtual osg::Node* operator() (const Item&);

      void skin(skin_type* s)       { _skin = s; }
      skin_type* skin()             { return _skin.get(); }
      const skin_type* skin() const { return _skin.get(); }

    protected:
      virtual ~SkinTile() {}

    private:
      ///\todo TODO: evaluate if this class is copyable

      // not implemented by design
      SkinTile(const SkinTile& tt);//: base_class(tt), _skin(tt._skin) {}

      SkinTile& operator =(const SkinTile& tt);
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
osg::Node* SkinTile<T>::operator ()(const Menu& m)
{
  // configure the skin
  _skin->mode( this->mode() );
  _skin->graphic_width( this->box().width() );

  // produce the graphic
  return( _skin->operator ()(m) );
}

template<typename T>
osg::Node* SkinTile<T>::operator ()(const Button& b)
{
  // configure the skin
  _skin->mode( this->mode() );
  _skin->graphic_width( this->box().width() );

  // produce the graphic
  return( _skin->operator ()(b) );
}

template<typename T>
float SkinTile<T>::height(const Menu& m) const
{
  return( _skin->height(m) );
}

template<typename T>
float SkinTile<T>::height(const Button& b) const
{
  return( _skin->height(b) );
}

template<typename T>
float SkinTile<T>::height(const Item* i) const
{
  return( _skin->height(i) );
}

template<typename T>
float SkinTile<T>::width(const Menu& m) const
{
  return( _skin->width(m) );
}

template<typename T>
float SkinTile<T>::width(const Button& b) const
{
  return( _skin->width(b) );
}

template<typename T>
float SkinTile<T>::width(const Item* i) const
{
  return( _skin->width(i) );
}

#endif
