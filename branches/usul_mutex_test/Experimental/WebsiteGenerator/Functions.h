
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

#ifndef _WEBSITE_GENERATOR_FUNCTIONS_
#define _WEBSITE_GENERATOR_FUNCTIONS_

#include <string>

namespace Usul { namespace Registry { class Node; } }
namespace XmlTree { class Node; }


struct Functions
{
  // Add the movie parameter.
  static void            addMovieParam ( XmlTree::Node &node, const std::string &name, const std::string &value );

  // Always returns a valid directory string, if only "./"
  static std::string     directory ( Usul::Registry::Node & );
  static std::string     directory ( const std::string & );

  // Return "http" or "https".
  static std::string     protocol();

  // Return the url.
  static std::string     urlDomain ( bool wantSlash );
  static std::string     urlScriptDir ( bool wantSlash );
  static std::string     urlScript();
};


#endif // _WEBSITE_GENERATOR_FUNCTIONS_
