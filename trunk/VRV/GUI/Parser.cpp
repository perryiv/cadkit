
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Menu parser class.
//
///////////////////////////////////////////////////////////////////////////////

#include "Precompiled.h"
#include "Parser.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Bits/Bits.h"

#include "VRV/Interfaces/IButtonCallback.h"
#include "VRV/Interfaces/IMenuCallback.h"

#include "XmlTools/Reader.h"
#include "XmlTools/Policy.h"

using namespace VRV;
using namespace VRV::GUI;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Parser::Parser() :
  _caller   ( 0x0 ),
  _filename (),
  _menus    (),
  _root     ( 0x0 ),
  _button   ( 0x0 ),
  _item     ( 0x0 ),
  _callback ()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Parser::~Parser()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Restore default values.
//
///////////////////////////////////////////////////////////////////////////////

void Parser::_reset()
{
  // Reset all members.
  _caller = 0x0;
  _filename.clear();
  while ( !_menus.empty() )
    _menus.pop();
  _root   = 0x0;
  _button = 0x0;
  _item   = 0x0;
  _callback.clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the root menu.
//
///////////////////////////////////////////////////////////////////////////////

MenuKit::Menu *Parser::menu()
{
  return _root.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the root menu.
//
///////////////////////////////////////////////////////////////////////////////

const MenuKit::Menu *Parser::menu() const
{
  return _root.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the file and build the menu.
//
///////////////////////////////////////////////////////////////////////////////

void Parser::read ( const std::string &filename, IUnknown *caller )
{
  // Reset all state variables to defaults.
  this->_reset();

  // Set these members.
  _caller   = caller;
  _filename = filename;

  // Call the other one.
  this->_read();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the data from file.
//
///////////////////////////////////////////////////////////////////////////////

void Parser::_read()
{
  // Define the xml-reader type.
  typedef void (Parser::*Function) ( const std::string & );
  typedef Usul::Adaptors::MemberFunction < Parser*, Function > MemFun;
  typedef Usul::Errors::ThrowingPolicy < std::runtime_error > ErrorPolicy;
  typedef XML::Reader < MemFun, XML::ValueCB, ErrorPolicy > XmlReader;

  // Make some member function adaptors.
  MemFun startMenu    ( this, &Parser::_startMenu      );
  MemFun startButton  ( this, &Parser::_startButton    );
  MemFun startToggle  ( this, &Parser::_startToggle    );
  MemFun startRadio   ( this, &Parser::_startRadio     );
  MemFun startSep     ( this, &Parser::_startSep       );
  MemFun finishMenu   ( this, &Parser::_finishMenu     );
  MemFun finishButton ( this, &Parser::_finishButton   );
  MemFun setText      ( this, &Parser::_setText        );
  MemFun setInfo      ( this, &Parser::_setInfo        );
  MemFun setLayout    ( this, &Parser::_setLayout      );
  MemFun setCallback  ( this, &Parser::_setCallback    );

  // Declare the reader.
  XmlReader reader;

  // Add the callbacks that get called when the field starts.
  reader.add ( XmlReader::NODE_START,  "menu",      startMenu    );
  reader.add ( XmlReader::NODE_START,  "button",    startButton  );
  reader.add ( XmlReader::NODE_START,  "toggle",    startToggle  );
  reader.add ( XmlReader::NODE_START,  "radio",     startRadio   );
  reader.add ( XmlReader::NODE_START,  "separator", startSep     );
  reader.add ( XmlReader::NODE_START,  "text",      setText      );
  reader.add ( XmlReader::NODE_START,  "info",      setInfo      );
  reader.add ( XmlReader::NODE_START,  "layout",    setLayout    );
  reader.add ( XmlReader::NODE_START,  "id",        setCallback  );

  // Add the callbacks that get called when the field ends.
  reader.add ( XmlReader::NODE_FINISH, "menu",      finishMenu   );
  reader.add ( XmlReader::NODE_FINISH, "button",    finishButton );
  reader.add ( XmlReader::NODE_FINISH, "toggle",    finishButton );
  reader.add ( XmlReader::NODE_FINISH, "radio",     finishButton );
  reader.add ( XmlReader::NODE_FINISH, "separator", finishButton );

  // Ignore DOM hierarchy.
  unsigned int bits ( XmlReader::USE_HIERARCHY );
  reader.flags ( Usul::Bits::remove ( reader.flags(), bits ) );

  // Read the file.
  try
  {
    reader.read ( _filename );
  }

  // Catch exceptions from the reader and throw a better one.
  catch ( const XML::Error &e )
  {
    Usul::Exceptions::Thrower<std::runtime_error>
      ( "Error 2752051467, failed to read file.",
        "\n\tFile:     ", _filename,
        "\n\tLine:     ", e.line(),
        "\n\tPosition: ", e.position(),
        "\n\tMessage:  ", e.message() );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Start the menu.
//
///////////////////////////////////////////////////////////////////////////////

void Parser::_startMenu ( const std::string & )
{
  // Make the new menu.
  MenuPtr menu ( new MenuKit::Menu );
  menu->layout ( MenuKit::Menu::VERTICAL );

  // If there is no root...
  if ( !_root.valid() )
  {
     // Then this is it.
    _root = menu;
  }

  // Otherwise...
  else
  {
    // Append the new menu.
    ErrorChecker ( 2695201676u, !_menus.empty() );
    _menus.top()->append ( menu.get() );
  }

  // Push it onto the stack.
  _menus.push ( menu );

  // Assign the current item.
  _item = menu;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Finish the menu.
//
///////////////////////////////////////////////////////////////////////////////

void Parser::_finishMenu ( const std::string & )
{
  ErrorChecker ( 3338094476u, !_menus.empty() );
  ErrorChecker ( 3011687392u, _root.valid() );

  // Set the callback.
  VRV::Interfaces::IMenuCallback::ValidQueryPtr setter ( _caller );
  setter->setCallback ( _callback, _menus.top() );

  // Pop this menu off the stack.
  _menus.pop();

  // Assign the current item.
  _item = ( _menus.empty() ) ? _root : _menus.top();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Start the button.
//
///////////////////////////////////////////////////////////////////////////////

void Parser::_startButton ( const std::string & )
{
  ErrorChecker ( 3354659664u, !_menus.empty() );
  ErrorChecker ( 3460192759u, !_button.valid() );

  // Make the new button.
  _button = new MenuKit::Button;

  // Append the button to the current menu.
  _menus.top()->append ( _button.get() );

  // Assign the current item.
  _item = _button;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Finish the button.
//
///////////////////////////////////////////////////////////////////////////////

void Parser::_finishButton ( const std::string & )
{
  ErrorChecker ( 3820962905u, !_menus.empty() );
  ErrorChecker ( 2424631443u, _button.valid() );

  // Set the callback.
  VRV::Interfaces::IButtonCallback::ValidQueryPtr setter ( _caller );
  setter->setCallback ( _callback, _button.get() );

  // Set the callback.
  setter->setCallback ( _callback, _button.get() );

  // Reset.
  _button = 0x0;
  _callback.clear();

  // Assign the current item.
  _item = _menus.top();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Start the toggle button.
//
///////////////////////////////////////////////////////////////////////////////

void Parser::_startToggle ( const std::string &s )
{
  ErrorChecker ( 2280523368u, !_menus.empty() );
  ErrorChecker ( 1522832755u, !_button.valid() );

  // Start the button.
  this->_startButton ( s );

  // It is a toggle button.
  _button->toggle ( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Start the radio button.
//
///////////////////////////////////////////////////////////////////////////////

void Parser::_startRadio ( const std::string &s )
{
  ErrorChecker ( 3701626196u, !_menus.empty() );
  ErrorChecker ( 2376463484u, !_button.valid() );

  // Start the button.
  this->_startButton ( s );

  // It is a radio button.
  _button->radio ( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Start the separator.
//
///////////////////////////////////////////////////////////////////////////////

void Parser::_startSep ( const std::string &s )
{
  ErrorChecker ( 1084137278u, !_menus.empty() );
  ErrorChecker ( 1084137279u, !_button.valid() );

  // Start the button.
  this->_startButton ( s );

  // It is a separator.
  _button->separator ( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the text.
//
///////////////////////////////////////////////////////////////////////////////

void Parser::_setText ( const std::string &s )
{
  ErrorChecker ( 3379040717u, _item.valid() );
#if 0
  // Append a '>' if the item is a menu with a vertical parent.
  MenuKit::Menu *menu = dynamic_cast < MenuKit::Menu * > ( _item.get() );
  if ( menu && menu->parent() && menu->parent()->layout() == MenuKit::Menu::VERTICAL )
    _item->text ( s + "  >" );

  // Otherwise, set the item's text.
  else
#endif
  _item->text ( s );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the information string.
//
///////////////////////////////////////////////////////////////////////////////

void Parser::_setInfo ( const std::string &s )
{
  ErrorChecker ( 3122363773u, _item.valid() );

  // Set the item's text.
  _item->info ( s );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the menu layout.
//
///////////////////////////////////////////////////////////////////////////////

void Parser::_setLayout ( const std::string &str )
{
  ErrorChecker ( 2831962913u, !_menus.empty() );

  // Make a copy and make it lower-case.
  std::string s ( str );
  std::transform ( s.begin(), s.end(), s.begin(), ::tolower );

  // Set the menu's layout.
  if ( "horizontal" == s )
    _menus.top()->layout ( MenuKit::Menu::HORIZONTAL );
  else if ( "vertical" == s )
    _menus.top()->layout ( MenuKit::Menu::VERTICAL );
  else
  {
    Usul::Exceptions::Thrower<std::runtime_error>
      ( "Error 1820395511, invalid menu layout.",
        "\n\tFile:   ", _filename,
        "\n\tMenu:   ", _menus.top()->text(),
        "\n\tLayout: ", str );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the callback name.
//
///////////////////////////////////////////////////////////////////////////////

void Parser::_setCallback ( const std::string &s )
{
  ErrorChecker ( 1316719875u, _button.valid() );
  _callback = s;
}
