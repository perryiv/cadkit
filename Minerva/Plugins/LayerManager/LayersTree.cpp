
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Plugins/LayerManager/LayersTree.h"
#include "Minerva/Plugins/LayerManager/PropertiesAction.h"
#include "Minerva/Core/Commands/RemoveLayer.h"
#include "Minerva/Core/Commands/HideLayer.h"
#include "Minerva/Core/Commands/ShowLayer.h"
#include "Minerva/Interfaces/IDirtyScene.h"
#include "Minerva/Interfaces/IAddLayer.h"
#include "Minerva/Interfaces/IRemoveLayer.h"

#include "Usul/Adaptors/Bind.h"
#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Commands/GenericCommand.h"
#include "Usul/Components/Manager.h"
#include "Usul/Interfaces/ILayerAddGUIQt.h"
#include "Usul/Interfaces/Qt/IMainWindow.h"

#include "QtTools/Action.h"
#include "QtTools/ScopedSignals.h"
#include "QtTools/TreeControl.h"

#include "QtGui/QHeaderView"
#include "QtGui/QTreeWidget"
#include "QtGui/QVBoxLayout"
#include "QtGui/QHBoxLayout"
#include "QtGui/QPushButton"
#include "QtGui/QTabWidget"
#include "QtGui/QDialog"
#include "QtGui/QMainWindow"
#include "QtGui/QMenu"
#include "QtGui/QSlider"

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

LayersTree::LayersTree ( Usul::Interfaces::IUnknown* caller, QWidget * parent ) : 
  BaseClass ( parent ),
  _tree ( 0x0 ),
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
  
  _tree = new QtTools::TreeControl ( caller, parent );
  
  // We want a custom context menu.
  _tree->setContextMenuPolicy ( Qt::CustomContextMenu );
  
  // We want exteneded selection.
  _tree->selectionMode ( QAbstractItemView::ExtendedSelection );
  
  // Add the tree to the layout.
  treeLayout->addWidget ( _tree );
  
  topLayout->addLayout ( buttonLayout );
  topLayout->addLayout ( treeLayout );
  topLayout->addWidget ( new QSlider ( Qt::Horizontal ));

  this->_connectTreeViewSlots ();

  connect ( addLayer,    SIGNAL ( clicked () ), this,     SLOT   ( _onAddLayerClick () ) );
  connect ( removeLayer, SIGNAL ( clicked () ), this,     SLOT   ( _onRemoveLayerClick () ) );
  connect ( refresh,     SIGNAL ( clicked () ), this,     SLOT   ( _onRefreshClick () ) );

  connect ( this,        SIGNAL ( enableWidgets ( bool ) ), addLayer,     SLOT   ( setEnabled ( bool ) ) );
  connect ( this,        SIGNAL ( enableWidgets ( bool ) ), removeLayer,  SLOT   ( setEnabled ( bool ) ) );
  connect ( this,        SIGNAL ( enableWidgets ( bool ) ), refresh,      SLOT   ( setEnabled ( bool ) ) );

  // Disable by default.
  emit enableWidgets( false );
  
  // Hide in release.
#ifdef NDEBUG
  refresh->setVisible ( false );
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

