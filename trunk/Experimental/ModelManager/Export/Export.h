
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Jeff Conner
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Defines what USUL_EXPORT means.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _LIB_MODEL_MANAGER_DLL_EXPORT_H_
#define _LIB_MODEL_MANAGER_DLL_EXPORT_H_

#ifdef _WIN32
# pragma warning ( disable : 4275 ) // Irrelevant VC6 warning.
# pragma warning ( disable : 4251 ) // Irrelevant VC6 warning.
# ifdef _COMPILING_LIB_MODEL_MANAGER
#   define LIB_MODEL_MANAGER_EXPORT __declspec ( dllexport ) // We are compiling this library so the classes are exported.
# else
#   define LIB_MODEL_MANAGER_EXPORT __declspec ( dllimport ) // The classes will be imported into the client's code.
# endif
#else // _WIN32
# define LIB_MODEL_MANAGER_EXPORT
#endif

#endif // _LIB_MODEL_MANAGER_DLL_EXPORT_H_
