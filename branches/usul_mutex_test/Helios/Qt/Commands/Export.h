
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Defines what HELIOS_QT_COMMANDS_EXPORT means.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_HELIOS_QT_COMMANDS_IMPORT_EXPORT_H_
#define _CADKIT_HELIOS_QT_COMMANDS_IMPORT_EXPORT_H_


#ifdef _WIN32
# pragma warning ( disable : 4275 ) // Turn off this warning, it doesn't apply.
# pragma warning ( disable : 4251 ) // See note 3ce3a140-2edf-11d4-98a4-0040054c86c7.
# ifdef _COMPILING_HELIOS_QT_COMMANDS
#   define HELIOS_QT_COMMANDS_EXPORT __declspec ( dllexport ) // We are compiling this library so the classes are exported.
# else
#   define HELIOS_QT_COMMANDS_EXPORT __declspec ( dllimport ) // The classes will be imported into the client's code.
# endif
#else // _WIN32
# define HELIOS_QT_COMMANDS_EXPORT
#endif


#endif // _CADKIT_HELIOS_QT_COMMANDS_IMPORT_EXPORT_H_
