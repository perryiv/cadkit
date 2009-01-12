
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Defines what SERIALIZE_XML_EXPORT means.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _SERIALIZE_XML_EXPORT_H_
#define _SERIALIZE_XML_EXPORT_H_

#ifdef _WIN32
# pragma warning ( disable : 4275 ) // Irrelevant VC6 warning.
# pragma warning ( disable : 4251 ) // Irrelevant VC6 warning.
# ifdef _COMPILING_SERIALIZE_XML
#   define SERIALIZE_XML_EXPORT __declspec ( dllexport ) // We are compiling this library so the classes are exported.
# else
#   define SERIALIZE_XML_EXPORT __declspec ( dllimport ) // The classes will be imported into the client's code.
# endif
#else // _WIN32
# define SERIALIZE_XML_EXPORT
#endif

#endif // _SERIALIZE_XML_EXPORT_H_
