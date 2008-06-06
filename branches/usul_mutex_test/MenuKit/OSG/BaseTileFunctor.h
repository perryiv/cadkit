
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, John K. Grant.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _menukit_osg_basetilefunctor_h_
#define _menukit_osg_basetilefunctor_h_

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

    class BaseTileFunctor
    {
    public:
      BaseTileFunctor(): _box(0.0,1.0)
      {}

      BaseTileFunctor(const BaseTileFunctor& b): _box(b._box)
      {}

      virtual ~BaseTileFunctor()
      {}

      BaseTileFunctor& operator = (const BaseTileFunctor& btf)
      {
        _box = btf._box;
      }

      virtual float width(const Menu&)=0;
      virtual float width(const Button&)=0;
      // TODO: virtual static float width(Item&)=0;

      virtual osg::Node* operator() (const Menu&)=0;
      virtual osg::Node* operator() (const Button&)=0;
      // TODO: virtual osg::Node* operator() (Item& item)=0;

      void box(const Detail::Box& b) { _box=b; }
      const Detail::Box& box() const { return _box; }
      Detail::Box& box() { return _box; }

    private:
      Detail::Box _box;
    };

  };

};

#endif
