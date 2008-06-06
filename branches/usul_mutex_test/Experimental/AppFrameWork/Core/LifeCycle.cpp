
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Convenience class for ensuring the program is instantiated and destroyed.
//
///////////////////////////////////////////////////////////////////////////////

#include "AppFrameWork/Core/LifeCycle.h"
#include "AppFrameWork/Core/Program.h"
#include "AppFrameWork/Core/Application.h"
#include "AppFrameWork/Core/Define.h"

#include "Usul/Errors/Assert.h"

#include <sstream>
#include <iostream>

using namespace AFW::Core;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

ProgramLifeCycle::ProgramLifeCycle()
{
  try { Program::instance(); }
  AFW_CATCH_BLOCK ( 8550804340ul );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

ProgramLifeCycle::~ProgramLifeCycle()
{
  try { Program::destroy(); }
  AFW_CATCH_BLOCK ( 1784739761ul );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

PluginsLifeCycle::PluginsLifeCycle()
{
  // Print any errors now. This will also initialize the errors again.
  std::cout << Program::instance().errors() << std::flush;

  // Load all plugins.
  try { Program::instance().pluginsLoad(); }
  AFW_CATCH_BLOCK ( 1575296186ul );

  // Print errors again.
  std::cout << Program::instance().errors() << std::flush;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

PluginsLifeCycle::~PluginsLifeCycle()
{
  // Print any errors now. This will also initialize the errors again.
  std::cout << Program::instance().errors() << std::flush;

  // Release all plugins.
  try { Program::instance().pluginsRelease(); }
  AFW_CATCH_BLOCK ( 1856474491ul );

  // Print errors again.
  std::cout << Program::instance().errors() << std::flush;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

AppLifeCycle::AppLifeCycle()
{
  // Print any errors now. This will also initialize the errors again.
  std::cout << Program::instance().errors() << std::flush;

  // Make application.
  try { Program::instance().app ( Program::instance().newObject<Application>() ); }
  AFW_CATCH_BLOCK ( 1345089604ul );

  // Print errors again.
  std::cout << Program::instance().errors() << std::flush;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

AppLifeCycle::~AppLifeCycle()
{
  // Print any errors now. This will also initialize the errors again.
  std::cout << Program::instance().errors() << std::flush;

  // Release application.
  try
  {
    if ( 0x0 != Program::instance().app() )
    {
      USUL_ASSERT ( 1 == Program::instance().app()->refCount() );
      Program::instance().app ( 0x0 );
    }
  }
  AFW_CATCH_BLOCK ( 1440365800ul );

  // Print errors again.
  std::cout << Program::instance().errors() << std::flush;
}