LayersTree::~LayersTree()
{
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

  _tree->buildTree( document );
  
  // See if the correct interface is implemented.
  Minerva::Interfaces::IAddLayer::QueryPtr add ( document );
  Minerva::Interfaces::IRemoveLayer::QueryPtr remove ( document );
  
  emit enableWidgets( add.valid() && remove.valid() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Connect slots and signals for the tree view.
//
///////////////////////////////////////////////////////////////////////////////

void LayersTree::_connectTreeViewSlots ()
{
  connect ( _tree, SIGNAL ( onItemChanged ( QTreeWidgetItem*, int ) ), this, SLOT ( _onItemChanged( QTreeWidgetItem*, int ) ) );
  
  // Notify us when a context menu is requested.
  connect ( _tree, SIGNAL ( customContextMenuRequested ( const QPoint& ) ),
            this,  SLOT   ( _onContextMenuShow ( const QPoint& ) ) );
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
  Usul::Interfaces::IUnknown::QueryPtr unknown ( _tree->unknown ( item ) );
  Minerva::Interfaces::IDirtyScene::QueryPtr dirty ( _document );
  if ( dirty.valid() )
    dirty->dirtyScene ( true, unknown );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a layer.
//
///////////////////////////////////////////////////////////////////////////////

void LayersTree::_onAddLayerClick ()
{
  this->_addLayer ( _document );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a layer.
//
///////////////////////////////////////////////////////////////////////////////

void LayersTree::_addLayer ( Usul::Interfaces::IUnknown *parent )
{
  typedef Usul::Components::Manager PluginManager;
  typedef PluginManager::UnknownSet Unknowns;
  
  Unknowns unknowns ( PluginManager::instance().getInterfaces ( Usul::Interfaces::ILayerAddGUIQt::IID ) );
  
  Usul::Interfaces::Qt::IMainWindow::QueryPtr mw ( _caller );
  
  QDialog dialog ( mw.valid() ? mw->mainWindow() : 0x0 );
  QTabWidget *tabs ( new QTabWidget ( &dialog ) );
  QPushButton *ok ( new QPushButton ( "Ok" ) );
  QPushButton *cancel ( new QPushButton ( "Cancel" ) );
  
  connect ( ok,     SIGNAL ( clicked () ), &dialog, SLOT ( accept () ) );
  connect ( cancel, SIGNAL ( clicked () ), &dialog, SLOT ( reject () ) );
  
  QVBoxLayout *topLayout ( new QVBoxLayout );
  dialog.setLayout ( topLayout );
  
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
    Usul::Interfaces::ILayerAddGUIQt::QueryPtr gui ( (*iter).get() );
    tabs->addTab ( gui->layerAddGUI (), gui->name ().c_str() );
  }
  
  dialog.setModal ( true );
  
  if ( QDialog::Accepted == dialog.exec() )
  {
    for ( Unknowns::iterator iter = unknowns.begin (); iter != unknowns.end(); ++iter )
    {
      Usul::Interfaces::ILayerAddGUIQt::QueryPtr gui ( (*iter).get() );
      gui->apply ( parent, _caller );
    }
  }
  
  this->buildTree ( _document );
}


///////////////////////////////////////////////////////////////////////////////
//
//  
//
///////////////////////////////////////////////////////////////////////////////

void LayersTree::_onRemoveLayerClick ()
{
  // Get the current item.
  QTreeWidgetItem *item ( _tree->currentItem() );
  
  if ( 0x0 == item )
    return;
  
  // Get the unknown for the item.
  Usul::Interfaces::IUnknown::QueryPtr unknown ( _tree->unknown ( item ) );
  Usul::Interfaces::ILayer::QueryPtr layer ( unknown );

  if ( layer.valid () )
  {
    Minerva::Core::Commands::RemoveLayer::RefPtr command ( new Minerva::Core::Commands::RemoveLayer ( layer.get() ) );
    command->execute ( _document );

    _tree->removeItem( item );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  
//
///////////////////////////////////////////////////////////////////////////////

void LayersTree::_onRefreshClick ()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Show the context menu.
//
///////////////////////////////////////////////////////////////////////////////

void LayersTree::_onContextMenuShow ( const QPoint& pos )
{
  if ( 0x0 == _tree )
    return;

  Usul::Interfaces::IUnknown::QueryPtr unknown ( _tree->currentUnknown() );
  Usul::Interfaces::ILayer::QueryPtr layer ( unknown );

  QMenu menu;
  
  Usul::Commands::Command::RefPtr command ( Usul::Commands::genericCommand ( "Add...", Usul::Adaptors::bind1 ( unknown.get(), Usul::Adaptors::memberFunction ( this, &LayersTree::_addLayer ) ), Usul::Commands::TrueFunctor() ) );
  QtTools::Action add ( command.get() );
  menu.addAction ( &add );
  
  Minerva::Interfaces::IAddLayer::QueryPtr al ( unknown );
  add.setEnabled( al.valid() );
  
  if ( layer.valid () )
  {
    QAction favorites ( 0x0 );
    favorites.setText( "Add to favorites" );
    favorites.setToolTip( "Add layer to favorites" );
    QObject::connect ( &favorites, SIGNAL ( triggered() ), this, SLOT ( _onAddLayerFavorites() ) );
    
    PropertiesAction action ( layer.get(), _caller.get() );
    
    menu.addAction ( &favorites );
    menu.addAction ( &action );
    
    //this->buildTree( _document );
  }
  
  menu.exec ( _tree->mapToGlobal ( pos ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add the current layer to the favorites.
//
///////////////////////////////////////////////////////////////////////////////

void LayersTree::_onAddLayerFavorites()
{
  Usul::Interfaces::IUnknown::QueryPtr unknown ( _tree->currentUnknown() );
  emit addLayerFavorites ( unknown.get() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add layer to the tree.
//
///////////////////////////////////////////////////////////////////////////////

void LayersTree::addLayer ( Usul::Interfaces::IUnknown * unknown )
{
  this->buildTree( _document );
}
