
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  SlConvertStrings: String conversion functions.
//
//  Note: If CadKit::convert() works correctly then this file will be 
//  depreciated.
//
///////////////////////////////////////////////////////////////////////////////


#ifndef _CADKIT_STANDARD_LIBRARY_CONVERT_STRINGS_HEADER_
#define _CADKIT_STANDARD_LIBRARY_CONVERT_STRINGS_HEADER_

#include "SlString.h"
#include "SlUnicode.h"


namespace CadKit
{
///////////////////////////////////////////////////////////////////////////////
//
//  Unicode to ansi.
//
///////////////////////////////////////////////////////////////////////////////

inline SlAString unicodeToAnsi ( const SlWString &ws )
{
  SlAString cs;
  HRESULT hr = unicodeToAnsi ( ws, cs );
  return ( SUCCEEDED ( hr ) ) ? cs : SlAString ( "" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Ansi to unicode.
//
///////////////////////////////////////////////////////////////////////////////

inline SlWString ansiToUnicode ( const SlAString &cs )
{
  SlWString ws;
  HRESULT hr = ansiToUnicode ( cs, ws );
  return ( SUCCEEDED ( hr ) ) ? ws : SlWString ( L"" );
}


}; // namespace CadKit.


///////////////////////////////////////////////////////////////////////////////
//
//  For convenience.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef CK_W2A
#define CK_W2A CadKit::unicodeToAnsi
#endif // CK_W2A

#ifndef CK_A2W
#define CK_A2W CadKit::ansiToUnicode
#endif // CK_A2W


#endif // _CADKIT_STANDARD_LIBRARY_CONVERT_STRINGS_HEADER_
