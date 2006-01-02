
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The plugin registry class.
//
///////////////////////////////////////////////////////////////////////////////

#include "AppFrameWork/Core/PluginRegistry.h"
#include "AppFrameWork/Core/Define.h"

#include "XmlDom/Policy.h"
#include "XmlDom/Reader.h"
#include "XmlDom/BuildTree.h"
#include "XmlDom/Node.h"
#include "XmlDom/Visitor.h"
#include "XmlDom/File.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/CommandLine/Arguments.h"
#include "Usul/File/Path.h"
#include "Usul/Predicates/FileExists.h"

#include <fstream>
#include <iostream>
#include <stdexcept>

using namespace AFW::Core;


///////////////////////////////////////////////////////////////////////////////
//
//  Default constructor.
//
///////////////////////////////////////////////////////////////////////////////

PluginRegistry::PluginRegistry() :
  _file(),
  _plugins()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

PluginRegistry::~PluginRegistry()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Look for plugin registry file and read it.
//
///////////////////////////////////////////////////////////////////////////////

bool PluginRegistry::read ( PluginRegistry::PluginFiles &plugins )
{
  // Safely...
  try
  {
    // Determine the name of registry file.
    const std::string program ( Usul::File::base ( Usul::CommandLine::Arguments::instance().program() ) );
    const std::string file ( Usul::CommandLine::Arguments::instance().directory() + "/" + program + ".plugs" );

    // Call the other one.
    return this->read ( file, plugins );
  }

  // Catch all exceptions.
  AFW_CATCH_BLOCK ( 2480101775ul );

  // If we get to here then it did not work.
  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the file and return the list of plugins.
//
///////////////////////////////////////////////////////////////////////////////

bool PluginRegistry::read ( const std::string &name, PluginRegistry::PluginFiles &plugins )
{
  // Safely...
  try
  {
    // Call the other one.
    return this->_read ( name, plugins );
  }

  // Catch all exceptions.
  AFW_CATCH_BLOCK ( 6148032540ul );

  // If we get to here then it did not work.
  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the file and return the list of plugins.
//
///////////////////////////////////////////////////////////////////////////////

bool PluginRegistry::_read ( const std::string &name, PluginRegistry::PluginFiles &plugins )
{
  // Typedefs.
  typedef XML::Config::Policy<> Policy;
  typedef void (PluginRegistry::*Function) ( const std::string &s );
  typedef Usul::Adaptors::MemberFunction < PluginRegistry*, Function > MemFun;
  typedef XML::Node < Policy > Node;
  typedef XML::Callback::BuildTree < Node > BuildTree;
  typedef XML::Reader < Policy, BuildTree > Reader;
  typedef XML::Visitor < Policy, Node, MemFun > Visitor;

  // Initialize containers.
  _file.clear();
  _plugins.clear();

  // Return false now if the file does not exist.
  Usul::Predicates::FileExists exists;
  if ( false == exists ( name ) )
    return false;

  // Read xml file and build DOM tree.
  XML::File < Policy > file ( name );
  Reader reader;
  reader.read ( file.begin(), file.end() );

  // Make visitor.
  Visitor visitor;
  visitor.add ( Visitor::NODE_START, "plugins/plugin/file", MemFun ( this, &PluginRegistry::_moduleFile ) );
  visitor.add ( Visitor::NODE_END,   "plugins/plugin",      MemFun ( this, &PluginRegistry::_addModule  ) );

  // Visit the DOM tree.
  reader.callback().root()->accept ( visitor );

  // Set the given list.
  plugins = _plugins;

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the file.
//
///////////////////////////////////////////////////////////////////////////////

void PluginRegistry::_moduleFile ( const std::string &s )
{
  // The extension for the debug-built plugin 
  // will have a 'd' appended to it.
#ifdef _DEBUG
  _file = s + 'd';
#else
  _file = s;
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add the module.
//
///////////////////////////////////////////////////////////////////////////////

void PluginRegistry::_addModule ( const std::string & )
{
  // Handle empty file.
  if ( _file.empty() )
    return;

  // If the file does not have path information then use the program's directory.
  if ( ( Usul::File::base ( _file ) + '.' + Usul::File::extension ( _file ) ) == _file )
    _plugins.push_back ( Usul::CommandLine::Arguments::instance().directory() + '/' + _file );

  // Otherwise, add the file as-is.
  else
    _plugins.push_back ( _file );
}
