
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Defines what GSG_CORE_EXPORT means.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _GENERIC_SCENE_GRAPH_CORE_DLL_IMPORT_EXPORT_H_
#define _GENERIC_SCENE_GRAPH_CORE_DLL_IMPORT_EXPORT_H_


///////////////////////////////////////////////////////////////////////////////
//
//  Here we define what GSG_CORE_EXPORT means.
//
///////////////////////////////////////////////////////////////////////////////

#ifdef _WIN32
# pragma warning ( disable : 4275 ) // Turn off this warning, it doesn't apply.
# pragma warning ( disable : 4251 ) // See note 3ce3a140-2edf-11d4-98a4-0040054c86c7.
# ifdef COMPILING_GSG_CORE
#   define GSG_CORE_EXPORT __declspec ( dllexport ) // We are compiling this library so the classes are exported.
# else
#   define GSG_CORE_EXPORT __declspec ( dllimport ) // The classes will be imported into the client's code.
# endif
#else // _WIN32
# define GSG_CORE_EXPORT
#endif


#endif // _GENERIC_SCENE_GRAPH_CORE_DLL_IMPORT_EXPORT_H_
