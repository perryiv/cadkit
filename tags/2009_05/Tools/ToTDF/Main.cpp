
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
//  Program to convert files to Triangle Document Format (TDF).
//
///////////////////////////////////////////////////////////////////////////////

#include "Usul/Base/Referenced.h"
#include "Usul/Components/Manager.h"
#include "Usul/Documents/Manager.h"
#include "Usul/File/Path.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Interfaces/IDocument.h"
#include "Usul/Interfaces/IPlugin.h"
#include "Usul/Interfaces/GUI/IProgressBar.h"
#include "Usul/Threads/Mutex.h"

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <list>
#include <stdexcept>
#include <sstream>
#include <string>
#include <vector>


///////////////////////////////////////////////////////////////////////////////
//
//  Program to convert the files.
//
///////////////////////////////////////////////////////////////////////////////

class Program : Usul::Base::Referenced,
                Usul::Interfaces::IProgressBar
{
public:

  typedef Usul::Base::Referenced BaseClass;
  typedef std::vector<std::string> Files;
  typedef std::list<std::string> Args;

  USUL_DECLARE_QUERY_POINTERS ( Program );
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  Program ( int argc, char **argv );

  void          run();

protected:

  // Usul::Interfaces::IProgressBar
  virtual void  showProgressBar(){}
  virtual void  setTotalProgressBar ( unsigned int value ){}
  virtual void  updateProgressBar ( unsigned int value );
  virtual void  hideProgressBar(){}

  virtual ~Program();

private:

  void          _loadPlugin ( const std::string &file );

  void          _openDocument ( const std::string &file, Usul::Documents::Document *document );

  void          _processArguments();
  void          _processFiles();
  void          _processFile ( const std::string &file );

  void          _saveDocument ( const std::string &file, Usul::Documents::Document *document );

  Args _args;
  Files _files;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Interface plumbing.
//
///////////////////////////////////////////////////////////////////////////////

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( Program, Program::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Program::Program ( int argc, char **argv ) :
  _args ( argv, argv + argc ),
  _files()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Program::~Program()
{
  // Release all plugins.
  Usul::Components::Manager::instance().clear ( &std::cout );

  // Set the mutex factory to null so that we can find late uses of it.
  Usul::Threads::Mutex::createFunction ( 0x0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query the interfaces
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *Program::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::IProgressBar::IID:
    return static_cast < Usul::Interfaces::IProgressBar * > ( this );
  default:
    return 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Run the program.
//
///////////////////////////////////////////////////////////////////////////////

void Program::run()
{
  // Process arguments.
  this->_processArguments();

  // Loop through files.
  this->_processFiles();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Process the command-line arguments.
//
///////////////////////////////////////////////////////////////////////////////

void Program::_processArguments()
{
  // Make a copy.
  Args args ( _args );

  // Check number of arguments.
  if ( args.size() < 3 )
  {
    std::ostringstream out;
    out << "Usage:\n\t" << args.front() << " <plugin file> <infile> [infile] ... [infile]";
    throw std::runtime_error ( out.str() );
  }

  // Pop the program name.
  args.pop_front();

  // Load the plugin file.
  this->_loadPlugin ( args.front() );
  args.pop_front();

  // Copy files.
  _files.reserve ( args.size() );
  std::copy ( args.begin(), args.end(), std::back_inserter ( _files ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Load the plugin file.
//
///////////////////////////////////////////////////////////////////////////////

void Program::_loadPlugin ( const std::string &file )
{
  Usul::Components::Manager::instance().load ( Usul::Interfaces::IPlugin::IID, file );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Process the files to be converted.
//
///////////////////////////////////////////////////////////////////////////////

void Program::_processFiles()
{
  for ( Files::const_iterator i = _files.begin(); i != _files.end(); ++i )
  {
    const std::string file ( Usul::File::fullPath ( *i ) );
    try
    {
      this->_processFile ( file );
    }
    catch ( const std::exception &e )
    {
      std::cout << ( ( 0x0 != e.what() ) ? e.what() : ( "Standard exception caught while processing file: " + file ) ) << std::endl;
    }
    catch ( ... )
    {
      std::cout << "Unknown exception caught while processing file: " << file << std::endl;
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Process the file.
//
///////////////////////////////////////////////////////////////////////////////

void Program::_processFile ( const std::string &file )
{
  typedef Usul::Documents::Document Document;
  typedef Usul::Documents::Manager DocManager;
  typedef DocManager::DocumentInfo Info;

  // This will create a new document.
  Info info ( DocManager::instance().find ( file, 0x0 ) );
  Document::RefPtr document ( info.document );

  // Check to see if we created a document.
  if ( false == document.valid() )
  {
    throw std::runtime_error ( "Error: Could not find document for file: " + file );
  }

  // Ask the document to open the file.
  this->_openDocument ( file, document.get() );

  // Ask the document write a TDF file.
  const std::string output ( Usul::File::directory ( file, true ) + Usul::File::base ( file ) + ".tdf" );
  this->_saveDocument ( output, document.get() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Open the document.
//
///////////////////////////////////////////////////////////////////////////////

void Program::_openDocument ( const std::string &file, Usul::Documents::Document *document )
{
  if ( 0x0 == document )
    return;

  std::cout << "Opening file: " << file << " ... " << std::endl;
  document->open ( file, this );
  std::cout << "Done" << std::endl;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Save the document.
//
///////////////////////////////////////////////////////////////////////////////

void Program::_saveDocument ( const std::string &file, Usul::Documents::Document *document )
{
  if ( 0x0 == document )
    return;

  std::cout << "Saving file: " << file << " ... " << std::endl;
  document->saveAs ( file, this, 0x0 );
  std::cout << "Done" << std::endl;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Feedback.
//
///////////////////////////////////////////////////////////////////////////////

void Program::updateProgressBar ( unsigned int value )
{
  std::cout << std::setw ( 3 ) << value << '%' << '\r' << std::flush;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Main function.
//
///////////////////////////////////////////////////////////////////////////////

int main ( int argc, char **argv )
{
  try
  {
    // Single threaded execution.
    Usul::Threads::Mutex::createFunction ( &Usul::Threads::newSingleThreadedMutexStub );

    // Declare and execute the program.
    Program::RefPtr program ( new Program ( argc, argv ) );
    program->run();

    // It worked.
    return 0;
  }
  catch ( const std::exception &e )
  {
    std::cout << ( ( 0x0 != e.what() ) ? e.what() : "Standard exception caught" ) << std::endl;
    return 1;
  }
  catch ( ... )
  {
    std::cout << "Unknown exception caught" << std::endl;
    return 1;
  }
}
