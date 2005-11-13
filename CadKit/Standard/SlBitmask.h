
///////////////////////////////////////////////////////////////////////////////
//
//  BSD License
//  http://www.opensource.org/licenses/bsd-license.html
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without 
//  modification, are permitted provided that the following conditions are met:
//
//  - Redistributions of source code must retain the above copyright notice, 
//    this list of conditions and the following disclaimer. 
//
//  - Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution. 
//
//  - Neither the name of the CAD Toolkit nor the names of its contributors may
//    be used to endorse or promote products derived from this software without
//    specific prior written permission. 
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
//  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
//  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
//  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
//  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
//  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
//  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
//  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
//  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
//  POSSIBILITY OF SUCH DAMAGE.
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

template<class N, class B> inline bool hasBits ( const N &number, const B &bits )
{
  return ( ( number & bits ) == ( static_cast<N>(bits) ) );
}


//////////////////////////////////////////////////////////////////////////
//
//  See if the "bits" are in "number". These non-template functions are 
//  here to eliminate VC++ warning 4018.
//
//////////////////////////////////////////////////////////////////////////

inline bool hasBits ( const unsigned long &number, const unsigned long &bits )
{
  return ( ( number & bits ) == bits );
}


//////////////////////////////////////////////////////////////////////////
//
//  Add the "bits" to "number".
//
//////////////////////////////////////////////////////////////////////////

template<class N, class B> inline N addBits ( const N &number, const B &bits )
{
  return ( number | bits );
}


//////////////////////////////////////////////////////////////////////////
//
//  Remove the "bits" from "number".
//
//////////////////////////////////////////////////////////////////////////

template<class N, class B> inline N removeBits ( const N &number, const B &bits )
{
  return ( ( number & bits ) ? ( number ^ bits ) : number );
}


//////////////////////////////////////////////////////////////////////////
//
//  Toggle the "bits" in "number".
//
//////////////////////////////////////////////////////////////////////////

template<class N, class B> inline N toggleBits ( const N &number, const B &bits )
{
  return ( number ^ bits );
}


}; // namespace CadKit.


//////////////////////////////////////////////////////////////////////////
//
//  Nifty macro used to declare bitmask handeling members functions.
//  Put this in your class declaration.
//
//////////////////////////////////////////////////////////////////////////

#define SL_DECLARE_BITMASK_FUNCTIONS(the_function_name,flag_type,the_flags) \
  public: \
  const flag_type &get##the_function_name() const { return the_flags; } \
  bool has##the_function_name    ( const flag_type &bits ) { return CadKit::hasBits ( the_flags, bits ); } \
  void add##the_function_name    ( const flag_type &bits ) { the_flags = CadKit::addBits ( the_flags, bits ); } \
  void remove##the_function_name ( const flag_type &bits ) { the_flags = CadKit::removeBits ( the_flags, bits ); } \
  void set##the_function_name    ( const flag_type &bits ) { the_flags = bits; } \
  void toggle##the_function_name ( const flag_type &bits ) { the_flags = CadKit::toggleBits ( the_flags, bits ); }


#endif // _CADKIT_STANDARD_LIBRARY_BITMASK_H_
