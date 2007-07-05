
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The SceneTreeComponent class.
//
///////////////////////////////////////////////////////////////////////////////

#include "SceneTreeComponent.h"

#include "Usul/Interfaces/Qt/IMainWindow.h"

#include "osg/Node"
#include "osg/Drawable"
#include "osg/Group"
#include "osg/Geode"

#include "QtGui/QTreeWidget"
#include "QtGui/QMainWindow"
#include "QtGui/QDockWidget"
#include "QtGui/QHeaderView"


USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( SceneTreeComponent , SceneTreeComponent::BaseClass );

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

SceneTreeComponent::SceneTreeComponent() : BaseClass(),
  _caller(),
  _dock ( 0x0 ),
  _sceneTree ( 0x0 )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

SceneTreeComponent::~SceneTreeComponent()
{
  // Clear the SceneTree
  _sceneTree->clear();

  // This removes the top level item.  We only add to the top level.
  _sceneTree->takeTopLevelItem ( 0 );

  Usul::Interfaces::Qt::IMainWindow::QueryPtr mainWindow ( _caller );

  if( mainWindow.valid() )
  {
    // Remove the DockWidget from the MainWindow.
    QMainWindow * main  ( mainWindow->mainWindow() );
    main->removeDockWidget ( _dock );
  }

  _dock->setWidget ( 0x0 );

  // Delete the DockWidget.
  delete _dock;
  _dock = 0x0;

  // Qt deletes the scene tree.
  _sceneTree = 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *SceneTreeComponent::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::IAddDockWindow::IID:
    return static_cast< Usul::Interfaces::IAddDockWindow*>(this);
  case Usul::Interfaces::IUpdateTreeControls::IID:
    return static_cast< Usul::Interfaces::IUpdateTreeControls*> ( this );
  case Usul::Interfaces::IPlugin::IID:
    return static_cast < Usul::Interfaces::IPlugin*>(this);
  default:
    return 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add the tab for the scene tree.
//
///////////////////////////////////////////////////////////////////////////////

void SceneTreeComponent::addDockWindow ( Usul::Interfaces::IUnknown *caller )
{
  _caller = caller;

  Usul::Interfaces::Qt::IMainWindow::QueryPtr mainWindow ( caller );

  if( mainWindow.valid() )
  {
    QMainWindow * main  ( mainWindow->mainWindow() );

    // Build the docking window.
    _dock = new QDockWidget ( QObject::tr ( "Scene" ), main );
    _dock->setAllowedAreas ( Qt::AllDockWidgetAreas );

    // Create the tree for the scene graph.
    _sceneTree = new QTreeWidget ( _dock );

    // Hide the header.
    _sceneTree->header()->setHidden ( 1 );

    // Add the dock to the main window.
    _dock->setWidget( _sceneTree );
    main->addDockWidget ( Qt::LeftDockWidgetArea, _dock );
  } 
}


namespace Detail
{

  ///////////////////////////////////////////////////////////////////////////////
  //
  //  Add the tree item.
  //
  ///////////////////////////////////////////////////////////////////////////////

  QTreeWidgetItem *createTreeItem ( osg::Node *node, QTreeWidgetItem *parent )
  {
    // Handle bad input.
    if ( 0x0 == node )
      return 0x0;

    // Get the item text.
    QString name ( ( node->getName().empty() ) ? "" : node->getName().c_str() );
    QString cn   ( node->className() );
    QString text ( ( name.isEmpty() ) ? cn : cn + " (" + name + ")" );

    // Add the item.
    QTreeWidgetItem *item ( new QTreeWidgetItem ( parent ) );
    item->setText ( 0, text );
    
    return item;
  }


  ///////////////////////////////////////////////////////////////////////////////
  //
  //  Add the tree item.
  //
  ///////////////////////////////////////////////////////////////////////////////

  QTreeWidgetItem *createTreeItem ( osg::Drawable *drawable, QTreeWidgetItem *parent )
  {
    // Handle bad input.
    if ( 0x0 == drawable )
      return 0x0;

    // Add the item.
    QTreeWidgetItem *item ( new QTreeWidgetItem ( parent ) );
    item->setText( 0, drawable->className() );

    return item;
  }


  ///////////////////////////////////////////////////////////////////////////////
  //
  //  Build the scene-tree control.
  //
  ///////////////////////////////////////////////////////////////////////////////

  QTreeWidgetItem * buildSceneTree ( osg::Node *node, QTreeWidgetItem *parent )
  {
    // Handle bad input.
    if ( 0x0 == node )
      return 0x0;

    // See if the node is a group.
    osg::Group *group = dynamic_cast < osg::Group * > ( node );
    if ( group )
    {
      parent = Detail::createTreeItem ( group, parent );
      for ( unsigned int i = 0; i < group->getNumChildren(); ++i )
        Detail::buildSceneTree ( group->getChild ( i ), parent );
      return parent;
    }

    // See if the node is a geode.
    osg::Geode *geode = dynamic_cast < osg::Geode * > ( node );
    if ( geode )
    {
      parent = Detail::createTreeItem ( geode, parent );
      for ( unsigned int i = 0; i < geode->getNumDrawables(); ++i )
        Detail::createTreeItem ( geode->getDrawable ( i ), parent );
      return parent;
    }

    // Otherwise...
    else
    {
      // If we get this far then it's a node.
      return Detail::createTreeItem ( node, parent );
    }

    return 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update this scene tree.
//
///////////////////////////////////////////////////////////////////////////////

void SceneTreeComponent::updateTreeControls( osg::Node *scene  )
{
  if( 0x0 == _sceneTree )
    return;
  
  _sceneTree->clear();

  // This removes the top level item.  We only add to the top level.
  _sceneTree->takeTopLevelItem ( 0 );

  QTreeWidgetItem *root ( Detail::buildSceneTree ( scene, 0x0 ) );
  _sceneTree->addTopLevelItem ( root );
}

