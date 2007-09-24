
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Plugins/LayerManager/LayersTree.h"
#include "Minerva/Core/Commands/HideLayer.h"
#include "Minerva/Core/Commands/ShowLayer.h"

#include "Usul/Components/Manager.h"
#include "Usul/Interfaces/ILayerList.h"
#include "Usul/Interfaces/ILayerAddGUIQt.h"
#include "Usul/Interfaces/Qt/IMainWindow.h"

#include "QtGui/QTreeWidget"
#include "QtGui/QVBoxLayout"
#include "QtGui/QHBoxLayout"
#include "QtGui/QPushButton"
#include "QtGui/QTabWidget.h"
#include "QtGui/QDialog.h"
#include "QtGui/QMainWindow.h"

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

LayersTree::LayersTree ( Usul::Interfaces::IUnknown* caller, QWidget * parent ) : 
BaseClass ( parent ),
_tree ( 0x0 ),
_layerMap (),
_caller ( caller ),
_document ()
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
  _document = static_cast < Usul::Interfaces::IUnknown * > ( 0x0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the tree.
//
///////////////////////////////////////////////////////////////////////////////

void LayersTree::buildTree ( Usul::Interfaces::IUnknown * document )
{
  // Save the document;
  _document = document;

  // Clear anything we may have.
  _tree->clear();
  _layerMap.clear ( );

  // See if the correct interface is implemented.
  Usul::Interfaces::ILayerList::QueryPtr layers ( document );
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
        Minerva::Core::Commands::ShowLayer::RefPtr show ( new Minerva::Core::Commands::ShowLayer ( layer ) );
        show->execute ( _document );
      }
      else if ( Qt::Unchecked == state )
      {
        Minerva::Core::Commands::HideLayer::RefPtr hide ( new Minerva::Core::Commands::HideLayer ( layer ) );
        hide->execute ( _document );
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
  try
  {
    typedef Usul::Components::Manager PluginManager;
    typedef PluginManager::UnknownSet Unknowns;

    Unknowns unknowns ( PluginManager::instance().getInterfaces ( Usul::Interfaces::ILayerAddGUIQt::IID ) );

    Usul::Interfaces::Qt::IMainWindow::QueryPtr mw ( _caller );

    QDialog *dialog ( new QDialog ( mw.valid() ? mw->mainWindow() : 0x0 ) );
    QTabWidget *tabs ( new QTabWidget ( dialog ) );
    QPushButton *ok ( new QPushButton ( "Ok" ) );
    QPushButton *cancel ( new QPushButton ( "Cancel" ) );

    connect ( ok,     SIGNAL ( clicked () ), dialog, SLOT ( accept () ) );
    connect ( cancel, SIGNAL ( clicked () ), dialog, SLOT ( reject () ) );

    QVBoxLayout *topLayout ( new QVBoxLayout );
    dialog->setLayout ( topLayout );

    QVBoxLayout *vLayout ( new QVBoxLayout );
    QHBoxLayout *hLayout ( new QHBoxLayout );

    topLayout->addLayout ( vLayout );
    topLayout->addLayout ( hLayout );
    
    vLayout->addWidget ( tabs );
    hLayout->addStretch  ();
    hLayout->addWidget ( ok );
    hLayout->addWidget ( cancel );

    for ( Unknowns::iterator iter = unknowns.begin (); iter != unknowns.end(); ++iter )
    {
      Usul::Interfaces::ILayerAddGUIQt::QueryPtr gui ( *iter );
      tabs->addTab ( gui->layerAddGUI (), gui->name ().c_str() );
    }

    dialog->setModal ( true );
    
    if ( QDialog::Accepted == dialog->exec() )
    {
      for ( Unknowns::iterator iter = unknowns.begin (); iter != unknowns.end(); ++iter )
      {
        Usul::Interfaces::ILayerAddGUIQt::QueryPtr gui ( *iter );
        gui->apply ( _document );
      }
    }

    delete dialog;
    //delete toolbox;

    this->buildTree ( _document );
  }
  catch ( ... )
  {
  }
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
