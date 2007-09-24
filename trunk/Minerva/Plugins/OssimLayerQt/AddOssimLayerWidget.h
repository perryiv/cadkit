
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __ADD_OSSIM_LAYER_WIDGET_H__
#define __ADD_OSSIM_LAYER_WIDGET_H__

#include "Minerva/Plugins/OssimLayerQt/CompileGuard.h"

#include "QtGui/QWidget"

#include <vector>
#include <string>

class QListWidget;

namespace Usul { namespace Interfaces { struct IUnknown; } }

class AddOssimLayerWidget : public QWidget
{
  Q_OBJECT;
public:
  typedef QWidget BaseClass;

  AddOssimLayerWidget( QWidget *parent = 0x0 );
  virtual ~AddOssimLayerWidget();

  void apply ( Usul::Interfaces::IUnknown * caller );

protected:

private slots:

  void             _browseClicked();

private:
  QListWidget *_listView;
};


#endif // __ADD_OSSIM_LAYER_WIDGET_H__
