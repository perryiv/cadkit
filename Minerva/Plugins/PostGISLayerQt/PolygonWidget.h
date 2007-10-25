
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __POSTGIS_POLYGON_WIDGET_H__
#define __POSTGIS_POLYGON_WIDGET_H__

#include "Minerva/Plugins/PostGISLayerQt/ui_PolygonWidget.h"
#include "Minerva/Core/Layers/PolygonLayer.h"

#include "QtGui/QWidget"

class PolygonWidget : public QWidget,
  private Ui::PolygonWidget
{
  Q_OBJECT;
public:
  typedef QWidget BaseClass;
  typedef Minerva::Core::Layers::PolygonLayer PolygonLayer;

  PolygonWidget ( PolygonLayer* layer, QWidget* parent = 0x0 );

protected slots:
  void _drawBorderChanged ( int state );
  void _drawInteriorChanged ( int state );
  void _borderWidthChanged ( double value );
  void _borderColorChanged ();

private:
  PolygonLayer::RefPtr _layer;
};


#endif // __POSTGIS_POLYGON_WIDGET_H__
