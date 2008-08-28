
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Misc functions.
//
///////////////////////////////////////////////////////////////////////////////

#include "Functions.h"

#include "Usul/Registry/Node.h"
#include "Usul/Strings/Case.h"
#include "Usul/System/Environment.h"

#include "XmlTree/Node.h"

#include <algorithm>


///////////////////////////////////////////////////////////////////////////////
//
//  Find the directory.
//
///////////////////////////////////////////////////////////////////////////////

std::string Functions::directory ( Usul::Registry::Node &node )
{
  return Functions::directory ( node.get<std::string> ( "." ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Find the directory.
//
///////////////////////////////////////////////////////////////////////////////

std::string Functions::directory ( const std::string &s )
{
  std::string dir ( s );

  // Fix the slashes.
  std::replace ( dir.begin(), dir.end(), '\\', '/' );

  // Make sure we have something.
  dir = ( ( true == dir.empty() ) ? std::string ( "./" ) : dir );

  // Make sure there is a trailing slash.
  if ( false == dir.empty() )
  {
    const unsigned int last ( dir.size() - 1 );
    if ( '/' != dir.at(last) )
    {
      dir += '/';
    }
  }

  // Return the string.
  return dir;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return URL of the domain. Ex: http://www.mysite.com/
//
///////////////////////////////////////////////////////////////////////////////

std::string Functions::urlDomain ( bool wantSlash )
{
  const std::string protocol ( Functions::protocol() );
  const std::string serverName ( Usul::System::Environment::get ( "SERVER_NAME" ) );
  return Usul::Strings::format ( protocol + "://" + serverName + ( ( wantSlash ) ? "/" : "" ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return URL of the script. Ex: http://www.mysite.com/cgi-bin/my_dir/my_script
//
///////////////////////////////////////////////////////////////////////////////

std::string Functions::urlScript()
{
  const std::string domain ( Functions::urlDomain ( false ) );
  std::string script ( Usul::System::Environment::get ( "SCRIPT_NAME" ) );
  return Usul::Strings::format ( domain, script );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return URL of the script. Ex: http://www.mysite.com/cgi-bin/my_dir/
//
///////////////////////////////////////////////////////////////////////////////

std::string Functions::urlScriptDir ( bool wantSlash )
{
  std::string script ( Functions::urlScript() );
  std::vector<char> temp ( script.rbegin(), script.rend() );
  temp.erase ( temp.begin(), std::find ( temp.begin(), temp.end(), '/' ) );
  return std::string ( temp.rbegin(), temp.rend() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the protocol, either "http" or "https".
//
///////////////////////////////////////////////////////////////////////////////

std::string Functions::protocol()
{
  return ( ( "on" == Usul::Strings::lowerCase ( Usul::System::Environment::get ( "HTTPS" ) ) ) ? "https" : "http" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add the movie parameter.
//
///////////////////////////////////////////////////////////////////////////////

void Functions::addMovieParam ( XmlTree::Node &node, const std::string &name, const std::string &value )
{
  XmlTree::Node::ValidRefPtr param ( node.append ( "param" ) );
  param->attributes()["name"]  = name;
  param->attributes()["value"] = value;
}
