
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  FoxDll.cpp : Defines the initialization routines for the DLL.
//
///////////////////////////////////////////////////////////////////////////////

#include "Standard/SlPrint.h"

#ifdef _WIN32

#define NOMINMAX // Do not define min and max as macros.
#include <windows.h>


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
  SL_PRINT ( "In SgDll::_init()\n" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Exit function for Linux shared library.
//
///////////////////////////////////////////////////////////////////////////////

void _fini()
{
  SL_PRINT ( "In SgDll::_fini()\n" );
}


#endif
