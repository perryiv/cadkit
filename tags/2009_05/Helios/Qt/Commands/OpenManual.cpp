
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Helios/Qt/Commands/OpenManual.h"

#include "Usul/CommandLine/Arguments.h"
#include "Usul/Predicates/FileExists.h"
#include "Usul/Strings/Format.h"
#include "Usul/Trace/Trace.h"

#ifdef _MSC_VER
#include <windows.h>
#include <shellapi.h>
#endif

#include <algorithm>

using namespace CadKit::Helios::Commands;

USUL_IMPLEMENT_COMMAND ( OpenManual );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

OpenManual::OpenManual ( IUnknown *caller, const std::string &manual ) : BaseClass ( caller ),
  _manual ( manual )
{
  USUL_TRACE_SCOPE;
  this->text ( "&Manual" );
  this->shortcut ( "" );
  this->statusTip ( "View the manual" );
  this->toolTip ( "View the manual" );
  this->iconPath ( "" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

OpenManual::~OpenManual()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Export the image.
//
///////////////////////////////////////////////////////////////////////////////

void OpenManual::_execute()
{
  USUL_TRACE_SCOPE;
  
  std::string file ( this->_manualFile() );

#if _MSC_VER

  std::replace ( file.begin(), file.end(), '\\', '/' );
  ::ShellExecuteA ( 0x0, "open", file.c_str(), 0x0, 0x0, 0 );

#elif __APPLE__
  
  const std::string command ( Usul::Strings::format ( "open ", file ) );
  ::system ( command.c_str() );

#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Are we enabled?
//
///////////////////////////////////////////////////////////////////////////////

bool OpenManual::updateEnable() const
{
  return Usul::Predicates::FileExists::test ( this->_manualFile() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the name of the manual file.
//
///////////////////////////////////////////////////////////////////////////////

std::string OpenManual::_manualFile() const
{
#if __APPLE__
  
  const std::string relativePath ( "/../Resources/" );

#else

  const std::string relativePath ( "/../docs/" );

#endif

  return Usul::Strings::format ( Usul::CommandLine::Arguments::instance().directory(), relativePath, _manual );
}
