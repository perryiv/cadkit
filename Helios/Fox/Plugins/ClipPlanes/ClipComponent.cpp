
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The component class.
//
///////////////////////////////////////////////////////////////////////////////

#include "ClipComponent.h"

#include "FoxTools/Headers/TabItem.h"
#include "FoxTools/Headers/VerticalFrame.h"
#include "FoxTools/Headers/TabBook.h"
#include "FoxTools/Headers/Button.h"
#include "FoxTools/Headers/List.h"

#include "FoxTools/Functions/Enable.h"

#include "Usul/Interfaces/Fox/IFoxTabBook.h"
#include "Usul/Interfaces/Fox/IFoxTabItem.h"
#include "Usul/Interfaces/IActiveView.h"
#include "Usul/Interfaces/IClippingPlanes.h"

#include "Usul/Cast/Cast.h"

#include "osg/Group"
#include "osg/ClipNode"
#include "osg/ClipPlane"
#include "osg/Plane"


FXDEFMAP ( ClipComponent ) MessageMap[] = 
{
  //          Message Type     ID                   Message Handler.
  FXMAPFUNC ( FX::SEL_COMMAND, ClipComponent::ID_NEW_CLIP_PLANE,    ClipComponent::onCommandNewClipPlane       ),
  FXMAPFUNC ( FX::SEL_UPDATE,  ClipComponent::ID_NEW_CLIP_PLANE,    ClipComponent::onUpdateNewClipPlane        ),
  FXMAPFUNC ( FX::SEL_COMMAND, ClipComponent::ID_CLIP_LIST,         ClipComponent::onCommandClipList           ),
  FXMAPFUNC ( FX::SEL_UPDATE,  ClipComponent::ID_CLIP_LIST,         ClipComponent::onUpdateClipList            ),
  FXMAPFUNC ( FX::SEL_COMMAND, ClipComponent::ID_DELETE_CLIP_PLANE, ClipComponent::onCommandDeleteClipPlane    ),
  FXMAPFUNC ( FX::SEL_UPDATE,  ClipComponent::ID_DELETE_CLIP_PLANE, ClipComponent::onUpdateDeleteClipPlane     ),
};

FOX_TOOLS_IMPLEMENT ( ClipComponent, FX::FXObject, MessageMap, ARRAYNUMBER ( MessageMap ) );

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( ClipComponent , ClipComponent::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

ClipComponent::ClipComponent() : BaseClass(),
_caller(),
_contents( 0x0 ),
_list ( 0x0 )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

ClipComponent::~ClipComponent()
{
  if( _contents )
    delete _contents;

  _caller = static_cast < Usul::Interfaces::IUnknown* > ( 0x0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *ClipComponent::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::IPlugin::IID:
    return static_cast < Usul::Interfaces::IPlugin* > ( this );
    case Usul::Interfaces::IFoxAddTab::IID:
    return static_cast < Usul::Interfaces::IFoxAddTab* > ( this );
  default:
    return 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  
//
///////////////////////////////////////////////////////////////////////////////

void ClipComponent::foxAddTab( Usul::Interfaces::IUnknown *caller )
{
  Usul::Interfaces::IFoxTabItem::QueryPtr tabItem ( caller );
  Usul::Interfaces::IFoxTabBook::QueryPtr foxTabBook ( caller );

  if( tabItem.valid() && foxTabBook.valid() )
  {
    _caller = caller;

    tabItem->addTab ( "Clip" );
    FX::FXTabItem *clipTab ( tabItem->tabItem( "Clip" ) );

    FX::FXTabBook *tabBook ( foxTabBook->tabBook() );

    _contents = new FX::FXVerticalFrame ( tabBook, LAYOUT_FILL_X|LAYOUT_FILL_Y );

    new FX::FXButton ( _contents, "New Clipping Plane", 0x0, this, ClipComponent::ID_NEW_CLIP_PLANE );
    new FX::FXButton ( _contents, "Delete Clipping Plane", 0x0, this, ClipComponent::ID_DELETE_CLIP_PLANE );

    _list = new FX::FXList   ( _contents, this, ClipComponent::ID_CLIP_LIST, FX::LIST_SINGLESELECT | FX::LAYOUT_FILL_X | FX::LAYOUT_FILL_Y );

  }
}



///////////////////////////////////////////////////////////////////////////////
//
//  
//
///////////////////////////////////////////////////////////////////////////////

long ClipComponent::onCommandNewClipPlane  ( FX::FXObject *, FX::FXSelector, void * )
{
  Usul::Interfaces::IActiveView::QueryPtr activeView ( _caller );
  Usul::Interfaces::IClippingPlanes::QueryPtr clip   ( activeView.valid() ? activeView->getActiveView() : 0x0 );

  if( clip.valid() )
    clip->addClippingPlane();

  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  
//
///////////////////////////////////////////////////////////////////////////////

long ClipComponent::onUpdateNewClipPlane  ( FX::FXObject *object, FX::FXSelector, void * )
{
  Usul::Interfaces::IActiveView::QueryPtr activeView ( _caller );

  FoxTools::Functions::enable ( ( activeView.valid() && activeView->getActiveView() ), object );

  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  
//
///////////////////////////////////////////////////////////////////////////////

long ClipComponent::onCommandClipList      ( FX::FXObject *, FX::FXSelector, void * )
{
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  
//
///////////////////////////////////////////////////////////////////////////////

long ClipComponent::onUpdateClipList      ( FX::FXObject *object, FX::FXSelector, void * )
{
  Usul::Interfaces::IActiveView::QueryPtr activeView ( _caller );

  Usul::Interfaces::IClippingPlanes::QueryPtr clip  ( activeView.valid() ? activeView->getActiveView() : 0x0 );

  if( clip.valid() )
  {
    _list->show();


    if( clip->numClippingPlanes() != _list->getNumItems() )
    {
      _list->clearItems();

      for( unsigned int i = 0; i < clip->numClippingPlanes(); ++i )
      {
        std::ostringstream os;
        os << "Plane " << i;
        _list->appendItem( os.str().c_str() );
      }
    }
  }
  else
  {
    _list->hide();
  }

  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  
//
///////////////////////////////////////////////////////////////////////////////

long ClipComponent::onCommandDeleteClipPlane  ( FX::FXObject *, FX::FXSelector, void * )
{
  if( _list->getNumItems() == 0 )
    return 0;

  Usul::Interfaces::IActiveView::QueryPtr activeView ( _caller );
  Usul::Interfaces::IClippingPlanes::QueryPtr clip   ( activeView.valid() ? activeView->getActiveView() : 0x0 );

  if( clip.valid() )
    clip->removeClippingPlane( _list->getCurrentItem() );

  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  
//
///////////////////////////////////////////////////////////////////////////////

long ClipComponent::onUpdateDeleteClipPlane  ( FX::FXObject *object, FX::FXSelector, void * )
{
  Usul::Interfaces::IActiveView::QueryPtr activeView ( _caller );

  FoxTools::Functions::enable ( ( activeView.valid() && activeView->getActiveView() ), object );

  return 1;
}

