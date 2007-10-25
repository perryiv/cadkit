
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __QT_TOOLS_COLOR_H__
#define __QT_TOOLS_COLOR_H__

#include "QtGui/QColor"

namespace QtTools
{
  template < class ColorType >
  class Color
  {
  public:
    static void convert ( const ColorType& c, QColor& q )
    {
      q.setRed   ( c[0] * 255 );
      q.setGreen ( c[1] * 255 );
      q.setBlue  ( c[2] * 255 );
      q.setAlpha ( c[3] * 255 );
    }

    static void convert ( const QColor& q, ColorType& c )
    {
      c.set ( q.redF(), q.greenF(), q.blueF(), q.alphaF() );
    }

    static QColor convert ( const ColorType& c )
    {
      QColor q;
      convert ( c, q );
      return q;
    }

    static ColorType convert ( const QColor& q )
    {
      ColorType c;
      convert ( q, c );
      return c;
    }
  };
}


#endif
