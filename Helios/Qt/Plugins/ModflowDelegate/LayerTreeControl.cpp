
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
#include "Usul/Interfaces/ILayerList.h"

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
  _layerMap(),
  _caller ( caller ),
  _document()
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
  _layerMap.clear();

  // Not necessary but good practice.
  _caller   = static_cast < Usul::Interfaces::IUnknown * > ( 0x0 );
  _document = static_cast < Usul::Interfaces::IUnknown * > ( 0x0 );
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
  _layerMap.clear();

  // See if the correct interface is implemented.
  Usul::Interfaces::ILayerList::QueryPtr layers ( unknown );
  if ( false == layers.valid() )
    return;

  // Disconnect signal while adding to tree.
  _tree->disconnect ( this );

  // For each layer...
  const unsigned int numLayers ( layers->numberLayers() );
  for ( unsigned int i = 0; i < numLayers; ++i )
  {
    // Get the layer.
    Usul::Interfaces::ILayer::QueryPtr layer ( layers->layer ( i ) );
    if ( true == layer.valid() )
    {
      // Make a tree item.
      QTreeWidgetItem *item ( new QTreeWidgetItem ( _tree ) );
      item->setText ( 0, layer->name().c_str() );

      // Set the checked state.
      Qt::CheckState state ( layer->showLayer() ? Qt::Checked : Qt::Unchecked );
      item->setCheckState ( 0, state );
      _tree->addTopLevelItem ( item );

      // Save the tree and layer items in the map.
      _layerMap.insert ( LayerMap::value_type ( item, layer.get() ) );
    }
  }

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
//  Item has been changed.
//
///////////////////////////////////////////////////////////////////////////////

void LayerTreeControl::_itemChanged ( QTreeWidgetItem *item, int column )
{
  if ( 0x0 != item )
  {
    Qt::CheckState state ( item->checkState ( column ) );
    ILayer::RefPtr layer ( _layerMap[item] );

    if ( layer.valid() )
    {
      layer->showLayer ( Qt::Checked == state );
    }
  }
}
