
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifdef __sgi
#define _CPP_CMATH 1
#endif

#include "Helios/Fox/Core/Preferences.h"

#include "Usul/Registry/Constants.h"

#include "FoxTools/Headers/VerticalFrame.h"
#include "FoxTools/Headers/HorizontalFrame.h"
#include "FoxTools/Headers/TabBook.h"
#include "FoxTools/Headers/TabItem.h"
#include "FoxTools/Headers/Button.h"
#include "FoxTools/Headers/App.h"
#include "FoxTools/Headers/CheckButton.h"
#include "FoxTools/Headers/Spinner.h"
#include "FoxTools/Headers/Separator.h"

#include "FoxTools/Dialogs/Message.h"
#include "FoxTools/Icons/Factory.h"
#include "FoxTools/Functions/App.h"
#include "FoxTools/Registry/Registry.h"

#include "Usul/Cast/Cast.h"
#include "Usul/Shared/Preferences.h"

using namespace Helios::Core;

FXDEFMAP ( Preferences ) PreferencesMap[] =
{
  FXMAPFUNC ( FX::SEL_COMMAND, Preferences::ID_SET_ANIMATION_TIME,     Preferences::onCommandSetAnimateTime ),
  FXMAPFUNC ( FX::SEL_UPDATE,  Preferences::ID_SET_ANIMATION_TIME,     Preferences::onUpdateSetAnimateTime  ),
  FXMAPFUNC ( FX::SEL_COMMAND, Preferences::ID_WARN_NO_UNDO,           Preferences::onCommandWarnNoUndo     ),
  FXMAPFUNC ( FX::SEL_UPDATE,  Preferences::ID_WARN_NO_UNDO,           Preferences::onUpdateWarnNoUndo      ),
  FXMAPFUNC ( FX::SEL_COMMAND, Preferences::ID_VIEW_ALL_ON_LOAD,       Preferences::onCommandViewAllOnLoad  ),
  FXMAPFUNC ( FX::SEL_UPDATE,  Preferences::ID_VIEW_ALL_ON_LOAD,       Preferences::onUpdateViewAllOnLoad   ),
  FXMAPFUNC ( FX::SEL_COMMAND, Preferences::ID_CLEAR_REGISTRY,         Preferences::onCommandClearRegistry  ),
};

//This class, base class, array of message handlers, size of the array.
FOX_TOOLS_IMPLEMENT ( Preferences, BaseClass, PreferencesMap, ARRAYNUMBER ( PreferencesMap ) );


///////////////////////////////////////////////////////////////////////////////
//
//  Static data member(s).
//
///////////////////////////////////////////////////////////////////////////////

Preferences* Preferences::_instance ( 0x0 );
FX::FXWindow* Preferences::_owner ( 0x0 );


///////////////////////////////////////////////////////////////////////////////
//
//  Get the instance for this singleton.
//
///////////////////////////////////////////////////////////////////////////////

