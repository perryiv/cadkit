//
// Author: John K. Grant
// All rights reserved.
//

#ifndef _menukit_osg_tilefunctor_h_
#define _menukit_osg_tilefunctor_h_

#include "../Box.h"
#include "../Menu.h"
#include "../Button.h"

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
    class TileFunctor
    {
    public:
      enum DisplayMode { DISABLED, HIGHLIGHT, NORMAL };

      TileFunctor(): _box(1.0,0.0), _mode(NORMAL)
      {}

      TileFunctor(const TileFunctor& b): _box(b._box), _mode(b._mode)
      {}

      virtual ~TileFunctor()
      {}

      TileFunctor& operator = (const TileFunctor& btf)
      {
        _box = btf._box;
        _mode = btf._mode;
        return( *this );
      }

      virtual float height(const Menu&)=0;
      virtual float height(const Button&)=0;

      virtual float width(const Menu&)=0;
      virtual float width(const Button&)=0;
      virtual float width(const Item*)=0;
      // TODO: virtual static float width(Item&)=0;

      virtual osg::Node* operator() (const Menu&)=0;
      virtual osg::Node* operator() (const Button&)=0;
      // TODO: virtual osg::Node* operator() (Item& item)=0;

      void box(const Detail::Box& b) { _box=b; }
      const Detail::Box& box() const { return _box; }
      Detail::Box& box() { return _box; }

      void mode(DisplayMode d) { _mode = d; }
      DisplayMode mode() const { return _mode; }

    private:
      Detail::Box _box;
      DisplayMode _mode;
    };

  };

};

#endif
