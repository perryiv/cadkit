
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Defines what MINERVA_QT_WIDGETS_EXPORT means.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _MINERVA_QT_WIDGETS_DLL_EXPORT_H_
#define _MINERVA_QT_WIDGETS_DLL_EXPORT_H_

#ifdef _WIN32
# pragma warning ( disable : 4275 ) // Irrelevant VC6 warning.
# pragma warning ( disable : 4251 ) // Irrelevant VC6 warning.
# ifdef _COMPILING_MINERVA_QT_WIDGETS
#   define MINERVA_QT_WIDGETS_EXPORT __declspec ( dllexport ) // We are compiling this library so the classes are exported.
# else
#   define MINERVA_QT_WIDGETS_EXPORT __declspec ( dllimport ) // The classes will be imported into the client's code.
# endif
#else // _WIN32
# define MINERVA_QT_WIDGETS_EXPORT
#endif

#endif // _MINERVA_QT_WIDGETS_DLL_EXPORT_H_
