//
// Author: John K. Grant
// All rights reserved.
//

#ifndef _menukit_osg_colortheme_h_
#define _menukit_osg_colortheme_h_

#include "osg/Vec4"

// TODO: rename this file to Theme.h
namespace MenuKit
{

  namespace OSG
  {

    template<class ColorType>
    class ColorTheme
    {
    public:
      ColorTheme():
        _front( ColorType(0.0,0.0,0.0,1.0)/*BLACK*/),
        _back( ColorType(1.0,1.0,1.0,1.0)/*WHITE*/)
      {}

      ColorTheme(const ColorTheme& bt):
        _front(bt._front),
        _back(bt._back)
      {}

      ColorTheme(const ColorType& f, const ColorType& b):
        _front(f), _back(b)
      {}

      virtual ~ColorTheme()
      {}

      ColorTheme& operator= (const ColorTheme& bt)
      {
        _back = bt._back;
        _front = bt._front;
        return( *this );
      }

      void back(const ColorType& c) { _back = c; }
      const ColorType& backg() const { return _back; }

      void front(const ColorType& c) { _front = c; }
      const ColorType& front() const { return _front; }

    private:
      ColorType _back, _front;
    };

    typedef ColorTheme<osg::Vec4> osgColor;
  };

};

#endif
