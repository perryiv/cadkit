
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __QT_TOOLS_COLOR_BUTTON_H__
#define __QT_TOOLS_COLOR_BUTTON_H__

#include "QtTools/Export.h"

#include "QtGui/QColor"
#include "QtGui/QPushButton"

class QWidget;

namespace QtTools {


class QT_TOOLS_EXPORT ColorButton : public QPushButton
{
  Q_OBJECT;
public:
  typedef QPushButton BaseClass;

  ColorButton ( QWidget* parent = 0x0 );
  virtual ~ColorButton();

  /// Get/Set the color.
  void                   color ( const QColor& c );
  QColor                 color () const;

signals:
  void colorChanged ();
  void colorChangedF ( float, float, float, float );

protected:
  virtual void paintEvent( QPaintEvent *e );

protected slots:
  void _buttonClicked ();

private:
  QColor _color;
};


}

#endif // __QT_TOOLS_COLOR_BUTTON_H__
