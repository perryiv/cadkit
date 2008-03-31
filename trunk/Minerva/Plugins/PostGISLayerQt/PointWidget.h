
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __POSTGIS_POINT_WIDGET_H__
#define __POSTGIS_POINT_WIDGET_H__

#include "ui_PointWidget.h"
#include "Minerva/Layers/PostGIS/PointLayer.h"

#include "QtGui/QWidget"

class PointWidget : public QWidget,
  private Ui::PointWidget
{
  Q_OBJECT;
public:
  typedef QWidget BaseClass;
  typedef Minerva::Layers::PostGIS::PointLayer PointLayer;

  PointWidget ( PointLayer* layer, QWidget* parent = 0x0 );
  virtual ~PointWidget();

protected slots:
  void _stackPointsChanged ( int state );
  void _autoTransformChanged ( int state );
  void _primitiveTypeChanged ( int index );
  void _sizeColumnChanged ( int index );
  void _sizeChanged ( double value );
  void _secondarySizeChanged ( double value );
  void _qualityChanged ( int value );

private:
  PointLayer::RefPtr _layer;
};


#endif // __POSTGIS_POINT_WIDGET_H__
