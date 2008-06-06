
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

#include "EditPolygonComponent.h"
#include "Primitive.h"
#include "Geometry.h"
#include "Islands.h"
#include "AddTriangle.h"
#include "FlipNormal.h"

#include "FoxTools/Icons/Factory.h"
#include "FoxTools/Cursors/Factory.h"
#include "FoxTools/ToolBar/Bar.h"
#include "FoxTools/Functions/Enable.h"
#include "FoxTools/Functions/Check.h"
#include "FoxTools/Headers/MenuRadio.h"
#include "FoxTools/Headers/MenuPane.h"
#include "FoxTools/Headers/Button.h"
#include "FoxTools/Headers/MenuCascade.h"
#include "FoxTools/Menu/RadioButton.h"

#include "Usul/Cast/Cast.h"
#include "Usul/Errors/Stack.h"
#include "Usul/State/Busy.h"

#include "Usul/Interfaces/IActiveView.h"
#include "Usul/Interfaces/ISetTool.h"
#include "Usul/Interfaces/ICleanUp.h"


USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( EditPolygonComponent, EditPolygonComponent::BaseClass );


FXDEFMAP ( EditPolygonComponent ) DeletionMap[] = 
{
  //          Message Type     ID                                     Message Handler.
  FXMAPFUNC ( FX::SEL_COMMAND, EditPolygonComponent::ID_DELETE_PRIMITIVE,        EditPolygonComponent::onCommandDeletePrimitive       ),
  FXMAPFUNC ( FX::SEL_UPDATE,  EditPolygonComponent::ID_DELETE_PRIMITIVE,        EditPolygonComponent::onUpdateDeletePrimitive        ),
  FXMAPFUNC ( FX::SEL_COMMAND, EditPolygonComponent::ID_DELETE_GEOMETRY,         EditPolygonComponent::onCommandDeleteGeometry        ),
  FXMAPFUNC ( FX::SEL_UPDATE,  EditPolygonComponent::ID_DELETE_GEOMETRY,         EditPolygonComponent::onUpdateDeleteGeometry         ),
  FXMAPFUNC ( FX::SEL_COMMAND, EditPolygonComponent::ID_KEEP_CONNECTED,          EditPolygonComponent::onCommandKeepConnected         ),
  FXMAPFUNC ( FX::SEL_UPDATE,  EditPolygonComponent::ID_KEEP_CONNECTED,          EditPolygonComponent::onUpdateKeepConnected          ),
  FXMAPFUNC ( FX::SEL_COMMAND, EditPolygonComponent::ID_CAP_POLYGONS,            EditPolygonComponent::onCommandCapPolygons           ),
  FXMAPFUNC ( FX::SEL_UPDATE,  EditPolygonComponent::ID_CAP_POLYGONS,            EditPolygonComponent::onUpdateCapPolygons            ),
  FXMAPFUNC ( FX::SEL_COMMAND, EditPolygonComponent::ID_ADD_TRIANGLE,            EditPolygonComponent::onCommandAddTriangle           ),
  FXMAPFUNC ( FX::SEL_UPDATE,  EditPolygonComponent::ID_ADD_TRIANGLE,            EditPolygonComponent::onUpdateAddTriangle            ),
  FXMAPFUNC ( FX::SEL_COMMAND, EditPolygonComponent::ID_FLIP_NORMAL,             EditPolygonComponent::onCommandFlipNormal            ),
  FXMAPFUNC ( FX::SEL_UPDATE,  EditPolygonComponent::ID_FLIP_NORMAL,             EditPolygonComponent::onUpdateFlipNormal             ),
};

FOX_TOOLS_IMPLEMENT ( EditPolygonComponent, FX::FXObject, DeletionMap, ARRAYNUMBER ( DeletionMap ) );

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

EditPolygonComponent::EditPolygonComponent() : BaseClass(),
  _caller          ( ), 
  _deletePrimitive ( ), 
  _deleteGeometry  ( ),
  _keepConnected   ( ),
  _addTriangle     ( ),
  _capPolygons     ( )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

EditPolygonComponent::~EditPolygonComponent()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *EditPolygonComponent::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::IFoxSubMenu::IID:
    return static_cast <Usul::Interfaces::IFoxSubMenu*>(this);
  case Usul::Interfaces::IFoxToolbar::IID:
    return static_cast < Usul::Interfaces::IFoxToolbar*>(this);
  case Usul::Interfaces::IPlugin::IID:
    return static_cast < Usul::Interfaces::IPlugin*>(this);
  default:
    return 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the sub-menu.
