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

    template<class ColorType>
    class ColorTheme
    {
    public:
      ColorTheme():
        _text(), _middle(), _back(), _special()
      {}

      ColorTheme(const ColorTheme& bt):
        _text(bt._text), _middle(bt._middle), _back(bt._back), _special(bt._special)
      {}

      ColorTheme(const ColorType& f, const ColorType& m, const ColorType& b, const ColorType& s):
        _text(f), _middle(m), _back(b), _special(s)
      {}

      virtual ~ColorTheme()
      {}

      ColorTheme& operator= (const ColorTheme& bt)
      {
        _text = bt._text;
        _middle = bt._middle;
        _back = bt._back;
        _special = bt._special;
        return( *this );
      }

      void back(const ColorType& c) { _back = c; }
      const ColorType& back() const { return _back; }

      void text(const ColorType& c) { _text = c; }
      const ColorType& text() const { return _text; }

      void middle(const ColorType& c) { _middle = c; }
      const ColorType& middle() const { return _middle; }

      void special(const ColorType& c) { _special = c; }
      const ColorType& special() const { return _special; }

    private:
      ColorType _text, _middle, _back, _special;
    };

    typedef ColorTheme<osg::Vec4> osgColor;
  };

};

#endif
