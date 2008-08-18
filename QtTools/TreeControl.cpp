
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "QtTools/TreeControl.h"
#include "QtTools/TreeModel.h"
#include "QtTools/TreeNode.h"

#include "QtGui/QHeaderView"

using namespace QtTools;


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to get the TreeNode from QModelIndex.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  inline QtTools::TreeNode* nodeFromIndex ( const QModelIndex& index )
  {
    return ( index.isValid() ? static_cast<TreeNode*> ( index.internalPointer() ) : 0x0 );
  }
}

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

TreeControl::TreeControl ( QWidget *parent ) : BaseClass ( parent ),
  _model ( 0x0 )
{
  // Make the model.
  _model = new TreeModel ( this );
  
  // Set the model.
  this->setModel ( _model );
  
  // Hide the header.
  this->header()->setHidden ( 1 );
  
  // Connect slots and signals.
  QObject::connect ( _model, SIGNAL ( dataChanged  ( const QModelIndex&, const QModelIndex& ) ), 
                     this,   SLOT   ( _dataChanged ( const QModelIndex&, const QModelIndex& ) ) );
  
  QObject::connect ( this, SIGNAL ( doubleClicked  ( const QModelIndex& ) ), 
                     this, SLOT   ( _onDoubleClicked ( const QModelIndex& ) ) );
  
  QObject::connect ( this->selectionModel(), SIGNAL ( currentChanged ( const QModelIndex &, const QModelIndex & ) ),
                     this,                   SLOT   ( _onCurrentChanged ( const QModelIndex&, const QModelIndex& ) ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

TreeControl::~TreeControl()
{
  delete _model;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the tree.
//
///////////////////////////////////////////////////////////////////////////////

void TreeControl::buildTree ( Usul::Interfaces::IUnknown* unknown )
{
  this->setRootNode ( new TreeNode ( Usul::Interfaces::ITreeNode::QueryPtr ( unknown ) ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the current node.
//
///////////////////////////////////////////////////////////////////////////////

const TreeNode* TreeControl::currentNode() const
{
  QModelIndex index ( 0x0 != this->selectionModel() ? this->selectionModel()->currentIndex() : QModelIndex() );
  return Helper::nodeFromIndex ( index );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the current node.
//
///////////////////////////////////////////////////////////////////////////////

TreeNode* TreeControl::currentNode()
{
  QModelIndex index ( 0x0 != this->selectionModel() ? this->selectionModel()->currentIndex() : QModelIndex() );
  return Helper::nodeFromIndex ( index );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the root node.
//
///////////////////////////////////////////////////////////////////////////////

void TreeControl::setRootNode ( TreeNode * node )
{
  if ( 0x0 != _model )
    _model->root ( node );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Data has changed.
//
///////////////////////////////////////////////////////////////////////////////

void TreeControl::_dataChanged ( const QModelIndex& topLeft, const QModelIndex& bottomRight )
{
  this->update();
  
  TreeNode* node ( Helper::nodeFromIndex ( topLeft ) );
  emit onTreeNodeChanged ( node );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove the item.
//
///////////////////////////////////////////////////////////////////////////////

void TreeControl::removeNode ( TreeNode *node )
{
  if ( 0x0 == node )
    return;
  
  TreeNode *parent ( node->parent() );
  
  if ( 0x0 != parent )
  {
    const int index ( parent->index ( node ) );
    parent->takeChild ( index );
    
    // takeChild does not delete the item.
    delete node;
  }

  // The node is the root node.  Set to null.
  else if ( 0x0 != _model )
  {
    _model->root ( 0x0 );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the selected items.
//
///////////////////////////////////////////////////////////////////////////////

TreeControl::TreeNodeList TreeControl::selectedItems() const
{
  TreeNodeList selection;
  
  if ( 0x0 != this->selectionModel() )
  {
    QModelIndexList indices ( this->selectionModel()->selectedIndexes() );
    for ( QModelIndexList::iterator iter = indices.begin(); iter != indices.end(); ++iter )
      selection.push_back ( Helper::nodeFromIndex ( *iter ) );
  }
  
  return selection;
}


///////////////////////////////////////////////////////////////////////////////
//
//  The index was double clicked.
//
///////////////////////////////////////////////////////////////////////////////

void TreeControl::_onDoubleClicked ( const QModelIndex& index )
{
  TreeNode* node ( Helper::nodeFromIndex ( index ) );
  emit onTreeNodeDoubleClicked ( node );
}


///////////////////////////////////////////////////////////////////////////////
//
//  The current item has changed.
//
///////////////////////////////////////////////////////////////////////////////

void TreeControl::_onCurrentChanged ( const QModelIndex& current, const QModelIndex& previous )
{
  emit onSelectionChanged();
}
