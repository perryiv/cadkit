
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

#include "Usul/Interfaces/ITreeNode.h"

#include "QtGui/QWidget"
#include "QtGui/QAbstractItemView"

#include <map>

class QTreeWidget;
class QTreeWidgetItem;


namespace QtTools {


class QT_TOOLS_EXPORT TreeControl : public QWidget
{
  Q_OBJECT;

public:

  typedef QWidget BaseClass;
  typedef Usul::Interfaces::IUnknown Unknown;
  typedef Usul::Interfaces::ITreeNode ITreeNode;
  typedef std::map < QTreeWidgetItem *, ITreeNode::RefPtr > NodeMap;
  typedef QAbstractItemView::SelectionMode SelectionMode;

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
  
  // Remove the item.
  void                removeItem ( QTreeWidgetItem *item );
  
  // Get/set the selection mode.
  SelectionMode       selectionMode() const;
  void                selectionMode( QAbstractItemView::SelectionMode );
  
signals:
  void                onItemChanged ( QTreeWidgetItem *item, int columnNumber );
  
protected slots:

  void                _onItemChanged ( QTreeWidgetItem *item, int columnNumber );

private:

  void                _connectTreeViewSlots();

  void                _itemChanged ( QTreeWidgetItem *item, int columnNumber );

  QTreeWidget *_tree;
  NodeMap _nodeMap;
  Unknown::QueryPtr _caller;
  Unknown::QueryPtr _document;
  bool _processingItem;
};


} // namespace QtTools


#endif // _QT_TOOLS_TREE_CONTROL_CLASS_H_
