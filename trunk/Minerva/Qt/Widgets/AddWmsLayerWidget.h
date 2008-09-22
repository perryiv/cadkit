
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

#include "Minerva/Qt/Widgets/Export.h"

#include "ui_WmsWidget.h"

#include "Minerva/Core/Layers/LayerInfo.h"

#include "QtGui/QWidget"

#include <vector>
#include <string>

class QButtonGroup;
class QStringListModel;

namespace Usul { namespace Interfaces { struct IUnknown; } }


namespace Minerva {
namespace Widgets {


class MINERVA_QT_WIDGETS_EXPORT BaseAddNetworkLayerWidget : public QWidget,
                                                            protected Ui::WmsWidget
{
  Q_OBJECT;
public:
  typedef QWidget BaseClass;
  typedef std::map<std::string,std::string> Options;
  typedef std::vector<Minerva::Core::Layers::LayerInfo> LayerInfos;

  BaseAddNetworkLayerWidget ( const Options&, const std::string&, QWidget *parent = 0x0 );
  virtual ~BaseAddNetworkLayerWidget();

  virtual void apply ( Usul::Interfaces::IUnknown* parent, Usul::Interfaces::IUnknown * caller ) = 0;
  
  /// Get the cache directory.
  std::string  cacheDirectory() const;
  
  /// Get the server.
  std::string  server() const;
  
signals:
  void serverValid ( bool );

protected slots:
  void on_browseDirectory_clicked();
  void on_capabilitiesButton_clicked();
  void on_viewOptionsButton_clicked();
  
  void _onServerTextChanged ( const QString& text );

protected:
  
  void                 _addRecentServer ( const std::string& url );
  virtual LayerInfos   _getCapabilities() = 0;
  
  QButtonGroup *_imageTypes;
  QStringListModel *_recentServers;
  Options _options;
};

  
}
}

#endif // __ADD_WMS_LAYER_WIDGET_H__
