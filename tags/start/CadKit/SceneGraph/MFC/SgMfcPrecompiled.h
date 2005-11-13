
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

/////////////////////////////////////////////////////////////////////////////
//
//  SgMfcPrecompiled.h: The precompiled header file.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_SCENEGRAPH_MFC_WRAPPER_INTERNAL_PRECOMPILED_HEADERS_H_
#define _CADKIT_SCENEGRAPH_MFC_WRAPPER_INTERNAL_PRECOMPILED_HEADERS_H_

#ifdef _WIN32
# if _MSC_VER > 1000
#   pragma once
# endif // _MSC_VER > 1000
# define VC_EXTRALEAN    // Exclude rarely-used stuff from Windows headers
# include <afxwin.h>     // MFC core and standard components
# include <afxext.h>     // MFC extensions
# include <afxdtctl.h>   // MFC support for Internet Explorer 4 Common Controls
# include <afxpriv.h>    // MFC CPreviewView class
# ifndef _AFX_NO_AFXCMN_SUPPORT
#   include <afxcmn.h>   // MFC support for Windows Common Controls
# endif // _AFX_NO_AFXCMN_SUPPORT
# pragma warning(disable:4786) // Truncated debug names.
#endif // _WIN32

// Needed headers.
#ifdef _CADKIT_USE_PRECOMPILED_HEADERS
# include "SceneGraph/OpenGL/SgglClientHeaders.h"
#endif

// Redefine assert to be MFC's assert.
#ifdef _DEBUG
# ifdef SL_ASSERT
# undef SL_ASSERT
# define SL_ASSERT ASSERT
# endif // SL_ASSERT
#endif  // _DEBUG

#endif // _CADKIT_SCENEGRAPH_MFC_WRAPPER_INTERNAL_PRECOMPILED_HEADERS_H_
