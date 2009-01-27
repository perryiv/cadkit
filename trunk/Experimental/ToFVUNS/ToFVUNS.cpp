
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Jeff Conner, Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

#include "ToFVUNS.h"

#include "Usul/File/Path.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Interfaces/IDocument.h"
#include "Usul/Interfaces/IPlugin.h"
#include "Usul/Threads/Mutex.h"
#include "Usul/Scope/CurrentDirectory.h"

#include <fstream>
#include <iomanip>
#include <iostream>

///////////////////////////////////////////////////////////////////////////////
//
//  Interface plumbing.
//
///////////////////////////////////////////////////////////////////////////////

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( ToFVUNS, ToFVUNS::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

ToFVUNS::ToFVUNS ( int argc, char **argv ) :
  _args ( argv, argv + argc ),
  _files(),
  _dimensions( 0, 0, 0 ),
  _grid( ),
  _workingDirectory( "" ),
  _baseFilename( "" )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

ToFVUNS::~ToFVUNS()
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

Usul::Interfaces::IUnknown *ToFVUNS::queryInterface ( unsigned long iid )
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
//  Run the ToFVUNS.
//
///////////////////////////////////////////////////////////////////////////////

void ToFVUNS::run()
{
  // Process arguments.
  this->_processArguments();

  // Loop through files.
  this->_processFiles();

  // write the converted file
  this->_writeResultsFile( _workingDirectory + _baseFilename + ".results" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Process the command-line arguments.
//
///////////////////////////////////////////////////////////////////////////////

void ToFVUNS::_processArguments()
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

  // Pop the ToFVUNS name.
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

void ToFVUNS::_loadPlugin ( const std::string &file )
{
  Usul::Components::Manager::instance().load ( Usul::Interfaces::IPlugin::IID, file );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Process the files to be converted.
//
///////////////////////////////////////////////////////////////////////////////

void ToFVUNS::_processFiles()
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
    
    // set the working directory
    _workingDirectory = Usul::File::directory( file, true );

    // set the base filename
    _baseFilename = Usul::File::base( file );

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

void ToFVUNS::_processFile ( const std::string &file, Grid2D &grid )
{
 // create a file handle
  std::ifstream* ifs ( new std::ifstream );

  // open the file
  ifs->open( file.c_str(), std::ifstream::out | std::ifstream::binary );

  // make sure the file was opened
  if( false == ifs->is_open() )
    throw std::runtime_error ( "Error 1188374386: Failed to open file: " + file );

  // output info
  std::cout << "Now reading file: " << file << std::endl;

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

void ToFVUNS::_openDocument ( const std::string &file, Usul::Documents::Document *document )
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

void ToFVUNS::_saveDocument ( const std::string &file, Usul::Documents::Document *document )
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

void ToFVUNS::updateProgressBar ( unsigned int value )
{
  std::cout << std::setw ( 3 ) << value << '%' << '\r' << std::flush;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the results file.
//
///////////////////////////////////////////////////////////////////////////////

void ToFVUNS::_writeResultsFile( const std::string &name )
{

  // output info
  std::cout << "Writing results file: " << name << std::endl;
  // create a file handle
  std::ofstream* ofs ( new std::ofstream );

  // open the file
  ofs->open( name.c_str(), std::ofstream::out | std::ifstream::binary );

  // make sure the file was opened
  if( false == ofs->is_open() )
    throw std::runtime_error ( "Error 1188374386: Failed to open file: " + name );

  // Write the magic number
  {    
    Usul::Types::Int32 magicNumber( 66051 );
    ofs->write ( reinterpret_cast< char* > ( &magicNumber ), sizeof( Usul::Types::Int32 ) );
  }

  // Write the fieldview header
  {
    char fvid[80] = "FIELDVIEW";
    ofs->write ( &fvid[0], 80 );
  }

  // write the version number
  {    
    Usul::Types::Int32 vn1( 3 );
    Usul::Types::Int32 vn2( 0 );
    ofs->write ( reinterpret_cast< char* > ( &vn1 ), sizeof( Usul::Types::Int32 ) );
    ofs->write ( reinterpret_cast< char* > ( &vn2 ), sizeof( Usul::Types::Int32 ) );
  }

  // Reserved
  {
    Usul::Types::Int32 reserved( 0 );
    ofs->write ( reinterpret_cast< char* > ( &reserved ), sizeof( Usul::Types::Int32 ) );
  }

  // Number of grids
  {
    Usul::Types::Int32 nGrids( 1 );
    ofs->write ( reinterpret_cast< char* > ( &nGrids ), sizeof( Usul::Types::Int32 ) );
  }

  // Write the data header
  {
    // write the number of columns
    Usul::Types::Int32 numCols( 4 );
    ofs->write ( reinterpret_cast< char* > ( &numCols ), sizeof( Usul::Types::Int32 ) );

    // Pressure
    char pValue[80] = "pressure";
    ofs->write ( &pValue[0], 80 );

    // U of Velocity
    char uVel[80] = "u; Velocity";
    ofs->write ( &uVel[0], 80 );

    // V of Velocity
    char vVel[80] = "v";
    ofs->write ( &vVel[0], 80 );

    // W of Velocity
    char wVel[80] = "w";
    ofs->write ( &wVel[0], 80 );
  }

  // Write boundary header information
  {
    // no boundary data for now
    Usul::Types::Int32 numCols( 0 );
    ofs->write ( reinterpret_cast< char* > ( &numCols ), sizeof( Usul::Types::Int32 ) );
  }

  // Node information
  {
    // header FV_NODES
    Usul::Types::Int32 fvNodes( 1001 );
    ofs->write ( reinterpret_cast< char* > ( &fvNodes ), sizeof( Usul::Types::Int32 ) );

    // Number of nodes
    Usul::Types::Int32 numNodes( _dimensions[0] * _dimensions[1] * _dimensions[2] );
    ofs->write ( reinterpret_cast< char* > ( &numNodes ), sizeof( Usul::Types::Int32 ) );

  }

  // output the variables
  {
    // header FV_VARIABLES
    Usul::Types::Int32 fvVars( 1004 );
    ofs->write ( reinterpret_cast< char* > ( &fvVars ), sizeof( Usul::Types::Int32 ) );

    // output the node data
    this->_writeGrid( name, ofs );
  }

  // output the boundary variables
  {
    // header FV_BNDRY_VARS
    Usul::Types::Int32 fvBndryVars( 1006 );
    ofs->write ( reinterpret_cast< char* > ( &fvBndryVars ), sizeof( Usul::Types::Int32 ) );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the results file.
//
///////////////////////////////////////////////////////////////////////////////

void ToFVUNS::_writeGrid( const std::string &name, std::ofstream* ofs )
{
  unsigned int denominator( _dimensions[2] );
  for( unsigned int k = 0; k < _dimensions[2]; ++k )
  {
    // progress
    float percentage ( static_cast< float > ( k ) / static_cast< float > ( denominator ) );
    std::cout << "\rWrite function percent complete ( " << percentage * 100.0f << "% ) -- ( " <<  k << " / " << denominator << " )" << std::flush;
    
    for( unsigned int j = 0; j < _dimensions[1]; ++j )
    {
      for( unsigned int i = 0; i < _dimensions[0]; ++i )
      {
        // current node index
        unsigned int index ( j + ( i * _dimensions[1] ) );

        // P value
        Usul::Types::Float32 p ( static_cast< Usul::Types::Float32 > ( _grid.at( 3 ).at( k ).at( index ) ) );
        ofs->write ( reinterpret_cast< char* > ( &p ), sizeof( Usul::Types::Float32 ) );

        // U value
        Usul::Types::Float32 u ( static_cast< Usul::Types::Float32 > ( _grid.at( 0 ).at( k ).at( index ) ) );
        ofs->write ( reinterpret_cast< char* > ( &u ), sizeof( Usul::Types::Float32 ) );

        // VU value
        Usul::Types::Float32 v ( static_cast< Usul::Types::Float32 > ( _grid.at( 1 ).at( k ).at( index ) ) );
        ofs->write ( reinterpret_cast< char* > ( &v ), sizeof( Usul::Types::Float32 ) );

        // W value
        Usul::Types::Float32 w ( static_cast< Usul::Types::Float32 > ( _grid.at( 2 ).at( k ).at( index ) ) );
        ofs->write ( reinterpret_cast< char* > ( &w ), sizeof( Usul::Types::Float32 ) );

      }
    }
  }
}