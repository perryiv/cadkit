
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Defines what VTK_TOOLS_EXPORT means.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __VTK_TOOLS_DLL_EXPORT_H__
#define __VTK_TOOLS_DLL_EXPORT_H__

#ifdef _WIN32
# pragma warning ( disable : 4275 ) // Irrelevant VC6 warning.
# pragma warning ( disable : 4251 ) // Irrelevant VC6 warning.
# ifdef _COMPILING_VTK_TOOLS
#   define VTK_TOOLS_EXPORT __declspec ( dllexport ) // We are compiling this library so the classes are exported.
# else
#   define VTK_TOOLS_EXPORT __declspec ( dllimport ) // The classes will be imported into the client's code.
# endif
#else // _WIN32
# define VTK_TOOLS_EXPORT
#endif

#endif // __VTK_TOOLS_DLL_EXPORT_H__
