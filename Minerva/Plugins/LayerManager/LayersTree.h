
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_PLUGINS_LAYERS_TREE_H__
#define __MINERVA_PLUGINS_LAYERS_TREE_H__

#include "Usul/Interfaces/ILayer.h"

#include "QtGui/QWidget"

#include <map>

class QTreeWidget;
class QTreeWidgetItem;

class LayersTree : public QWidget
{
  Q_OBJECT;
public:
  typedef QWidget                                        BaseClass;
  typedef Usul::Interfaces::ILayer                       Layer;
  typedef std::map < QTreeWidgetItem *, Layer::RefPtr >  LayerMap;

  LayersTree ( QWidget *parent = 0x0 );
  virtual ~LayersTree ();

  void     buildTree ( Usul::Interfaces::IUnknown * caller );

protected:
  void     _connectTreeViewSlots ();

protected slots:
  void _onDoubleClick ( QTreeWidgetItem * item, int columnNumber );
  void _onItemChanged ( QTreeWidgetItem * item, int columnNumber );
  void _onAddLayerClick ();
  void _onRemoveLayerClick ();
  void _onRefreshClick ();

private:
  QTreeWidget *_tree;
  LayerMap _layerMap;
  Usul::Interfaces::IUnknown::QueryPtr _caller;
};


#endif // __MINERVA_PLUGINS_LAYERS_TREE_H__
