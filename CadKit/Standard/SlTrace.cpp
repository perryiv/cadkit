
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  SlTrace: Trace definitions.
//
///////////////////////////////////////////////////////////////////////////////

#include "SlPrecompiled.h"
#include "SlTrace.h"
#include "SlAssert.h"
#include "SlFunctionDefines.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include <stdio.h>
# include <stdarg.h>
# include <string.h>
#endif // _CADKIT_USE_PRECOMPILED_HEADERS


namespace CadKit
{
///////////////////////////////////////////////////////////////////////////////
//
//  This will dump to the Output window on VC++, and to stdout on unix.
//
///////////////////////////////////////////////////////////////////////////////

void _cadkitStandardLibraryDebugTrace ( const char *formatString, ... )
{
  SL_ASSERT ( formatString );

  // The string we write to. Do not make these static because I want it 
  // to be thread-safe.
  const unsigned int stringLength = 32767;
  char theString[stringLength + 1];

  // Argument pointer.
  va_list ap;

  // Start variable argument processing.
  va_start ( ap, formatString );

  // Format the text into the string.
  SL_VSNPRINTF ( theString, stringLength, formatString, ap );

  // End variable argument processing.
  va_end ( ap );

  // Check for memory problems.
  SL_ASSERT ( ::strlen ( theString ) <= stringLength );

  // Print the string.
#ifdef _WIN32
  ::OutputDebugString ( theString );
#else
  ::printf ( theString );
  ::fflush ( stdout ); 
#endif
}

}; // namespace CadKit
