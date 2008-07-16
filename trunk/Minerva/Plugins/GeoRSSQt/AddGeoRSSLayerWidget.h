
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __ADD_GEO_RSS_LAYER_WIDGET_H__
#define __ADD_GEO_RSS_LAYER_WIDGET_H__

#include "Minerva/Plugins/GeoRSSQt/CompileGuard.h"
#include "ui_AddGeoRSSLayerWidget.h"

#include "Usul/Interfaces/IUnknown.h"

#include "QtGui/QWidget"

class AddGeoRSSLayerWidget : public QWidget,
                             private Ui::AddGeoRSSLayerWidget
{
  Q_OBJECT;
public:
  typedef QWidget BaseClass;

  AddGeoRSSLayerWidget( Usul::Interfaces::IUnknown* caller = 0x0, QWidget *parent = 0x0 );
  virtual ~AddGeoRSSLayerWidget();

  void apply ( Usul::Interfaces::IUnknown* parent, Usul::Interfaces::IUnknown * caller );

private:
  Usul::Interfaces::IUnknown::QueryPtr _caller;
};


#endif // __ADD_GEO_RSS_LAYER_WIDGET_H__
