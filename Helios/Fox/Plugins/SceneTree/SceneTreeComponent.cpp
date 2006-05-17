
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The SceneTreeComponent class.
//
///////////////////////////////////////////////////////////////////////////////

#include "SceneTreeComponent.h"

#include "Usul/Interfaces/Fox/IFoxTabItem.h"
#include "Usul/Interfaces/Fox/IFoxTabBook.h"


USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( SceneTreeComponent , SceneTreeComponent::BaseClass );

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

SceneTreeComponent::SceneTreeComponent() : BaseClass(),
  _caller(),
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
  if ( _sceneTree )
    delete _sceneTree;
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
  case Usul::Interfaces::IFoxAddTab::IID:
    return static_cast< Usul::Interfaces::IFoxAddTab*>(this);
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

void SceneTreeComponent::foxAddTab( Usul::Interfaces::IUnknown *caller )
{
  _caller = caller;

  Usul::Interfaces::IFoxTabItem::QueryPtr tabItem ( caller );
  Usul::Interfaces::IFoxTabBook::QueryPtr foxTabBook ( caller );

  if( tabItem.valid() && foxTabBook.valid() )
  {
    tabItem->addTab ( "Scene" );
    FX::FXTabItem *tab ( tabItem->tabItem( "Scene" ) );

    FX::FXTabBook *tabBook ( foxTabBook->tabBook() );
    const unsigned int layout ( FX::FRAME_SUNKEN | FX::FRAME_THICK | FX::LAYOUT_FILL_X | FX::LAYOUT_FILL_Y | FX::LAYOUT_TOP | FX::LAYOUT_RIGHT | FX::TREELIST_SHOWS_LINES | FX::TREELIST_SHOWS_BOXES );
    _sceneTree = new SceneTree ( tabBook, 0x0, 0, layout );

    //FX::FXTreeList *tree = new SceneTree ( tabBook, 0x0, 0, layout ); //new FX::FXTreeList ( tabBook, 0x0, 0, layout );
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
  //if( _sceneTree->id() == 0 )
  //  _sceneTree->create();
  _sceneTree->update( scene );
}


