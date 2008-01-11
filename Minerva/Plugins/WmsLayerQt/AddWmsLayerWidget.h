
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __ADD_WMS_LAYER_WIDGET_H__
#define __ADD_WMS_LAYER_WIDGET_H__

#include "Minerva/Plugins/WmsLayerQt/CompileGuard.h"
#include "Minerva/Plugins/WmsLayerQt/ui_WmsWidget.h"

#include "QtGui/QWidget"

#include <vector>
#include <string>

class QLineEdit;
class QButtonGroup;
class OptionWidget;

namespace Usul { namespace Interfaces { struct IUnknown; } }

class AddWmsLayerWidget : public QWidget,
                          private Ui::WmsWidget
{
  Q_OBJECT;
public:
  typedef QWidget BaseClass;

  AddWmsLayerWidget( QWidget *parent = 0x0 );
  virtual ~AddWmsLayerWidget();

  void apply ( Usul::Interfaces::IUnknown * caller );

protected slots:
  void on_browseDirectory_clicked();
  void on_addOptionButton_clicked();

private:
  typedef std::vector<OptionWidget*> Options;
  
  QButtonGroup *_imageTypes;
  Options _options;
};


#endif // __ADD_WMS_LAYER_WIDGET_H__
