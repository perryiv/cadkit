
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Plugins/LayerManager/LayersTree.h"
#include "Minerva/Core/Commands/AddLayer.h"
#include "Minerva/Core/Commands/RemoveLayer.h"
#include "Minerva/Interfaces/ILayerList.h"

#include "QtGui/QTreeWidget"
#include "QtGui/QVBoxLayout"
#include "QtGui/QHBoxLayout"
#include "QtGui/QPushButton"

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

LayersTree::LayersTree ( QWidget * parent ) : 
BaseClass ( parent ),
_tree ( 0x0 ),
_layerMap (),
_caller ()
{
  QVBoxLayout *topLayout ( new QVBoxLayout ( parent ) );
  this->setLayout ( topLayout );

  QHBoxLayout *buttonLayout ( new QHBoxLayout );
  QVBoxLayout *treeLayout ( new QVBoxLayout );

  QPushButton *addLayer ( new QPushButton ( "Add" ) );
  buttonLayout->addWidget ( addLayer );

  QPushButton *removeLayer ( new QPushButton ( "Remove" ) );
  buttonLayout->addWidget ( removeLayer );

  QPushButton *refresh ( new QPushButton ( "Refresh" ) );
  buttonLayout->addWidget ( refresh );

  _tree = new QTreeWidget;
  treeLayout->addWidget ( _tree );
  
  topLayout->addLayout ( buttonLayout );
  topLayout->addLayout ( treeLayout );

  this->_connectTreeViewSlots ();

  connect ( addLayer, SIGNAL ( clicked () ),
            this,     SLOT   ( _onAddLayerClick () ) );

  connect ( removeLayer, SIGNAL ( clicked () ),
            this,        SLOT   ( _onRemoveLayerClick () ) );

  connect ( refresh, SIGNAL ( clicked () ),
            this,    SLOT   ( _onRefreshClick () ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

LayersTree::~LayersTree()
{
  _layerMap.clear ( );
  _caller = static_cast < Usul::Interfaces::IUnknown * > ( 0x0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the tree.
//
///////////////////////////////////////////////////////////////////////////////

void LayersTree::buildTree ( Usul::Interfaces::IUnknown * caller )
{
  // Save the caller;
  _caller = caller;

  // Clear anything we may have.
  _tree->clear();
  _layerMap.clear ( );

  // See if the correct interface is implemented.
  Minerva::Interfaces::ILayerList::QueryPtr layers ( caller );
  if ( false == layers.valid() )
    return;

  unsigned int numLayers ( layers->numberLayers () );

  // Disconnect signal while adding to tree.
  _tree->disconnect ( this );

  for ( unsigned int i = 0; i < numLayers; ++i )
  {
    Usul::Interfaces::ILayer::QueryPtr layer ( layers->layer ( i ) );

    if ( layer.valid () )
    {
      QTreeWidgetItem *item ( new QTreeWidgetItem ( _tree ) );
      item->setText ( 0, layer->name ().c_str () );

      Qt::CheckState state ( layer->showLayer() ? Qt::Checked : Qt::Unchecked );
      item->setCheckState ( 0, state );
      _tree->addTopLevelItem ( item );

      _layerMap.insert ( LayerMap::value_type ( item, layer.get() ) );
    }
  }

  // Re-connect slots.
  this->_connectTreeViewSlots ();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Connect slots and signals for the tree view.
//
///////////////////////////////////////////////////////////////////////////////

void LayersTree::_connectTreeViewSlots ()
{
  // Notify us when an item is double clicked.
  connect ( _tree, SIGNAL ( itemDoubleClicked ( QTreeWidgetItem *, int ) ),
            this,  SLOT   ( _onDoubleClick    ( QTreeWidgetItem *, int ) ) );

  // Notify us when an item is changed.
  connect ( _tree, SIGNAL ( itemChanged       ( QTreeWidgetItem *, int ) ),
            this,  SLOT   ( _onItemChanged    ( QTreeWidgetItem *, int ) ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Item has been double clicked.
//
///////////////////////////////////////////////////////////////////////////////

void LayersTree::_onDoubleClick ( QTreeWidgetItem * item, int columnNumber )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Item has been changed.
//
///////////////////////////////////////////////////////////////////////////////

void LayersTree::_onItemChanged ( QTreeWidgetItem * item, int columnNumber )
{
  if ( 0x0 != item )
  {
    Qt::CheckState state ( item->checkState ( columnNumber ) );
    Layer::RefPtr layer ( _layerMap [ item ] );

    if ( layer.valid () )
    {
      if ( Qt::Checked == state )
      {
        Minerva::Core::Commands::AddLayer::RefPtr addLayer ( new Minerva::Core::Commands::AddLayer ( layer ) );
        addLayer->execute ( _caller );
      }
      else if ( Qt::Unchecked == state )
      {
        Minerva::Core::Commands::RemoveLayer::RefPtr addLayer ( new Minerva::Core::Commands::RemoveLayer ( layer ) );
        addLayer->execute ( _caller );
      }
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  
//
///////////////////////////////////////////////////////////////////////////////

void LayersTree::_onAddLayerClick ()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  
//
///////////////////////////////////////////////////////////////////////////////

void LayersTree::_onRemoveLayerClick ()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  
//
///////////////////////////////////////////////////////////////////////////////

void LayersTree::_onRefreshClick ()
{
}
