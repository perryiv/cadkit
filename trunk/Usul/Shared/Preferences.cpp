
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Preference class to hold application-defined state.
//
///////////////////////////////////////////////////////////////////////////////

#include "Usul/Shared/Preferences.h"


using namespace Usul;
using namespace Usul::Shared;


///////////////////////////////////////////////////////////////////////////////
//
//  Return the single instance.
//
///////////////////////////////////////////////////////////////////////////////

namespace Usul
{
  namespace Shared
  {
    namespace Detail
    {
      Usul::Shared::Preferences *_instance ( 0x0 );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Preferences::Preferences() : 
  _bools(),
  _ints(),
  _uints(),
  _uints64(),
  _floats(),
  _doubles(),
  _strings()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Preferences::~Preferences()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the single instance.
//
///////////////////////////////////////////////////////////////////////////////

Preferences &Preferences::instance()
{
  if ( 0x0 == Usul::Shared::Detail::_instance )
    Usul::Shared::Detail::_instance = new Preferences;
  return *(Usul::Shared::Detail::_instance);
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the value.
//
///////////////////////////////////////////////////////////////////////////////

bool Preferences::getBool ( const std::string &key )
{
  return _bools[key];
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the value.
//
///////////////////////////////////////////////////////////////////////////////

const std::string &Preferences::getString ( const std::string &key )
{
  return _strings[key];
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the value.
//
///////////////////////////////////////////////////////////////////////////////

int Preferences::getInt ( const std::string &key )
{
  return _ints[key];
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the value.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Preferences::getUint ( const std::string &key )
{
  return _uints[key];
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the value.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Types::Uint64 Preferences::getUint64 ( const std::string &key )
{
  return _uints64[key];
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the value.
//
///////////////////////////////////////////////////////////////////////////////

float Preferences::getFloat ( const std::string &key, float defaultValue )
{
  Floats::iterator i ( _floats.find ( key ) );
  if ( _floats.end() == i )
  {
    _floats.insert ( Floats::value_type ( key, defaultValue ) );
    return defaultValue;
  }
  else
  {
    return i->second;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the value.
//
///////////////////////////////////////////////////////////////////////////////

double Preferences::getDouble ( const std::string &key )
{
  return _doubles[key];
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the value.
//
///////////////////////////////////////////////////////////////////////////////

void Preferences::setBool ( const std::string &key, bool value )
{
  _bools[key] = value;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the value.
//
///////////////////////////////////////////////////////////////////////////////

void Preferences::setString ( const std::string &key, const std::string &value )
{
  _strings[key] = value;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the value.
//
///////////////////////////////////////////////////////////////////////////////

void Preferences::setInt ( const std::string &key, int value )
{
  _ints[key] = value;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the value.
//
///////////////////////////////////////////////////////////////////////////////

void Preferences::setUint ( const std::string &key, unsigned int value )
{
  _uints[key] = value;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the value.
//
///////////////////////////////////////////////////////////////////////////////

void Preferences::setUint64 ( const std::string &key, Usul::Types::Uint64 value )
{
  _uints64[key] = value;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the value.
//
///////////////////////////////////////////////////////////////////////////////

void Preferences::setFloat ( const std::string &key, float value )
{
  _floats[key] = value;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the value.
//
///////////////////////////////////////////////////////////////////////////////

void Preferences::setDouble ( const std::string &key, double value )
{
  _doubles[key] = value;
}

