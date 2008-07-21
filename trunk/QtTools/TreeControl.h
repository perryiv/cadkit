
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Perry L Miller IV
//  Adapted from Adam Kubach's LayerTree for Minerva.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _QT_TOOLS_TREE_CONTROL_CLASS_H_
#define _QT_TOOLS_TREE_CONTROL_CLASS_H_

#include "QtTools/Export.h"

#include "Usul/Interfaces/IDataChangedListener.h"
#include "Usul/Interfaces/ITreeNode.h"
#include "Usul/Pointers/Pointers.h"

#include "QtGui/QWidget"
#include "QtGui/QAbstractItemView"

#include <map>

class QTreeWidget;
class QTreeWidgetItem;


namespace QtTools {


class QT_TOOLS_EXPORT TreeControl : public QWidget,
                                    public Usul::Interfaces::IDataChangedListener
{
  Q_OBJECT;

public:

  typedef QWidget BaseClass;
  typedef Usul::Interfaces::IUnknown Unknown;
  typedef Usul::Interfaces::ITreeNode ITreeNode;
  typedef std::map < QTreeWidgetItem *, ITreeNode::RefPtr > NodeMap;
  typedef QAbstractItemView::SelectionMode SelectionMode;
  typedef QList<QTreeWidgetItem*> TreeWidgetItems;
  
  USUL_DECLARE_QUERY_POINTERS ( TreeControl );
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  TreeControl ( Unknown *caller, QWidget *parent = 0x0 );
  virtual ~TreeControl();

  // Build the tree.
  void                buildTree ( Usul::Interfaces::IUnknown *document );

  // Get the item.
  QTreeWidgetItem*    currentItem() const;
  
  // Get the unknown.
  Unknown*            currentUnknown() const;
  Unknown*            unknownAt ( const QPoint& pos ) const;
  Unknown*            unknown ( QTreeWidgetItem *item ) const;
  
  // Called when data has changed.
  virtual void        dataChangedNotify ( Usul::Interfaces::IUnknown *caller );
  
  // Remove the item.
  void                removeItem ( QTreeWidgetItem *item );
  
  // Get all the selected items.
  TreeWidgetItems     selectedItems() const;
  
  // Get/set the selection mode.
  SelectionMode       selectionMode() const;
  void                selectionMode( QAbstractItemView::SelectionMode );
  
  // Get the tree widget.
  QTreeWidget*        treeWidget();
  
signals:
  void                onItemChanged ( QTreeWidgetItem *item, int columnNumber );
  
protected slots:

  void                _onItemChanged ( QTreeWidgetItem *item, int columnNumber );
  void                _onDataChanged ( unsigned long );

private:

  void                _clear();
  void                _connectTreeViewSlots();
  
  void                _itemChanged ( QTreeWidgetItem *item, int columnNumber );
  
  void                _rebuildTree ( Unknown* );
  
  // Get the tree item that corresponds to the unknown.
  QTreeWidgetItem *   _treeItem ( Unknown* unknown ) const;

  QTreeWidget *_tree;
  NodeMap _nodeMap;
  Unknown::QueryPtr _caller;
  Unknown::QueryPtr _document;
  bool _processingItem;
  unsigned int _refCount;
};


} // namespace QtTools


#endif // _QT_TOOLS_TREE_CONTROL_CLASS_H_
