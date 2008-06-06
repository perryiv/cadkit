
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Functions to make Usul pointers compatible with boost serialization.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_POINTERS_BOOST_SERIALIZE_H__
#define __USUL_POINTERS_BOOST_SERIALIZE_H__

#include <boost/config.hpp>

#include <boost/serialization/utility.hpp>
#include <boost/serialization/split_free.hpp>

#include "Usul/Pointers/SmartPointer.h"
#include "Usul/Pointers/QueryPointer.h"

namespace boost { 
namespace serialization {


///////////////////////////////////////////////////////////////////////////////
//
//  Save smart pointer.
//
///////////////////////////////////////////////////////////////////////////////

template<class Archive, class T, class Config_ >
inline void save(
    Archive & ar,
    const Usul::Pointers::SmartPointer< T, Config_ > &t,
    const unsigned int /* file_version */ )
{
  const T * t_ptr ( t.get() );
  ar << boost::serialization::make_nvp( "px", t_ptr );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Load smart pointer.
//
///////////////////////////////////////////////////////////////////////////////

template<class Archive, class T, class Config_ >
inline void load(
    Archive & ar,
    Usul::Pointers::SmartPointer< T, Config_ > &t,
    const unsigned int /* file_version */ )
{
  T * p ( 0x0 );
  ar >> boost::serialization::make_nvp( "px", p );
  t = p;
}


///////////////////////////////////////////////////////////////////////////////
//
// split non-intrusive serialization function member into separate
// non intrusive save/load member functions
//
///////////////////////////////////////////////////////////////////////////////

template<class Archive, class T, class Config_ >
inline void serialize(
    Archive & ar,
    Usul::Pointers::SmartPointer< T, Config_ > &t,
    const unsigned int file_version
){
    boost::serialization::split_free(ar, t, file_version);
}


///////////////////////////////////////////////////////////////////////////////
//
//  Save query pointer.
//
///////////////////////////////////////////////////////////////////////////////

template<class Archive, class T, class Config_ >
inline void save(
    Archive & ar,
    const Usul::Pointers::QueryPointer< T, Config_ > &t,
    const unsigned int /* file_version */ )
{
  const T * t_ptr ( t.get() );
  ar << boost::serialization::make_nvp( "px", t_ptr );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Load query pointer.
//
///////////////////////////////////////////////////////////////////////////////

template<class Archive, class T, class Config_ >
inline void load(
    Archive & ar,
    Usul::Pointers::QueryPointer< T, Config_ > &t,
    const unsigned int /* file_version */ )
{
  T * p ( 0x0 );
  ar >> boost::serialization::make_nvp( "px", p );
  t = p;
}


///////////////////////////////////////////////////////////////////////////////
//
// split non-intrusive serialization function member into separate
// non intrusive save/load member functions
//
///////////////////////////////////////////////////////////////////////////////

template<class Archive, class T, class Config_ >
inline void serialize(
    Archive & ar,
    Usul::Pointers::QueryPointer< T, Config_ > &t,
    const unsigned int file_version
){
    boost::serialization::split_free(ar, t, file_version);
}

} // serialization
} // namespace boost

#endif // __USUL_POINTERS_BOOST_SERIALIZE_H__
