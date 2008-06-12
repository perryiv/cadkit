
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Plugins/LayerManager/LayersTree.h"
#include "Minerva/Plugins/LayerManager/Favorites.h"
#include "Minerva/Core/Commands/RemoveLayer.h"
#include "Minerva/Core/Commands/HideLayer.h"
#include "Minerva/Core/Commands/ShowLayer.h"
#include "Minerva/Interfaces/IDirtyScene.h"
#include "Minerva/Interfaces/IAddLayer.h"
#include "Minerva/Interfaces/ILookAtLayer.h"
#include "Minerva/Interfaces/IRemoveLayer.h"

#include "QtTools/Action.h"
#include "QtTools/Menu.h"
#include "QtTools/ScopedSignals.h"
#include "QtTools/TreeControl.h"

#include "Usul/Adaptors/Bind.h"
#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Commands/GenericCommand.h"
#include "Usul/Components/Manager.h"
#include "Usul/Interfaces/IDocument.h"
#include "Usul/Interfaces/ILayerAddGUIQt.h"
#include "Usul/Interfaces/ILayerModifyGUIQt.h"
#include "Usul/Interfaces/IRasterAlphas.h"
#include "Usul/Interfaces/Qt/IMainWindow.h"

#include "QtGui/QHeaderView"
#include "QtGui/QTreeWidget"
#include "QtGui/QVBoxLayout"
#include "QtGui/QHBoxLayout"
#include "QtGui/QPushButton"
#include "QtGui/QTabWidget"
#include "QtGui/QDialog"
#include "QtGui/QMainWindow"
#include "QtGui/QMenu"
#include "QtGui/QMessageBox"
#include "QtGui/QSlider"

