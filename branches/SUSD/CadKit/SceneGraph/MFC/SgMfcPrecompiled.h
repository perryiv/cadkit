
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
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
# define NOMINMAX
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
# include "SceneGraph/OpenGL/SgGlClientHeaders.h"
#endif

// Redefine assert to be MFC's assert.
#ifdef _DEBUG
# ifdef SL_ASSERT
# undef SL_ASSERT
# define SL_ASSERT ASSERT
# endif // SL_ASSERT
#endif  // _DEBUG

#endif // _CADKIT_SCENEGRAPH_MFC_WRAPPER_INTERNAL_PRECOMPILED_HEADERS_H_
