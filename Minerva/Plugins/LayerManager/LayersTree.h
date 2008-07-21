
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
class QSlider;
class Favorites;

namespace QtTools { class TreeControl; }

class LayersTree : public QWidget
{
  Q_OBJECT;
public:
  typedef QWidget                                        BaseClass;
  typedef Usul::Interfaces::ILayer                       Layer;
  typedef std::map < QTreeWidgetItem *, Layer::RefPtr >  LayerMap;
  typedef Usul::Interfaces::IUnknown                     IUnknown;

  LayersTree ( Usul::Interfaces::IUnknown* caller, QWidget *parent = 0x0 );
  virtual ~LayersTree ();

  void        buildTree ( Usul::Interfaces::IUnknown * document );

  /// Set/get the favorites.
  void        favorites ( Favorites* );
  Favorites*  favorites() const;

signals:
  void        addLayerFavorites ( Usul::Interfaces::IUnknown * layer );

public slots:
  void        addLayer ( Usul::Interfaces::IUnknown * layer );
  
protected:
  bool        _canMoveLayerUp ( QTreeWidgetItem *item ) const;
  bool        _canMoveLayerDown ( QTreeWidgetItem *item ) const;
  bool        _canRefreshLayer ( Usul::Interfaces::IUnknown *unknown ) const;
  void        _connectTreeViewSlots ();
  void        _addLayer ( Usul::Interfaces::IUnknown *parent );
  void        _editLayerProperties ( Usul::Interfaces::IUnknown *unknown, Usul::Interfaces::IUnknown *editor );
  void        _dirtyAndRedraw ( Usul::Interfaces::IUnknown *unknown );
  IUnknown*   _findEditor ( Usul::Interfaces::IUnknown* unknown );
  void        _moveLayerUp ( QTreeWidgetItem *item );
  void        _moveLayerDown ( QTreeWidgetItem *item );
  void        _refreshLayer ( Usul::Interfaces::IUnknown *unknown );
  void        _removeSelectedLayers();
  void        _swapLayers ( QTreeWidgetItem *item0, QTreeWidgetItem *item1, QTreeWidgetItem *parent );

protected slots:
  void        _onDoubleClick ( QTreeWidgetItem * item, int columnNumber );
  void        _onItemChanged ( QTreeWidgetItem * item, int columnNumber );
  void        _onContextMenuShow ( const QPoint& pos );
  void        _onAddLayerFavorites();
  void        _onSliderReleased();
  void        _onItemSelectionChanged();
  
private:
  QtTools::TreeControl *_tree;
  QSlider *_slider;
  Usul::Interfaces::IUnknown::QueryPtr _caller;
  Usul::Interfaces::IUnknown::QueryPtr _document;
  Favorites *_favorites;
};


#endif // __MINERVA_PLUGINS_LAYERS_TREE_H__
