
////////////////////////////////////////////////////////////////////////////////
//
//  BSD License
//  http://www.opensource.org/licenses/bsd-license.html
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without 
//  modification, are permitted provided that the following conditions are met:
//
//  - Redistributions of source code must retain the above copyright notice, 
//    this list of conditions and the following disclaimer. 
//
//  - Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution. 
//
//  - Neither the name of the CAD Toolkit nor the names of its contributors may
//    be used to endorse or promote products derived from this software without
//    specific prior written permission. 
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
//  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
//  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
//  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
//  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
//  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
//  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
//  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
//  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
//  POSSIBILITY OF SUCH DAMAGE.
//
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//
//  SlTrace: Trace definitions.
//
////////////////////////////////////////////////////////////////////////////////

#include "SlPrecompiled.h"
#include "SlTrace.h"
#include "SlAssert.h"
#include "SlFunctionDefines.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include <stdio.h>
# ifdef _WIN32
#  include <stdarg.h>
# else
#  include <streambuf.h> // For va_list, is there a better header? Can't
                         // find where it is actually defined, but this 
                         // makes it compile.
# endif
#endif // _CADKIT_USE_PRECOMPILED_HEADERS


namespace CadKit
{
////////////////////////////////////////////////////////////////////////////////
//
//  This will dump to the Output window on VC++, and to stdout on unix.
//
////////////////////////////////////////////////////////////////////////////////

void CadKit::_modelspaceStandardLibraryDebugTrace ( const char *formatString, ... )
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
