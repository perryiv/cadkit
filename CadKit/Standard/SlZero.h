
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  SlZero: Functions for working with the value zero. Used in other templates.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_STANDARD_LIBRARY_ZERO_FUNCTIONS_H_
#define _CADKIT_STANDARD_LIBRARY_ZERO_FUNCTIONS_H_


namespace CadKit
{
///////////////////////////////////////////////////////////////////////////////
//
//  Is the number zero? Needed for template support.
//
///////////////////////////////////////////////////////////////////////////////

inline bool isZero ( const          double &num ) { return ( 0.0  == num ); }
inline bool isZero ( const          float  &num ) { return ( 0.0f == num ); }
inline bool isZero ( const unsigned int &num )    { return ( 0    == num ); }
inline bool isZero ( const          int &num )    { return ( 0    == num ); }
inline bool isZero ( const unsigned short &num )  { return ( 0    == num ); }
inline bool isZero ( const          short &num )  { return ( 0    == num ); }
inline bool isZero ( const unsigned long &num )   { return ( 0    == num ); }
inline bool isZero ( const          long &num )   { return ( 0    == num ); }


///////////////////////////////////////////////////////////////////////////////
//
//  Get the number zero. Needed for template support.
//
///////////////////////////////////////////////////////////////////////////////

inline void getZero ( double &zero )         { zero = 0.0; }
inline void getZero ( float  &zero )         { zero = 0.0f; }
inline void getZero ( unsigned int   &zero ) { zero = 0; }
inline void getZero (          int   &zero ) { zero = 0; }
inline void getZero ( unsigned short &zero ) { zero = 0; }
inline void getZero (          short &zero ) { zero = 0; }
inline void getZero ( unsigned long  &zero ) { zero = 0; }
inline void getZero (          long  &zero ) { zero = 0; }


}; // namespace CadKit.


#endif // _CADKIT_STANDARD_LIBRARY_TEST_ZERO_FUNCTIONS_H_
