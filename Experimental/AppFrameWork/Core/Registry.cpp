
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
#include "AppFrameWork/Core/Frame.h"

#include "XmlTree/Document.h"

#include "Usul/File/Make.h"
#include "Usul/File/Stats.h"
#include "Usul/Predicates/FileExists.h"
#include "usul/Strings/Split.h"
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

  // Get vendor directory.
  const std::string vendorDir ( Usul::User::Directory::vendor ( vendorName, true ) );

  // Make sure appropriate configuration file exists.
  _file = ( vendorDir + appName + '/' + appName + ".xml" );
  Usul::File::make ( _file );

  // If the file is empty then write a default one.
  if ( 0 == Usul::File::size ( _file ) )
    this->_initFile ( _file );

  // Parse the file.
  XmlTree::Document::RefPtr xml ( new XmlTree::Document ( _file ) );

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
//  Initialize the XML file.
//
///////////////////////////////////////////////////////////////////////////////

void Registry::_initFile ( const std::string &file )
{
  USUL_ASSERT ( true == Usul::Predicates::FileExists::test ( file ) );
  std::ofstream out ( file.c_str() );
  out << XmlTree::Constants::HEADER << "\n<registry>\n</registry>\n";
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write frame geometry.
//
///////////////////////////////////////////////////////////////////////////////

void Registry::writeGeometry ( const AFW::Core::Frame *frame )
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
    this->writeGeometry ( static_cast < const AFW::Core::Window * > ( frame ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write window geometry.
//
///////////////////////////////////////////////////////////////////////////////

void Registry::writeGeometry ( const AFW::Core::Window *window )
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

  // Split the path.
  typedef std::vector < std::string > Parts;
  Parts parts;
  Usul::Strings::split ( section + '/' + key, '/', false, parts );

  // Loop through the parts and get the node. Create it if needed.
  XmlTree::Node::RefPtr node ( _xml );
  for ( Parts::const_iterator i = parts.begin(); i != parts.end(); ++i )
    node = ( ( node ) ? node->child ( 0, *i ) : 0x0 );

  // If we found a node...
  if ( node.valid() )
  {
    // Set value of node.
    node->value ( value );

    // We're dirty.
    _dirty = true;
  }
}
