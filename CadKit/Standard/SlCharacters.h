
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  SlCharacters: Special characters.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_STANDARD_LIBRARY_CHARACTERS_H_
#define _CADKIT_STANDARD_LIBRARY_CHARACTERS_H_


namespace CadKit 
{
// These are #defined instead of true constants because if we introduce 
// wchar_t we need <wchar.h>, plus we would need 4 definitions instead of 
// just 2 (2 for char, 2 for wchar_t).
#define SL_CHAR_COPYRIGHT      = 0xA9;
#define SL_CHAR_BULLET_SOLID   = 0xF9;
};


#endif // _CADKIT_STANDARD_LIBRARY_CHARACTERS_H_
