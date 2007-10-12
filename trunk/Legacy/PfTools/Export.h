
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Defines what PF_TOOLS_EXPORT means.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _PF_TOOLS_DLL_EXPORT_H_
#define _PF_TOOLS_DLL_EXPORT_H_

#ifdef _WIN32
# pragma warning ( disable : 4275 ) // Irrelevant VC6 warning.
# pragma warning ( disable : 4251 ) // Irrelevant VC6 warning.
# ifdef _COMPILING_PF_TOOLS
#   define PF_TOOLS_EXPORT __declspec ( dllexport ) // We are compiling this library so the classes are exported.
# else
#   define PF_TOOLS_EXPORT __declspec ( dllimport ) // The classes will be imported into the client's code.
# endif
#else // not _WIN32
# define PF_TOOLS_EXPORT
#endif

#endif // _PF_TOOLS_DLL_EXPORT_H_
