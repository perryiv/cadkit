#ifndef _menukit_osg_gfx_functor_
#define _menukit_osg_gfx_functor_

#include "../Referenced.h"

#include "Export.h"
#include "Skin.h"

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
    class MENUKIT_OSG_EXPORT GFX : public Referenced
    {
    public:
      typedef Referenced BaseClass;
      GFX ( Skin *skin, Item *active );
      GFX ( const GFX & );
      GFX& operator = (const GFX&);

      MENUKIT_DECLARE_POINTER(GFX);

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
      Skin::Ptr _skin;
      Item::Ptr _active;
    };

  };

};

#endif
