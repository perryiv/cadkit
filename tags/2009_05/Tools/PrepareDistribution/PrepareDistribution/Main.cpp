
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Program to prepare a distribution.
//
///////////////////////////////////////////////////////////////////////////////

#ifdef _MSC_VER
#error "This program assumes that library names end in .so, and that the "
#error "commands 'ldd', 'mkdir', 'cp', and 'ln' exist in the path."
#endif

#include "Usul/CommandLine/Arguments.h"
#include "Usul/Exceptions/Thrower.h"
#include "Usul/File/Contents.h"
#include "Usul/File/Path.h"
#include "Usul/File/Temp.h"
#include "Usul/Predicates/FileExists.h"
#include "Usul/Scope/RemoveFile.h"
#include "Usul/Strings/Format.h"
#include "Usul/Strings/Split.h"
#include "Usul/Scope/CurrentDirectory.h"
#include "Usul/Threads/Mutex.h"

#include <cstdlib>
#include <iostream>
#include <list>
#include <set>
#include <stdexcept>
#include <string>


///////////////////////////////////////////////////////////////////////////////
//
//  Usage exception.
//
///////////////////////////////////////////////////////////////////////////////

struct UsageException : public std::invalid_argument
{
  typedef std::invalid_argument BaseClass;

  UsageException ( const std::string &s ) : BaseClass ( s )
  {
  }
};


///////////////////////////////////////////////////////////////////////////////
//
//  Class to run the program.
//
///////////////////////////////////////////////////////////////////////////////

class Program
{
public:

  typedef Usul::CommandLine::Arguments CommandLine;
  typedef CommandLine::Args Arguments;
  typedef std::list<std::string> Lines;
  typedef std::list<std::string> Strings;
  typedef std::set<std::string> Files;
  typedef std::pair<Files::iterator,bool> InsertResult;
  typedef std::set<std::string> Links;
  typedef std::set<std::string> Skip;

  Program ( int argc, char **argv );
  ~Program();

  void        run();

protected:

  void        _addFile ( const std::string &file );
  void        _addLink ( const std::string &file );

  void        _copyFile ( const std::string &file ) const;
  void        _copyFiles() const;

  void        _execute ( const std::string &command ) const;
  void        _execute ( const std::string &command, Lines &lines ) const;

  void        _makeLink ( const std::string &file ) const;
  void        _makeLinks() const;

  void        _parseCommandLine();
  void        _processLinkedFiles ( const Lines &lines );
  void        _processLine ( const std::string &line );

  std::string _rootFileName ( const std::string &file ) const;

  void        _throwUsageException() const;

private:

