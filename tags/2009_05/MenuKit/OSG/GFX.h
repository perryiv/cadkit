
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, John K. Grant.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _menukit_osg_gfx_functor_
#define _menukit_osg_gfx_functor_

#include "Export.h"
#include "Skin.h"

#include "Usul/Base/Object.h"

#include "osg/Vec4"
#include "osgText/Font"

namespace osg
{
  class Node;
};

namespace MenuKit
{
  class Item;
  class Menu;

  namespace OSG
  {

    // abstract base class for all skins
    class MENUKIT_OSG_EXPORT GFX : public Usul::Base::Object
    {
    public:
      typedef Usul::Base::Object BaseClass;
      USUL_DECLARE_REF_POINTERS ( GFX );

      GFX ( Skin *skin, Item *active );
      GFX ( const GFX & );
      GFX& operator = (const GFX&);

      void skin(Skin* s) { _skin = s; }
      const Skin* skin() { return _skin.get(); }

      virtual osg::Node* operator() ( const Item* item );

    protected:
      osg::Vec3 calculate_move(const Menu*,const Item*,const Item*,
                               unsigned int,float,float,float&,float&);
      virtual osg::Node* traverse(const MenuKit::Menu*,float);
      GFX();
      virtual ~GFX() {}

      void _setSkinMode ( const Item *item );
      bool _isInActivePath ( const Item *item );

    private:
      Skin::RefPtr _skin;
      Item::RefPtr _active;
    };

  };

};

#endif
