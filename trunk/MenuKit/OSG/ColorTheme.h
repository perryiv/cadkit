//
// Author: John K. Grant
// All rights reserved.
//

#ifndef _menukit_osg_colortheme_h_
#define _menukit_osg_colortheme_h_

#include "osg/Vec4"

namespace MenuKit
{

  namespace OSG
  {

    class ColorTheme
    {
    public:
      typedef osg::Vec4 Color;
//      typedef const Color(0.0,0.0,0.0,1.0) BLACK;
//      typedef const Color(1.0,1.0,1.0,1.0) WHITE;
//      typedef const Color(1.0,0.0,0.0,1.0) RED;
//      typedef const Color(0.0,0.0,1.0,1.0) GREEN;
//      typedef const Color(0.0,0.0,1.0,1.0) BLUE;

      ColorTheme():
        _text( osg::Vec4(0.0,0.0,0.0,1.0)/*BLACK*/),
        _background( osg::Vec4(1.0,1.0,1.0,1.0)/*WHITE*/) {}

      ColorTheme(const ColorTheme& ct):
        _text(ct._text),
        _background(ct._background) {}

      virtual ~ColorTheme() {}

      ColorTheme& operator= (const ColorTheme& ct)
      {
        _background = ct._background;
        _text= ct._text;
      }

      void background(const Color& c) { _background = c; }
      const Color& background() const { return _background; }

      void text(const Color& c) { _text = c; }
      const Color& text() const { return _text; }

    private:
      Color _background, _text;
    };

  };

};

#endif