  std::string _target;
  Files _files;
  Links _links;
  Skip _skip;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Program::Program ( int argc, char **argv ) :
  _target(),
  _files(),
  _links(),
  _skip()
{
  CommandLine::instance().set ( argc, argv );
  Usul::Threads::Mutex::createFunction ( &Usul::Threads::newSingleThreadedMutexStub );

  _skip.insert ( "/lib" );
  _skip.insert ( "/usr/lib" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Program::~Program()
{
  Usul::Threads::Mutex::createFunction ( 0x0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Run the program.
//
///////////////////////////////////////////////////////////////////////////////

void Program::run()
{
  // Parse command-line arguments.
  this->_parseCommandLine();

  // Copy files.
  this->_copyFiles();

  // Make links.
  this->_makeLinks();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Copy the files.
//
///////////////////////////////////////////////////////////////////////////////

void Program::_copyFiles() const
{
  // Loop through the files.
  for ( Files::const_iterator i = _files.begin(); i != _files.end(); ++i )
  {
    this->_copyFile ( *i );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Copy the files.
//
///////////////////////////////////////////////////////////////////////////////

void Program::_copyFile ( const std::string &file ) const
{
  // If the file does not exist then ignore it.
  if ( false == Usul::Predicates::FileExists::test ( file ) )
    return;

  // Get the target path.
  const std::string base ( Usul::File::base ( file ) );
  std::string extension ( Usul::File::extension ( file ) );
  extension = ( ( true == extension.empty() ) ? "" : Usul::Strings::format ( '.', extension ) );
  const std::string target ( Usul::Strings::format ( _target, '/', base, extension ) );

  // Make the command.
  const std::string command ( Usul::Strings::format ( "cp ", file, ' ', target ) );

  // Feedback.
  std::cout << command << std::endl;

  // Execute the command.
  this->_execute ( command );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make the links.
//
///////////////////////////////////////////////////////////////////////////////

void Program::_makeLinks() const
{
  // Make the target directory current.
  Usul::System::CurrentDirectory scope ( _target );

  // Loop through the links.
  for ( Links::const_iterator i = _links.begin(); i != _links.end(); ++i )
  {
    this->_makeLink ( *i );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make the link.
//
///////////////////////////////////////////////////////////////////////////////

void Program::_makeLink ( const std::string &name ) const
{
  // Get the link file name.
  const std::string link ( Usul::Strings::format ( this->_rootFileName ( name ), ".so" ) );

  // Make the command.
  const std::string command ( Usul::Strings::format ( "ln -s -f ", name, " ./", link ) );

  // Feedback.
  std::cout << command << std::endl;

  // Execute the command.
  this->_execute ( command );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Parse the command-line.
//
///////////////////////////////////////////////////////////////////////////////

void Program::_parseCommandLine()
{
  // Get command-line arguments.
  Arguments args ( CommandLine::instance().args() );

  // Are there enough?
  if ( 3 != args.size() )
    this->_throwUsageException();

  // Get the arguments.
  const std::string source ( args.at(1) );
  _target = args.at(2);

  // Make the target directory.
  this->_execute ( Usul::Strings::format ( "mkdir --parents ", _target ) );

  // Copy the source.
  this->_addFile ( source );

  // Get output from command.
  Lines lines;
  this->_execute ( Usul::Strings::format ( "ldd ", source ), lines );

  // Process the lines.
  this->_processLinkedFiles ( lines );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Parse the command-line.
//
///////////////////////////////////////////////////////////////////////////////

void Program::_processLinkedFiles ( const Lines &lines )
{
  typedef std::vector<std::string> Strings;

  // Loop through the lines.
  for ( Lines::const_iterator i = lines.begin(); i != lines.end(); ++i )
  {
    // Loop through the strings.
    this->_processLine ( *i );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Parse the command-line.
//
///////////////////////////////////////////////////////////////////////////////

void Program::_processLine ( const std::string &line )
{
  // Split the line into strings.
  Strings strings;
  Usul::Strings::split ( line, ' ', false, strings );

  // Loop through the strings.
  for ( Strings::const_iterator i = strings.begin(); i != strings.end(); ++i )
  {
    // Add the file.
    this->_addFile ( *i );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add the file.
//
///////////////////////////////////////////////////////////////////////////////

void Program::_addFile ( const std::string &file )
{
  // See if it's a file.
  if ( false == Usul::Predicates::FileExists::test ( file ) )
    return;

  // See if it's in a directory we skip.
  const std::string dir ( Usul::File::directory ( file, false ) );
  if ( _skip.end() != _skip.find ( dir ) )
    return;

  // Save file in the set.
  const InsertResult result ( _files.insert ( file ) );
  if ( true == result.second )
  {
    // Feedback.
    std::cout << "Will add file: " << file << std::endl;

    // Add a link if necessary.
    this->_addLink ( file );
  }

  // Get the lines of files linked into this file.
  Lines lines;
  this->_execute ( Usul::Strings::format ( "ldd ", file ), lines );

  // Process the lines.
  this->_processLinkedFiles ( lines );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a link.
//
///////////////////////////////////////////////////////////////////////////////

void Program::_addLink ( const std::string &path )
{
  // See if it's a file.
  if ( false == Usul::Predicates::FileExists::test ( path ) )
    return;

  // Get the extension.
  const std::string ext ( Usul::File::extension ( path ) );
  if ( ext == "so" )
    return;

  // Get the name.
  const std::string name ( Usul::Strings::format ( Usul::File::base ( path ), '.', ext ) );

  // Put the name in the map.
  _links.insert ( name );

  // Feedback.
  const std::string root ( this->_rootFileName ( name ) );
  std::cout << Usul::Strings::format ( "Will add link: ", root, ".so --> ", name ) << std::endl;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Execute the string.
//
///////////////////////////////////////////////////////////////////////////////

void Program::_execute ( const std::string &cmd, Lines &lines ) const
{
  // Temporarily redirect stdout and stderr.
  const std::string output ( Usul::File::Temp::file() );
  Usul::Scope::RemoveFile remove ( output, false );

  // Make the full command line. Append the file to redirect to.
  const std::string command ( Usul::Strings::format ( cmd, " > ", output ) );

  // Execute the command.
  this->_execute ( command );

  // Get the output file's contents.
  std::string contents;
  contents.reserve ( Usul::File::size ( output ) );
  Usul::File::contents ( output, false, contents );

  // Split the lines.
  Usul::Strings::split ( contents, ' ', false, lines );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Execute the string.
//
///////////////////////////////////////////////////////////////////////////////

void Program::_execute ( const std::string &command ) const
{
  // Execute the command.
  if ( 0 != ::system ( command.c_str() ) )
  {
    Usul::Exceptions::Thrower<std::runtime_error>
      ( "Error 1420716083: Command '", command, "' failed to execute" );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Throw usage exception.
//
///////////////////////////////////////////////////////////////////////////////

void Program::_throwUsageException() const
{
  Usul::Exceptions::Thrower<UsageException>
    ( "Usage: ", CommandLine::instance().program(), " <source> <destination directory>" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the root file name. Ex: libstdc++.so.6 returns libstdc++
//
///////////////////////////////////////////////////////////////////////////////

std::string Program::_rootFileName ( const std::string &file ) const
{
  std::string base ( Usul::File::base ( file ) );
  std::string::size_type position ( base.find ( '.' ) );
  while ( std::string::npos != position )
  {
    base = base.erase ( position, std::string::npos );
    position = base.find ( '.' );
  }
  return base;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Main function.
//
///////////////////////////////////////////////////////////////////////////////

int main ( int argc, char **argv )
{
  int result ( 1 );

  try
  {
    Program program ( argc, argv );
    program.run();
    result = 0;
  }

  catch ( const UsageException &e )
  {
    std::cout << e.what() << std::endl;
  }

  catch ( const std::exception &e )
  {
    const std::string what ( e.what() );
    std::cout << ( ( e.what() ) ? e.what() : "Standard exception caught" ) << std::endl;
  }

  catch ( ... )
  {
    std::cout << "Unknown exception caught" << std::endl;
  }

  return result;
}
