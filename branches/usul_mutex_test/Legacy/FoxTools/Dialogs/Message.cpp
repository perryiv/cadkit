
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Message dialog.
//
///////////////////////////////////////////////////////////////////////////////

#include "FoxTools/Dialogs/Message.h"
#include "FoxTools/Registry/Registry.h"
#include "FoxTools/Icons/Factory.h"
#include "FoxTools/Functions/MainWindow.h"

#include "FoxTools/Headers/MainWindow.h"
#include "FoxTools/Headers/Registry.h"
#include "FoxTools/Headers/App.h"
#include "FoxTools/Headers/GIFIcon.h"
#include "FoxTools/Headers/Separator.h"
#include "FoxTools/Headers/Button.h"
#include "FoxTools/Headers/CheckButton.h"
#include "FoxTools/Headers/DialogBox.h"
#include "FoxTools/Headers/VerticalFrame.h"
#include "FoxTools/Headers/HorizontalFrame.h"
#include "FoxTools/Headers/Matrix.h"
#include "FoxTools/Headers/Label.h"

#include "Usul/Errors/Assert.h"
#include "Usul/Cast/Cast.h"

#include <stdexcept>
#include <algorithm>

using namespace FoxTools;
using namespace Dialogs;


///////////////////////////////////////////////////////////////////////////////
//
//  Some details.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  const char REGISTRY_SECTION[] = "MessageDialogResults";
  typedef std::map<std::string,std::string> Results;
  Results _results;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Message maps.
//
///////////////////////////////////////////////////////////////////////////////

FXDEFMAP ( Message ) MessageMap[] = 
{
  //          Message Type     ID                           Message Handler.
  FXMAPFUNC ( FX::SEL_COMMAND, Message::ID_CLOSE,           Message::onCommandClose       ),
  FXMAPFUNC ( FX::SEL_COMMAND, Message::ID_CHECK_BUTTON,    Message::onCommandCheckButton ),
};

FOX_TOOLS_IMPLEMENT ( Message, Message::BaseClass, MessageMap, ARRAYNUMBER ( MessageMap ) );


///////////////////////////////////////////////////////////////////////////////
//
//  Initializer list.
//
///////////////////////////////////////////////////////////////////////////////

#define INITIALIZER_LIST \
  _title     (), \
  _cells     (), \
  _checks    (), \
  _buttons   (), \
  _resizable ( false ), \
  _icon      ( Message::Icon ( 0x0, false ) ), \
  _result    (), \
  _close     (), \
  _id        (), \
  _cacheIf   ( std::string(), Bools ( false, false ) )


///////////////////////////////////////////////////////////////////////////////
//
//  Default constructor.
//
///////////////////////////////////////////////////////////////////////////////