//
///////////////////////////////////////////////////////////////////////////////

void EditPolygonComponent::buildSubMenu( Usul::Interfaces::IUnknown *caller, FoxTools::Menu::Group *menuPane  )
{
  this->_init( caller );

  FoxTools::Menu::Group::ValidRefPtr group ( new FoxTools::Menu::Group ( "Edit Polygons" ) );

  group->append ( new FoxTools::Menu::RadioButton ( "Delete Polygon", "", "Delete Polygon that was clicked on", this, EditPolygonComponent::ID_DELETE_PRIMITIVE ) );
  group->append ( new FoxTools::Menu::RadioButton ( "Delete All Connected", "", "Delete all primitives connected to the one that was clicked on", this,  EditPolygonComponent::ID_DELETE_GEOMETRY ) );
  group->append ( new FoxTools::Menu::RadioButton ( "Keep All Connected", "", "Keep all primitives connected to the one that was clicked on", this, EditPolygonComponent::ID_KEEP_CONNECTED ) );
  group->append ( new FoxTools::Menu::RadioButton ( "Add Triangle", "", "Add a triangle", this, EditPolygonComponent::ID_ADD_TRIANGLE ) );
  group->append ( new FoxTools::Menu::RadioButton ( "Flip Normal", "", "Flip a normal", this, EditPolygonComponent::ID_FLIP_NORMAL ) );
  group->append ( new FoxTools::Menu::Button ( "Find uncapped loops", "", "Find loops that need to be filled in", this, EditPolygonComponent::ID_CAP_POLYGONS ) );

  menuPane->append( group );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add the buttons to the tool-bar.
//
///////////////////////////////////////////////////////////////////////////////

void EditPolygonComponent::addButtons ( Usul::Interfaces::IUnknown* caller, FoxTools::ToolBar::Bar* toolbar )
{
  this->_init( caller );

  typedef FoxTools::Icons::Factory Icons;
  typedef FoxTools::ToolBar::ToggleButton ToggleButton;
  toolbar->append ( new ToggleButton ( "Add Triangle",         "Add Triangle.",                  Icons::ICON_TRIANGLE,         this, EditPolygonComponent::ID_ADD_TRIANGLE ) );
  toolbar->append ( new ToggleButton ( "Delete Polygon",       "Delete selected polygon.",       Icons::ICON_NO_TRIANGLE,      this, EditPolygonComponent::ID_DELETE_PRIMITIVE ) );
  toolbar->append ( new ToggleButton ( "Delete All Connected", "Delete all connected polygons.", Icons::ICON_DELETE_CONNECTED, this, EditPolygonComponent::ID_DELETE_GEOMETRY ) );
  toolbar->append ( new ToggleButton ( "Keep All Connected",   "Keep all connected polygons.",   Icons::ICON_KEEP_CONNECTED,   this, EditPolygonComponent::ID_KEEP_CONNECTED ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Load delete primitive tool
//
///////////////////////////////////////////////////////////////////////////////

long EditPolygonComponent::onCommandDeletePrimitive ( FX::FXObject *, FX::FXSelector, void * )
{
  // Delegate
  return this->_onCommand ( _deletePrimitive );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Load the delete all connected tool
//
///////////////////////////////////////////////////////////////////////////////

long EditPolygonComponent::onCommandDeleteGeometry ( FX::FXObject *, FX::FXSelector, void * )
{
  // Delegate
  return this->_onCommand ( _deleteGeometry );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Load the keep all connected tool
//
///////////////////////////////////////////////////////////////////////////////

long EditPolygonComponent::onCommandKeepConnected ( FX::FXObject *, FX::FXSelector, void * )
{
  // Delegate
  return this->_onCommand ( _keepConnected );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the delete primitive button
//
///////////////////////////////////////////////////////////////////////////////

long  EditPolygonComponent::onUpdateDeletePrimitive  ( FX::FXObject *object, FX::FXSelector, void * )
{
  // Delegate
  return this->_onUpdate( object, _deletePrimitive );
}

///////////////////////////////////////////////////////////////////////////////
//
//  Update the delete all connected button
//
///////////////////////////////////////////////////////////////////////////////

long  EditPolygonComponent::onUpdateDeleteGeometry   ( FX::FXObject *object, FX::FXSelector, void * )
{
  // Delegate
  return this->_onUpdate( object, _deleteGeometry );
}
    

///////////////////////////////////////////////////////////////////////////////
//
//  Update the keep all connected button
//
///////////////////////////////////////////////////////////////////////////////

long EditPolygonComponent::onUpdateKeepConnected ( FX::FXObject *object, FX::FXSelector, void * )
{
  // Delegate
  return this->_onUpdate( object, _keepConnected );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set data members
//
///////////////////////////////////////////////////////////////////////////////

void EditPolygonComponent::_init   ( Usul::Interfaces::IUnknown *caller )
{
  if ( !_caller.valid() )
  {
    _caller          = caller;
    _deletePrimitive = new Primitive       ( ); 
    _deleteGeometry  = new Geometry        ( );
    _keepConnected   = new Islands         ( );
    _addTriangle     = new AddTriangle     ( );
    _flipNormal      = new FlipNormal      ( );
    _capPolygons     = new CapPolygons     ( caller );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Cap holes in polygons
//
///////////////////////////////////////////////////////////////////////////////

long EditPolygonComponent::onCommandCapPolygons ( FX::FXObject *, FX::FXSelector, void * )
{
  _capPolygons->execute( 0x0 );

  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update cap polygons menu entry
//
///////////////////////////////////////////////////////////////////////////////

long EditPolygonComponent::onUpdateCapPolygons ( FX::FXObject *object, FX::FXSelector, void * )
{
  Usul::Interfaces::IActiveView::QueryPtr activeView ( _caller );
  FoxTools::Functions::enable( activeView.valid() && 0x0 != activeView->getActiveView(), object );

  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a Triangle
//
///////////////////////////////////////////////////////////////////////////////

long EditPolygonComponent::onCommandAddTriangle( FX::FXObject *, FX::FXSelector, void * )
{
  // Delegate
  return this->_onCommand ( _addTriangle );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update add triangle menu entry
//
///////////////////////////////////////////////////////////////////////////////

long EditPolygonComponent::onUpdateAddTriangle ( FX::FXObject *object, FX::FXSelector, void * )
{
  // Delegate
  return this->_onUpdate( object, _addTriangle );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Flip a normal.
//
///////////////////////////////////////////////////////////////////////////////

long EditPolygonComponent::onCommandFlipNormal( FX::FXObject *, FX::FXSelector, void * )
{
  // Delegate
  return this->_onCommand ( _flipNormal );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update flip normal menu entry.
//
///////////////////////////////////////////////////////////////////////////////

long EditPolygonComponent::onUpdateFlipNormal ( FX::FXObject *object, FX::FXSelector, void * )
{
  // Delegate
  return this->_onUpdate( object, _flipNormal );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Act on command
//
///////////////////////////////////////////////////////////////////////////////

long EditPolygonComponent::_onCommand ( Usul::Interfaces::ITool *tool )
{
  Usul::Interfaces::IActiveView::ValidQueryPtr activeView ( _caller );
  Usul::Interfaces::ISetTool::ValidQueryPtr    setTool    ( activeView->getActiveView() );

  // Is the tool loaded?
  bool loaded ( setTool.valid() ? setTool->getTool() == tool : false );

  // Set the tool if it's not active, unset it if it is currently active.
  if( !loaded )
    setTool->setTool ( tool );
  else
  {
    Usul::Interfaces::ICleanUp::QueryPtr cleanUp ( tool );
    if( cleanUp.valid() )
      cleanUp->cleanUp( activeView->getActiveView() );
    setTool->setTool( 0x0 );
  }

  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update FXObject.
//
///////////////////////////////////////////////////////////////////////////////

long EditPolygonComponent::_onUpdate  ( FX::FXObject* object, Usul::Interfaces::ITool *tool )
{
  Usul::Interfaces::IActiveView::QueryPtr activeView ( _caller );
  Usul::Interfaces::ISetTool::QueryPtr setTool ( activeView.valid() ? activeView->getActiveView() : 0x0 );

  // Is the tool loaded?
  bool loaded ( setTool.valid() ? setTool->getTool() == tool : false );

  FoxTools::Functions::enable ( activeView.valid() && activeView->getActiveView() && setTool.valid(), object );
  FoxTools::Functions::check  ( loaded, object );

  return 1;
}


