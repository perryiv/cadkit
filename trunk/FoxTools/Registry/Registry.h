
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Registry functions.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _FOX_TOOLS_REGISTRY_FUNCTIONS_H_
#define _FOX_TOOLS_REGISTRY_FUNCTIONS_H_

#include "FoxTools/Headers/Registry.h"

#include <sstream>

namespace FoxTools {
namespace Registry {
namespace Detail {


///////////////////////////////////////////////////////////////////////////////
//
//  Generic definition.
//
///////////////////////////////////////////////////////////////////////////////

template < class T > struct IO{};


///////////////////////////////////////////////////////////////////////////////
//
//  Read/write a string.
//
///////////////////////////////////////////////////////////////////////////////

template <> struct IO < std::string >
{
  static inline void write ( FX::FXRegistry &reg, const std::string &section, const std::string &key, const std::string &value )
  {
    reg.writeStringEntry ( section.c_str(), key.c_str(), value.c_str() );
  }
  static std::string read ( FX::FXRegistry &reg, const std::string &section, const std::string &key, const std::string &defaultValue )
  {
    return std::string ( reg.readStringEntry ( section.c_str(), key.c_str(), defaultValue.c_str() ) );
  }
};


///////////////////////////////////////////////////////////////////////////////
//
//  Read/write an integer.
//
///////////////////////////////////////////////////////////////////////////////

template <> struct IO < int >
{
  static inline void write ( FX::FXRegistry &reg, const std::string &section, const std::string &key, int value )
  {
    reg.writeIntEntry ( section.c_str(), key.c_str(), value );
  }
  static int read ( FX::FXRegistry &reg, const std::string &section, const std::string &key, int defaultValue )
  {
    return reg.readIntEntry ( section.c_str(), key.c_str(), defaultValue );
  }
};


///////////////////////////////////////////////////////////////////////////////
//
//  Read/write an unsigned integer.
//
///////////////////////////////////////////////////////////////////////////////

template <> struct IO < unsigned int >
{
  static inline void write ( FX::FXRegistry &reg, const std::string &section, const std::string &key, unsigned int value )
  {
    reg.writeUnsignedEntry ( section.c_str(), key.c_str(), value );
  }
  static unsigned int read ( FX::FXRegistry &reg, const std::string &section, const std::string &key, unsigned int defaultValue )
  {
    return reg.readUnsignedEntry ( section.c_str(), key.c_str(), defaultValue );
  }
};


///////////////////////////////////////////////////////////////////////////////
//
//  Read/write a double.
//
///////////////////////////////////////////////////////////////////////////////

template <> struct IO < double >
{
  static inline void write ( FX::FXRegistry &reg, const std::string &section, const std::string &key, double value )
  {
    reg.writeRealEntry ( section.c_str(), key.c_str(), value );
  }
  static double read ( FX::FXRegistry &reg, const std::string &section, const std::string &key, double defaultValue )
  {
    return reg.readRealEntry ( section.c_str(), key.c_str(), defaultValue );
  }
};


///////////////////////////////////////////////////////////////////////////////
//
//  Read/write a float.
//
///////////////////////////////////////////////////////////////////////////////

template <> struct IO < float >
{
  static inline void write ( FX::FXRegistry &reg, const std::string &section, const std::string &key, float value )
  {
    reg.writeRealEntry ( section.c_str(), key.c_str(), value );
  }
  static float read ( FX::FXRegistry &reg, const std::string &section, const std::string &key, float defaultValue )
  {
    return static_cast < float > ( reg.readRealEntry ( section.c_str(), key.c_str(), defaultValue ) );
  }
};


///////////////////////////////////////////////////////////////////////////////
//
//  Read the vector.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __APPLE__ // TODO, fix
template < class Vector, unsigned int Size > struct ReadVector
{
  Vector operator () ( FX::FXRegistry &reg, const std::string &section, const std::string &key, const Vector &vec )
  {
    std::istringstream s ( FoxTools::Registry::read ( reg, section, key, std::string() ) );
    Vector temp;
    for ( unsigned int i = 0; i < Size; ++i )
    {
      if ( s.eof() )
        temp[i] = vec[i];
      else
        s >> temp[i];
    }
    return temp;
  }
};
#endif


}; // namespace Detail


///////////////////////////////////////////////////////////////////////////////
//
//  Write the value.
//
///////////////////////////////////////////////////////////////////////////////

template < class T > inline void write ( FX::FXRegistry &reg, const std::string &section, const std::string &key, const T &value )
{
  FoxTools::Registry::Detail::IO<T>::write ( reg, section, key, value );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the value.
//
///////////////////////////////////////////////////////////////////////////////

template < class T > inline T read ( FX::FXRegistry &reg, const std::string &section, const std::string &key, const T &defaultValue )
{
  return FoxTools::Registry::Detail::IO<T>::read ( reg, section, key, defaultValue );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the container.
//
///////////////////////////////////////////////////////////////////////////////

template < class Itr > inline void write ( FX::FXRegistry &reg, const std::string &section, const std::string &key, Itr begin, Itr end )
{
  std::ostringstream s;
  for ( Itr i = begin; i != end; ++i )
    s << *i << " ";
  FoxTools::Registry::write ( reg, section, key, s.str() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Fill the container.
//
///////////////////////////////////////////////////////////////////////////////

template < class Itr > inline void read ( FX::FXRegistry &reg, const std::string &section, const std::string &key, Itr begin, Itr end )
{
  std::istringstream s ( FoxTools::Registry::read ( reg, section, key, std::string() ) );
  Itr i = begin;
  while ( !s.eof() && end != i )
  {
    s >> *i;
    ++i;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Fill the container.
//
///////////////////////////////////////////////////////////////////////////////

template < class C > inline void read ( FX::FXRegistry &reg, const std::string &section, const std::string &key, C &c )
{
  typedef typename C::value_type Value;
  std::istringstream s ( FoxTools::Registry::read ( reg, section, key, std::string() ) );
  while ( !s.eof() )
  {
    Value value;
    s >> value;
    c.insert ( c.end(), value );
  }
}


}; // namespace Registry
}; // namespace FoxTools


#endif // _FOX_TOOLS_REGISTRY_FUNCTIONS_H_
