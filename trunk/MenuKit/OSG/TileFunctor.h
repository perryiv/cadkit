//
// Author: John K. Grant
// All rights reserved.
//

#ifndef _menukit_osg_tilefunctor_h_
#define _menukit_osg_tilefunctor_h_

#include "MenuKit/Referenced.h"  // for base class
#include "MenuKit/Box.h"
#include "MenuKit/Menu.h"
#include "MenuKit/Button.h"

namespace osg
{
  class Node;
};

namespace MenuKit
{

  namespace OSG
  {

     /** TileFunctor
      *
      * This class is an abstract base class that implements
      * the template required functions for the TileMason
      * Visitor class.  Along with the pure virtual functions,
      * the DisplayMode enumeration and Box class members are
      * both template requirements.
      */
    class TileFunctor : public Referenced
    {
    public:
      typedef Referenced base_class;
      MENUKIT_DECLARE_POINTER( TileFunctor );
      enum display_mode { DISABLED, NORMAL, HIGHLIGHT };

      TileFunctor(): _box(1.0,1.0), _mode(display_mode::NORMAL) {}

      virtual float height(const Menu&) const=0;
      virtual float height(const Button&) const=0;

      virtual float width(const Menu&) const=0;
      virtual float width(const Button&) const=0;
      virtual float width(const Item*) const=0;
      // TODO: virtual static float width(Item&)=0;

      virtual osg::Node* operator() (const Menu&)=0;
      virtual osg::Node* operator() (const Button&)=0;
      // TODO: virtual osg::Node* operator() (Item& item)=0;

      void box(const Detail::Box& b) { _box=b; }
      const Detail::Box& box() const { return _box; }
      Detail::Box& box() { return _box; }

      void mode(display_mode d) { _mode = d; }
      display_mode mode() const { return _mode; }

    protected:
      virtual ~TileFunctor() {}

    private:
      ///\todo TODO: evaluate if this class is copyable
      // not implemented by design
      TileFunctor(const TileFunctor& b);//: _box(b._box), _mode(b._mode) {}

      TileFunctor& operator = (const TileFunctor& btf);
      //{
      //  _box = btf._box;
      //  _mode = btf._mode;
      //  return( *this );
      //}

      Detail::Box _box;
      display_mode _mode;
    };

  };

};

#endif
