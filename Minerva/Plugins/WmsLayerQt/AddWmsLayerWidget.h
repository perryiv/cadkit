
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

#include "QtGui/QWidget"

#include <vector>
#include <string>

class QLineEdit;
class QButtonGroup;

namespace Usul { namespace Interfaces { struct IUnknown; } }

class AddWmsLayerWidget : public QWidget
{
  Q_OBJECT;
public:
  typedef QWidget BaseClass;

  AddWmsLayerWidget( QWidget *parent = 0x0 );
  virtual ~AddWmsLayerWidget();

  void apply ( Usul::Interfaces::IUnknown * caller );

private:
  QLineEdit *_server;
  QLineEdit *_cacheDirectory;
  QButtonGroup *_imageTypes;
};


#endif // __ADD_WMS_LAYER_WIDGET_H__
