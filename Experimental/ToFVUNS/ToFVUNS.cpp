
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
#include "Usul/Math/Constants.h"
#include "Usul/Strings/Format.h"
#include "Usul/Convert/Convert.h"

#include "osg/Geode"
#include "osg/Group"
#include "osg/Geometry"
#include "osg/Array"

#include "osgDB/WriteFile"

#include <fstream>
#include <iomanip>
#include <iostream>

#define A_WALL         (07)
#define NOT_A_WALL     (0)

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
  _values( ),
  _workingDirectory( "" ),
  _baseFilename( "" ),
  _indexGrid(),
  _xValues(),
  _yValues(),
  _zValues(),
  _colorRamp()
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

  // calculate the y grid values
  // this->_calculateYGrid();

  // write the converted file
  this->_writeResultsFile( _workingDirectory + _baseFilename + ".results" );  

  // Write the grid file
  this->_writeGridFile( _workingDirectory + _baseFilename + ".grid" );

  // Write the osg file
  unsigned int y ( _dimensions[1] / 2 );
  this->_writeOsgSlice( y, _workingDirectory + _baseFilename + ".osg" );

  // write debug file
//#ifdef _DEBUG
//  this->_writeDebugFile( _workingDirectory + _baseFilename + ".debug.txt" );
//#endif
//
//#ifdef _RELEASE
//  this->_writeDebugFile( _workingDirectory + _baseFilename + ".debug.txt" );
//#endif
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
  if ( args.size() < 6 )
  {
    std::ostringstream out;
    out << "Usage:\n\t" << args.front() << " <u_filename> <v_filename> <w_filename> <p_filename> <grid_x> <grid_y>";
    throw std::runtime_error ( out.str() );
  }

  // Pop the ToFVUNS name.
  args.pop_front();

  // reserve space for the grids
  _values.reserve( args.size() );

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
  // Read the velocity and pressure files
  for ( unsigned int i = 0; i < _files.size(); ++i )
  {
    // The file
    std::string file ( Usul::File::fullPath( _files.at( i ) ) );
    
    // set the working directory
    _workingDirectory = Usul::File::directory( file, true );

    if( _baseFilename == "" )
    {
      // set the base filename
      _baseFilename = Usul::File::base( file );
    }

    try
    {
      if( i < 4 )
      {
        // Temp Value 
        Grid2D grid;

        // process the values
        this->_processFile ( file, grid );
        _values.push_back( grid );
      }
      else
      {
        // Temp Grid
        Grid1D grid;

        // Process the grid
        this->_procesGridFile( file, grid );

        // Add to the grid vector
        _grid.push_back( grid );
      }
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
  //ifs->read ( reinterpret_cast< char* > ( &h1 ), sizeof( Usul::Types::Uint16 ) );
  //ifs->read ( reinterpret_cast< char* > ( &h2 ), sizeof( Usul::Types::Uint16 ) );

  ifs->read ( reinterpret_cast< char* > ( &ni ), sizeof( Usul::Types::Int32 ) );
  ifs->read ( reinterpret_cast< char* > ( &nj ), sizeof( Usul::Types::Int32 ) );
  ifs->read ( reinterpret_cast< char* > ( &nk ), sizeof( Usul::Types::Int32 ) );
  ifs->read ( reinterpret_cast< char* > ( &nv ), sizeof( Usul::Types::Int32 ) );

  // debugging
  std::cout << "Resolution of: " << Usul::File::base( file ) << "." << Usul::File::extension( file ) << " is ( " << ni << " x " << nj << " x " << nk << " ) last number is: " << nv << std::endl;

  // read the footer
  //ifs->read ( reinterpret_cast< char* > ( &h1 ), sizeof( Usul::Types::Uint16 ) );
  //ifs->read ( reinterpret_cast< char* > ( &h2 ), sizeof( Usul::Types::Uint16 ) );

  // set the dimensions
  _dimensions = Usul::Math::Vec3ui ( static_cast< Usul::Types::Uint32 > ( ni ), 
                                     static_cast< Usul::Types::Uint32 > ( nj ), 
                                     static_cast< Usul::Types::Uint32 > ( nk ) );
#if 0
  for( unsigned int k = 0; k < nk; ++k )
  {
    // temp 2d grid
    Grid1D grid1d;
    h1 = 0;h2 = 0;h3 = 0;h4 = 0;

    // read the header
    ifs->read ( reinterpret_cast< char* > ( &h1 ), sizeof( Usul::Types::Uint16 ) );
    ifs->read ( reinterpret_cast< char* > ( &h2 ), sizeof( Usul::Types::Uint16 ) );
    
    // set the size
    Usul::Types::Uint32 size ( nj * ni * sizeof( GridType ) );

    // resize the grid
    grid1d.resize( nj * ni );

    // read the 2d grid
    ifs->read( reinterpret_cast< char * > ( &grid1d[0] ), size );

    ifs->read ( reinterpret_cast< char* > ( &h3 ), sizeof( Usul::Types::Uint16 ) );
    ifs->read ( reinterpret_cast< char* > ( &h4 ), sizeof( Usul::Types::Uint16 ) );
  
    // add to the 3d grid
    grid.push_back( grid1d );
  }
#else
#if 0
  unsigned int isize ( ni );
  unsigned int jsize ( nj );
  unsigned int ksize ( nk );

  for( unsigned int k = 0; k < ksize; ++k )
  {
    // temp 2d grid
    Grid1D grid1d;

    // resize the grid
    grid1d.resize( jsize * isize );

    for( unsigned int i = 0; i < isize; ++i )    
    {
      h1 = 0;h2 = 0;h3 = 0;h4 = 0;

      // read the header
      //ifs->read ( reinterpret_cast< char* > ( &h1 ), sizeof( Usul::Types::Uint16 ) );
      //ifs->read ( reinterpret_cast< char* > ( &h2 ), sizeof( Usul::Types::Uint16 ) );

      for( unsigned int j = 0; j < jsize; ++j )  
      {

        unsigned int index ( i + ( j * isize ) );

        h1 = 0;h2 = 0;h3 = 0;h4 = 0;

        // read the header
        //ifs->read ( reinterpret_cast< char* > ( &h1 ), sizeof( Usul::Types::Uint16 ) );
        //ifs->read ( reinterpret_cast< char* > ( &h2 ), sizeof( Usul::Types::Uint16 ) );  
        
        // read the 2d grid
        GridType value ( 0 );
        ifs->read( reinterpret_cast< char * > ( &value ), sizeof( GridType ) );
      
        //ifs->read ( reinterpret_cast< char* > ( &h3 ), sizeof( Usul::Types::Uint16 ) );
        //ifs->read ( reinterpret_cast< char* > ( &h4 ), sizeof( Usul::Types::Uint16 ) );
      
        grid1d.at( index ) = value;
        
      }
      
      //ifs->read ( reinterpret_cast< char* > ( &h3 ), sizeof( Usul::Types::Uint16 ) );
      //ifs->read ( reinterpret_cast< char* > ( &h4 ), sizeof( Usul::Types::Uint16 ) );
      
    }

    // add to the 3d grid
    grid.push_back( grid1d );
  }
#else

  unsigned int isize ( ni );
  unsigned int jsize ( nj );
  unsigned int ksize ( nk );
  for( unsigned int k = 0; k < ksize; ++k )
  {
    // temp 2d grid
    Grid1D grid1d;
    h1 = 0;h2 = 0;h3 = 0;h4 = 0;

    // read the header
    //ifs->read ( reinterpret_cast< char* > ( &h1 ), sizeof( Usul::Types::Uint16 ) );
    //ifs->read ( reinterpret_cast< char* > ( &h2 ), sizeof( Usul::Types::Uint16 ) );
    
    // set the size
    Usul::Types::Uint32 size ( jsize * isize * sizeof( GridType ) );

    // resize the grid
    grid1d.resize( jsize * isize );

    // read the 2d grid
    ifs->read( reinterpret_cast< char * > ( &grid1d[0] ), size );

    //ifs->read ( reinterpret_cast< char* > ( &h3 ), sizeof( Usul::Types::Uint16 ) );
    //ifs->read ( reinterpret_cast< char* > ( &h4 ), sizeof( Usul::Types::Uint16 ) );
  
    // add to the 3d grid
    grid.push_back( grid1d );
  }
#endif

#endif
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

  // File type -- results file
  {
    Usul::Types::Int32 type( 2 );
    ofs->write ( reinterpret_cast< char* > ( &type ), sizeof( Usul::Types::Int32 ) );
  }

  // Reserved
  {
    Usul::Types::Int32 reserved( 0 );
    ofs->write ( reinterpret_cast< char* > ( &reserved ), sizeof( Usul::Types::Int32 ) );
  }

  // Solution time, FSMACH, ALPHA, and RE
  {
    Usul::Types::Float32 time( 0.0 );
    ofs->write ( reinterpret_cast< char* > ( &time ), sizeof( Usul::Types::Float32 ) );

    Usul::Types::Float32 fsmach( 0.0 );
    ofs->write ( reinterpret_cast< char* > ( &fsmach ), sizeof( Usul::Types::Float32 ) );

    Usul::Types::Float32 alpha( 0.0 );
    ofs->write ( reinterpret_cast< char* > ( &alpha ), sizeof( Usul::Types::Float32 ) );

    Usul::Types::Float32 re( 0.0 );
    ofs->write ( reinterpret_cast< char* > ( &re ), sizeof( Usul::Types::Float32 ) );
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

    // U of Velocity
    char uVel[80] = "u; Velocity";
    ofs->write ( &uVel[0], 80 );

    // V of Velocity
    char vVel[80] = "v";
    ofs->write ( &vVel[0], 80 );

    // W of Velocity
    char wVel[80] = "w";
    ofs->write ( &wVel[0], 80 );

    // Pressure
    char pValue[80] = "pressure";
    ofs->write ( &pValue[0], 80 );
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
    this->_writeResults( name, ofs );
  }

  // output the boundary variables
  {
    // header FV_BNDRY_VARS
    Usul::Types::Int32 fvBndryVars( 1006 );
    ofs->write ( reinterpret_cast< char* > ( &fvBndryVars ), sizeof( Usul::Types::Int32 ) );
  }

  //close the file
  ofs->close();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the results file.
//
///////////////////////////////////////////////////////////////////////////////

void ToFVUNS::_writeResults( const std::string &name, std::ofstream* ofs )
{
  // debug
  std::cout << "Results dimensions are: ( " << _dimensions[0] << " x " << _dimensions[1] << " x " << _dimensions[2] << " )" << std::endl;

  unsigned int xsize( _dimensions[0] );
  unsigned int ysize( _dimensions[1] );
  unsigned int zsize( _dimensions[2] );

  unsigned int denominator( zsize );
  for( unsigned int valueNumber = 0; valueNumber < _values.size(); ++valueNumber )
  {

    for( unsigned int k = 0; k < zsize; ++k )
    {
      // progress
      unsigned int numerator = k;
      float percentage ( static_cast< float > ( numerator ) / static_cast< float > ( denominator ) );
      
      std::cout << "\rWrite function for value " << valueNumber << " -- percent complete ( " << percentage * 100.0f << "% ) -- ( " <<  numerator << " / " << denominator << " )" << std::flush;
      
      for( unsigned int j = 0; j < ysize; ++j )
      {        
        for( unsigned int i = 0; i < xsize; ++i )
        {          
          // current node index
#if 0
          // read by row
          unsigned int index ( j + ( i * ( ysize ) ) );
#else
          // read by column
          unsigned int index ( i + ( j * ( xsize ) ) );
#endif
          //unsigned int index ( ( i * ( xsize - 1 ) ) + j );

          try
          {
          // P value
          Usul::Types::Float32 value ( static_cast< Usul::Types::Float32 > ( _values.at( valueNumber ).at( zsize - 1 - k ).at( index ) ) );
          ofs->write ( reinterpret_cast< char* > ( &value ), sizeof( Usul::Types::Float32 ) );
          }
          catch( ... )
          {
            std::cout << "Failed on i=" << i << " -- j=" << j << "  -- k=" << k << std::endl;
            std::cout << "Index is: " << index << "Size of values at k=" << k << " is: " << _values.at( valueNumber ).size() << std::endl;

          }
          // U value
          //Usul::Types::Float32 u ( static_cast< Usul::Types::Float32 > ( _values.at( 0 ).at( k ).at( index ) ) );
          //ofs->write ( reinterpret_cast< char* > ( &u ), sizeof( Usul::Types::Float32 ) );

          //// VU value
          //Usul::Types::Float32 v ( static_cast< Usul::Types::Float32 > ( _values.at( 1 ).at( k ).at( index ) ) );
          //ofs->write ( reinterpret_cast< char* > ( &v ), sizeof( Usul::Types::Float32 ) );

          //// W value
          //Usul::Types::Float32 w ( static_cast< Usul::Types::Float32 > ( _values.at( 2 ).at( k ).at( index ) ) );
          //ofs->write ( reinterpret_cast< char* > ( &w ), sizeof( Usul::Types::Float32 ) );

        }
      }
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Process the Grid Files
//
///////////////////////////////////////////////////////////////////////////////

void ToFVUNS::_procesGridFile( const std::string &file, Grid1D &grid )
{
  // create a file handle
  std::ifstream ifs;

  // open the file
  ifs.open( file.c_str() );

  // make sure the file was opened
  if( false == ifs.is_open() )
    throw std::runtime_error ( "Error 1188374386: Failed to open file: " + file );

  // read the number of points
  Usul::Types::Uint32 numPoints ( 0 );

  // Read
  ifs >> numPoints;

  // output info
  std::cout << "Now reading file: " << file << "  -- # of points is " << numPoints << std::endl;

  for( unsigned int i = 0; i < numPoints; ++i )
  {
    // Index value
    Usul::Types::Uint32 index;

    // grid point value
    Usul::Types::Float32 value;

    // read and assign
    ifs >> index >> value;

    // store
    grid.push_back( value );
    
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the Grid file.
//
///////////////////////////////////////////////////////////////////////////////

void ToFVUNS::_writeGridFile( const std::string &name )
{
  // output info
  std::cout << "\nWriting Grid file: " << name << std::endl;

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

  // File Type
  {
    Usul::Types::Int32 type( 1 );
    ofs->write ( reinterpret_cast< char* > ( &type ), sizeof( Usul::Types::Int32 ) );
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

  // boundary types
  {
    // Number
    Usul::Types::Int32 numBoundary( 1 );
    ofs->write ( reinterpret_cast< char* > ( &numBoundary ), sizeof( Usul::Types::Int32 ) );

    // Surface results
    Usul::Types::Int32 surfaceResults( 0 );
    ofs->write ( reinterpret_cast< char* > ( &surfaceResults ), sizeof( Usul::Types::Int32 ) );

    // clockness
    Usul::Types::Int32 clockness( 0 );
    ofs->write ( reinterpret_cast< char* > ( &clockness ), sizeof( Usul::Types::Int32 ) );

    // Name
    char fvid[80] = "wall";
    ofs->write ( &fvid[0], 80 );

  }

  // get the size of the x grid
  unsigned int xGridSize( _dimensions[0] );

  // get the size of the y grid
  unsigned int yGridSize( _dimensions[1] );

  // get the size fo the z grid
  unsigned int zGridSize( _dimensions[2] );

  // Node keyword and size
  
  Usul::Types::Int32 keyWord ( 1001 );
  Usul::Types::Int32 count ( xGridSize * yGridSize * zGridSize );

  // Write
  ofs->write ( reinterpret_cast< char* > ( &keyWord ), sizeof( Usul::Types::Int32 ) );
  ofs->write ( reinterpret_cast< char* > ( &count ), sizeof( Usul::Types::Int32 ) );

  // write the grid nodes
  {
    // Hold the X values
    FloatVector xValues;
    xValues.resize( count );

    // Hold the Y values
    FloatVector yValues;
    yValues.resize( count );

    // Hold the Z values
    FloatVector zValues;
    zValues.resize( count );

    Usul::Types::Uint32 index ( 0 );

    unsigned int denominator( zGridSize );

    // resize the index grid
    _indexGrid.resize( zGridSize );
  
    // loop through the values in the grid(s)
    for( unsigned int k = 0; k < zGridSize; ++k )    
    {
       
      // progress
      float percentage ( static_cast< float > ( k ) / static_cast< float > ( denominator ) );
      std::cout << "\rWrite function percent complete ( " << percentage * 100.0f << "% ) -- ( " <<  k << " / " << denominator << " )" << std::flush;

      // resize the index grid at j
      _indexGrid.at( k ).resize( yGridSize );

      for( unsigned int j = 0; j < yGridSize; ++j )
      {
        // yvalue calculation X(I)=REAL(I-1)/REAL(NX-2)*2.*PI
      //float yGridValue = ( static_cast< float > ( j ) / static_cast< float > ( yGridSize - 1 ) ) * 2 * static_cast< float > ( Usul::Math::PIE );
        
        // resize the index grid at jk
        _indexGrid.at( k ).at( j ).resize( xGridSize );
        for( unsigned int i = 0; i < xGridSize; ++i )
        {
          // Set the xgrid
          //float xGridValue = static_cast< Float32 > ( _grid.at( 0 ).at( i ) );

          // Set the final grid values for this iteration
          //xValues.at( index ) = xGridValue * static_cast< Float32 > ( cos( static_cast< double > ( yGridValue ) ) );
          //yValues.at( index ) = xGridValue * static_cast< Float32 > ( sin( static_cast< double > ( yGridValue ) ) );
          //zValues.at( index ) = static_cast< Float32 > ( _grid.at( 1 ).at( k ) );

          xValues.at( index ) = static_cast< Float32 > ( _grid.at( 0 ).at( i ) );
          yValues.at( index ) = static_cast< Float32 > ( _grid.at( 2 ).at( j ) );
          zValues.at( index ) = static_cast< Float32 > ( _grid.at( 1 ).at( k ) );

          // push back the index counter
          _indexGrid.at( k ).at( j ).at( i ) = index + 1;

          // increment the index
          ++index;

        }
      }
    }

    // Grid size in bytes
    Usul::Types::Uint32 sizeInBytes( count * sizeof( Float32 ) );

    // write the x grid    
    ofs->write ( reinterpret_cast< char* > ( &xValues[0] ), sizeInBytes );

    // write the y grid    
    ofs->write ( reinterpret_cast< char* > ( &yValues[0] ), sizeInBytes );

    // write the z grid    
    ofs->write ( reinterpret_cast< char* > ( &zValues[0] ), sizeInBytes );

    // debug
    _xValues = xValues;
    _yValues = yValues;
    _zValues = zValues;
  }  

  // Faces section
  {
    // faces keyword
    Usul::Types::Int32 fvFaces( 1002 );
    ofs->write ( reinterpret_cast< char* > ( &fvFaces ), sizeof( Usul::Types::Int32 ) );

    // type
    Usul::Types::Int32 fvFacesType( 1 );
    ofs->write ( reinterpret_cast< char* > ( &fvFacesType ), sizeof( Usul::Types::Int32 ) );

    // count
    Usul::Types::Int32 fvFacesCount( 0 );
    ofs->write ( reinterpret_cast< char* > ( &fvFacesCount ), sizeof( Usul::Types::Int32 ) );
  }

#if 0
  // Arbitrary Faces section
  {
    // faces keyword
    Usul::Types::Int32 fvArbPolyFaces( 1007 );
    ofs->write ( reinterpret_cast< char* > ( &fvArbPolyFaces ), sizeof( Usul::Types::Int32 ) );

    // type
    Usul::Types::Int32 fvFacesType( 0 );
    ofs->write ( reinterpret_cast< char* > ( &fvFacesType ), sizeof( Usul::Types::Int32 ) );

    // count
    Usul::Types::Int32 fvFacesCount( 0 );
    ofs->write ( reinterpret_cast< char* > ( &fvFacesCount ), sizeof( Usul::Types::Int32 ) );
  }
#endif

  // Elements section
  {
    // faces keyword
    Usul::Types::Int32 fvElements( 1003 );
    ofs->write ( reinterpret_cast< char* > ( &fvElements ), sizeof( Usul::Types::Int32 ) );

    // Tetrahedron count
    Usul::Types::Int32 numTetrahedrons( 0 );
    ofs->write ( reinterpret_cast< char* > ( &numTetrahedrons ), sizeof( Usul::Types::Int32 ) );

    // Hexahedron count
    Usul::Types::Uint32 hexaHedronCount ( ( _dimensions[0] - 1 ) * ( _dimensions[1] - 1 ) * ( _dimensions[2] - 1 ) );
    Usul::Types::Int32 numHexahedrons( hexaHedronCount );
    ofs->write ( reinterpret_cast< char* > ( &numHexahedrons ), sizeof( Usul::Types::Int32 ) );

    // Prisms count
    Usul::Types::Int32 numPrisms( 0 );
    ofs->write ( reinterpret_cast< char* > ( &numPrisms ), sizeof( Usul::Types::Int32 ) );

    // Pyramids count
    Usul::Types::Int32 numPryamids( 0 );
    ofs->write ( reinterpret_cast< char* > ( &numPryamids ), sizeof( Usul::Types::Int32 ) );
  }

  // Write the nodes
  this->_writeElementNodes( ofs );

  // Arbitrary Polyhedron Elements section
  //{
  //  // keyword
  //  Usul::Types::Int32 fvArbPolyElements( 1004 );
  //  ofs->write ( reinterpret_cast< char* > ( &fvArbPolyElements ), sizeof( Usul::Types::Int32 ) );

  //  // Tetrahedron count
  //  Usul::Types::Int32 count( 0 );
  //  ofs->write ( reinterpret_cast< char* > ( &count ), sizeof( Usul::Types::Int32 ) );
  //  
  //}

  // close the file
  ofs->close();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the element header.
//
///////////////////////////////////////////////////////////////////////////////

void ToFVUNS::_writeElementNodeHeader( std::ofstream* ofs )
{
  //#define MAX_NUM_ELEM_FACES     6
//#define BITS_PER_WALL  3
//#define ELEM_TYPE_BIT_SHIFT    (MAX_NUM_ELEM_FACES*BITS_PER_WALL)
  /* Wall values for element faces. 
    static int hex1_walls[6] = { A_WALL, A_WALL, NOT_A_WALL,
	    NOT_A_WALL, A_WALL, A_WALL };
    static int hex2_walls[6] = { A_WALL, A_WALL, NOT_A_WALL,
	    NOT_A_WALL, A_WALL, A_WALL };
      */

  // define the header storage variable
  unsigned int header ( 0 );
  
  // shift the appropriate bits for a hexahedral cell, see write_split_binary_uns.c
  header = ( 4 << 18 );

  // number of faces
  unsigned int nfaces ( 6 );

  // No walls in our node
  unsigned int wall_info[] = { NOT_A_WALL, NOT_A_WALL, NOT_A_WALL, NOT_A_WALL, NOT_A_WALL, NOT_A_WALL };

  // loop through the faces
  for (unsigned int i = 0; i < nfaces; i++)
  {
    // grab a face
    unsigned int u = wall_info[i];

    // output the face type to the header
    header |= (u << ( i * 3 ) );

  }

  // write the header
  ofs->write ( reinterpret_cast< char* > ( &header ), sizeof( unsigned int ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the element nodes.
//
///////////////////////////////////////////////////////////////////////////////

void ToFVUNS::_writeElementNodes( std::ofstream* ofs )
{ 
  // Spacing 
  std::cout << std::endl;

 // useful typedef
  typedef Usul::Types::Uint32 Uint32;

  float denominator ( static_cast< float > ( _dimensions[2] - 1 ) );

  // Write out the actual elements, which are hexahedral cells
  for( unsigned int k = 0; k < _dimensions[2] - 1; ++k )
  {
    float numerator ( static_cast< float >( k ) );

    // Feedback
    std::cout << "\rWriting Element Nodes -- " << ( ( numerator * 100 ) / denominator ) << "% complete" << std::flush;

    for( unsigned int j = 0; j < _dimensions[1] - 1; ++j )
    {
      for( unsigned int i = 0; i < _dimensions[0] - 1; ++i )
      {

#if 0
        // vertex 1
        Uint32 v1 ( _indexGrid.at( k ).at( j ).at( i ) );

        // vertex 2
        Uint32 v2 ( _indexGrid.at( k ).at( j ).at( i + 1 ) );

        // vertex 3
        Uint32 v3 ( _indexGrid.at( k + 1 ).at( j ).at( i ) );

        // vertex 4
        Uint32 v4 ( _indexGrid.at( k + 1 ).at( j ).at( i + 1 ) );

        // vertex 5
        Uint32 v5 ( _indexGrid.at( k ).at( j + 1 ).at( i ) );

        // vertex 6
        Uint32 v6 ( _indexGrid.at( k ).at( j + 1 ).at( i + 1 ) );

        // vertex 7
        Uint32 v7 ( _indexGrid.at( k + 1 ).at( j + 1 ).at( i ) );

        // vertex 8
        Uint32 v8 ( _indexGrid.at( k + 1 ).at( j + 1 ).at( i + 1 ) );
#else
                // vertex 1
        Uint32 v1 ( _indexGrid.at( k ).at( j ).at( i ) );

        // vertex 2
        Uint32 v2 ( _indexGrid.at( k + 1 ).at( j ).at( i ) );

        // vertex 3
        Uint32 v3 ( _indexGrid.at( k  ).at( j + 1 ).at( i ) );

        // vertex 4
        Uint32 v4 ( _indexGrid.at( k + 1  ).at( j + 1 ).at( i) );

        // vertex 5
        Uint32 v5 ( _indexGrid.at( k ).at( j ).at( i + 1  ) );

        // vertex 6
        Uint32 v6 ( _indexGrid.at( k + 1 ).at( j ).at( i + 1 ) );

        // vertex 7
        Uint32 v7 ( _indexGrid.at( k ).at( j + 1 ).at( i + 1  ) );

        // vertex 8
        Uint32 v8 ( _indexGrid.at( k + 1 ).at( j + 1 ).at( i + 1 ) );
#endif

        
        // Write the header
        this->_writeElementNodeHeader( ofs );

        // write the nodes
        ofs->write ( reinterpret_cast< char* > ( &v1 ), sizeof( Usul::Types::Uint32 ) );
        ofs->write ( reinterpret_cast< char* > ( &v2 ), sizeof( Usul::Types::Uint32 ) );
        ofs->write ( reinterpret_cast< char* > ( &v3 ), sizeof( Usul::Types::Uint32 ) );
        ofs->write ( reinterpret_cast< char* > ( &v4 ), sizeof( Usul::Types::Uint32 ) );
        ofs->write ( reinterpret_cast< char* > ( &v5 ), sizeof( Usul::Types::Uint32 ) );
        ofs->write ( reinterpret_cast< char* > ( &v6 ), sizeof( Usul::Types::Uint32 ) );
        ofs->write ( reinterpret_cast< char* > ( &v7 ), sizeof( Usul::Types::Uint32 ) );
        ofs->write ( reinterpret_cast< char* > ( &v8 ), sizeof( Usul::Types::Uint32 ) );

      }
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
//
//  Debug method to write the results in ascii for visual validation
//
///////////////////////////////////////////////////////////////////////////////

void ToFVUNS::_writeDebugFile( const std::string& filename )
{
  // create a file handle
  std::ofstream ofs;

  // open the file
  ofs.open( filename.c_str() );

  // make sure the file was opened
  if( false == ofs.is_open() )
    throw std::runtime_error ( "Error 1188374386: Failed to open file: " + filename );

  // output debug file write notification
  std::cout << "\nWriting debug file: " << filename << std::endl;

  // output the grid dimensions
  std::string gridMessage( Usul::Strings::format( "Grid Dimensions ( ", _dimensions[0], " x " , _dimensions[1] , " x " , _dimensions[2] , " )\n" ) );
  ofs << gridMessage.c_str();

  // grid file information
  ofs << "************* Grid Information *****************\n";
  ofs << "X\tY\tZ\n";

  unsigned int size( _xValues.size() );

  for( unsigned int i = 0; i < size; ++i )
  {
    std::string gridline ( Usul::Strings::format( _xValues.at( i ), "\t", _yValues.at( i ), "\t", _zValues.at( i ), "\n" ) );
    ofs << gridline.c_str();
  }
     
  ofs << "-------------- End of Grid Information ------------------\n\n\n";

  // results file information
  ofs << "****************** Results File Information **********************\n";
  ofs << "X\tY\tZ\tU\tV\tW\tP\n";

  for( unsigned int k = 0; k < _dimensions[2]; ++k )
  {
    for( unsigned int j = 0; j < _dimensions[1]; ++j )
    {
      for( unsigned int i = 0; i < _dimensions[0]; ++i )
      {
        unsigned int index ( i + ( j * _dimensions[0] ) );
        std::string result ( Usul::Strings::format( i+1, "\t", j+1, "\t", k+1, "\t" ) );
        result = Usul::Strings::format( result, _values.at( 0 ).at( k ).at( index ), "\t", 
                                                _values.at( 1 ).at( k ).at( index ), "\t", 
                                                _values.at( 2 ).at( k ).at( index ), "\t", 
                                                _values.at( 3 ).at( k ).at( index ), "\n" );
        ofs << result.c_str();
      }
    }
  }

  ofs << "------------------- End of Results Data ------------------\n\n\n";

  // Index data for elements
  ofs << "******************* Element Node Index Data **********************\n";
  ofs << "I\tJ\tK\tIndex\n";

  for( unsigned int k = 0; k < _dimensions[2]; ++k )
  {
    for( unsigned int j = 0; j < _dimensions[1]; ++j )
    {
      for( unsigned int i = 0; i < _dimensions[0]; ++i )
      {
        std::string indexMessage ( Usul::Strings::format( i+1, "\t", j+1, "\t", k+1, "\t", _indexGrid.at( k ).at( j ).at( i ), "\n" ) );
        ofs << indexMessage.c_str();
      }
    }
  }
  ofs << "------------------- End of Element Node Data ---------------------\n";

  // close the file
  ofs.close();

}

#define CHANNEL_TO_TEST 0

///////////////////////////////////////////////////////////////////////////////
//
// write an osg file for debugging
//
///////////////////////////////////////////////////////////////////////////////

void ToFVUNS::_findMinMax( float& min, float& max )
{
  unsigned int xsize( _dimensions[0] );
  unsigned int ysize( _dimensions[1] );
  unsigned int zsize( _dimensions[2] );
  
  min = std::numeric_limits<float>::max(); //( static_cast< float > ( _values.at( 2 ).at( 0 ).at( 0 ) ) );
  max = std::numeric_limits<float>::min();

  for( unsigned int k = 0; k < zsize; ++k )
  {     
    for( unsigned int j = 0; j < ysize; ++j )
    {        
      for( unsigned int i = 0; i < xsize; ++i )
      {  
        unsigned int index ( i + ( j * ( xsize ) ) );
        float value ( static_cast< float > ( _values.at( CHANNEL_TO_TEST ).at( k ).at( index ) ) );

        if( value < min )
        {
          min = value;
        }
        if( value > max )
        {
          max = value;
        }
      }
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
//
// Initialize the color ramp
//
///////////////////////////////////////////////////////////////////////////////

void ToFVUNS::_intializeColorRamp()
{
  unsigned int r ( 0 ), g( 0 ), b( 0 );

  for( b = 254; b != 0; b-=1 )
  {
    Usul::Math::Vec3ui color ( r, g, b );
    _colorRamp.push_back( color );

    g+= 1;
  }
  for( r = 0; r < 255; r+=1 )
	{
    Usul::Math::Vec3ui color ( r, g, b );
    _colorRamp.push_back( color );

    g -= 1;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
// write an osg file for debugging
//
///////////////////////////////////////////////////////////////////////////////

osg::Vec3 ToFVUNS::_getInterpolatedValue( unsigned int x, unsigned int y, unsigned int z, float min, float max )
{
  osg::Vec3 color ( 0.0, 0.0, 0.0 );

  unsigned int index ( ( x ) + ( ( y ) * ( _dimensions[0] ) ) );
  float value ( static_cast< float > ( _values.at( CHANNEL_TO_TEST ).at( z ).at( index ) ) );

  float percentage ( ( value - min ) / ( max - min ) );

  unsigned int colorIndex ( percentage * _colorRamp.size() );

  if( colorIndex < _colorRamp.size() && colorIndex >= 0 )
  {
    Usul::Math::Vec3ui colorAtIndex( _colorRamp.at( colorIndex ) );
    color = osg::Vec3( colorAtIndex[0], colorAtIndex[1], colorAtIndex[2] );
  }

  return color;
}


///////////////////////////////////////////////////////////////////////////////
//
// write an osg file for debugging
//
///////////////////////////////////////////////////////////////////////////////

void ToFVUNS::_writeOsgSlice( unsigned int y, const std::string& filename )
{ 
  // useful typedefs
  typedef osg::ref_ptr< osg::Group > GroupPtr;
  typedef osg::ref_ptr< osg::Geometry > GeometryPtr;
  typedef osg::ref_ptr< osg::Geode > GeodePtr;
  typedef osg::ref_ptr< osg::Vec3Array > Vec3ArrayPtr;

  // grab the grid points
  Grid1D xGrid ( _grid.at( 0 ) );
  Grid1D yGrid ( _grid.at( 2 ) );
  Grid1D zGrid ( _grid.at( 1 ) ) ;

  // initialize the color ramp
  this->_intializeColorRamp();

  // get the min and max of the w values
  float min ( 0 );
  float max( 0 );
  this->_findMinMax( min, max );

  // create the points array
  Vec3ArrayPtr points ( new osg::Vec3Array );

  // create the color array
  Vec3ArrayPtr colors ( new osg::Vec3Array );

  // create the normals array
  Vec3ArrayPtr normals ( new osg::Vec3Array );

  for( unsigned int i = 0; i < xGrid.size() - 1; ++i )
  {
    for( unsigned int k = 0; k < zGrid.size() - 1; ++k )
    {
      osg::Vec3f p0 ( xGrid.at( i     ), yGrid.at( y ), zGrid.at( k     ) );
      osg::Vec3f p1 ( xGrid.at( i + 1 ), yGrid.at( y ), zGrid.at( k     ) );
      osg::Vec3f p2 ( xGrid.at( i     ), yGrid.at( y ), zGrid.at( k + 1 ) );
      osg::Vec3f p3 ( xGrid.at( i + 1 ), yGrid.at( y ), zGrid.at( k + 1 ) );

      osg::Vec3f c0 ( this->_getInterpolatedValue( i    , y, k    , min, max ) );
      osg::Vec3f c1 ( this->_getInterpolatedValue( i + 1, y, k    , min, max ) );
      osg::Vec3f c2 ( this->_getInterpolatedValue( i    , y, k + 1, min, max ) );
      osg::Vec3f c3 ( this->_getInterpolatedValue( i + 1, y, k + 1, min, max ) );

      points->push_back( p0 );points->push_back( p1 );points->push_back( p3 );
      points->push_back( p0 );points->push_back( p3 );points->push_back( p2 );

      colors->push_back( c0 / 255 );colors->push_back( c1 / 255 );colors->push_back( c3 / 255 );
      colors->push_back( c0 / 255 );colors->push_back( c3 / 255 );colors->push_back( c2 / 255 );

      for( unsigned int n = 0; n < 6; ++n )
      {
        normals->push_back( osg::Vec3f( 0.0f, 1.0f, 0.0f ) );
      }

    }
  }

  // create the geometry
  GeometryPtr geometry ( new osg::Geometry );

  // set the normal array and binding
  geometry->setNormalArray( normals.get() );
  geometry->setNormalBinding( osg::Geometry::BIND_PER_VERTEX );

  // set the color array and binding
  geometry->setColorArray( colors.get() );
  geometry->setColorBinding( osg::Geometry::BIND_PER_VERTEX );

  // set the vertices and create the primitives
  geometry->setVertexArray( points.get() );
  geometry->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::TRIANGLES, 0, points->size() ) );

  // create the geode and add the geometry
  GeodePtr geode ( new osg::Geode );
  geode->addDrawable( geometry.get() );

  // create the group and add the geode
  GroupPtr scene ( new osg::Group );
  scene->addChild( geode.get() );

  // write the osg file
  std::cout << "Writing OSG file for: " << filename << "..." << std::endl;
  osgDB::writeNodeFile( *scene.get(), filename );
}