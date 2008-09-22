
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __EDIT_WMS_LAYER_WIDGET_H__
#define __EDIT_WMS_LAYER_WIDGET_H__

#include "Minerva/Plugins/WmsLayerQt/CompileGuard.h"
#include "ui_EditWmsLayerWidget.h"

#include "Minerva/Core/Layers/RasterLayerWms.h"

#include "QtGui/QWidget"

class EditWmsLayerWidget : public QWidget,
                           private Ui::EditWmsLayerWidget
{
  Q_OBJECT;
public:
  typedef QWidget BaseClass;
  typedef Minerva::Core::Layers::RasterLayerNetwork RasterLayerNetwork;
  
  EditWmsLayerWidget ( RasterLayerNetwork *layer, QWidget * parent = 0x0 );
  virtual ~EditWmsLayerWidget();
  
protected slots:
  void _nameFinishedEditing();
  void _serverFinishedEditing();
  
  void on_viewOptionsButton_clicked();
  void on_viewAlphasButton_clicked();
  void on_viewCacheButton_clicked();
  void on_deleteCacheButton_clicked();
  
private:
  RasterLayerNetwork::RefPtr _layer;
};


#endif // __EDIT_WMS_LAYER_WIDGET_H__
