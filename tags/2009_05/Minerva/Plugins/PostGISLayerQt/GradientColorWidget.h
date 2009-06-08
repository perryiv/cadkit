
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __POSTGIS_GRADIENT_COLOR_WIDGET_H__
#define __POSTGIS_GRADIENT_COLOR_WIDGET_H__

#include "Minerva/Layers/PostGIS/Layer.h"
#include "Minerva/Core/Functors/GradientColorFunctor.h"

#include "QtGui/QWidget"

namespace QtTools { class ColorButton; }

class QComboBox;

class GradientColorWidget : public QWidget
{
  Q_OBJECT;
public:
  typedef QWidget BaseClass;
  typedef Minerva::Layers::PostGIS::Layer  Layer;
  typedef Minerva::Core::Functors::GradientColorFunctor GradientColor;

  GradientColorWidget ( Layer *layer, QWidget *parent = 0x0 );
  virtual ~GradientColorWidget();

protected slots:
  void _minColorChanged ();
  void _maxColorChanged ();
  void _columnChanged ( int );

private:
  Layer::RefPtr _layer;
  GradientColor::RefPtr _gradientColor;
  QtTools::ColorButton *_minColorButton;
  QtTools::ColorButton *_maxColorButton;
  QComboBox *_columns;
};


#endif // __POSTGIS_GRADIENT_COLOR_WIDGET_H__
