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
        _text(), _middle(), _back(), _border(), _special(), _specialbg()
      {}

      ColorTheme(const ColorTheme& bt):
        _text(bt._text), _middle(bt._middle), _back(bt._back), _border(bt._border),
          _special(bt.special), _specialbg(bt._specialbg)
      {}

      ColorTheme(const ColorType& t, const ColorType& m, const ColorType& bak,
                 const ColorType& bor, const ColorType& sp, const ColorType& spbg):
        _text(t), _middle(m), _back(bak), _border(bor), _special(sp), _specialbg(spbg)
      {}

      virtual ~ColorTheme()
      {}

      ColorTheme& operator= (const ColorTheme& bt)
      {
        _text = bt._text;
        _middle = bt._middle;
        _back = bt._back;
        _border = bt._border;
        _special = bt._special;
        _specialbg = bt._specialbg;
        return( *this );
      }

      void back(const ColorType& c) { _back = c; }
      const ColorType& back() const { return _back; }

      void text(const ColorType& c) { _text = c; }
      const ColorType& text() const { return _text; }

      void middle(const ColorType& c) { _middle = c; }
      const ColorType& middle() const { return _middle; }

      void border(const ColorType& c) { _border = c; }
      const ColorType& border() const { return _border; }

      void special(const ColorType& c) { _special = c; }
      const ColorType& special() const { return _special; }

      void special_back(const ColorType& c) { _specialbg = c; }
      const ColorType& special_back() const { return _specialbg; }

    private:
      ColorType _text, _middle, _back, _border, _special, _specialbg;
    };

    typedef ColorTheme<osg::Vec4> osgColor;
  };

};

#endif
