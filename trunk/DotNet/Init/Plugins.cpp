
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Plugins.h"

#include "Usul/Interfaces/IPlugin.h"
#include "Usul/Components/Manager.h"
#include "Usul/Strings/Managed.h"

using namespace CadKit::Init::Glue;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Plugins::Plugins()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Load a plugin.
//
///////////////////////////////////////////////////////////////////////////////

void Plugins::loadPlugin( System::String^ filename )
{
  std::string name ( Usul::Strings::convert ( filename ) );

#ifdef _DEBUG
  name += "d";
#endif

  Usul::Components::Manager::Strings strings;
  strings.push_back( name );

  Usul::Components::Manager::instance().load ( Usul::Interfaces::IPlugin::IID, strings );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Un load plugins.
//
///////////////////////////////////////////////////////////////////////////////

void Plugins::unloadPlugins()
{
  Usul::Components::Manager::instance().clear( 0x0 );
}
