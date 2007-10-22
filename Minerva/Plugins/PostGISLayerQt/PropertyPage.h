
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __POSTGIS_COLOR_PAGE_H__
#define __POSTGIS_COLOR_PAGE_H__

#include "Minerva/Plugins/PostGISLayerQt/ui_LayerProperties.h"

#include "Minerva/Core/DB/Connection.h"
#include "Minerva/Core/Layers/Layer.h"

#include "QtTools/ColorButton.h"

#include "QtGui/QWidget"

class AddPostGISLayerWidget;

class PropertyPage : public QWidget,
                     private Ui::LayerProperties
{
  Q_OBJECT;
public:
  typedef QWidget BaseClass;
  typedef Minerva::Core::DB::Connection Connection;
  typedef Minerva::Core::Layers::Layer  Layer;

  PropertyPage ( Layer *layer, QWidget *parent = 0x0 );
  ~PropertyPage ();

protected slots:
  void _colorTypeChanged ( int index );
  void _tabChanged ( int index );

private:
  Layer::RefPtr _layer;
  QWidget *_colorWidget;

};


#endif // __POSTGIS_COLOR_PAGE_H__
