
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
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

  const T1 &    getFirst()  const { return this->first; }
  T1 &          getFirst()        { return this->first; }
  void          setFirst ( T1 &t1 ) { this->first = t1; }

  const T2 &    getSecond() const { return this->second; }
  T2 &          getSecond()       { return this->second; }
  void          setSecond ( T2 &t2 ) { this->second = t2; }

  void          getValue ( T1 &t1, T2 &t2 ) { t1 = this->first; t2 = this->second; }
  void          setValue ( const T1 &t1, const T2 &t2 ) { this->first = t1; this->second = t2; }
  void          setValue ( const SlPair &pair ) { this->setValue ( pair.getFirst(), pair.getSecond() ); }
};


template<class T1, class T2, class T3> class SlTriplet : public SlPair<SlPair<T1,T2>,T3>
{
public:

  SlTriplet() : SlPair<SlPair<T1,T2>,T3>(){}
  SlTriplet ( const T1 &t1, const T2 &t2, const T3 &t3 ) : SlPair<SlPair<T1,T2>,T3 > ( SlPair<T1,T2> ( t1, t2 ), t3 ){}

  const T1 &    getFirst()  const { return this->first.first; }
  T1 &          getFirst()        { return this->first.first; }
  void          setFirst ( T1 &t1 ) { this->first.first = t1; }

  const T2 &    getSecond() const { return this->first.second; }
  T2 &          getSecond()       { return this->first.second; }
  void          setSecond ( T2 &t2 ) { this->first.second = t2; }

  const T3 &    getThird() const { return this->second; }
  T3 &          getThird()       { return this->second; }
  void          setThird ( T3 &t3 ) { this->second = t3; }

  void          getValue ( T1 &t1, T2 &t2, T3 &t3 ) { t1 = this->first.first; t2 = this->first.second; t2 = this->second; }
  void          setValue ( const T1 &t1, const T2 &t2, const T3 &t3 ) { this->first.first = t1; this->first.second = t2; this->second = t3; }
  void          setValue ( const SlTriplet &triplet ) { this->setValue ( triplet.getFirst(), triplet.getSecond(), triplet.getThird() ); }
};


}; // namespace CadKit

#endif
