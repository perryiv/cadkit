
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

#include "FoxTools/Registry/Registry.h"
#include "FoxTools/Functions/App.h"
#include "FoxTools/Headers/Registry.h"
#include "FoxTools/Headers/App.h"

#include <stdexcept>
#include <string>


namespace FoxTools {
namespace Registry {


///////////////////////////////////////////////////////////////////////////////
//
//  Read the value.
//
///////////////////////////////////////////////////////////////////////////////

std::string read ( const std::string &section, const std::string &key, const std::string &defaultValue )
{
  FX::FXRegistry &reg ( FoxTools::Functions::application()->reg() );
  const FX::FXchar *temp ( reg.readStringEntry ( section.c_str(), key.c_str(), defaultValue.c_str() ) );
  return ( temp ) ? temp : defaultValue;
}
unsigned int read ( const std::string &section, const std::string &key, unsigned int defaultValue )
{
  FX::FXRegistry &reg ( FoxTools::Functions::application()->reg() );
  return reg.readUnsignedEntry ( section.c_str(), key.c_str(), defaultValue );
}
int read ( const std::string &section, const std::string &key, int defaultValue )
{
  FX::FXRegistry &reg ( FoxTools::Functions::application()->reg() );
  return reg.readIntEntry ( section.c_str(), key.c_str(), defaultValue );
}
bool read ( const std::string &section, const std::string &key, bool defaultValue )
{
  FX::FXRegistry &reg ( FoxTools::Functions::application()->reg() );
  unsigned int temp ( reg.readUnsignedEntry ( section.c_str(), key.c_str(), defaultValue ) );
  return ( temp ) ? true : false;
}
float read ( const std::string &section, const std::string &key, float defaultValue )
{
  FX::FXRegistry &reg ( FoxTools::Functions::application()->reg() );
  return (float) reg.readRealEntry ( section.c_str(), key.c_str(), defaultValue );
}
double read ( const std::string &section, const std::string &key, double defaultValue )
{
  FX::FXRegistry &reg ( FoxTools::Functions::application()->reg() );
  return reg.readRealEntry ( section.c_str(), key.c_str(), defaultValue );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the value.
//
///////////////////////////////////////////////////////////////////////////////

void write ( const std::string &section, const std::string &key, const std::string &value )
{
  FX::FXRegistry &reg ( FoxTools::Functions::application()->reg() );
  if ( !reg.writeStringEntry ( section.c_str(), key.c_str(), value.c_str() ) )
    throw std::runtime_error ( "Error 3567717200: failed to write string to registry" );
}
void write ( const std::string &section, const std::string &key, unsigned int value )
{
  FX::FXRegistry &reg ( FoxTools::Functions::application()->reg() );
  if ( !reg.writeUnsignedEntry ( section.c_str(), key.c_str(), value ) )
    throw std::runtime_error ( "Error 2372154973: failed to write unsigned integer to registry" );
}
void write ( const std::string &section, const std::string &key, int value )
{
  FX::FXRegistry &reg ( FoxTools::Functions::application()->reg() );
  if ( !reg.writeIntEntry ( section.c_str(), key.c_str(), value ) )
    throw std::runtime_error ( "Error 2186142228: failed to write unsigned integer to registry" );
}
void write ( const std::string &section, const std::string &key, bool value )
{
  FX::FXRegistry &reg ( FoxTools::Functions::application()->reg() );
  if ( !reg.writeUnsignedEntry ( section.c_str(), key.c_str(), value ) )
    throw std::runtime_error ( "Error 1352040159: failed to write boolean to registry" );
}
void write ( const std::string &section, const std::string &key, float value )
{
  FX::FXRegistry &reg ( FoxTools::Functions::application()->reg() );
  if ( !reg.writeRealEntry ( section.c_str(), key.c_str(), value ) )
    throw std::runtime_error ( "Error 1120008990: failed to write float to registry" );
}
void write ( const std::string &section, const std::string &key, double value )
{
  FX::FXRegistry &reg ( FoxTools::Functions::application()->reg() );
  if ( !reg.writeRealEntry ( section.c_str(), key.c_str(), value ) )
    throw std::runtime_error ( "Error 3737703437: failed to write double to registry" );
}


}; // namespace Registry
}; // namespace FoxTools
