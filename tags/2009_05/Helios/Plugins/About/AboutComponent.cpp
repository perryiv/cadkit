
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  
//
///////////////////////////////////////////////////////////////////////////////

#include "AboutComponent.h"

#include "Usul/Headers/OpenGL.h"
#include "Usul/Cast/Cast.h"

#include <sstream>
#include <algorithm>

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( AboutComponent , AboutComponent::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
// Constructor
//
///////////////////////////////////////////////////////////////////////////////

AboutComponent::AboutComponent() 
{
}


///////////////////////////////////////////////////////////////////////////////
//
// Destructor
//
///////////////////////////////////////////////////////////////////////////////

AboutComponent::~AboutComponent() 
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *AboutComponent::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::IPlugin::IID:
    return static_cast < Usul::Interfaces::IPlugin* > ( this );
  case Usul::Interfaces::IAboutString::IID:
    return static_cast < Usul::Interfaces::IAboutString* > ( this );
  default:
    return 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Usul/Interfaces/IPlugin.h implementation
//
///////////////////////////////////////////////////////////////////////////////

std::string AboutComponent::getPluginName() const 
{
  return "About String";
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the about string.
//
///////////////////////////////////////////////////////////////////////////////

std::string AboutComponent::aboutString() const
{
  // Get the date.
  std::istringstream date ( __DATE__ );
  std::string day, month, year;
  date >> month >> day >> year;

  // Format the string.
  std::ostringstream message;
  message << "3D viewer application based on:";
  message << "\n  -- CAD Toolkit (cadkit.sourceforge.net)";
  message << "\n  -- Qt (www.trolltech.com)";
  message << "\n  -- Open Scene Graph (www.openscenegraph.org)";
  message << "\n  -- OpenGL (www.opengl.org)";
  message << '\n';
  message << "\nPrimary authors include:";
  message << "\n  -- Perry L Miller IV";
  message << "\n  -- Adam Kubach";
  message << "\n  -- Mike Jackson";
#if 0
  // If there is an active view...
  if ( Usul::App::Controller::instance().activeView() )
  {
    //this->activeView()->makeCurrent();
    message << '\n';
    message << "\nOpenGL Information:";
    const char *vendor     ( USUL_UNSAFE_CAST ( const char *, ::glGetString ( GL_VENDOR     ) ) );
    const char *renderer   ( USUL_UNSAFE_CAST ( const char *, ::glGetString ( GL_RENDERER   ) ) );
    const char *version    ( USUL_UNSAFE_CAST ( const char *, ::glGetString ( GL_VERSION    ) ) );
    const char *extensions ( USUL_UNSAFE_CAST ( const char *, ::glGetString ( GL_EXTENSIONS ) ) );
    if ( vendor )
      message << "\n  -- Vendor: " << vendor;
    if ( renderer )
      message << "\n  -- Renderer: " << renderer;
    if ( version )
      message << "\n  -- Version: " << version;

    #ifdef _DEBUG

    if ( extensions )
    {
      std::string e ( extensions );
      std::replace ( e.begin(), e.end(), ' ', '\n' );
      message << "\n  -- Extensions: " << e;
    }

    #endif
  }
#endif
  message << "\n\nBuilt on " << day << ' ' << month << ' ' << year << " at " << __TIME__;

  return message.str();
}

