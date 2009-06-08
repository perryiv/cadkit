
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  A write-only, sharable log file.
//
///////////////////////////////////////////////////////////////////////////////

#include "Usul/File/Log.h"
#include "Usul/File/LineEnding.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Strings/Format.h"

#include "Usul/Config/Config.h"

using namespace Usul::File;

USUL_IMPLEMENT_TYPE_ID ( Log );

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Log::Log ( const std::string &file, bool lazyOpen ) : BaseClass(),
  _file ( file ),
  _out(),
  _count ( 0 )
{
  // Initialize the name to the given file.
  this->name ( file );

  // Are we supposed to open the file now?
  if ( false == lazyOpen )
    this->_open();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Log::~Log()
{
  Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( this, &Log::_destroy ), "2581088740" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destroy the object.
//
///////////////////////////////////////////////////////////////////////////////

void Log::_destroy()
{
  Guard guard ( this );

  _file.clear();
  _out.close();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Open the file.
//
///////////////////////////////////////////////////////////////////////////////

void Log::_open()
{
  Guard guard ( this );

  if ( true == _out.is_open() )
    return;

  if ( true == _file.empty() )
    throw std::runtime_error ( "Error 1878474700: log file name is empty" );

  _out.open ( _file.c_str() );

  if ( false == _out.is_open() )
    throw std::runtime_error ( Usul::Strings::format ( "Error 7948138890: failed to open log file: ", _file ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the string.
//
///////////////////////////////////////////////////////////////////////////////

void Log::write ( const std::string &s, bool appendNewLine, bool prependEventCount )
{
#ifdef USUL_USE_LOG_FILES
  Guard guard ( this );

  // Make sure it's open.
  this->_open();

  // Write the event count if we should.
  if ( true == prependEventCount )
    _out << _count << ", ";

  // Write the string.
  _out << s;

  // Append a new line and flush if we should.
  if ( true == appendNewLine )
    _out << Usul::File::lineEnding();

  // Increment the event count.
  ++_count;
#endif
}
