//
// Author: John K. Grant
// All rights reserved.
//

#ifndef _menukit_osg_basetilefunctor_h_
#define _menukit_osg_basetilefunctor_h_

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
      BaseTileFunctor():
        _height(1.0), _width(1.0)
      {}

      BaseTileFunctor(const BaseTileFunctor& b):
        _height(b._height), _width(b._width)
      {}

      virtual ~BaseTileFunctor()
      {}

      BaseTileFunctor& operator = (const BaseTileFunctor& btf)
      {
        _height = btf._height;
        _width = btf._width;
      }

      virtual float calculate_tile_width(Menu&)=0;
      virtual float calculate_tile_width(Button&)=0;
      // TODO: virtual static float calculate_text_width(Item&)=0;

      virtual osg::Node* operator() (Menu&)=0;
      virtual osg::Node* operator() (Button&)=0;
      // TODO: virtual osg::Node* operator() (Item& item)=0;

      void height(float h) { _height = h; }
      float height() const { return _height; }

      void width(float w) { _width = w; }
      float width() const { return _width; }

    private:
      float _height, _width;
    };

  };

};

#endif
