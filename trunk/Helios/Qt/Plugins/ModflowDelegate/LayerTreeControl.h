
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Perry L Miller IV
//  Adapted from Adam Kubach's LayerTree for Minerva.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _MODFLOW_DELEGATE_LAYER_TREE_CONTROL_CLASS_H_
#define _MODFLOW_DELEGATE_LAYER_TREE_CONTROL_CLASS_H_

#include "Usul/Interfaces/ILayer.h"

#include "QtGui/QWidget"

#include <map>

class QTreeWidget;
class QTreeWidgetItem;

class LayerTreeControl : public QWidget
{
  Q_OBJECT;

public:

  typedef QWidget BaseClass;
  typedef Usul::Interfaces::IUnknown Unknown;
  typedef Usul::Interfaces::ILayer ILayer;
  typedef std::map < QTreeWidgetItem *, ILayer::RefPtr > LayerMap;

  LayerTreeControl ( Unknown *caller, QWidget *parent = 0x0 );
  virtual ~LayerTreeControl();

  // Build the tree.
  void                buildTree ( Usul::Interfaces::IUnknown *document );

protected slots:

  void                _onItemChanged ( QTreeWidgetItem *item, int columnNumber );

private:

  void                _connectTreeViewSlots();

  void                _itemChanged ( QTreeWidgetItem *item, int columnNumber );

  QTreeWidget *_tree;
  LayerMap _layerMap;
  Unknown::QueryPtr _caller;
  Unknown::QueryPtr _document;
};


#endif // _MODFLOW_DELEGATE_LAYER_TREE_CONTROL_CLASS_H_
