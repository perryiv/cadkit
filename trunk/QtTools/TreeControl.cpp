
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Perry L Miller IV
//  Adapted from Adam Kubach's LayerTree for Minerva.
//
///////////////////////////////////////////////////////////////////////////////

#include "QtTools/TreeControl.h"
#include "QtTools/ScopedSignals.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Errors/Assert.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Interfaces/IDataChangedNotify.h"
#include "Usul/Interfaces/IBooleanState.h"
#include "Usul/Scope/Reset.h"

#include "QtGui/QHeaderView"
#include "QtGui/QTreeWidget"
#include "QtGui/QVBoxLayout"

using namespace QtTools;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

TreeControl::TreeControl ( Usul::Interfaces::IUnknown *caller, QWidget *parent ) : 
  BaseClass ( parent ),
  _tree ( 0x0 ),
  _nodeMap(),
  _caller ( caller ),
  _document(),
  _processingItem ( false ),
  _refCount ( 0 )
{
  // Make the top-level layout.
  QVBoxLayout *layout ( new QVBoxLayout ( parent ) );
  this->setLayout ( layout );
  layout->setMargin ( 0 );

  // Make the tree widget and add it to the layout.
  _tree = new QTreeWidget;
  _tree->header()->setHidden ( 1 );
  layout->addWidget ( _tree );

  // Hook up the functions.
  this->_connectTreeViewSlots();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

TreeControl::~TreeControl()
{
  // Note: _tree gets deleted by it's parent.

  // Clear the map.
  _nodeMap.clear();

  // Not necessary but good practice.
  _caller   = static_cast < Usul::Interfaces::IUnknown * > ( 0x0 );
  _document = static_cast < Usul::Interfaces::IUnknown * > ( 0x0 );
  
  // Better be zero
  USUL_ASSERT ( _refCount == 0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper templates to add items to the tree.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  void addItem ( QTreeWidget *parent, QTreeWidgetItem *item )
  {
    if ( parent && item )
    {
      parent->addTopLevelItem ( item );
    }
  }
  void addItem ( QTreeWidgetItem *parent, QTreeWidgetItem *item )
  {
    if ( parent && item )
    {
      parent->addChild ( item );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to add a node to the tree.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  template < class MapType, class WidgetType > void addTreeNode ( MapType &items, WidgetType *widget, Usul::Interfaces::IUnknown *unknown, Usul::Interfaces::IUnknown *caller )
  {
    // Handle bad input.
    if ( 0x0 == widget )
      return;

    // See if the correct interface is implemented.
    Usul::Interfaces::ITreeNode::QueryPtr node ( unknown );
    if ( false == node.valid() )
      return;

    // For each node...
    const unsigned int numNodes ( node->getNumChildNodes() );
    for ( unsigned int i = 0; i < numNodes; ++i )
    {
      // Get the child node.
      Usul::Interfaces::ITreeNode::QueryPtr child ( node->getChildNode ( i ) );
      if ( true == child.valid() )
      {
        // Make a tree item.
        QTreeWidgetItem *item ( new QTreeWidgetItem ( widget ) );
        item->setText ( 0, child->getTreeNodeName().c_str() );
        item->setToolTip ( 0, item->text ( 0 ) );

        // Set the checked state.
        Usul::Interfaces::IBooleanState::QueryPtr boolean ( child );
        if ( true == boolean.valid() )
        {
          item->setCheckState ( 0, ( ( boolean->getBooleanState() ) ? Qt::Checked : Qt::Unchecked ) );
        }
        
        // See if the item will notify us when data has changed.
        Usul::Interfaces::IDataChangedNotify::QueryPtr notify ( child );
        if ( notify.valid() )
          notify->addDataChangedListener ( caller );

        // Add the item to the tree.
        Helper::addItem ( widget, item );

        // Save in the map.
        items[item] = child.get();

        // Build sub-tree.
        Helper::addTreeNode ( items, item, child.get(), caller );
      }
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the tree.
//
///////////////////////////////////////////////////////////////////////////////

void TreeControl::buildTree ( Usul::Interfaces::IUnknown *unknown )
{
  // Save the document;
  _document = unknown;

  // Clear anything we may have.
  this->_clear();

  // Disconnect signal while adding to tree.
  _tree->disconnect ( this );

  // Add the tree node.
  Helper::addTreeNode ( _nodeMap, _tree, unknown, this->queryInterface ( Usul::Interfaces::IUnknown::IID ) );

  // Re-connect slots.
  this->_connectTreeViewSlots();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Connect slots and signals for the tree view.
//
///////////////////////////////////////////////////////////////////////////////

void TreeControl::_connectTreeViewSlots()
{
  // Notify us when an item is changed.
  this->connect ( _tree, SIGNAL ( itemChanged    ( QTreeWidgetItem *, int ) ), 
                   this, SLOT   ( _onItemChanged ( QTreeWidgetItem *, int ) ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Item has been changed.
//
///////////////////////////////////////////////////////////////////////////////

void TreeControl::_onItemChanged ( QTreeWidgetItem *item, int column )
{
  Usul::Functions::safeCallV1V2 ( Usul::Adaptors::memberFunction ( this, &TreeControl::_itemChanged ), item, column, "2798574950" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to count visible children.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  unsigned int countVisibleChildren ( const QTreeWidgetItem *item, Qt::CheckState state )
  {
    unsigned int count ( 0 );
    if ( 0x0 != item )
    {
      const int numChildren ( item->childCount() );
      for ( int i = 0; i < numChildren; ++i )
      {
        const QTreeWidgetItem *child ( item->child ( i ) );
        if ( ( 0x0 != child ) && ( state == child->checkState ( 0 ) ) )
        {
          ++count;
        }
      }
    }
    return count;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to set checks.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  void setChecks ( QTreeWidgetItem *item, TreeControl::NodeMap &items )
  {
    // Handle no item
    if ( 0x0 == item )
      return;

    // Loop through all the children.
    const int numChildren ( item->childCount() );
    for ( int i = 0; i < numChildren; ++i )
    {
      // Get the child.
      QTreeWidgetItem *child ( item->child ( i ) );

      // Depth-first traversal.
      Helper::setChecks ( child, items );
    }

    // Special case num children.
    if ( 0 == numChildren )
    {
      Usul::Interfaces::IBooleanState::QueryPtr boolean ( items[item] );
      Qt::CheckState check ( boolean.valid() ? ( ( boolean->getBooleanState() ) ? Qt::Checked : Qt::Unchecked ) : Qt::Unchecked );
      item->setCheckState ( 0, check );
      return;
    }
    
    // Get the number of visible children.
    const unsigned int numVisibleChildren ( Helper::countVisibleChildren ( item, Qt::Checked ) );
    const unsigned int numPartiallyVisibleChildren ( Helper::countVisibleChildren ( item, Qt::PartiallyChecked ) );

    // Are all the children visible?
    if ( static_cast<int> ( numVisibleChildren ) == numChildren )
    {
      item->setCheckState ( 0, Qt::Checked );
    }
    
    // If anly children are partially visible, then this item is also.
    else if ( numPartiallyVisibleChildren > 0 )
    {
      item->setCheckState ( 0, Qt::PartiallyChecked );
    }

    // Are none of the chldren visible?
    else if ( 0 == numVisibleChildren )
    {
      item->setCheckState ( 0, Qt::Unchecked );
    }

    // Otherwise, it's mixed.
    else
    {
      item->setCheckState ( 0, Qt::PartiallyChecked );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to set checks.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  void setChecks ( QTreeWidget *tree, TreeControl::NodeMap &items )
  {
    // Handle no tree
    if ( 0x0 == tree )
      return;

    // Loop through all the children.
    const int numChildren ( tree->topLevelItemCount() );
    for ( int i = 0; i < numChildren; ++i )
    {
      Helper::setChecks ( tree->topLevelItem ( i ), items );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Item has been changed.
//
///////////////////////////////////////////////////////////////////////////////

void TreeControl::_itemChanged ( QTreeWidgetItem *item, int column )
{
  // This function should not result in a call to itself.
  if ( true == _processingItem )
    return;

  // Always set/reset this flag.
  Usul::Scope::Reset<bool> processingItem ( _processingItem, true, false );

  // If the item is valid...
  if ( 0x0 != item )
  {
    // Get the state and interface.
    Qt::CheckState state ( item->checkState ( column ) );
    Usul::Interfaces::IBooleanState::QueryPtr boolean ( _nodeMap[item] );

    // If the interface is valid...
    if ( boolean.valid() )
    {
      // Set the state.
      boolean->setBooleanState ( Qt::Checked == state );

      // Top-down traversal setting all the checks.
      Helper::setChecks ( _tree, _nodeMap );
    }
  }
  
  // Redirect.
  emit onItemChanged( item, column );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the current item.
//
///////////////////////////////////////////////////////////////////////////////

QTreeWidgetItem* TreeControl::currentItem() const
{
  return _tree->currentItem();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the IUnknown that corresponds to the current item.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown* TreeControl::currentUnknown() const
{
  NodeMap::const_iterator iter ( _nodeMap.find ( _tree->currentItem() ) );
  return ( Usul::Interfaces::IUnknown::QueryPtr ( iter != _nodeMap.end() ? iter->second.get() : 0x0 ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the IUnknown that corresponds to the position.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown* TreeControl::unknownAt ( const QPoint& pos ) const
{
  NodeMap::const_iterator iter ( _nodeMap.find ( _tree->itemAt( pos ) ) );
  return ( Usul::Interfaces::IUnknown::QueryPtr ( iter != _nodeMap.end() ? iter->second.get() : 0x0 ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the IUnknown that corresponds to the item.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown* TreeControl::unknown ( QTreeWidgetItem *item ) const
{
  NodeMap::const_iterator iter ( _nodeMap.find ( item ) );
  return ( Usul::Interfaces::IUnknown::QueryPtr ( iter != _nodeMap.end() ? iter->second.get() : 0x0 ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove the item.
//
///////////////////////////////////////////////////////////////////////////////

void TreeControl::removeItem ( QTreeWidgetItem *item )
{
  _nodeMap.erase ( item );
  
  QTreeWidgetItem *parent ( item->parent() );
  
  if ( 0x0 != parent )
  {
    const int index ( parent->indexOfChild ( item ) );
    parent->takeChild ( index );
  }
  else
  {
    const int index ( _tree->indexOfTopLevelItem( item ) );
    _tree->takeTopLevelItem( index );
  }
  
  // takeChild does not delete the item.
  delete item;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the selection mode.
//
///////////////////////////////////////////////////////////////////////////////

QAbstractItemView::SelectionMode TreeControl::selectionMode() const
{
  return _tree->selectionMode();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the selection mode.
//
///////////////////////////////////////////////////////////////////////////////

void TreeControl::selectionMode( QAbstractItemView::SelectionMode mode )
{
  _tree->setSelectionMode ( mode );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get all the selected items.
//
///////////////////////////////////////////////////////////////////////////////

QList<QTreeWidgetItem*> TreeControl::selectedItems() const
{
  return _tree->selectedItems();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the tree widget.
//
///////////////////////////////////////////////////////////////////////////////

QTreeWidget* TreeControl::treeWidget()
{
  return _tree;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Reference.
//
///////////////////////////////////////////////////////////////////////////////

void TreeControl::ref()
{
  ++_refCount;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Unreference.  Qt handles deletion.
//
///////////////////////////////////////////////////////////////////////////////

void TreeControl::unref ( bool )
{
  --_refCount;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown* TreeControl::queryInterface ( unsigned long iid )
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

void TreeControl::dataChangedNotify ( Usul::Interfaces::IUnknown *caller )
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

void TreeControl::_onDataChanged ( unsigned long pointer )
{
  Usul::Interfaces::IUnknown* caller ( reinterpret_cast<Usul::Interfaces::IUnknown*> ( pointer ) );
  
  // Rebuild the tree.
  Usul::Functions::safeCallV1 ( Usul::Adaptors::memberFunction ( this, &TreeControl::_rebuildTree ), caller, "1646245025" );
  
  Usul::Pointers::unreference ( caller );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear.
//
///////////////////////////////////////////////////////////////////////////////

void TreeControl::_clear()
{
  for ( NodeMap::iterator iter = _nodeMap.begin(); iter != _nodeMap.end(); ++iter )
  {
    Usul::Interfaces::IDataChangedNotify::QueryPtr notify ( iter->second );
    if ( notify.valid() )
      notify->removeDataChangedListener ( this->queryInterface ( Usul::Interfaces::IUnknown::IID ) );
  }
  
  _tree->clear();
  _nodeMap.clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Rebuild the tree node for the unknown.
//
///////////////////////////////////////////////////////////////////////////////

void TreeControl::_rebuildTree ( Unknown* unknown )
{
  QTreeWidgetItem *item ( this->_treeItem ( unknown ) );
  
  if ( 0x0 == item || 0x0 == unknown )
    return;
  
  // Block signals during the rest of this function.
  QtTools::ScopedSignals scoped ( *_tree );
  
  typedef QList<QTreeWidgetItem *> Items;
  Items children ( item->takeChildren() );
  for ( Items::iterator iter = children.begin(); iter != children.end(); ++iter )
  {
    _nodeMap.erase ( *iter );
    delete *iter;
  }
  children.clear();
  
  // Set the name.
  Usul::Interfaces::ITreeNode::QueryPtr node ( unknown );
  if ( node.valid() )
    item->setText( 0, node->getTreeNodeName().c_str() );
  
  // Build sub-tree.
  Helper::addTreeNode ( _nodeMap, item, unknown, this->queryInterface ( Usul::Interfaces::IUnknown::IID ) );
  
  // Make sure the checks are set.  This doesn't seem to be working properly.  It turns off layers that should be turned off.
  //Helper::setChecks ( _tree, _nodeMap );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the tree item that corresponds to the unknown.
//
///////////////////////////////////////////////////////////////////////////////

QTreeWidgetItem * TreeControl::_treeItem ( Unknown* unknown ) const
{
  Usul::Interfaces::ITreeNode::QueryPtr node ( unknown );
  
  if ( false == node.valid() )
    return 0x0;
  
  for ( NodeMap::const_iterator iter = _nodeMap.begin(); iter != _nodeMap.end(); ++iter )
  {
    if ( iter->second.get() == node.get() )
      return iter->first;
  }
  
  return 0x0;
}
