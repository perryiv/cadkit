
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Tree Control.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __QT_TOOLS_TREE_CONTROL_H__
#define __QT_TOOLS_TREE_CONTROL_H__

#include "QtTools/Export.h"

#include "QtCore/QList"
#include "QtGui/QTreeView"

namespace Usul { namespace Interfaces { struct IUnknown; } }

namespace QtTools {
  
  class TreeNode;
  class TreeModel;
  
class QT_TOOLS_EXPORT TreeControl : public QTreeView
{
  Q_OBJECT;
public:
  typedef QTreeView         BaseClass;
  typedef QList<TreeNode*>  TreeNodeList;
  
  TreeControl ( QWidget *parent = 0x0 );
  virtual ~TreeControl();
  
  /// Build the tree.  This is equvialent to setRootNode ( new TreeNode ( unknown ) );
  void             buildTree ( Usul::Interfaces::IUnknown* unknown );
  
  /// Get the current node.
  const TreeNode*  currentNode() const;
  TreeNode*        currentNode();
  
  /// Remove the node.
  void             removeNode ( TreeNode* child );
  
  /// Set the root node.
  void             setRootNode ( TreeNode * );
  
  /// Get the selected items.
  TreeNodeList     selectedItems() const;
  
signals:
  void onTreeNodeChanged ( TreeNode * node );
  void onTreeNodeDoubleClicked ( TreeNode * node );
  void onSelectionChanged();
  
private slots:
  void _dataChanged ( const QModelIndex&, const QModelIndex& );
  void _onDoubleClicked ( const QModelIndex& );
  void _onCurrentChanged ( const QModelIndex&, const QModelIndex& );
  
private:
  TreeModel *_model;
};

}


#endif // __QT_TOOLS_TREE_CONTROL_H__
