
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _menukit_detail_box_h_
#define _menukit_detail_box_h_

namespace MenuKit
{

  namespace Detail
  {

    class Box
    {
    public:
      Box(): _height(0.0f), _width(0.0f) {}
      Box(float h, float w): _height(h), _width(w) {}
      Box(const Box& b): _height(b._height), _width(b._width) {}
      ~Box() {}
      Box& operator =(const Box& b) { _height=b._height; _width=b._width; return(*this); }

      void height(float h) { _height=h; }
      float height() const { return _height; }

      void width(float w) { _width=w; }
      float width() const { return _width; }

    private:
      float _height, _width;
    };

  };

};

#endif
