//
// Author: John K. Grant
// All rights reserved.
//

#ifndef _menukit_osg_theme_h_
#define _menukit_osg_theme_h_

#include "osg/Vec4"

namespace MenuKit
{

  namespace OSG
  {

    template<class ColorType>
    class Theme
    {
    public:
      Theme():
        _front( ColorType(0.0,0.0,0.0,1.0)/*BLACK*/),
        _back( ColorType(1.0,1.0,1.0,1.0)/*WHITE*/) {}

      Theme(const Theme& bt):
        _front(bt._front),
        _back(bt._back) {}

      virtual ~Theme() {}

      Theme& operator= (const Theme& bt)
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

    typedef Theme<osg::Vec4> osgColor;
  };

};

#endif
