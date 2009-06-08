
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Custom item model.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __QT_TOOLS_TREE_MODEL_H__
#define __QT_TOOLS_TREE_MODEL_H__

#include "QtGui/QTreeView"

namespace Usul { namespace Interfaces { struct ITreeNode; } }

namespace QtTools {

  class TreeNode;
  
class TreeModel : public QAbstractItemModel
{
public:
  typedef QAbstractItemModel BaseClass;
  
  TreeModel ( QObject *parent = 0x0 );
  virtual ~TreeModel();
  
  /// Set the root node.
  void root ( TreeNode* );
  
  /// Set/get the data.
  virtual bool           setData ( const QModelIndex& index, const QVariant& value, int role );
  virtual QVariant       data    ( const QModelIndex& index, int role = Qt::DisplayRole ) const;
  
  void                   emitDataChanged( TreeNode *node );
  
  /// Get the flags.
  virtual Qt::ItemFlags  flags ( const QModelIndex &index ) const;
  
  /// Get the ModelIndex.
  virtual QModelIndex    index ( int row, int column, const QModelIndex& parent ) const;
  
  /// Get the row and column count.
  virtual int            rowCount ( const QModelIndex &parent = QModelIndex() ) const;
  virtual int            columnCount ( const QModelIndex &parent = QModelIndex() ) const;
  
  // Get the parent.
  virtual QModelIndex    parent ( const QModelIndex & index ) const;
  
  // Begin/End insert rows.
  void                   beginInsertRows ( TreeNode* parent, int row, int count );
  void                   endInsertRows();
  
  // Begin/End remove rows.
  void                   beginRemoveRows ( TreeNode* parent, int row, int count );
  void                   endRemoveRows();
  
private:
  QModelIndex _indexFromNode ( TreeNode *node ) const;
  TreeNode*   _nodeFromIndex ( const QModelIndex& index ) const;
  
  TreeNode *_root;
};

}
  
#endif // __QT_TOOLS_TREE_MODEL_H__
