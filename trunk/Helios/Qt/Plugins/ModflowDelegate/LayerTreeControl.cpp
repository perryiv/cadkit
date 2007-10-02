
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Perry L Miller IV
//  Adapted from Adam Kubach's LayerTree for Minerva.
//
///////////////////////////////////////////////////////////////////////////////

#include "LayerTreeControl.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Interfaces/IBooleanState.h"
#include "Usul/Scope/Reset.h"

#include "QtGui/QHeaderView"
#include "QtGui/QTreeWidget"
#include "QtGui/QVBoxLayout"


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

LayerTreeControl::LayerTreeControl ( Usul::Interfaces::IUnknown *caller, QWidget *parent ) : 
  BaseClass ( parent ),
  _tree ( 0x0 ),
  _nodeMap(),
  _caller ( caller ),
  _document(),
  _processingItem ( false )
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

LayerTreeControl::~LayerTreeControl()
{
  // Note: _tree gets deleted by it's parent.

  // Clear the map.
  _nodeMap.clear();

  // Not necessary but good practice.
  _caller   = static_cast < Usul::Interfaces::IUnknown * > ( 0x0 );
  _document = static_cast < Usul::Interfaces::IUnknown * > ( 0x0 );
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
  template < class MapType, class WidgetType > void addTreeNode ( MapType &items, WidgetType *widget, Usul::Interfaces::IUnknown *unknown )
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

        // Set the checked state.
        Usul::Interfaces::IBooleanState::QueryPtr boolean ( child );
        if ( true == boolean.valid() )
        {
          item->setCheckState ( 0, ( ( boolean->getBooleanState() ) ? Qt::Checked : Qt::Unchecked ) );
        }

        // Add the item to the tree.
        Helper::addItem ( widget, item );

        // Save in the map.
        items[item] = child.get();

        // Build sub-tree.
        Helper::addTreeNode ( items, item, child.get() );
      }
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the tree.
//
///////////////////////////////////////////////////////////////////////////////

void LayerTreeControl::buildTree ( Usul::Interfaces::IUnknown *unknown )
{
  // Save the document;
  _document = unknown;

  // Clear anything we may have.
  _tree->clear();
  _nodeMap.clear();

  // Disconnect signal while adding to tree.
  _tree->disconnect ( this );

  // Add the tree node.
  Helper::addTreeNode ( _nodeMap, _tree, unknown );

  // Re-connect slots.
  this->_connectTreeViewSlots();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Connect slots and signals for the tree view.
//
///////////////////////////////////////////////////////////////////////////////

void LayerTreeControl::_connectTreeViewSlots()
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

void LayerTreeControl::_onItemChanged ( QTreeWidgetItem *item, int column )
{
  Usul::Functions::safeCallV1V2 ( Usul::Adaptors::memberFunction ( this, &LayerTreeControl::_itemChanged ), item, column, "2798574950" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to set the item's parent's check.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  void setParentCheck ( QTreeWidgetItem *item )
  {
    if ( item )
    {
      QTreeWidgetItem *parent ( item->parent() );
      if ( parent )
      {
        unsigned int checked   ( 0 );
        unsigned int unChecked ( 0 );
        unsigned int partial   ( 0 );

        // Determine children's state.
        const int numChildren ( parent->childCount() );
        for ( int i = 0; i < numChildren; ++i )
        {
          QTreeWidgetItem *child ( parent->child ( i ) );
          if ( child )
          {
            const Qt::CheckState state ( child->checkState ( 0 ) );
            if ( state == Qt::Checked )
            {
              ++checked;
            }
            if ( state == Qt::Unchecked )
            {
              ++unChecked;
            }
            if ( state == Qt::PartiallyChecked )
            {
              ++partial;
            }
          }
        }

        // Set check state according the children's state.
        if ( numChildren == checked )
        {
          parent->setCheckState ( 0, Qt::Checked );
        }
        else if ( numChildren == unChecked )
        {
          parent->setCheckState ( 0, Qt::Unchecked );
        }
        else
        {
          parent->setCheckState ( 0, Qt::PartiallyChecked );
        }

        // Call this function for the parent.
        Helper::setParentCheck ( parent->parent() );
      }
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to set the item's children's check.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  void setChildrenCheck ( QTreeWidgetItem *parent, Qt::CheckState state )
  {
    if ( parent )
    {
      const int numChildren ( parent->childCount() );
      for ( int i = 0; i < numChildren; ++i )
      {
        QTreeWidgetItem *child ( parent->child ( i ) );
        if ( child )
        {
          child->setCheckState ( 0, state );
        }
      }
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Item has been changed.
//
///////////////////////////////////////////////////////////////////////////////

void LayerTreeControl::_itemChanged ( QTreeWidgetItem *item, int column )
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

      // Set check for the children.
      Helper::setChildrenCheck ( item, state );

      // Set the check for the parent.
      Helper::setParentCheck ( item );
    }
  }
}
