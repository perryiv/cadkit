
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
//  Defines what MINERVA_DOCUMENT_EXPORT means.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _MINERVA_DOCUMENT_EXPORT_H_
#define _MINERVA_DOCUMENT_EXPORT_H_

#ifdef _WIN32
# pragma warning ( disable : 4275 ) // Irrelevant VC6 warning.
# pragma warning ( disable : 4251 ) // Irrelevant VC6 warning.
# ifdef _COMPILING_MINERVA_DOCUMENT
#   define MINERVA_DOCUMENT_EXPORT __declspec ( dllexport ) // We are compiling this library so the classes are exported.
# else
#   define MINERVA_DOCUMENT_EXPORT __declspec ( dllimport ) // The classes will be imported into the client's code.
# endif
#else // _WIN32
# define MINERVA_DOCUMENT_EXPORT
#endif

#endif // MINERVA_DOCUMENT_EXPORT
