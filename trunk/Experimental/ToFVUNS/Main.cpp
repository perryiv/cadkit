
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
#include "Usul/Scope/CurrentDirectory.h"
#include "Usul/Math/Vector3.h"

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <list>
#include <stdexcept>
#include <sstream>
#include <string>
#include <vector>
#include <fstream>


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

  typedef std::vector< Usul::Math::Vec3ui > Vec3uiArray;
  typedef Usul::Types::Float64 GridType;
  typedef std::vector< GridType > Grid1D;
  typedef std::vector< Grid1D > Grid2D;
  typedef std::vector< Grid2D > GridSpace;

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
  void          _processFile ( const std::string &file, Grid2D &grid );

  void          _saveDocument ( const std::string &file, Usul::Documents::Document *document );

  Args _args;
  Files _files;

  Usul::Math::Vec3ui          _dimensions;
  GridSpace _grid;
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
  _files(),
  _dimensions( 0, 0, 0 ),
  _grid( )
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
  // Usul::Threads::Mutex::createFunction ( 0x0 );
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
  if ( args.size() < 4 )
  {
    std::ostringstream out;
    out << "Usage:\n\t" << args.front() << " <u_filename> <v_filename> <w_filename> <p_filename>";
    throw std::runtime_error ( out.str() );
  }

  // Pop the program name.
  args.pop_front();

  // reserve space for the grids
  _grid.reserve( args.size() );

  // Load the plugin file.
  //this->_loadPlugin ( args.front() );
  //args.pop_front();

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
    // Get the extension, base and directory of the first file to process
    /*std::string extension ( Usul::File::extension( *i ) );
    std::string base( Usul::File::base( *i ) );
    std::string directory ( Usul::File::directory( *i, true ) );*/

    // Temp Grid 
    Grid2D grid;

    // The file
    std::string file ( Usul::File::fullPath( *i ) );

    try
    {
      this->_processFile ( file, grid );
      _grid.push_back( grid );
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

void Program::_processFile ( const std::string &file, Grid2D &grid )
{
 // create a file handle
  std::ifstream* ifs ( new std::ifstream );

  // open the file
  ifs->open( file.c_str(), std::ifstream::out | std::ifstream::binary );

  // make sure the file was opened
  if( false == ifs->is_open() )
    throw std::runtime_error ( "Error 1188374386: Failed to open file: " + file );

  // temp values to store the header
  Usul::Types::Uint16 h1 ( 0 );
  Usul::Types::Uint16 h2 ( 0 );
  Usul::Types::Uint16 h3 ( 0 );
  Usul::Types::Uint16 h4 ( 0 );

  // Read and set the dimensions
  Usul::Types::Uint32 ni ( 0 );
  Usul::Types::Uint32 nj ( 0 );
  Usul::Types::Uint32 nk ( 0 );
  Usul::Types::Uint32 nv ( 0 );

  // read the header
  ifs->read ( reinterpret_cast< char* > ( &h1 ), sizeof( Usul::Types::Uint16 ) );
  ifs->read ( reinterpret_cast< char* > ( &h2 ), sizeof( Usul::Types::Uint16 ) );

  ifs->read ( reinterpret_cast< char* > ( &ni ), sizeof( Usul::Types::Int32 ) );
  ifs->read ( reinterpret_cast< char* > ( &nj ), sizeof( Usul::Types::Int32 ) );
  ifs->read ( reinterpret_cast< char* > ( &nk ), sizeof( Usul::Types::Int32 ) );
  ifs->read ( reinterpret_cast< char* > ( &nv ), sizeof( Usul::Types::Int32 ) );

  // read the footer
  ifs->read ( reinterpret_cast< char* > ( &h1 ), sizeof( Usul::Types::Uint16 ) );
  ifs->read ( reinterpret_cast< char* > ( &h2 ), sizeof( Usul::Types::Uint16 ) );

  // set the dimensions
  _dimensions = Usul::Math::Vec3ui ( static_cast< Usul::Types::Uint32 > ( ni ), 
                                     static_cast< Usul::Types::Uint32 > ( nj ), 
                                     static_cast< Usul::Types::Uint32 > ( nk ) );

  // size of the grid array
  Usul::Types::Uint64 size ( _dimensions[0] * _dimensions[1] * _dimensions[2] * sizeof( GridType ) );
      
  for( unsigned int k = 0; k < nk; ++k )
  {
    // temp 2d grid
    Grid1D grid1d;
    h1 = 0;h2 = 0;h3 = 0;h4 = 0;

    // read the header
    ifs->read ( reinterpret_cast< char* > ( &h1 ), sizeof( Usul::Types::Uint16 ) );
    ifs->read ( reinterpret_cast< char* > ( &h2 ), sizeof( Usul::Types::Uint16 ) );



    double tf ( 0 );
    //ifs->read ( reinterpret_cast< char* > ( &tf ), sizeof( double ) );
    //ifs->read ( reinterpret_cast< char* > ( &tf ), sizeof( Usul::Types::Float32 ) );

    //Usul::Endian::FromBigToSystem::convert ( tf );
    
    // set the size
    Usul::Types::Uint32 size ( nj * ni * sizeof( GridType ) );

    // resize the grid
    grid1d.resize( nj * ni );

    // read the 2d grid
    ifs->read( reinterpret_cast< char * > ( &grid1d[0] ), size );

    ifs->read ( reinterpret_cast< char* > ( &h3 ), sizeof( Usul::Types::Uint16 ) );
    ifs->read ( reinterpret_cast< char* > ( &h4 ), sizeof( Usul::Types::Uint16 ) );

    //// read the footer
    //ifs->read ( reinterpret_cast< char* > ( &h1 ), sizeof( Usul::Types::Uint16 ) );
    //ifs->read ( reinterpret_cast< char* > ( &h2 ), sizeof( Usul::Types::Uint16 ) );

  
    // add to the 3d grid
    grid.push_back( grid1d );
  }
#if 0
  Usul::Types::Float64 minValue( 0 );
  Usul::Types::Float32 maxValue( 0 );
  for( unsigned int i = 0; i < grid.size(); ++i )
  {
    for( unsigned int j = 0; j < grid.at( i ).size(); ++j )
    {
      Usul::Types::Float64 v ( grid.at( i ).at( j ) );
      if( v < minValue )
        minValue = v;

      if( v > maxValue )
        maxValue = v;
    }
  }
#endif

}


///////////////////////////////////////////////////////////////////////////////
//
//  Open the document.
//
///////////////////////////////////////////////////////////////////////////////

void Program::_openDocument ( const std::string &file, Usul::Documents::Document *document )
{
  /*if ( 0x0 == document )
    return;

  std::cout << "Opening file: " << file << " ... " << std::endl;
  document->open ( file, this );
  std::cout << "Done" << std::endl;*/
}


///////////////////////////////////////////////////////////////////////////////
//
//  Save the document.
//
///////////////////////////////////////////////////////////////////////////////

void Program::_saveDocument ( const std::string &file, Usul::Documents::Document *document )
{
  //if ( 0x0 == document )
  //  return;

  //std::cout << "Saving file: " << file << " ... " << std::endl;
  //document->saveAs ( file, this, 0x0 );
  //std::cout << "Done" << std::endl;
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
    // Usul::Threads::Mutex::createFunction ( &Usul::Threads::newSingleThreadedMutexStub );

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
