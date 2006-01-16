
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Registry class for persistent application data.
//
///////////////////////////////////////////////////////////////////////////////

#include "AppFrameWork/Core/Registry.h"
#include "AppFrameWork/Core/Constants.h"
#include "AppFrameWork/Windows/Frame.h"

#include "XmlTree/Document.h"

#include "Usul/File/Make.h"
#include "Usul/File/Stats.h"
#include "Usul/Math/Absolute.h"
#include "Usul/Predicates/FileExists.h"
#include "Usul/Strings/Case.h"
#include "Usul/User/Directory.h"

#include <iostream>
#include <stdexcept>
#include <fstream>

using namespace AFW::Core;

AFW_IMPLEMENT_OBJECT ( Registry );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Registry::Registry() : BaseClass(),
  _xml   ( 0x0 ),
  _file  (),
  _dirty ( false )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Registry::~Registry()
{
  // Safely...
  try
  {
    this->flush();
    Usul::Pointers::unreference ( _xml );
  }

  // Catch all exceptions.
  AFW_CATCH_BLOCK ( 3565839347ul );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize.
//
///////////////////////////////////////////////////////////////////////////////

void Registry::init ( const std::string &vendorName, const std::string &appName )
{
  // Handle bad input.
  if ( vendorName.empty() || appName.empty() )
    throw std::runtime_error ( "Error 2538613677: Empty vendor or application name given." );

  // Make sure appropriate configuration file exists. This will not write over 
  // an existing file, but if the file does not exist, we need to punt here if 
  // we cannot write one. That way the member "_xml" remains NULL (an exception 
  // is thrown), and the program will proceed because we check for NULL elsewhere.
  const std::string vendorDir ( Usul::User::Directory::vendor ( vendorName, true ) );
  _file = ( vendorDir + appName + '/' + appName + ".xml" );
  Usul::File::make ( _file );

  // Make document and load file.
  XmlTree::Document::RefPtr xml ( new XmlTree::Document ( "registry" ) );
  xml->load ( _file );

  // Feedback.
  std::cout << "Registry file: " << _file << std::endl;

  // Reference the xml tree.
  _xml = xml.get();
  Usul::Pointers::reference ( _xml );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the tree back to file.
//
///////////////////////////////////////////////////////////////////////////////

void Registry::flush()
{
  Guard guard ( this->mutex() );
  if ( _xml )
    _xml->write ( _file );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write frame geometry.
//
///////////////////////////////////////////////////////////////////////////////

void Registry::writeGeometry ( const AFW::Windows::Frame *frame )
{
  Guard guard ( this->mutex() );

  // Handle bad input.
  if ( 0x0 == frame )
    return;

  // Is it maximized?
  const bool isMaximized ( frame->maximized() );
  this->writeValue ( frame->persistentName(), AFW::Registry::Keys::MAXIMIZED, isMaximized );

  // If we are not maximized, then write the dimensions.
  if ( false == isMaximized )
    this->writeGeometry ( static_cast < const AFW::Windows::Window * > ( frame ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write window geometry.
//
///////////////////////////////////////////////////////////////////////////////

void Registry::writeGeometry ( const AFW::Windows::Window *window )
{
  Guard guard ( this->mutex() );

  // Handle bad input.
  if ( 0x0 == window )
    return;

  // Write the position and size.
  this->writeValue ( window->persistentName(), window->position() );
  this->writeValue ( window->persistentName(), window->size() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the value.
//
///////////////////////////////////////////////////////////////////////////////

void Registry::writeValue ( const std::string &section, const AFW::Core::Types::Size &s )
{
  Guard guard ( this->mutex() );

  // Write the values.
  this->writeValue ( section, AFW::Registry::Keys::WIDTH,  s[0] );
  this->writeValue ( section, AFW::Registry::Keys::HEIGHT, s[1] );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the value.
//
///////////////////////////////////////////////////////////////////////////////

void Registry::writeValue ( const std::string &section, const AFW::Core::Types::Position &p )
{
  Guard guard ( this->mutex() );

  // Write the values.
  this->writeValue ( section, AFW::Registry::Keys::X, p[0] );
  this->writeValue ( section, AFW::Registry::Keys::Y, p[1] );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the value.
//
///////////////////////////////////////////////////////////////////////////////

void Registry::writeValue ( const std::string &section, const std::string &key, unsigned int value )
{
  Guard guard ( this->mutex() );
  std::ostringstream out;
  out << value;
  this->writeValue ( section, key, out.str() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the value.
//
///////////////////////////////////////////////////////////////////////////////

void Registry::writeValue ( const std::string &section, const std::string &key, int value )
{
  Guard guard ( this->mutex() );
  std::ostringstream out;
  out << value;
  this->writeValue ( section, key, out.str() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the value.
//
///////////////////////////////////////////////////////////////////////////////

void Registry::writeValue ( const std::string &section, const std::string &key, bool value )
{
  Guard guard ( this->mutex() );
  std::ostringstream out;
  out << ( ( value ) ? "true" : "false" );
  this->writeValue ( section, key, out.str() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the value.
//
///////////////////////////////////////////////////////////////////////////////

void Registry::writeValue ( const std::string &section, const std::string &key, const std::string &value )
{
  Guard guard ( this->mutex() );
  if ( 0x0 == _xml )
    return;

  // Get or make needed child.
  XmlTree::Node::RefPtr node ( _xml->child ( 0, section + '/' + key, '/' ) );

  // If we found a node...
  if ( node.valid() )
  {
    // Set value of node.
    node->value ( value );

    // We're dirty.
    _dirty = true;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the value.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Registry::readValue ( const std::string &section, const std::string &key, unsigned int defaultValue )
{
  Guard guard ( this->mutex() );
  std::ostringstream out;
  out << defaultValue;
  const std::string answer ( this->readValue ( section, key, out.str() ) );
  return ( ( answer == out.str() ) ? defaultValue : static_cast < unsigned int > ( Usul::Math::absolute ( ::atoi ( answer.c_str() ) ) ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the value.
//
///////////////////////////////////////////////////////////////////////////////

int Registry::readValue ( const std::string &section, const std::string &key, int defaultValue )
{
  Guard guard ( this->mutex() );
  std::ostringstream out;
  out << defaultValue;
  const std::string answer ( this->readValue ( section, key, out.str() ) );
  return ( ( answer == out.str() ) ? defaultValue : ::atoi ( answer.c_str() ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the value.
//
///////////////////////////////////////////////////////////////////////////////

bool Registry::readValue ( const std::string &section, const std::string &key, bool defaultValue )
{
  Guard guard ( this->mutex() );
  const std::string answer ( Usul::Strings::lowerCase ( this->readValue ( section, key, std::string() ) ) );
  if ( answer == "true" || answer == "1" )
    return true;
  else if ( answer == "false" || answer == "0" )
    return false;
  else
    return defaultValue;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the value.
//
///////////////////////////////////////////////////////////////////////////////

std::string Registry::readValue ( const std::string &section, const std::string &key, const std::string &defaultValue )
{
  Guard guard ( this->mutex() );
  if ( 0x0 == _xml )
    return defaultValue;

  XmlTree::Node::RefPtr node ( _xml->child ( 0, section + '/' + key, '/', false ) );
  return ( ( node.valid() ) ? node->value() : defaultValue );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the value.
//
///////////////////////////////////////////////////////////////////////////////

AFW::Core::Types::Position Registry::readValue ( const std::string &section, const AFW::Core::Types::Position &defaultValue )
{
  Guard guard ( this->mutex() );
  return Position ( this->readValue ( section, AFW::Registry::Keys::WIDTH,  defaultValue[0] ),
                    this->readValue ( section, AFW::Registry::Keys::HEIGHT, defaultValue[1] ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the value.
//
///////////////////////////////////////////////////////////////////////////////

AFW::Core::Types::Size Registry::readValue ( const std::string &section, const AFW::Core::Types::Size &defaultValue )
{
  Guard guard ( this->mutex() );
  return Size ( this->readValue ( section, AFW::Registry::Keys::WIDTH,  defaultValue[0] ),
                this->readValue ( section, AFW::Registry::Keys::HEIGHT, defaultValue[1] ) );
}