Preferences* Preferences::instance()
{
  if ( !_owner )
    throw std::runtime_error ( "Error 1528077751: no owner of preference dialog" );

  if ( !_instance )
    _instance = new Preferences ( _owner );
  return _instance;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the owner.
//
///////////////////////////////////////////////////////////////////////////////

void Preferences::owner ( FX::FXWindow *owner )
{
  _owner = owner;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Delete the instance for this singleton.
//
///////////////////////////////////////////////////////////////////////////////

void Preferences::releaseInstance()
{
  if( _instance )
    delete _instance;
  _instance = 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

Preferences::Preferences( FX::FXWindow* owner) :
BaseClass( owner,Usul::Registry::Sections::PREFERENCES.c_str(),DECOR_TITLE|DECOR_BORDER|DECOR_RESIZE,0,0,0,0, 10,10,10,10, 4,4 ),
  _tabBook ( 0x0 ),
  _tabItems()
{
  this->setWidth( (int) (.6 * owner->getWidth()) );
  this->setHeight( (int) (.6 * owner->getHeight()) );

  FX::FXVerticalFrame *contents=new FX::FXVerticalFrame( this , LAYOUT_FILL_X|LAYOUT_FILL_Y);

  _tabBook = new FX::FXTabBook ( contents, 0x0, 0, LAYOUT_FILL_X|LAYOUT_FILL_Y );

  // Fill the local maps from the registry.
  this->_populate();

  this->_initGeneralTab();

  _tabBook->layout();
  _tabBook->repaint();

  //TODO add accept and cancel
  new FX::FXButton( contents,"&Close",NULL, this,FXDialogBox::ID_ACCEPT,BUTTON_INITIAL|BUTTON_DEFAULT|LAYOUT_RIGHT|FRAME_RAISED|FRAME_THICK,0,0,0,0, 20,20);
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

Preferences::~Preferences()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the current values to the registry
//
///////////////////////////////////////////////////////////////////////////////

namespace Helios
{
  namespace Core
  {
    namespace Detail
    {
      template < class C > inline void _writeOutValues ( C &c )
      {
        typedef typename C::const_iterator Itr;
        for ( Itr i = c.begin(); i != c.end(); ++i )
        {
          FoxTools::Registry::write ( Usul::Registry::Sections::PREFERENCES, i->first.c_str(), i->second );
        }
      }
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the current values to the registry
//
///////////////////////////////////////////////////////////////////////////////

void Preferences::writeOutValues() const
{
  typedef Usul::Shared::Preferences Pref;
  // Write out all of the values.
  Detail::_writeOutValues ( Pref::instance().getBools() );
  Detail::_writeOutValues ( Pref::instance().getInts() );
  Detail::_writeOutValues ( Pref::instance().getUints() );
  Detail::_writeOutValues ( Pref::instance().getFloats() );
  Detail::_writeOutValues ( Pref::instance().getDoubles() );
  Detail::_writeOutValues ( Pref::instance().getStrings() );

  // Flush the cached values and actually write them to the registry.
  FoxTools::Functions::application()->reg().write();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Show the preferences dialog
//
///////////////////////////////////////////////////////////////////////////////

void Preferences::showDialog( )
{
  //If we haven't been created yet...
  if( 0x0 != this->id() ) 
  {
    this->create();
  }

  this->execute( FX::PLACEMENT_OWNER );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Populate the maps with the values from the registry
//
///////////////////////////////////////////////////////////////////////////////

void Preferences::_populate()
{
  if( _owner )
  { 
    // To make the lines shorter
    const std::string section ( Usul::Registry::Sections::PREFERENCES );
    typedef Usul::Shared::Preferences Pref;

    // Get the animation time
    double animationTime ( FoxTools::Registry::read ( section, Usul::Registry::Keys::ANIMATION_TIME, (double) 1.0 ) );
    Pref::instance().setDouble ( Usul::Registry::Keys::ANIMATION_TIME, animationTime );

    // Get whether we should warn if command cannot be undone
    bool noWarn ( FoxTools::Registry::read ( section, Usul::Registry::Keys::WARN_NO_UNDO, true ) );
    Pref::instance().setBool ( Usul::Registry::Keys::WARN_NO_UNDO, noWarn );

    // Get whether we should view all on load
    bool viewAll ( FoxTools::Registry::read ( section, Usul::Registry::Keys::VIEW_ALL_ON_LOAD, true ) );
    Pref::instance().setBool ( Usul::Registry::Keys::VIEW_ALL_ON_LOAD, viewAll );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the animation time
//
///////////////////////////////////////////////////////////////////////////////

long Preferences::onCommandSetAnimateTime ( FX::FXObject *, FX::FXSelector, void *animationTime )
{
  unsigned int time = ( ( unsigned int ) animationTime );
  Usul::Shared::Preferences::instance().setDouble ( Usul::Registry::Keys::ANIMATION_TIME, time );

  // The message was handled.
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the animation time
//
///////////////////////////////////////////////////////////////////////////////

long Preferences::onUpdateSetAnimateTime     ( FX::FXObject *object, FX::FXSelector, void * )
{
  FX::FXSpinner *spinner = SAFE_CAST_FOX ( FX::FXSpinner, object );
  if( !spinner )
    return 0;

  spinner->setValue ( static_cast < FX::FXint > ( Usul::Shared::Preferences::instance().getDouble ( Usul::Registry::Keys::ANIMATION_TIME ) ) );

  //Handled
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the flag for warning about no undo possible
//
///////////////////////////////////////////////////////////////////////////////

long Preferences::onCommandWarnNoUndo ( FX::FXObject *, FX::FXSelector, void *data )
{
  bool warn ( data ? true : false );
  Usul::Shared::Preferences::instance().setBool ( Usul::Registry::Keys::WARN_NO_UNDO, warn );
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update check box for warn no undo
//
///////////////////////////////////////////////////////////////////////////////

long Preferences::onUpdateWarnNoUndo ( FX::FXObject *object, FX::FXSelector, void * )
{
  FX::FXCheckButton *check = SAFE_CAST_FOX ( FX::FXCheckButton, object);
  if( !check )
    return 0;

  check->setCheck ( Usul::Shared::Preferences::instance().getBool ( Usul::Registry::Keys::WARN_NO_UNDO ) );

  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Reference this instance.
//
///////////////////////////////////////////////////////////////////////////////

void Preferences::ref()
{
  // Intentionally left empty.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Unreference this instance.
//
///////////////////////////////////////////////////////////////////////////////

void Preferences::unref ( bool allowDeletion )
{
  // Intentionally left empty.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *Preferences::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::IFoxTabItem::IID:
    return static_cast < Usul::Interfaces::IFoxTabItem*>(this);
  case Usul::Interfaces::IFoxTabBook::IID:
    return static_cast < Usul::Interfaces::IFoxTabBook*>(this);
  default:
    return 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create a new tab item and add it to the map.
//
///////////////////////////////////////////////////////////////////////////////

void Preferences::addTab ( const std::string &name )
{
  std::auto_ptr<FX::FXTabItem> item ( new FX::FXTabItem ( _tabBook, name.c_str(), 0x0 ) );
  _tabItems[name] = item.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove a tab item from the map.
//
///////////////////////////////////////////////////////////////////////////////

void Preferences::deleteTab ( const std::string &name )
{
  // Look for the tab.
  TabItems::iterator i = _tabItems.find ( name );
  if ( _tabItems.end() != i )
  {
    // Delete the tab and the accompanying window.
    delete i->second;

    // Remove this entry from the map.
    _tabItems.erase ( i );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Delete all tabs
//
///////////////////////////////////////////////////////////////////////////////

void Preferences::deleteTabs()
{
  // Loop through and delete all the tab items.
  for ( TabItems::iterator i = _tabItems.begin(); i != _tabItems.end(); ++i )
  {
    delete i->second;
  }

  // Clear the map.
  _tabItems.clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get a pointer to a tab item
//
///////////////////////////////////////////////////////////////////////////////

FX::FXTabItem * Preferences::tabItem ( const std::string &name )
{
  TabItems::iterator i = _tabItems.find ( name );
  return ( _tabItems.end() != i ) ? i->second : 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get a pointer to a constant tab item
//
///////////////////////////////////////////////////////////////////////////////

const FX::FXTabItem * Preferences::tabItem ( const std::string &name ) const
{
  TabItems::const_iterator i = _tabItems.find ( name );
  return ( _tabItems.end() != i ) ? i->second : 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the tab book
//
///////////////////////////////////////////////////////////////////////////////

FX::FXTabBook * Preferences::tabBook  (  )
{
  return _tabBook;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the tab book
//
///////////////////////////////////////////////////////////////////////////////

const FX::FXTabBook * Preferences::tabBook  (  ) const
{
  return _tabBook;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize the general tab
//
///////////////////////////////////////////////////////////////////////////////

void Preferences::_initGeneralTab()
{
  //General tab
  new FX::FXTabItem ( _tabBook, "General", 0x0 );

  //frame for general tab
  FX::FXVerticalFrame *generalpane = new FX::FXVerticalFrame ( _tabBook,LAYOUT_FILL_X|LAYOUT_FILL_Y|FRAME_THICK );

  //warn no undo
  FX::FXHorizontalFrame* paneOne = new FX::FXHorizontalFrame(generalpane,LAYOUT_FILL_X|FRAME_NONE);
  new FX::FXCheckButton ( paneOne, "Warn if command is not undoable", this, Preferences::ID_WARN_NO_UNDO );

  //view all time
  FX::FXHorizontalFrame *paneTwo = new FX::FXHorizontalFrame( generalpane, LAYOUT_FILL_X|FRAME_NONE);
  new FX::FXLabel ( paneTwo, "View All Time", 0x0, FX::LAYOUT_LEFT );
  FX::FXSpinner* spinner=new FX::FXSpinner( paneTwo, 2, this ,Preferences::ID_SET_ANIMATION_TIME,JUSTIFY_RIGHT|FRAME_SUNKEN|FRAME_THICK|LAYOUT_LEFT|LAYOUT_FILL_COLUMN|LAYOUT_FILL_ROW);
  spinner->setRange(0,15);

  //View all on load
  FX::FXHorizontalFrame* paneThree = new FX::FXHorizontalFrame(generalpane,LAYOUT_FILL_X|FRAME_NONE);
  new FX::FXCheckButton ( paneThree, "View all when a file is loaded", this, Preferences::ID_VIEW_ALL_ON_LOAD );

  //Button to restore defaults
  new FX::FXButton ( generalpane, "Restore Defaults and Clear Saved Settings", 0x0, this, Preferences::ID_CLEAR_REGISTRY );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Toggle whether or not to view all on loading a file
//
///////////////////////////////////////////////////////////////////////////////

long Preferences::onCommandViewAllOnLoad ( FX::FXObject *, FX::FXSelector, void *data )
{
  bool view ( data ? true : false );
  Usul::Shared::Preferences::instance().setBool ( Usul::Registry::Keys::VIEW_ALL_ON_LOAD, view );
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update check button for viewing all on file loading
//
///////////////////////////////////////////////////////////////////////////////

long Preferences::onUpdateViewAllOnLoad ( FX::FXObject *object, FX::FXSelector, void * )
{
  FX::FXCheckButton *check = SAFE_CAST_FOX ( FX::FXCheckButton, object );
  if( !check )
    return 0;

  check->setCheck ( Usul::Shared::Preferences::instance().getBool ( Usul::Registry::Keys::VIEW_ALL_ON_LOAD ) );
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear the registry and restore defaults
//
///////////////////////////////////////////////////////////////////////////////

long Preferences::onCommandClearRegistry     ( FX::FXObject *, FX::FXSelector, void * )
{
  FX::FXRegistry &reg = _owner->getApp()->reg();

  bool result ( true );

  FoxTools::Dialogs::Message dialog ( "restoring_defaults_cannot_be_undone", false, false );
  dialog.text ( "No Undo!", "Restoring defaults cannot be undone. Continue?" );
  dialog.icon ( FoxTools::Icons::Factory::ICON_WARNING );
  dialog.buttons ( "Yes", "No" );

  result = ( "Yes" == dialog.run ( _owner ) );

  if( result )
  {
    //Clear the old values
    reg.clear();

    //Restore the defaults
    this->_populate();
  }

  //Handled
  return 1;
}
