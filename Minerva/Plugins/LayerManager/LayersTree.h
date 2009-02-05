
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

#include "Usul/Interfaces/IUnknown.h"

#include "QtGui/QWidget"

#include <map>

class QSlider;
class Favorites;

namespace QtTools { class TreeControl; class TreeNode; }

namespace Minerva {
  
  
class LayersTree : public QWidget
{
  Q_OBJECT;
public:
  typedef QWidget                        BaseClass;
  typedef Usul::Interfaces::IUnknown     IUnknown;
  typedef QtTools::TreeNode              TreeNode;

  LayersTree ( IUnknown* caller, QWidget *parent = 0x0 );
  virtual ~LayersTree ();

  void        buildTree ( IUnknown * document );

  void        clear();

  /// Set/get the favorites.
  void        favorites ( Favorites* );
  Favorites*  favorites() const;

signals:
  void        addLayerFavorites ( Usul::Interfaces::IUnknown * layer );

protected:
  bool        _canMoveLayerUp ( TreeNode *item ) const;
  bool        _canMoveLayerDown ( TreeNode *item ) const;
  bool        _canRefreshLayer ( IUnknown *unknown ) const;
  void        _connectTreeViewSlots ();
  void        _addLayer ( IUnknown *parent );
  void        _editLayerProperties ( IUnknown *unknown, IUnknown* parent, IUnknown *editor );
  void        _dirtyAndRedraw ( IUnknown *unknown );
  IUnknown*   _findEditor ( IUnknown* unknown );
  void        _moveLayerUp ( TreeNode *item );
  void        _moveLayerDown ( TreeNode *item );
  void        _refreshLayer ( IUnknown *unknown );
  void        _removeSelectedLayers();
  void        _swapLayers ( TreeNode *item0, TreeNode *item1, TreeNode *parent );

protected slots:
  void        _onTreeNodeChanged ( TreeNode * node );
  void        _onDoubleClick ( TreeNode * node );
  void        _onContextMenuShow ( const QPoint& pos );
  void        _onAddLayerFavorites();
  void        _onSliderReleased();
  void        _onItemSelectionChanged();

private:
  QtTools::TreeControl *_tree;
  QSlider *_slider;
  IUnknown::QueryPtr _caller;
  IUnknown::QueryPtr _document;
  Favorites *_favorites;
};


}

#endif // __MINERVA_PLUGINS_LAYERS_TREE_H__
