
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  FoxApi.h: Defines what GUI_FOX_API means.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_GUI_FOX_LIBRARY_API_H_
#define _CADKIT_GUI_FOX_LIBRARY_API_H_


///////////////////////////////////////////////////////////////////////////////
//
//  Here we define what GUI_FOX_API means.
//
///////////////////////////////////////////////////////////////////////////////

#ifdef _WIN32
# pragma warning(disable: 4275) // Turn off this warning, it doesn't apply.
# pragma warning(disable: 4251) // See note 3ce3a140-2edf-11d4-98a4-0040054c86c7.
# ifdef _CADKIT_COMPILING_GUI_FOX_LIBRARY
#   define GUI_FOX_API __declspec(dllexport) // We are compiling this library so the classes are exported.
# else
#   define GUI_FOX_API __declspec(dllimport) // The classes will be imported into the client's code.
# endif
#else // _WIN32
# define GUI_FOX_API
#endif


#endif // _CADKIT_GUI_FOX_LIBRARY_API_H_