namespace Detail
{
  const unsigned int SLIDER_STEPS ( 1000 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Typedefs.
//
///////////////////////////////////////////////////////////////////////////////

typedef Usul::Components::Manager PluginManager;
typedef PluginManager::UnknownSet Unknowns;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

LayersTree::LayersTree ( Usul::Interfaces::IUnknown* caller, QWidget * parent ) : 
  BaseClass ( parent ),
  _tree ( 0x0 ),
  _slider ( new QSlider ( Qt::Horizontal ) ),
  _caller ( caller ),
  _document (),
  _favorites ( 0x0 )
{
  QVBoxLayout *topLayout ( new QVBoxLayout ( parent ) );
  this->setLayout ( topLayout );

  QHBoxLayout *buttonLayout ( new QHBoxLayout );
  QVBoxLayout *treeLayout ( new QVBoxLayout );
  
  _tree = new QtTools::TreeControl ( caller, parent );
  
  // We want a custom context menu.
  _tree->setContextMenuPolicy ( Qt::CustomContextMenu );
  
  // We want exteneded selection.
  _tree->selectionMode ( QAbstractItemView::ExtendedSelection );
  
  // Add the tree to the layout.
  treeLayout->addWidget ( _tree );
  
  topLayout->addLayout ( buttonLayout );
  topLayout->addLayout ( treeLayout );
  topLayout->addWidget ( _slider );
  
  _slider->setRange ( 0, Detail::SLIDER_STEPS );
  _slider->setEnabled ( false );

  this->_connectTreeViewSlots ();
  
  connect ( _slider,     SIGNAL ( sliderReleased() ), SLOT ( _onSliderReleased() ) );
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

  _tree->buildTree ( document );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Connect slots and signals for the tree view.
//
///////////////////////////////////////////////////////////////////////////////

void LayersTree::_connectTreeViewSlots ()
{
  connect ( _tree, SIGNAL ( onItemChanged ( QTreeWidgetItem*, int ) ), this, SLOT ( _onItemChanged( QTreeWidgetItem*, int ) ) );
  connect ( _tree->treeWidget(), SIGNAL ( itemSelectionChanged() ), this, SLOT ( _onItemSelectionChanged() ) );
  connect ( _tree->treeWidget(), SIGNAL ( itemDoubleClicked( QTreeWidgetItem*, int ) ), this, SLOT ( _onDoubleClick( QTreeWidgetItem*, int ) ) );
  
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
  Minerva::Interfaces::ILookAtLayer::QueryPtr lookAt ( _document );

  if ( lookAt.valid() )
  {
    Usul::Interfaces::IUnknown::QueryPtr unknown ( _tree->currentUnknown() );
    lookAt->lookAtLayer ( unknown.get() );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Item has been changed.
//
///////////////////////////////////////////////////////////////////////////////

void LayersTree::_onItemChanged ( QTreeWidgetItem * item, int columnNumber )
{
  Usul::Interfaces::IUnknown::QueryPtr unknown ( _tree->unknown ( item ) );
  this->_dirtyAndRedraw ( unknown );
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
    tabs->addTab ( gui->layerAddGUI ( _caller ), gui->name ().c_str() );
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
  // Get all selected items.
  typedef QtTools::TreeControl::TreeWidgetItems Items;
  Items items ( _tree->selectedItems() );

  if ( items.size() > 0 )
  {
    std::string message ( Usul::Strings::format ( "Delete ", items.size(), " items?" ) );
    if ( QMessageBox::Ok != QMessageBox::question ( this, "Confirm", message.c_str(), QMessageBox::Ok | QMessageBox::Cancel ) )
      return;
  }
  
  // Loop through all items.
  for ( Items::iterator iter = items.begin(); iter != items.end(); ++iter )
  {
    // Get the item.
    QTreeWidgetItem *item ( *iter );
    
    // Get the unknown for the item.
    Usul::Interfaces::IUnknown::QueryPtr unknown ( _tree->unknown ( item ) );
    Usul::Interfaces::IUnknown::QueryPtr parent ( _tree->unknown ( item->parent() ) );
    Usul::Interfaces::ILayer::QueryPtr layer ( unknown );

    if ( layer.valid () && parent.valid() )
    {
      Minerva::Core::Commands::RemoveLayer::RefPtr command ( new Minerva::Core::Commands::RemoveLayer ( layer.get() ) );
      command->execute ( parent );

      _tree->removeItem( item );
      this->_dirtyAndRedraw ( unknown.get() );
    }
  }
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
  
  // Add button.
  QtTools::Action add ( command.get() );
  
  Minerva::Interfaces::IAddLayer::QueryPtr al ( unknown );
  add.setEnabled( al.valid() );

  // Remove button.
  QAction remove ( 0x0 );
  remove.setText ( "Remove" );
  remove.setToolTip ( "Remove selected layers." );
  QObject::connect ( &remove, SIGNAL ( triggered() ), this, SLOT ( _onRemoveLayerClick() ) );
  
  QAction favorites ( 0x0 );
  favorites.setText( "Add to favorites" );
  favorites.setToolTip( "Add layer to favorites" );
  favorites.setEnabled ( layer.valid() );
  QObject::connect ( &favorites, SIGNAL ( triggered() ), this, SLOT ( _onAddLayerFavorites() ) );

  // Editor for this layer.
  Usul::Interfaces::IUnknown::QueryPtr editor;

  // Attempt to find an editor.
  Unknowns unknowns ( PluginManager::instance().getInterfaces ( Usul::Interfaces::ILayerModifyGUIQt::IID ) );
  for ( Unknowns::iterator iter = unknowns.begin (); iter != unknowns.end(); ++iter )
  {
    Usul::Interfaces::ILayerModifyGUIQt::QueryPtr gui ( (*iter).get() );
    if ( gui->handle ( layer.get() ) )
    {
      editor = gui.get();
      break;
    }
  }
  
  QtTools::Action properties ( Usul::Commands::genericCommand ( "Properties...", Usul::Adaptors::bind2 ( unknown.get(), editor.get(), Usul::Adaptors::memberFunction ( this, &LayersTree::_editLayerProperties ) ), Usul::Commands::TrueFunctor() ) );
  properties.setToolTip ( tr ( "Show the property dialog for this layer" ) );
  properties.setEnabled ( layer.valid() && editor.valid() );
  
  // Add the actions to the menu.
  menu.addAction ( &add );
  menu.addAction ( &remove );
  menu.addAction ( &favorites );

  QtTools::Menu addFromFavorites ( "Add From Favorites" );

  if ( 0x0 != this->favorites() && al.valid() )
  {
    MenuKit::Menu::RefPtr subMenu ( this->favorites()->menu( unknown.get() ) );
    addFromFavorites.menu ( subMenu );
    menu.addMenu ( &addFromFavorites );
  }

  menu.addAction ( &properties );
  
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


///////////////////////////////////////////////////////////////////////////////
//
//  Edit layer properties.
//
///////////////////////////////////////////////////////////////////////////////

void LayersTree::_editLayerProperties ( Usul::Interfaces::IUnknown *unknown, Usul::Interfaces::IUnknown *editor )
{  
  Usul::Interfaces::ILayer::QueryPtr layer ( unknown );
  Usul::Interfaces::ILayerModifyGUIQt::QueryPtr gui ( editor );
  
  // Make sure we have a valid layer.
  if ( false == layer.valid() || false == gui.valid() )
    return;
  
  if ( gui->handle ( layer.get() ) )
  {
    gui->showModifyGUI( layer.get(), _document.get() );  
    QTreeWidgetItem *item ( _tree->currentItem() );
    item->setText( 0, layer->name().c_str() );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  The slider has been released.
//
///////////////////////////////////////////////////////////////////////////////

void LayersTree::_onSliderReleased()
{
  Usul::Interfaces::IUnknown::QueryPtr unknown ( _tree->currentUnknown() );
  Usul::Interfaces::IRasterAlphas::QueryPtr ra ( unknown );
  if ( ra.valid() )
  {
    const float alpha ( static_cast<float> ( _slider->value() ) / Detail::SLIDER_STEPS );
    ra->alpha ( alpha );
    this->_dirtyAndRedraw ( unknown );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  The selection has changed.
//
///////////////////////////////////////////////////////////////////////////////

void LayersTree::_onItemSelectionChanged()
{
  Usul::Interfaces::IUnknown::QueryPtr unknown ( _tree->currentUnknown() );
  Usul::Interfaces::IRasterAlphas::QueryPtr ra ( unknown );
  _slider->setEnabled( ra.valid() );
  
  if ( ra.valid() )
    _slider->setValue ( static_cast<int> ( ra->alpha() * Detail::SLIDER_STEPS ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Dirty and request a redraw.
//
///////////////////////////////////////////////////////////////////////////////

void LayersTree::_dirtyAndRedraw ( Usul::Interfaces::IUnknown *unknown )
{
  Minerva::Interfaces::IDirtyScene::QueryPtr dirty ( _document );
  if ( dirty.valid() )
    dirty->dirtyScene ( true, unknown );
  
  Usul::Interfaces::IDocument::QueryPtr document ( _document );
  if ( document.valid() )
    document->requestRedraw();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the favorites.
//
///////////////////////////////////////////////////////////////////////////////

void LayersTree::favorites ( Favorites* favorites )
{
  _favorites = favorites;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the favorites.
//
///////////////////////////////////////////////////////////////////////////////

Favorites* LayersTree::favorites() const
{
  return _favorites;
}
