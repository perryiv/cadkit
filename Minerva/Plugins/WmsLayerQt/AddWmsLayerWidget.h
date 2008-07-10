
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __ADD_WMS_LAYER_WIDGET_H__
#define __ADD_WMS_LAYER_WIDGET_H__

#include "Minerva/Plugins/WmsLayerQt/CompileGuard.h"
#include "ui_WmsWidget.h"

#include "Minerva/Core/Layers/RasterLayerWms.h"

#include "QtGui/QWidget"

#include <vector>
#include <string>

class QLineEdit;
class QButtonGroup;
class QTreeWidgetItem;
class QStringListModel;

namespace Usul { namespace Interfaces { struct IUnknown; } }

class AddWmsLayerWidget : public QWidget,
                          private Ui::WmsWidget
{
  Q_OBJECT;
public:
  typedef QWidget BaseClass;

  AddWmsLayerWidget( QWidget *parent = 0x0 );
  virtual ~AddWmsLayerWidget();

  void apply ( Usul::Interfaces::IUnknown* parent, Usul::Interfaces::IUnknown * caller );
  
signals:
  void serverValid ( bool );

protected slots:
  void on_browseDirectory_clicked();
  void on_capabilitiesButton_clicked();
  void on_viewOptionsButton_clicked();
  
  void _onServerTextChanged ( const QString& text );

private:
  typedef Minerva::Core::Layers::RasterLayer Layer;
  typedef Minerva::Core::Layers::RasterLayerWms::Extents Extents;
  typedef Minerva::Core::Layers::RasterLayerWms::Options Options;
  typedef QList<QTreeWidgetItem *> Items;
  
  void   _getCapabilities();
  Layer* _makeGroup ( const Items& items, const std::string& format ) const;
  Layer* _makeLayer ( const Extents& e, const std::string& format, const std::string& layers, const std::string& styles ) const;
  
  QButtonGroup *_imageTypes;
  QStringListModel *_recentServers;
  Options _options;
};


#endif // __ADD_WMS_LAYER_WIDGET_H__
