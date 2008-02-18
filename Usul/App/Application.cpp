
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Usul/App/Application.h"
#include "Usul/CommandLine/Arguments.h"
#include "Usul/User/Directory.h"
#include "Usul/Predicates/FileExists.h"

#include <algorithm>

using namespace Usul::App;


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize static data members.
//
///////////////////////////////////////////////////////////////////////////////

Application * Application::_instance ( 0x0 );


///////////////////////////////////////////////////////////////////////////////
//
//  Create instance.
//
///////////////////////////////////////////////////////////////////////////////

Application& Application::instance()
{
  if ( 0x0 == _instance )
  {
    _instance = new Application;
  }

  return *_instance;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Application::Application() :
	_splashImage(),
  _vendor(),
  _program()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Application::~Application()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the directory where the icons live.
//
///////////////////////////////////////////////////////////////////////////////

const std::string Application::iconDirectory() const
{
	const std::string dir ( Usul::CommandLine::Arguments::instance().directory() );
  
#ifdef __APPLE__
  std::string path ( dir + "/../Resources/icons/" );
#else
	std::string path ( dir + "/icons/" );
#endif

  return path;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the splash image name.
//
///////////////////////////////////////////////////////////////////////////////

void Application::splashImage ( const std::string& image )
{
	_splashImage = image;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the splash image name.
//
///////////////////////////////////////////////////////////////////////////////

const std::string & Application::splashImage() const
{
	return _splashImage;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the path to the splash screen image.
//
///////////////////////////////////////////////////////////////////////////////

std::string Application::splashImagePath() const
{
	return this->iconDirectory() + this->splashImage();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the vender.
//
///////////////////////////////////////////////////////////////////////////////

void Application::vendor ( const std::string& vendor )
{
  _vendor = vendor;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the vender.
//
///////////////////////////////////////////////////////////////////////////////

const std::string& Application::vendor() const
{
  return _vendor;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the program name.
//
///////////////////////////////////////////////////////////////////////////////

void Application::program ( const std::string& program )
{
  _program = program;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the program name.
//
///////////////////////////////////////////////////////////////////////////////

const std::string& Application::program() const
{
  return _program;
}
