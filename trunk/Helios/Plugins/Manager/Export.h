
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Defines what HELIOS_PLUGINS_EXPORT means.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_PLUGINS_MANAGER_EXPORT_H_
#define _CADKIT_PLUGINS_MANAGER_EXPORT_H_

#ifdef _WIN32
# pragma warning ( disable : 4275 ) // Irrelevant VC6 warning.
# pragma warning ( disable : 4251 ) // Irrelevant VC6 warning.
# ifdef _COMPILING_HELIOS_PLUGINS_MANAGER
#   define HELIOS_PLUGINS_EXPORT __declspec ( dllexport ) // We are compiling this library so the classes are exported.
# else
#   define HELIOS_PLUGINS_EXPORT __declspec ( dllimport ) // The classes will be imported into the client's code.
# endif
#else // _WIN32
# define HELIOS_PLUGINS_EXPORT
#endif

#endif // _CADKIT_PLUGINS_MANAGER_EXPORT_H_