Message::Message() : BaseClass(),
  INITIALIZER_LIST
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Message::Message ( const std::string &title,
                   const std::string &message ) : BaseClass(),
  INITIALIZER_LIST
{
  this->text ( title, message );
  _buttons.push_back ( "OK" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Message::Message ( const std::string &id, bool offerToRemember, bool persistAnswerToDisk ) : BaseClass(),
  INITIALIZER_LIST
{
  this->id ( id );

  if ( offerToRemember )
  {
    const std::string remember ( "Remember this decision" );
    this->checks()[remember] = false;
    this->cacheIf ( remember, true, persistAnswerToDisk );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Message::~Message()
{
  if ( _icon.first && _icon.second )
    delete _icon.first;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the cell.
//
///////////////////////////////////////////////////////////////////////////////

Message::Cell &Message::cell ( unsigned int r, unsigned int c )
{
  this->_grow ( r + 1, c + 1 );
  return _cells.at(r).at(c);
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the cell.
//
///////////////////////////////////////////////////////////////////////////////

const Message::Cell &Message::cell ( unsigned int r, unsigned int c ) const
{
  return _cells.at(r).at(c);
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the text.
//
///////////////////////////////////////////////////////////////////////////////

std::string &Message::text ( unsigned int r, unsigned int c )
{
  this->_grow ( r + 1, c + 1 );
  return _cells.at(r).at(c).first;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the text.
//
///////////////////////////////////////////////////////////////////////////////

const std::string &Message::text ( unsigned int r, unsigned int c ) const
{
  return _cells.at(r).at(c).first;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Convenience function for setting a single cell's text. This makes one 
//  cell and deletes any existing cells.
//
///////////////////////////////////////////////////////////////////////////////

void Message::text ( const std::string &m )
{
  _cells.resize ( 1 );
  _cells.at(0).resize ( 1 );
  _cells.at(0).at(0).first = m;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Convenience function for setting a single cell's text. This makes one 
//  cell and deletes any existing cells.
//
///////////////////////////////////////////////////////////////////////////////

void Message::text ( const std::string &title, const std::string &message )
{
  this->title ( title );
  this->text ( message );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set resizing.
//
///////////////////////////////////////////////////////////////////////////////

void Message::resizable ( bool r )
{
  _resizable = r;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set resizing.
//
///////////////////////////////////////////////////////////////////////////////

bool Message::resizable() const
{
  return _resizable;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the icon.
//
///////////////////////////////////////////////////////////////////////////////

void Message::icon ( FX::FXIcon *i, bool destroy )
{
  // Delete existing icon if we should.
  if ( _icon.first && _icon.second )
    delete _icon.first;

  // Assign icon.
  _icon = Message::Icon ( i, destroy );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the icon.
//
///////////////////////////////////////////////////////////////////////////////

const Message::Icon &Message::icon() const
{
  return _icon;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the icon.
//
///////////////////////////////////////////////////////////////////////////////

void Message::icon ( unsigned int id )
{
  this->icon ( FoxTools::Icons::Factory::instance()->icon ( id ), true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the close string.
//
///////////////////////////////////////////////////////////////////////////////

void Message::closeButton ( const std::string &c )
{
  _close = c;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the close string.
//
///////////////////////////////////////////////////////////////////////////////

const std::string &Message::closeButton() const
{
  return _close;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the id string.
//
///////////////////////////////////////////////////////////////////////////////

void Message::id ( const std::string &s )
{
  _id = s;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the id string.
//
///////////////////////////////////////////////////////////////////////////////

const std::string &Message::id() const
{
  return _id;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make sure there are enough cells.
//
///////////////////////////////////////////////////////////////////////////////

void Message::_grow ( unsigned int rows, unsigned int columns )
{
  // Make sure there are enough rows.
  if ( rows > _cells.size() )
  {
    _cells.resize ( rows );
  }

  // Assign the new number of rows, it may have changed above.
  rows = _cells.size();

  // Loop through the rows.
  for ( unsigned int i = 0; i < rows; ++i )
  {
    // Make sure there are enough columns.
    if ( columns > _cells.at(i).size() )
    {
      _cells.at(i).resize ( columns );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when a check-button is pressed.
//
///////////////////////////////////////////////////////////////////////////////

long Message::onCommandCheckButton ( FX::FXObject *object, FX::FXSelector, void * )
{
  FX::FXCheckButton *button = SAFE_CAST_FOX ( FX::FXCheckButton, object );

  if ( button && !button->getText().empty() )
  {
    const std::string text ( button->getText().text() );
    Checks::iterator i ( _checks.find ( text ) );
    if ( _checks.end() != i )
    {
      i->second = ( ( button->getCheck() ) ? true : false );
    }
  }

  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when a regular button is pressed.
//
///////////////////////////////////////////////////////////////////////////////

long Message::onCommandClose ( FX::FXObject *object, FX::FXSelector, void * )
{
  FX::FXButton *button = SAFE_CAST_FOX ( FX::FXButton, object );

  if ( button && !button->getText().empty() )
  {
    _result = button->getText().text();
    button->getApp()->stopModal();
  }

  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Cache the results of run() if this check-button has the final state.
//  Last argument determines if decision persists past program exit.
//
///////////////////////////////////////////////////////////////////////////////

void Message::cacheIf ( const std::string &check, bool state, bool persist )
{
  _cacheIf.first = check;
  _cacheIf.second.first = state;
  _cacheIf.second.second = persist;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set a single button. Deletes any existing buttons.
//
///////////////////////////////////////////////////////////////////////////////

void Message::button ( const std::string &b )
{
  this->buttons().clear();
  this->buttons().push_back ( b );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set two buttons. Deletes any existing buttons.
//
///////////////////////////////////////////////////////////////////////////////

void Message::buttons ( const std::string &b1, const std::string &b2 )
{
  this->buttons().clear();
  this->buttons().push_back ( b1 );
  this->buttons().push_back ( b2 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set three buttons. Deletes any existing buttons.
//
///////////////////////////////////////////////////////////////////////////////

void Message::buttons ( const std::string &b1, const std::string &b2, const std::string &b3 )
{
  this->buttons().clear();
  this->buttons().push_back ( b1 );
  this->buttons().push_back ( b2 );
  this->buttons().push_back ( b3 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Separate buttons with a '|' character. For example, "OK|Cancel".
//
///////////////////////////////////////////////////////////////////////////////

void Message::buttons ( const std::string &b )
{
  // Clear existing buttons.
  this->buttons().clear();

  // Initialize.
  std::string::const_iterator first = b.begin();
  std::string::const_iterator last  = first;

  // While we are not at the end...
  while ( b.end() != first )
  {
    // Search the string for '|' characters.
    last = std::find ( first, b.end(), '|' );

    // Append a button.
    this->buttons().push_back ( std::string ( first, last ) );

    // Set for next iteration.
    first = ( b.end() == last ) ? last : ++last;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Cache the result if we are supposed to.
//
///////////////////////////////////////////////////////////////////////////////

void Message::_save ( const std::string &result ) const
{
  // Loop through the check-buttons.
  for ( Checks::const_iterator i = _checks.begin(); i != _checks.end(); ++i )
  {
    // If the strings and button states are the same...
    if ( i->first == _cacheIf.first && i->second == _cacheIf.second.first )
    {
      // Should we persist?
      if ( _cacheIf.second.second )
        FoxTools::Registry::write ( Detail::REGISTRY_SECTION, this->id(), result );

      // Otherwise...
      else
        Detail::_results[this->id()] = result;
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Try to get a cached result.
//
///////////////////////////////////////////////////////////////////////////////

std::string Message::_find() const
{
  // Handle empty string.
  if ( this->id().empty() )
    return std::string();

  // Check the map.
  Detail::Results::const_iterator i = Detail::_results.find ( this->id() );
  if ( Detail::_results.end() != i )
    return i->second;

  // Check the registry.
  const std::string cached ( FoxTools::Registry::read ( Detail::REGISTRY_SECTION, this->id(), std::string() ) );
  if ( !cached.empty() )
    return cached;

  // We didn't find anything.
  return std::string();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Convenience function to display an error dialog. Separate buttons with 
//  a '|' character. For example, "OK|Cancel". Lets the user choose to supress 
//  all subsequent errors with same string.
//
///////////////////////////////////////////////////////////////////////////////

std::string Message::error ( const std::string &b, 
                             const std::string &title, 
                             const std::string &text,
                             bool offerToRemember,
                             FX::FXObject *owner )
{
  Message m;
  m.text ( title, text );
  m.icon ( FoxTools::Icons::Factory::ICON_ERROR );
  m.buttons ( b );

  if ( offerToRemember )
  {
    const std::string remember ( "Do not show this error message again" );
    m.checks()[remember] = false;
    m.cacheIf ( remember, true, false );
  }

  return m.run ( owner, FX::PLACEMENT_OWNER );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Convenience function to display an information dialog. Separate buttons 
//  with a '|' character. For example, "OK|Cancel".
//
///////////////////////////////////////////////////////////////////////////////

std::string Message::info ( const std::string &b, 
                            const std::string &title, 
                            const std::string &text,
                            FX::FXObject *owner )
{
  Message m;
  m.text ( title, text );
  m.icon ( FoxTools::Icons::Factory::ICON_INFO );
  m.buttons ( b );
  return m.run ( owner, FX::PLACEMENT_OWNER );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Convenience function to display a question dialog. Separate buttons 
//  with a '|' character. For example, "Yes|No|Cancel".
//
///////////////////////////////////////////////////////////////////////////////

std::string Message::question ( const std::string &b, 
                                const std::string &title, 
                                const std::string &text,
                                FX::FXObject *owner )
{
  Message m;
  m.text ( title, text );
  m.icon ( FoxTools::Icons::Factory::ICON_QUESTION );
  m.buttons ( b );
  return m.run ( owner, FX::PLACEMENT_OWNER );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Used for debugging the spacing below.
//
///////////////////////////////////////////////////////////////////////////////

//#define USE_DEBUG_COLORS
#ifdef USE_DEBUG_COLORS
#define SET_COLOR(ptr,r,g,b) (ptr)->setBackColor ( FXRGB ( r, g, b ) )
#else
#define SET_COLOR(ptr,r,g,b)
#endif


///////////////////////////////////////////////////////////////////////////////
//
//  Run the dialog in a modal loop. TODO, hook up alignment features.
//
///////////////////////////////////////////////////////////////////////////////

std::string Message::run ( FX::FXObject *object, unsigned int placement )
{
  // Initialize.
  _result.clear();

  // Make sure we have a window.
  FX::FXWindow *owner = SAFE_CAST_FOX ( FX::FXWindow, object );
  owner = ( owner ) ? owner : FoxTools::Functions::mainWindow();
  if ( 0x0 == owner )
    throw std::invalid_argument ( "Error 3116974582: invalid dialog owner given, and main windows is null" );

  // Set dialog's flags.
  unsigned int flags ( FX::DECOR_CLOSE | FX::DECOR_TITLE | FX::DECOR_BORDER );
  if ( this->resizable() )
    flags |= FX::DECOR_RESIZE;

  // The parent dialog.
  FX::FXDialogBox dialog ( owner, _title.c_str(), flags );
  SET_COLOR ( &dialog, 0, 0, 255 );

  // See if we already have the answer.
  const std::string cached ( this->_find() );
  if ( !cached.empty() )
    return cached;

  // Vertical frame holds everything.
  FX::FXVerticalFrame *outer ( new FX::FXVerticalFrame ( &dialog, FX::LAYOUT_FILL_X | FX::LAYOUT_FILL_Y, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ) );
  SET_COLOR ( outer, 0, 255, 0 );

  // Initialize the parent. This may be reset below.
  FX::FXComposite *parent = outer;

  // If there is an icon...
  if ( this->icon().first )
  {
    // Make a horizontal frame to hold the label and the matrix.
    parent = new FX::FXHorizontalFrame ( outer, FX::LAYOUT_FILL_X | FX::LAYOUT_FILL_Y, 0, 0, 0, 0, 0, 0, 0, 10, 0, 0 );
    SET_COLOR ( parent, 0, 255, 255 );

    // Make a label that contains the icon.
    new FX::FXLabel ( parent, "", this->icon().first, FX::ICON_BEFORE_TEXT | FX::LAYOUT_TOP | FX::LAYOUT_LEFT | FX::LAYOUT_FILL_X | FX::LAYOUT_FILL_Y );
  }

  // If there are cells...
  if ( !_cells.empty() )
  {
    // Matrix holds the cells.
    const unsigned int numColumns ( _cells.at(0).size() );
    FX::FXMatrix *matrix ( new FX::FXMatrix ( parent, numColumns, FX::LAYOUT_FILL_X | FX::LAYOUT_FILL_Y | FX::MATRIX_BY_COLUMNS ) );
    SET_COLOR ( matrix, 255, 0, 0 );

    // Loop through the rows.
    for ( Cells::const_iterator i = _cells.begin(); i != _cells.end(); ++i )
    {
      const Row &row = *i;
      USUL_ASSERT ( row.size() ==  numColumns );

      // Loop through the columns of this row.
      for ( Row::const_iterator j = row.begin(); j != row.end(); ++j )
      {
        // Shortcut to the cell.
        const Cell &cell = *j;

        // Get the text.
        std::string text ( ( cell.first.empty() ) ? "--" : cell.first ); 

        // FOX labels do not handle tab characters well, treats them as hot-key 
        // delimiters for buttons. Replace all tabs with spaces.
        std::replace ( text.begin(), text.end(), '\t', ' ' );

        // Make the label.
        new FX::FXLabel ( matrix, text.c_str(), 0x0, FX::JUSTIFY_LEFT | FX::LAYOUT_FILL_X | FX::LAYOUT_FILL_Y | FX::LAYOUT_FILL_ROW | FX::LAYOUT_FILL_COLUMN );
      }
    }
  }

  // If there are buttons...
  if ( !_buttons.empty() )
  {
    // Separator.
    new FX::FXHorizontalSeparator ( outer, FX::SEPARATOR_GROOVE | FX::LAYOUT_TOP | FX::LAYOUT_LEFT | FX::LAYOUT_FILL_X );

    // Add the regular-buttons to their own frame.
    {
      unsigned int flags ( FX::LAYOUT_TOP | FX::LAYOUT_LEFT | FX::LAYOUT_FILL_X | FX::PACK_UNIFORM_WIDTH );
      FX::FXHorizontalFrame *frame ( new FX::FXHorizontalFrame ( outer, flags, 0, 0, 0, 0, 5, 5, 15, 5 ) );
      SET_COLOR ( frame, 255, 0, 255 );

      for ( Buttons::const_iterator i = _buttons.begin(); i != _buttons.end(); ++i )
      {
        flags = FX::BUTTON_INITIAL | FX::BUTTON_DEFAULT | FX::FRAME_RAISED | FX::FRAME_THICK | FX::LAYOUT_TOP | FX::LAYOUT_LEFT | FX::LAYOUT_CENTER_X;
        FX::FXButton *button ( new FX::FXButton ( frame, i->c_str(), 0x0, this, Message::ID_CLOSE, flags, 0, 0, 0, 0, 30, 30, 2, 2 ) );
      }
    }
  }

  // If there are check-buttons...
  if ( !_checks.empty() )
  {
    // Separator.
    //new FX::FXHorizontalSeparator ( outer, FX::SEPARATOR_GROOVE | FX::LAYOUT_TOP | FX::LAYOUT_LEFT | FX::LAYOUT_FILL_X );

    // Add the check-buttons in their own frame.
    {
      FX::FXVerticalFrame *frame ( new FX::FXVerticalFrame ( outer, FX::LAYOUT_CENTER_X, 0, 0, 0, 0, 0, 0, 5, 0 ) );
      SET_COLOR ( frame, 255, 255, 0 );

      for ( Checks::const_iterator i = _checks.begin(); i != _checks.end(); ++i )
      {
        FX::FXCheckButton *button ( new FX::FXCheckButton ( frame, i->first.c_str(), this, Message::ID_CHECK_BUTTON ) );
        button->setCheck ( i->second );
      }
    }
  }

  // Run the dialog.
  dialog.execute ( placement );

  // Get the proper string.
  const std::string result ( ( _result.empty() ) ? this->closeButton() : _result );

  // Cache results if we are supposed to.
  this->_save ( result );

  // Return the result.
  return result;
}
