
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Class to encapsulate ITreeNode.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __QT_TOOLS_TREE_NODE_H__
#define __QT_TOOLS_TREE_NODE_H__

#include "QtTools/Export.h"

#include "Usul/Interfaces/ITreeNode.h"
#include "Usul/Interfaces/IDataChangedListener.h"

#include "QtCore/QList"
#include "QtCore/QString"
#include "QtCore/QVariant"

namespace QtTools {

  class TreeModel;
  
class QT_TOOLS_EXPORT TreeNode : public QObject,
                                 public Usul::Interfaces::IDataChangedListener
{
  Q_OBJECT;
public:
  typedef QObject BaseClass;
  typedef Usul::Interfaces::ITreeNode ITreeNode;
  typedef QList<TreeNode*> TreeNodeList;
  
  USUL_DECLARE_QUERY_POINTERS ( TreeNode );
  USUL_DECLARE_IUNKNOWN_MEMBERS;
  
  TreeNode ( ITreeNode *node, TreeNode* parent = 0x0 );
  ~TreeNode();
  
  // Get the i'th child.
  TreeNode*         child ( int i );
  const TreeNode*   child ( int i ) const;
  
  // Get the count.
  int               count() const;
  int               childCount() const;
  
  // Set/get the check state.
  void              checkState ( Qt::CheckState state );
  Qt::CheckState    checkState() const;
  
  // Get the index of node.
  int               index ( TreeNode* ) const;
  int               indexOfChild ( TreeNode* ) const;
  
  // Can the item be checked?
  bool              isCheckable() const;
  
  // Get the name.
  QString           name() const;
  
  // Get the node.
  ITreeNode::RefPtr node() const;
  
  // Get the parent.
  TreeNode*         parent() const;
  
  // Removes the child at index and returns it.
  TreeNode*         takeChild ( int index );
  
  // Swap locations of children.
  void              swap ( int index0, int index1 );
  
protected slots:
  
  void              _onDataChanged ( unsigned long );
  
private:
  
  // Add children to this node.
  void              _addChildren();
  
  // Clear this node.
  void              _clear();
  
  // Rebuild the tree.
  void              _rebuildTree ( Usul::Interfaces::IUnknown* );
  
  // Called when data has changed.
  virtual void      dataChangedNotify ( Usul::Interfaces::IUnknown *caller );
  
  friend class TreeModel;
  
  /// Set/get the data.
  bool              setData ( const QVariant& value, int role );
  QVariant          data    ( int role ) const;
  
  /// Set/get the model.
  void              model ( TreeModel* );
  TreeModel*        model();
  
  TreeModel* _model;
  TreeNode*  _parent;
  ITreeNode::RefPtr _node;
  TreeNodeList _children;
  unsigned int _refCount;
};

}

#endif  // __QT_TOOLS_TREE_NODE_H__
