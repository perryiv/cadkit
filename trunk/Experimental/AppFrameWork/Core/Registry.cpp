
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
#include "AppFrameWork/Core/Program.h"
#include "AppFrameWork/Core/Application.h"

#include "XmlTree/Root.h"

#include "Usul/File/MakeDir.h"
#include "Usul/System/Home.h"

#include <iostream>
#include <stdexcept>

using namespace AFW::Core;

AFW_IMPLEMENT_OBJECT ( Registry );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Registry::Registry() : BaseClass(),
  _xml ( 0x0 )
{
  try { this->_init(); }
  AFW_CATCH_BLOCK ( 1177195354ul, 4240596741ul );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Registry::~Registry()
{
  Usul::Pointers::unreference ( _xml );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize.
//
///////////////////////////////////////////////////////////////////////////////

void Registry::_init()
{
  // Get home directory.
  const std::string home ( Usul::System::Home::dir ( false ) );
  if ( home.empty() )
    throw std::runtime_error ( "Error 2535209286: Failed to find home directory." );

  // Make appropriate vendor directory.
  #ifdef _MSC_VER
  const std::string vendor ( Program::instance().vendor() );
  #else
  const std::string vendor ( '.' + Program::instance().app()->vendor() );
  #endif

  // Append vendor and program name to path.
  std::string path ( home + '/' + vendor + '/' + Program::instance().app()->name() );

  // Make the directory if we need to.
  if ( false == Usul::File::MakeDir::make ( path ) )
    throw std::runtime_error ( "Error 3484322278: Failed to make application directory: " + path );

  // Reference the xml tree.
  Usul::Pointers::reference ( _xml );
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
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the value.
//
///////////////////////////////////////////////////////////////////////////////

void Registry::writeValue ( const std::string &section, const std::string &key, int value )
{
  Guard guard ( this->mutex() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the value.
//
///////////////////////////////////////////////////////////////////////////////

void Registry::writeValue ( const std::string &section, const std::string &key, bool value )
{
  Guard guard ( this->mutex() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the value.
//
///////////////////////////////////////////////////////////////////////////////

void Registry::writeValue ( const std::string &section, const std::string &key, const std::string &value )
{
  Guard guard ( this->mutex() );
}
