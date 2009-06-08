
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
#include "Minerva/Interfaces/IRefreshData.h"
#include "Minerva/Interfaces/IRemoveLayer.h"
#include "Minerva/Interfaces/ISwapLayers.h"

#include "QtTools/Action.h"
#include "QtTools/Menu.h"
#include "QtTools/ScopedSignals.h"
#include "QtTools/TreeControl.h"
#include "QtTools/TreeNode.h"

#include "Usul/Adaptors/Bind.h"
#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Commands/GenericCommand.h"
#include "Usul/Components/Manager.h"
#include "Usul/Interfaces/IDocument.h"
#include "Usul/Interfaces/ILayerAddGUIQt.h"
#include "Usul/Interfaces/ILayerModifyGUIQt.h"
#include "Usul/Interfaces/IRasterAlphas.h"
#include "Usul/Interfaces/Qt/IMainWindow.h"

#include "QtGui/QVBoxLayout"
#include "QtGui/QHBoxLayout"
#include "QtGui/QPushButton"
#include "QtGui/QTabWidget"
#include "QtGui/QDialog"
#include "QtGui/QMainWindow"
#include "QtGui/QMenu"
#include "QtGui/QMessageBox"
#include "QtGui/QSlider"

using namespace Minerva;

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
  _tree ( new QtTools::TreeControl ( parent ) ),
  _slider ( new QSlider ( Qt::Horizontal ) ),
  _caller ( caller ),
  _document (),
  _favorites ( 0x0 )
{
  QVBoxLayout *topLayout ( new QVBoxLayout ( parent ) );
  this->setLayout ( topLayout );

  QHBoxLayout *buttonLayout ( new QHBoxLayout );
  QVBoxLayout *treeLayout ( new QVBoxLayout );
  
  // We want a custom context menu.
  _tree->setContextMenuPolicy ( Qt::CustomContextMenu );
  
  // We want exteneded selection.
  _tree->setSelectionMode ( QAbstractItemView::ExtendedSelection );
  
  // Add the tree to the layout.
  treeLayout->addWidget ( _tree );
  
  topLayout->addLayout ( buttonLayout );
  topLayout->addLayout ( treeLayout );
  topLayout->addWidget ( _slider );

  _slider->setRange ( 0, Detail::SLIDER_STEPS );
  _slider->setEnabled ( false );

  // Connect signals and slots for TreeControl.
  QObject::connect ( _tree, SIGNAL ( onTreeNodeChanged ( TreeNode * ) ), this, SLOT ( _onTreeNodeChanged ( TreeNode * ) ) );
  QObject::connect ( _tree, SIGNAL ( onSelectionChanged() ),  this, SLOT ( _onItemSelectionChanged() ) );
  QObject::connect ( _tree, SIGNAL ( onTreeNodeDoubleClicked( TreeNode * ) ), this, SLOT ( _onDoubleClick ( TreeNode * ) ) );
  QObject::connect ( _tree, SIGNAL ( customContextMenuRequested ( const QPoint& ) ), this,  SLOT   ( _onContextMenuShow ( const QPoint& ) ) );
  
  // Connect the slider.
  QObject::connect ( _slider,     SIGNAL ( sliderReleased() ), SLOT ( _onSliderReleased() ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

LayersTree::~LayersTree()
{
  this->clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear the members.
//
///////////////////////////////////////////////////////////////////////////////

void LayersTree::clear()
{
  _caller = static_cast < Usul::Interfaces::IUnknown * > ( 0x0 );
  _document = static_cast < Usul::Interfaces::IUnknown * > ( 0x0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Item has been double clicked.
//
///////////////////////////////////////////////////////////////////////////////

void LayersTree::_onDoubleClick ( TreeNode * node )
{
  Minerva::Interfaces::ILookAtLayer::QueryPtr lookAt ( _document );
  
  if ( lookAt.valid() )
  {
    Usul::Interfaces::IUnknown::QueryPtr unknown ( 0x0 != node ? node->node().get() : 0x0 );
    lookAt->lookAtLayer ( unknown.get() );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Item has been changed.
//
///////////////////////////////////////////////////////////////////////////////

void LayersTree::_onTreeNodeChanged ( TreeNode * node )
{
  Usul::Interfaces::IUnknown::QueryPtr unknown ( 0x0 != node ? node->node().get() : 0x0 );
  this->_dirtyAndRedraw ( unknown );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the tree.
//
///////////////////////////////////////////////////////////////////////////////

void LayersTree::buildTree ( Usul::Interfaces::IUnknown * document )
{
  Usul::Interfaces::ITreeNode::QueryPtr node ( document );

  if ( 0x0 != _tree )
    _tree->setRootNode ( new TreeNode ( node.get() ) );
  
  // Save the document;
  _document = document;
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
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove selected layers.
//
///////////////////////////////////////////////////////////////////////////////

void LayersTree::_removeSelectedLayers()
{
  if ( 0x0 == _tree )
    return;
    
  // Get all selected items.
  typedef QtTools::TreeControl::TreeNodeList Items;
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
    TreeNode *node ( *iter );
    
    if ( 0x0 != node )
    {
      // Get the unknown for the item.
      Usul::Interfaces::IUnknown::QueryPtr unknown ( node->node().get() );
      Usul::Interfaces::IUnknown::QueryPtr parent ( 0x0 != node->parent() ? node->parent()->node().get() : 0x0 );
      Usul::Interfaces::IUnknown::QueryPtr layer ( unknown );
    
      if ( layer.valid () && parent.valid() )
      {
        Minerva::Core::Commands::RemoveLayer::RefPtr command ( new Minerva::Core::Commands::RemoveLayer ( layer.get() ) );
        command->execute ( parent );
      
        _tree->removeNode ( node );
        this->_dirtyAndRedraw ( unknown.get() );
      }
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
  
  TreeNode *currentItem ( _tree->currentNode() );
  
  if ( 0x0 == currentItem )
    return;
  
  TreeNode *parentItem ( currentItem->parent() );
  
  Usul::Interfaces::IUnknown::QueryPtr unknown ( currentItem->node().get() );
  Usul::Interfaces::IUnknown::QueryPtr parent ( 0x0 != parentItem ? parentItem->node().get() : 0x0 );

  QMenu menu;
  
  // Add button.
  QtTools::Action add ( USUL_MAKE_COMMAND_ARG0 ( "Add...", "", this, &LayersTree::_addLayer, unknown.get() ) );
  
  Minerva::Interfaces::IAddLayer::QueryPtr al ( unknown );
  add.setEnabled( al.valid() );
  
  // Remove button.
  QtTools::Action remove ( USUL_MAKE_COMMAND ( "Remove", "", this, &LayersTree::_removeSelectedLayers ) );
  remove.setText ( "Remove" );
  remove.setToolTip ( "Remove selected layers." );
  
  QAction favorites ( 0x0 );
  favorites.setText( "Add to favorites" );
  favorites.setToolTip( "Add layer to favorites" );
  favorites.setEnabled ( unknown.valid() );
  QObject::connect ( &favorites, SIGNAL ( triggered() ), this, SLOT ( _onAddLayerFavorites() ) );
  
  // Move up and down actions.
  QtTools::Action moveUp   ( USUL_MAKE_COMMAND_ARG0 ( "Move up",   "", this, &LayersTree::_moveLayerUp,   currentItem ) );
  QtTools::Action moveDown ( USUL_MAKE_COMMAND_ARG0 ( "Move down", "", this, &LayersTree::_moveLayerDown, currentItem ) );
  moveUp.setEnabled   ( this->_canMoveLayerUp   ( currentItem ) );
  moveDown.setEnabled ( this->_canMoveLayerDown ( currentItem ) );
  
  // Add refresh button.
  QtTools::Action refresh ( USUL_MAKE_COMMAND_ARG0 ( "Refresh", "", this, &LayersTree::_refreshLayer, unknown.get() ) );
  
  // Editor for this layer.
  Usul::Interfaces::IUnknown::QueryPtr editor ( this->_findEditor ( unknown.get() ) );
  
  // Properties button.
  QtTools::Action properties ( Usul::Commands::genericCommand ( "Properties...", Usul::Adaptors::bind3 ( unknown.get(), parent.get(), editor.get(), Usul::Adaptors::memberFunction ( this, &LayersTree::_editLayerProperties ) ), Usul::Commands::TrueFunctor() ) );
  properties.setToolTip ( tr ( "Show the property dialog for this layer" ) );
  properties.setEnabled ( unknown.valid() && editor.valid() );
  
  // Add the actions to the menu.
  menu.addAction ( &add );
  menu.addAction ( &remove );
  menu.addAction ( &moveUp );
  menu.addAction ( &moveDown );
  
  // Add the refresh button if we can.
  if ( this->_canRefreshLayer ( unknown.get() ) )
    menu.addAction( &refresh );
  
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
//  Find an editor for the unknown.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown* LayersTree::_findEditor ( Usul::Interfaces::IUnknown* unknown )
{
  // Attempt to find an editor.
  Unknowns unknowns ( PluginManager::instance().getInterfaces ( Usul::Interfaces::ILayerModifyGUIQt::IID ) );
  for ( Unknowns::iterator iter = unknowns.begin (); iter != unknowns.end(); ++iter )
  {
    Usul::Interfaces::ILayerModifyGUIQt::QueryPtr gui ( (*iter).get() );
    if ( gui->handle ( unknown ) )
    {
      return gui.get();
    }
  }
  
  return 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add the current layer to the favorites.
//
///////////////////////////////////////////////////////////////////////////////

void LayersTree::_onAddLayerFavorites()
{
  TreeNode* node ( 0x0 != _tree ? _tree->currentNode() : 0x0 );
  Usul::Interfaces::IUnknown::QueryPtr unknown ( 0x0 != node ? node->node().get() : 0x0 );
  emit addLayerFavorites ( unknown.get() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Edit layer properties.
//
///////////////////////////////////////////////////////////////////////////////

void LayersTree::_editLayerProperties ( IUnknown *unknown, IUnknown *parent, IUnknown *editor )
{  
  Usul::Interfaces::ILayerModifyGUIQt::QueryPtr gui ( editor );
  
  // Make sure we have a valid editor.
  if ( false == gui.valid() )
    return;
  
  if ( gui->handle ( unknown ) )
  {
    gui->showModifyGUI ( unknown, parent, _document.get() );
    
#if 0
    //TreeNode *item ( _tree->currentNode() );
    //item->setText( 0, layer->name().c_str() );
#endif
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  The slider has been released.
//
///////////////////////////////////////////////////////////////////////////////

void LayersTree::_onSliderReleased()
{
  TreeNode* node ( 0x0 != _tree ? _tree->currentNode() : 0x0 );
  Usul::Interfaces::IUnknown::QueryPtr unknown ( 0x0 != node ? node->node().get() : 0x0 );
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
  TreeNode* node ( 0x0 != _tree ? _tree->currentNode() : 0x0 );
  Usul::Interfaces::IUnknown::QueryPtr unknown ( 0x0 != node ? node->node().get() : 0x0 );
  Usul::Interfaces::IRasterAlphas::QueryPtr ra ( unknown );
  _slider->setEnabled( ra.valid() );
  
  if ( ra.valid() )
    _slider->setValue ( static_cast<int> ( ra->alpha() * Detail::SLIDER_STEPS ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Move the given layer up.
//
///////////////////////////////////////////////////////////////////////////////

void LayersTree::_moveLayerUp ( TreeNode *item )
{
  if ( 0x0 == item || 0x0 == _tree )
    return;
  
  // Get the parent and the sibling.
  TreeNode *parent ( item->parent() );
  
  if ( 0x0 == parent )
    return;
  
  int index ( parent->indexOfChild ( item ) ); --index;
  TreeNode *sibling ( parent->child ( index ) );
  
  this->_swapLayers( item, sibling, parent );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Move the given layer down.
//
///////////////////////////////////////////////////////////////////////////////

void LayersTree::_moveLayerDown ( TreeNode *item )
{
  if ( 0x0 == item || 0x0 == _tree )
    return;
  
  // Get the parent and the sibling.
  TreeNode *parent ( item->parent() );
  
  if ( 0x0 == parent )
    return;
  
  int index ( parent->indexOfChild ( item ) ); ++index;
  TreeNode *sibling ( parent->child ( index ) );
  
  this->_swapLayers( item, sibling, parent );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Swap two layers.
//
///////////////////////////////////////////////////////////////////////////////

void LayersTree::_swapLayers ( TreeNode *item0, TreeNode *item1, TreeNode *parent )
{
  // Check input.
  if ( 0x0 == item0 || 0x0 == item1 || 0x0 == parent || 0x0 == _tree )
    return;

  Minerva::Interfaces::ISwapLayers::QueryPtr swap ( parent->node().get() );
  Usul::Interfaces::IUnknown::QueryPtr unknown0 ( item0->node().get() );
  Usul::Interfaces::IUnknown::QueryPtr unknown1 ( item1->node().get() );

  if ( swap.valid() && unknown0.valid() && unknown1.valid() )
  {
    swap->swapLayers ( unknown0, unknown1 );

    const int index0 ( parent->indexOfChild ( item0 ) );
    const int index1 ( parent->indexOfChild ( item1 ) );

    parent->swap ( index0, index1 );

    this->_dirtyAndRedraw ( unknown0 );
    this->_dirtyAndRedraw ( unknown1 );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Can the layer be moved up?
//
///////////////////////////////////////////////////////////////////////////////

bool LayersTree::_canMoveLayerUp ( TreeNode *item ) const
{
  if ( 0x0 == item || 0x0 == _tree )
    return false;
  
  // Get the parent.
  TreeNode *parent ( item->parent() );
  
  Minerva::Interfaces::ISwapLayers::QueryPtr swap ( 0x0 != parent ? parent->node().get() : 0x0 );
  const int index ( 0x0 != parent ? parent->indexOfChild ( item ) : -1 );
  return swap.valid() && index > 0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Can the layer be moved down?
//
///////////////////////////////////////////////////////////////////////////////

bool LayersTree::_canMoveLayerDown ( TreeNode *item ) const
{
  if ( 0x0 == item || 0x0 == _tree )
    return false;
  
  // Get the parent.
  TreeNode *parent ( item->parent() );
  
  Minerva::Interfaces::ISwapLayers::QueryPtr swap ( 0x0 != parent ? parent->node().get() : 0x0 );
  const int index ( 0x0 != parent ? parent->indexOfChild ( item ) : -1 );
  return swap.valid() && ( index + 1 ) < parent->childCount();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Can the layer be refreshed?
//
///////////////////////////////////////////////////////////////////////////////

bool LayersTree::_canRefreshLayer ( Usul::Interfaces::IUnknown *unknown ) const
{
  Minerva::Interfaces::IRefreshData::QueryPtr rd ( unknown );
  return rd.valid();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Refresh the layer.
//
///////////////////////////////////////////////////////////////////////////////

void LayersTree::_refreshLayer ( Usul::Interfaces::IUnknown *unknown )
{
  Minerva::Interfaces::IRefreshData::QueryPtr rd ( unknown );
  if ( rd.valid() )
    rd->refreshData();
  
  // Force a render.
  this->_dirtyAndRedraw ( unknown );
}
