
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//
//  SlBitmask: Inline functions for working with bits.
//
//////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_STANDARD_LIBRARY_BITMASK_H_
#define _CADKIT_STANDARD_LIBRARY_BITMASK_H_


namespace CadKit 
{
//////////////////////////////////////////////////////////////////////////
//
//  See if the "bits" are in "number".
//
//////////////////////////////////////////////////////////////////////////

template<class N, class B> inline bool hasBits ( N number, B bits )
{
  return ( ( number & bits ) == ( static_cast<N>(bits) ) );
//  return ( ( number & bits ) == bits );
}


//////////////////////////////////////////////////////////////////////////
//
//  See if the "bits" are in "number". These non-template functions are 
//  here to eliminate VC++ warning 4018.
//
//////////////////////////////////////////////////////////////////////////
/*
inline bool hasBits ( unsigned long number, unsigned long bits )
{
  return ( ( number & bits ) == bits );
}
*/

//////////////////////////////////////////////////////////////////////////
//
//  Add the "bits" to "number".
//
//////////////////////////////////////////////////////////////////////////

template<class N, class B> inline N addBits ( N number, B bits )
{
  return ( number | bits );
}


//////////////////////////////////////////////////////////////////////////
//
//  Remove the "bits" from "number".
//
//////////////////////////////////////////////////////////////////////////

template<class N, class B> inline N removeBits ( N number, B bits )
{
  return ( ( number & bits ) ? ( number ^ bits ) : number );
}


//////////////////////////////////////////////////////////////////////////
//
//  Toggle the "bits" in "number".
//
//////////////////////////////////////////////////////////////////////////

template<class N, class B> inline N toggleBits ( N number, B bits )
{
  return ( number ^ bits );
}


}; // namespace CadKit.


//////////////////////////////////////////////////////////////////////////
//
//  Nifty macro used to declare bitmask handling members functions.
//  Put this in your class declaration.
//
//////////////////////////////////////////////////////////////////////////

#define SL_DECLARE_BITMASK_FUNCTIONS(the_function_name,flag_type,the_flags) \
  public: \
  flag_type get##the_function_name() const { return the_flags; } \
  bool has##the_function_name    ( flag_type bits ) const { return CadKit::hasBits ( the_flags, bits ); } \
  void add##the_function_name    ( flag_type bits ) { the_flags = CadKit::addBits ( the_flags, bits ); } \
  void remove##the_function_name ( flag_type bits ) { the_flags = CadKit::removeBits ( the_flags, bits ); } \
  void set##the_function_name    ( flag_type bits ) { the_flags = bits; } \
  void toggle##the_function_name ( flag_type bits ) { the_flags = CadKit::toggleBits ( the_flags, bits ); }


#endif // _CADKIT_STANDARD_LIBRARY_BITMASK_H_
