
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

#ifndef _VRV_GUI_MENU_PARSER_CLASS_H_
#define _VRV_GUI_MENU_PARSER_CLASS_H_

#include "VRV/GUI/CompileGuard.h"

#include "MenuKit/Menu.h"
#include "MenuKit/Button.h"

#include "Usul/Pointers/Pointers.h"
#include "Usul/Errors/ThrowingPolicy.h"
#include "Usul/Interfaces/IUnknown.h"

#include <stdexcept>
#include <stack>


namespace VRV {
namespace GUI {


class Parser
{
public:

  // Typedefs.
  typedef Usul::Interfaces::IUnknown IUnknown;

  // Construction/destruction.
  Parser();
  ~Parser();

  // Read the configuration file.
  void                  read ( const std::string &filename, IUnknown *caller );

  // Get the root menu.
  const MenuKit::Menu * menu() const;
  MenuKit::Menu *       menu();

protected:

  typedef Usul::Errors::ThrowingPolicy<std::runtime_error> ErrorChecker;
  typedef USUL_VALID_ACCESS_REF_POINTER(MenuKit::Menu)   MenuPtr;
  typedef USUL_VALID_ACCESS_REF_POINTER(MenuKit::Button) ButtonPtr;
  typedef USUL_VALID_ACCESS_REF_POINTER(MenuKit::Item)   ItemPtr;
  typedef std::stack<MenuPtr> MenuStack;

  // Do not copy.
  Parser ( const Parser & );
  Parser &operator = ( const Parser & );

  void                  _read();
  void                  _reset();

  // Callbacks for the xml-reader.
  void                  _startMenu    ( const std::string & );
  void                  _startButton  ( const std::string & );
  void                  _startToggle  ( const std::string & );
  void                  _startRadio   ( const std::string & );
  void                  _startSep     ( const std::string & );
  void                  _finishMenu   ( const std::string & );
  void                  _finishButton ( const std::string & );
  void                  _finishToggle ( const std::string & );
  void                  _finishRadio  ( const std::string & );
  void                  _setText      ( const std::string & );
  void                  _setInfo      ( const std::string & );
  void                  _setLayout    ( const std::string & );
  void                  _setCallback  ( const std::string & );

private:

  IUnknown::RefPtr _caller;
  std::string _filename;
  MenuStack _menus;
  MenuPtr _root;
  ButtonPtr _button;
  ItemPtr _item;
  std::string _callback;
};


}; // namespace GUI
}; // namespace VRV


#endif // _VRV_GUI_MENU_PARSER_CLASS_H_
