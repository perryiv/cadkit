
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Defines what OPEN_THREADS_EXTENSIONS_EXPORT means.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _OPEN_THREADS_EXTENSIONS_DLL_EXPORT_H_
#define _OPEN_THREADS_EXTENSIONS_DLL_EXPORT_H_

#ifdef _WIN32
# pragma warning ( disable : 4275 ) // Irrelevant VC6 warning.
# pragma warning ( disable : 4251 ) // Irrelevant VC6 warning.
# ifdef _COMPILING_OPEN_THREADS_EXTENSIONS
#   define OPEN_THREADS_EXTENSIONS_EXPORT __declspec ( dllexport ) // We are compiling this library so the classes are exported.
# else
#   define OPEN_THREADS_EXTENSIONS_EXPORT __declspec ( dllimport ) // The classes will be imported into the client's code.
# endif
#else // not _WIN32
# define OPEN_THREADS_EXTENSIONS_EXPORT
#endif

#endif // _OPEN_THREADS_EXTENSIONS_DLL_EXPORT_H_
