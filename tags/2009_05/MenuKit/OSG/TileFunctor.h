
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, John K. Grant.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _menukit_osg_tilefunctor_h_
#define _menukit_osg_tilefunctor_h_

#include "MenuKit/Box.h"
#include "MenuKit/Menu.h"
#include "MenuKit/Button.h"

#include "Usul/Base/Object.h"

namespace osg { class Node; }

namespace MenuKit
{

  namespace OSG
  {

     /** Tile
      *
      * This class is an abstract base class that implements
      * the template required functions for the Mason
      * Visitor class.  Along with the pure virtual functions,
      * the DisplayMode enumeration and Box class members are
      * both template requirements.
      */
    class Tile : public Usul::Base::Object
    {
    public:
      typedef Usul::Base::Object base_class;
      USUL_DECLARE_REF_POINTERS ( Tile );
      enum DisplayMode { NORMAL, DISABLED, HIGHLIGHT };

      ///\todo make node_type a template requirement

      Tile(): _box(1.0,1.0), _mode(DisplayMode::NORMAL) {}

      virtual float height(const Menu&) const=0;
      virtual float height(const Button&) const=0;
      virtual float height(const Item*) const=0;

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

      void mode(DisplayMode d) { _mode = d; }
      DisplayMode mode() const { return _mode; }

    protected:
      virtual ~Tile() {}

    private:
      ///\todo TODO: evaluate if this class is copyable

      // not implemented by design
      Tile(const Tile& b);//: _box(b._box), _mode(b._mode) {}
      Tile& operator = (const Tile& btf);

      Detail::Box _box;
      DisplayMode _mode;
    };

  };

};

#endif
