
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Menu class.
//
///////////////////////////////////////////////////////////////////////////////

#include "Precompiled.h"
#include "Menu.h"
#include "Parser.h"

using namespace VRV;
using namespace VRV::GUI;

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( Menu, Menu::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Menu::Menu() : BaseClass(), 
  _menu ( 0x0 )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Menu::~Menu()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *Menu::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case VRV::Interfaces::IMenuRead::IID:
    return static_cast<VRV::Interfaces::IMenuRead*>(this);
  case VRV::Interfaces::IMenuGet::IID:
    return static_cast<VRV::Interfaces::IMenuGet*>(this);
  case Usul::Interfaces::IUnknown::IID:
    return static_cast<Usul::Interfaces::IUnknown*>(static_cast<VRV::Interfaces::IMenuRead*>(this));
  default:
    return 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the file and build the menu.
//
///////////////////////////////////////////////////////////////////////////////

void Menu::readMenuFile ( const std::string &filename, Unknown *caller )
{
  Parser parser;
  parser.read ( filename, caller );
  _menu = parser.menu();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the menu.
//
///////////////////////////////////////////////////////////////////////////////

const MenuKit::Menu *Menu::getMenu() const
{
  return _menu.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the menu.
//
///////////////////////////////////////////////////////////////////////////////

MenuKit::Menu *Menu::getMenu()
{
  return _menu.get();
}
