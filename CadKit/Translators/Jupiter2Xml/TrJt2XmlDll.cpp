
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  TrJt2Xml.cpp : Defines the initialization routines for the DLL.
//
///////////////////////////////////////////////////////////////////////////////

#include "TrJt2XmlPrecompiled.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include "Standard/SlPrint.h"
#endif


#ifdef _WIN32


///////////////////////////////////////////////////////////////////////////////
//
//  Entry point for Win32 DLL.
//
///////////////////////////////////////////////////////////////////////////////

BOOL APIENTRY DllMain ( HINSTANCE hModule, DWORD ul_reason_for_call, LPVOID lpReserved )
{
  return TRUE;
}


#else


///////////////////////////////////////////////////////////////////////////////
//
//  Entry point for Linux shared library.
//
///////////////////////////////////////////////////////////////////////////////

void _init()
{
  SL_PRINT ( "In TrJt2Xml::_init()\n" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Exit function for Linux shared library.
//
///////////////////////////////////////////////////////////////////////////////

void _fini()
{
  SL_PRINT ( "In TrJt2Xml::_fini()\n" );
}


#endif
