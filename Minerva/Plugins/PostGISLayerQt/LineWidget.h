
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __POSTGIS_LINE_WIDGET_H__
#define __POSTGIS_LINE_WIDGET_H__

#include "Minerva/Plugins/PostGISLayerQt/ui_LineWidget.h"
#include "Minerva/Core/Layers/LineLayer.h"

#include "QtGui/QWidget"

class LineWidget : public QWidget,
                   private Ui::LineWidget
{
  Q_OBJECT;
public:
  typedef QWidget BaseClass;
  typedef Minerva::Core::Layers::LineLayer LineLayer;

  LineWidget ( LineLayer* layer, QWidget* parent = 0x0 );

protected slots:
  void _lineWidthChanged( double value );

private:
  LineLayer::RefPtr _layer;
};

#endif // __POSTGIS_LINE_WIDGET_H__
