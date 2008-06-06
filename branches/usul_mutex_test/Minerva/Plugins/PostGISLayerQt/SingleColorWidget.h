
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __POSTGIS_SINGLE_COLOR_WIDGET_H__
#define __POSTGIS_SINGLE_COLOR_WIDGET_H__

#include "Minerva/Layers/PostGIS/Layer.h"
#include "Minerva/Core/Functors/SingleColorFunctor.h"

#include "QtGui/QWidget"

namespace QtTools { class ColorButton; }

class SingleColorWidget : public QWidget
{
  Q_OBJECT;
public:
  typedef QWidget BaseClass;
  typedef Minerva::Layers::PostGIS::Layer  Layer;

  SingleColorWidget ( Layer *layer, QWidget *parent = 0x0 );
  virtual ~SingleColorWidget();

protected slots:
  void _colorChanged ();

private:
  Layer::RefPtr _layer;
  QtTools::ColorButton *_colorButton;
  Minerva::Core::Functors::SingleColorFunctor::RefPtr _functor;
};


#endif // __POSTGIS_SINGLE_COLOR_WIDGET_H__
