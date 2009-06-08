
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __POSTGIS_UNIQUE_COLOR_WIDGET_H__
#define __POSTGIS_UNIQUE_COLOR_WIDGET_H__

#include "Minerva/Layers/PostGIS/Layer.h"

#include "QtGui/QWidget"

namespace QtTools { class ColorButton; }

class UniqueColorWidget : public QWidget
{
  Q_OBJECT;
public:
  typedef QWidget BaseClass;
  typedef Minerva::Layers::PostGIS::Layer  Layer;

  UniqueColorWidget ( Layer *layer, QWidget *parent = 0x0 );

private:
  Layer::RefPtr _layer;
};


#endif // __POSTGIS_UNIQUE_COLOR_WIDGET_H__
