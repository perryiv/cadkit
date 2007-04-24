
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Defines what XML_TREE_EXPORT means.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _XML_TREE_DLL_EXPORT_H_
#define _XML_TREE_DLL_EXPORT_H_

#ifdef _WIN32
# pragma warning ( disable : 4275 ) // Irrelevant VC6 warning.
# pragma warning ( disable : 4251 ) // Irrelevant VC6 warning.
# ifdef _COMPILING_XML_TREE
#   define XML_TREE_EXPORT __declspec ( dllexport ) // We are compiling this library so the classes are exported.
# else
#   define XML_TREE_EXPORT __declspec ( dllimport ) // The classes will be imported into the client's code.
# endif
#else // not _WIN32
# define XML_TREE_EXPORT
#endif

#endif // _XML_TREE_DLL_EXPORT_H_
