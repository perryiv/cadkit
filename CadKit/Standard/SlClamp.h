
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  SlClamp: Inline function for clamping a value.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_STANDARD_LIBRARY_INLINE_CLAMP_FUNCTION_H_
#define _CADKIT_STANDARD_LIBRARY_INLINE_CLAMP_FUNCTION_H_


namespace CadKit
{
///////////////////////////////////////////////////////////////////////////////
//
//  Clamp the value to the range.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline void clamp ( const T &minValue, const T &maxValue, T &value )
{
  if ( value < minValue )
    value = minValue;
  else if ( value > maxValue )
    value = maxValue;
}


}; // namespace CadKit.


///////////////////////////////////////////////////////////////////////////////
//
// For convenience and backward compatability.
//
///////////////////////////////////////////////////////////////////////////////

#define SL_CLAMP CadKit::clamp


#endif // _CADKIT_STANDARD_LIBRARY_INLINE_CLAMP_FUNCTION_H_
