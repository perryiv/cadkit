
///////////////////////////////////////////////////////////////////////////////
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
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  DbJtPrecompiled.h: The precompiled header file.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_DATABASE_JUPITER_LIBRARY_INTERNAL_PRECOMPILED_HEADERS_H_
#define _CADKIT_DATABASE_JUPITER_LIBRARY_INTERNAL_PRECOMPILED_HEADERS_H_

#ifdef _WIN32
# pragma warning(disable:4251) // See note 3ce3a140-2edf-11d4-98a4-0040054c86c7.
# pragma warning(disable:4786) // Truncated debug names.
# define WIN32_LEAN_AND_MEAN   // Exclude rarely-used stuff from Windows headers.
# define NOMINMAX              // Do not define min and max as macros.
# include <windows.h>          // Base Windows header file.
#endif // _WIN32

// Needed headers.
#ifdef _CADKIT_USE_PRECOMPILED_HEADERS
# include "Standard/SlClientHeaders.h"
# include <iostream>
#endif

#endif // _CADKIT_DATABASE_JUPITER_LIBRARY_INTERNAL_PRECOMPILED_HEADERS_H_
