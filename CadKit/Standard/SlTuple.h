
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
//  SlTuple.h: Some generic tuple classes.
//
//////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_STANDARD_LIBRARY_TUPLE_H_
#define _CADKIT_STANDARD_LIBRARY_TUPLE_H_


namespace CadKit
{
template<class T1, class T2> class SlPair : public std::pair<T1,T2>
{
public:

  SlPair() : std::pair<T1,T2>(){}
  SlPair ( const T1 &t1, const T2 &t2 ) : std::pair<T1,T2> ( t1, t2 ){}

  const T1 &    getFirst()  const { return first; }
  T1 &          getFirst()        { return first; }
  void          setFirst ( T1 &t1 ) { first = t1; }

  const T2 &    getSecond() const { return second; }
  T2 &          getSecond()       { return second; }
  void          setSecond ( T2 &t2 ) { second = t2; }

  void          getValue ( T1 &t1, T2 &t2 ) { t1 = first; t2 = second; }
  void          setValue ( const T1 &t1, const T2 &t2 ) { first = t1; second = t2; }
  void          setValue ( const SlPair &pair ) { this->setValue ( pair.getFirst(), pair.getSecond() ); }
};


template<class T1, class T2, class T3> class SlTriplet : public SlPair<SlPair<T1,T2>,T3>
{
public:

  SlTriplet() : SlPair<SlPair<T1,T2>,T3>(){}
  SlTriplet ( const T1 &t1, const T2 &t2, const T3 &t3 ) : SlPair<SlPair<T1,T2>,T3 > ( SlPair<T1,T2> ( t1, t2 ), t3 ){}

  const T1 &    getFirst()  const { return first.first; }
  T1 &          getFirst()        { return first.first; }
  void          setFirst ( T1 &t1 ) { first.first = t1; }

  const T2 &    getSecond() const { return first.second; }
  T2 &          getSecond()       { return first.second; }
  void          setSecond ( T2 &t2 ) { first.second = t2; }

  const T3 &    getThird() const { return second; }
  T3 &          getThird()       { return second; }
  void          setThird ( T3 &t3 ) { second = t3; }

  void          getValue ( T1 &t1, T2 &t2, T3 &t3 ) { t1 = first.first; t2 = first.second; t2 = second; }
  void          setValue ( const T1 &t1, const T2 &t2, const T3 &t3 ) { first.first = t1; first.second = t2; second = t3; }
  void          setValue ( const SlTriplet &triplet ) { this->setValue ( pair.getFirst(), pair.getSecond(), pair.getThird() ); }
};


}; // namespace CadKit

#endif
