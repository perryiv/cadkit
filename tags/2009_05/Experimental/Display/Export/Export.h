
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Defines what DISPLAY_LIBRARY_EXPORT means.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _MULTI_TARGET_RENDER_SCENE_LIBRARY_IMPORT_EXPORT_H_
#define _MULTI_TARGET_RENDER_SCENE_LIBRARY_IMPORT_EXPORT_H_


#ifdef _WIN32
# pragma warning ( disable : 4275 ) // Turn off this warning, it doesn't apply.
# pragma warning ( disable : 4251 ) // See note 3ce3a140-2edf-11d4-98a4-0040054c86c7.
# ifdef _COMPILING_DISPLAY_LIBRARY
#   define DISPLAY_LIBRARY_EXPORT __declspec ( dllexport ) // We are compiling this library so the classes are exported.
# else
#   define DISPLAY_LIBRARY_EXPORT __declspec ( dllimport ) // The classes will be imported into the client's code.
# endif
#else // _WIN32
# define DISPLAY_LIBRARY_EXPORT
#endif


#endif // _MULTI_TARGET_RENDER_SCENE_LIBRARY_IMPORT_EXPORT_H_
