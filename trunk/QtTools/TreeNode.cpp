
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

#include "QtTools/TreeNode.h"
#include "QtTools/TreeModel.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Errors/Assert.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Interfaces/IBooleanState.h"
#include "Usul/Interfaces/IDataChangedNotify.h"

using namespace QtTools;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

TreeNode::TreeNode ( ITreeNode *node, TreeNode *parent ) : BaseClass(),
  _model ( 0x0 ),
  _parent ( parent ),
  _node ( node ),
  _children(),
  _refCount ( 0 )
{
  // See if the item will notify us when data has changed.
  Usul::Interfaces::IDataChangedNotify::QueryPtr notify ( node );
  if ( notify.valid() )
    notify->addDataChangedListener ( this->queryInterface ( Usul::Interfaces::IUnknown::IID ) );
  
  this->_addChildren();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

TreeNode::~TreeNode()
{
  Usul::Interfaces::IDataChangedNotify::QueryPtr notify ( _node );
  if ( notify.valid() )
    notify->removeDataChangedListener ( this->queryInterface ( Usul::Interfaces::IUnknown::IID ) );
  
  TreeNode *parent ( this->parent() );
  if ( 0x0 != parent )
  {
    const int myIndex ( parent->indexOfChild ( this ) );
    if ( myIndex >= 0 && 0x0 != _model )
    {
      _model->beginRemoveRows ( parent, myIndex, 1 );
      parent->takeChild ( myIndex );
      _model->endRemoveRows();
    }
  }
  
  this->_clear();
  _model = 0x0;
  
  // Better be zero
  USUL_ASSERT ( _refCount == 0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the i'th child.
//
///////////////////////////////////////////////////////////////////////////////

TreeNode* TreeNode::child ( int which )
{
  return ( which >=0 && which < _children.size() ? _children[which] : 0x0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the i'th child.
//
///////////////////////////////////////////////////////////////////////////////

const TreeNode* TreeNode::child ( int which ) const
{
  return ( which >=0 && which < _children.size() ? _children[which] : 0x0 );
}
 

///////////////////////////////////////////////////////////////////////////////
//
//  Get the count.
//
///////////////////////////////////////////////////////////////////////////////

int TreeNode::count() const
{
  return _children.size();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the count.
//
///////////////////////////////////////////////////////////////////////////////

int TreeNode::childCount() const
{
  return _children.size();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the name.
//
///////////////////////////////////////////////////////////////////////////////

QString TreeNode::name() const
{
  return ( _node.valid() ? QString ( _node->getTreeNodeName().c_str() ) : "" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the node.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::ITreeNode::RefPtr TreeNode::node() const
{
  return _node;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the parent.
//
///////////////////////////////////////////////////////////////////////////////

TreeNode* TreeNode::parent() const
{
  return _parent;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the index of node.
//
///////////////////////////////////////////////////////////////////////////////

int TreeNode::index ( TreeNode* node ) const
{
  return _children.lastIndexOf ( node );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the index of node.
//
///////////////////////////////////////////////////////////////////////////////

int TreeNode::indexOfChild ( TreeNode* node ) const
{
  return this->index ( node );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to count visible children.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  void countChildrenState ( const TreeNode& node, unsigned int& visible, unsigned int& partially )
  { 
    const int numChildren ( node.childCount() );
    for ( int i = 0; i < numChildren; ++i )
    {
      const TreeNode *child ( node.child ( i ) );
      if ( 0x0 != child )
      {
        if ( Qt::Checked == child->checkState() )
          ++visible;
        else if ( Qt::PartiallyChecked == child->checkState() )
          ++partially;
      }
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the check state.
//
///////////////////////////////////////////////////////////////////////////////

void TreeNode::checkState ( Qt::CheckState state )
{
  Usul::Interfaces::IBooleanState::QueryPtr b ( _node );
  if ( b.valid() )
    b->setBooleanState ( Qt::Checked == state );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the check state.
//
///////////////////////////////////////////////////////////////////////////////

Qt::CheckState TreeNode::checkState() const
{
  const unsigned int numChildren ( this->childCount() );
  Usul::Interfaces::IBooleanState::QueryPtr state ( _node.get() );
  
  // Use our state if no children.
  if ( 0 == numChildren && state.valid() )
  {
    return ( state->getBooleanState() ? Qt::Checked : Qt::Unchecked );
  }
  
  // Check the children.
  unsigned int numVisible ( 0 ), numPartially ( 0 );
  Helper::countChildrenState ( *this, numVisible, numPartially );
  
  // If any children are partially checked, so are we.
  if ( numPartially > 0 )
    return Qt::PartiallyChecked;
  
  // Are all the children visible?
  if ( numVisible == numChildren )
    return Qt::Checked;
  
  // Are none of the chldren visible?
  if ( 0 == numVisible )
    return Qt::Unchecked;
  
  // Otherwise, it's mixed.
  return Qt::PartiallyChecked;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Can the item be checked?
//
///////////////////////////////////////////////////////////////////////////////

bool TreeNode::isCheckable() const
{
  Usul::Interfaces::IBooleanState::QueryPtr state ( _node.get() );
  return state.valid();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the model.
//
///////////////////////////////////////////////////////////////////////////////

void TreeNode::model ( TreeModel* m )
{
  _model = m;
  
  // Also set the children's model.
  for ( TreeNodeList::iterator iter = _children.begin(); iter != _children.end(); ++iter )
  {
    (*iter)->model ( m );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the model.
//
///////////////////////////////////////////////////////////////////////////////

TreeModel* TreeNode::model()
{
  return _model;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the data.
//
///////////////////////////////////////////////////////////////////////////////

QVariant TreeNode::data ( int role ) const
{ 
  if ( role == Qt::DisplayRole )
    return this->name();
  
  if ( role == Qt::CheckStateRole )
    return this->checkState();
  
  return QVariant();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the data.
//
///////////////////////////////////////////////////////////////////////////////

bool TreeNode::setData ( const QVariant& value, int role )
{
  if ( role == Qt::CheckStateRole )
  {
    Qt::CheckState state ( static_cast<Qt::CheckState> ( value.toInt() ) );
    this->checkState ( state );
    
    for ( TreeNodeList::iterator iter = _children.begin(); iter != _children.end(); ++iter )
    {
      TreeNode *node ( *iter );
      node->setData ( value, role );
    }

    if ( 0x0 != _model )
    {
      _model->emitDataChanged ( this );
      
      // Walk up the tree.
      TreeNode *parent ( this->parent() );
      while ( 0x0 != parent )
      {
        _model->emitDataChanged ( parent );
        parent = parent->parent();
      }
    }
    
    return true;
  }
  
  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Removes the child at index and returns it.
//
///////////////////////////////////////////////////////////////////////////////

TreeNode* TreeNode::takeChild ( int index )
{
  //if ( 0x0 != _model )
  //  _model->executePendingSort();
  
  if ( index >= 0 && index < _children.count() )
  {
    if ( 0x0 != _model )
      _model->beginRemoveRows ( this, index, 1 );

    TreeNode *node ( _children.takeAt ( index ) );
    
    if ( 0x0 != _model ) 
      _model->endRemoveRows();
    
    return node;
  }
  
  return 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add children to this node.
//
///////////////////////////////////////////////////////////////////////////////

void TreeNode::_addChildren()
{
  if ( false == _node.valid() )
    return;
  
  // Get the number of children.
  const unsigned int numChildren ( _node->getNumChildNodes() );

  // Return now if there are no children to add.
  if ( 0 == numChildren )
    return;
  
  if ( 0x0 != _model )
    _model->beginInsertRows ( this, 0, numChildren );
  
  for ( unsigned int i = 0; i < numChildren; ++i )
  {
    Usul::Interfaces::ITreeNode::QueryPtr tn ( _node->getChildNode ( i ).get() );
    TreeNode* child ( new TreeNode ( tn.get(), this ) );
    child->model ( this->model() );
    _children.push_back ( child );
  }
  
  if ( 0x0 != _model ) 
    _model->endInsertRows();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear this node.
//
///////////////////////////////////////////////////////////////////////////////

void TreeNode::_clear()
{
  // Return now if there are no children.
  if ( 0 == _children.size() )
    return;

  if ( 0x0 != _model )
    _model->beginRemoveRows ( this, 0, _children.size() );
  
  for ( TreeNodeList::iterator iter = _children.begin(); iter != _children.end(); ++iter )
  {
    delete *iter;
  }
  _children.clear();
  
  if ( 0x0 != _model ) 
    _model->endRemoveRows();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Reference.
//
///////////////////////////////////////////////////////////////////////////////

void TreeNode::ref()
{
  ++_refCount;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Unreference. Qt handles deletion.
//
///////////////////////////////////////////////////////////////////////////////

void TreeNode::unref ( bool )
{
  --_refCount;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown* TreeNode::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
    case Usul::Interfaces::IUnknown::IID:
    case Usul::Interfaces::IDataChangedListener::IID:
      return static_cast<Usul::Interfaces::IDataChangedListener*> ( this );
    default:
      return 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when data has changed.
//
///////////////////////////////////////////////////////////////////////////////

void TreeNode::dataChangedNotify ( Usul::Interfaces::IUnknown *caller )
{
  Usul::Pointers::reference ( caller );
  
  // Need to reinterpret_cast to get Qt to pass the pointer to the slot.
  // Qt mechanism for passing custom types, but I have had mixed results in the past (See DocumentProxy).
  QMetaObject::invokeMethod ( this, "_onDataChanged", Qt::QueuedConnection, 
                             Q_ARG ( unsigned long, reinterpret_cast<unsigned long> ( caller ) ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when data has changed.
//
///////////////////////////////////////////////////////////////////////////////

void TreeNode::_onDataChanged ( unsigned long pointer )
{
  Usul::Interfaces::IUnknown* caller ( reinterpret_cast<Usul::Interfaces::IUnknown*> ( pointer ) );
  
  // Rebuild the tree.
  Usul::Functions::safeCallV1 ( Usul::Adaptors::memberFunction ( this, &TreeNode::_rebuildTree ), caller, "1646245025" );
  
  Usul::Pointers::unreference ( caller );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Rebuild the tree node for the unknown.
//
///////////////////////////////////////////////////////////////////////////////

void TreeNode::_rebuildTree ( Usul::Interfaces::IUnknown* unknown )
{
  this->_clear();
  this->_addChildren();
  
  if ( 0x0 != _model )
    _model->emitDataChanged ( this );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Swap locations of children.
//
///////////////////////////////////////////////////////////////////////////////

void TreeNode::swap ( int index0, int index1 )
{
  TreeNode *temp ( this->child ( index0 ) );
  _children[index0] = this->child ( index1 );
  _children[index1] = temp;
  
  if ( 0x0 != _model )
    _model->emitDataChanged ( this );
}
