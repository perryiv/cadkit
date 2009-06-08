
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

#include "Minerva/Core/Layers/RasterGroup.h"

#include "Usul/Interfaces/IUnknown.h"

#include "QtGui/QWidget"

#include <vector>
#include <string>

class QListWidget;
class QCheckBox;

class AddOssimLayerWidget : public QWidget
{
  Q_OBJECT;
public:
  typedef QWidget BaseClass;

  AddOssimLayerWidget( Usul::Interfaces::IUnknown* caller = 0x0, QWidget *parent = 0x0 );
  virtual ~AddOssimLayerWidget();

  void             apply ( Usul::Interfaces::IUnknown* parent, Usul::Interfaces::IUnknown * caller );

private slots:

  void             _browseClicked();
  void             _searchDirectoryClicked();
  void             _removeSelectedFiles();
  
private:

  typedef std::vector<std::string> Filenames;
  
  // Pass the filesnames by copy so these functions can be threaded.
  static void      _loadData ( Filenames filenames, Usul::Interfaces::IUnknown* parent, Usul::Interfaces::IUnknown * caller );
  static void      _showDataExtents ( Filenames filenames, Usul::Interfaces::IUnknown* parent, Usul::Interfaces::IUnknown * caller );

  Usul::Interfaces::IUnknown::QueryPtr _caller;
  QListWidget *_listView;
  QCheckBox *_loadDataCheck;
  QCheckBox* _showExtentsCheck;
};


#endif // __ADD_OSSIM_LAYER_WIDGET_H__
