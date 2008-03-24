
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

namespace QtTools { class TreeControl; }

class LayersTree : public QWidget
{
  Q_OBJECT;
public:
  typedef QWidget                                        BaseClass;
  typedef Usul::Interfaces::ILayer                       Layer;
  typedef std::map < QTreeWidgetItem *, Layer::RefPtr >  LayerMap;

  LayersTree ( Usul::Interfaces::IUnknown* caller, QWidget *parent = 0x0 );
  virtual ~LayersTree ();

  void     buildTree ( Usul::Interfaces::IUnknown * document );

signals:
  void enableWidgets ( bool );
  void addLayerFavorites ( Usul::Interfaces::IUnknown * layer );

public slots:
  void addLayer ( Usul::Interfaces::IUnknown * layer );
  
protected:
  void     _connectTreeViewSlots ();
  void     _addLayer ( Usul::Interfaces::IUnknown *parent );

protected slots:
  void _onDoubleClick ( QTreeWidgetItem * item, int columnNumber );
  void _onItemChanged ( QTreeWidgetItem * item, int columnNumber );
  void _onAddLayerClick ();
  void _onRemoveLayerClick ();
  void _onContextMenuShow ( const QPoint& pos );
  void _onAddLayerFavorites();

private:
  QtTools::TreeControl *_tree;
  Usul::Interfaces::IUnknown::QueryPtr _caller;
  Usul::Interfaces::IUnknown::QueryPtr _document;
};


#endif // __MINERVA_PLUGINS_LAYERS_TREE_H__
