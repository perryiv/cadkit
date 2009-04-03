
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
  _indexGrid()
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
  for( unsigned int k = 0; k < nk; ++k )
  {
    // temp 2d grid
    Grid1D grid1d;

    // resize the grid
    grid1d.resize( nj * ni );

    for( unsigned int j = 0; j < nj; ++j )
    {
      h1 = 0;h2 = 0;h3 = 0;h4 = 0;

      // read the header
      ifs->read ( reinterpret_cast< char* > ( &h1 ), sizeof( Usul::Types::Uint16 ) );
      ifs->read ( reinterpret_cast< char* > ( &h2 ), sizeof( Usul::Types::Uint16 ) );

      for( unsigned int i = 0; i < ni; ++i )
      {

        unsigned int index ( j + ( i * _dimensions[1] ) );

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
      
      ifs->read ( reinterpret_cast< char* > ( &h3 ), sizeof( Usul::Types::Uint16 ) );
      ifs->read ( reinterpret_cast< char* > ( &h4 ), sizeof( Usul::Types::Uint16 ) );
      
    }

    // add to the 3d grid
    grid.push_back( grid1d );
  }
#else

  unsigned int isize ( ni + 1 );
  unsigned int jsize ( nj + 1 );
  unsigned int ksize ( nk + 1 );
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

  unsigned int denominator( ysize );
  for( unsigned int valueNumber = 0; valueNumber < _values.size(); ++valueNumber )
  {
    for( unsigned int j = 0; j < ysize; ++j )  
    {
      // progress
      unsigned int numerator = j;
      float percentage ( static_cast< float > ( numerator ) / static_cast< float > ( denominator ) );
      
      std::cout << "\rWrite function for value " << valueNumber << " -- percent complete ( " << percentage * 100.0f << "% ) -- ( " <<  numerator << " / " << denominator << " )" << std::flush;
      
      for( unsigned int k = 0; k < zsize; ++k )
      {        
        for( unsigned int i = 0; i < xsize; ++i )
        {
          //std::cout << "\rTrying i=" << i << " -- j=" << j << "  -- k=" << k << std::flush;
          // current node index
          unsigned int index ( j + ( i * ( ysize ) ) );
          //unsigned int index ( ( i * ( xsize - 1 ) ) + j );
          try
          {
          // P value
          Usul::Types::Float32 value ( static_cast< Usul::Types::Float32 > ( _values.at( valueNumber ).at( k ).at( index ) ) );
          ofs->write ( reinterpret_cast< char* > ( &value ), sizeof( Usul::Types::Float32 ) );
          }
          catch( ... )
          {
            std::cout << "Failed on i=" << i << " -- j=" << j << "  -- k=" << k << std::endl;
            std::cout << "Index is: " << index << "Size of values at k=" << k << " is: " << _values.at( k ).size() << std::endl;

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
    typedef Usul::Types::Float32 Float32;
    typedef std::vector< Float32 > FloatVector;
    
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

    unsigned int denominator( yGridSize );

    // resize the index grid
    _indexGrid.resize( yGridSize );
  
    // loop through the values in the grid(s)
    for( unsigned int j = 0; j < yGridSize; ++j )
    {
      // yvalue calculation X(I)=REAL(I-1)/REAL(NX-2)*2.*PI
      float yGridValue = ( static_cast< float > ( j ) / static_cast< float > ( yGridSize - 2 ) ) * 2 * static_cast< float > ( Usul::Math::PIE );
         
      // progress
      float percentage ( static_cast< float > ( j ) / static_cast< float > ( denominator ) );
      std::cout << "\rWrite function percent complete ( " << percentage * 100.0f << "% ) -- ( " <<  j << " / " << denominator << " )" << std::flush;

      // resize the index grid at j
      _indexGrid.at( j ).resize( zGridSize );

      for( unsigned int k = 0; k < zGridSize; ++k )
      {
        // resize the index grid at jk
        _indexGrid.at( j ).at( k ).resize( xGridSize );
        for( unsigned int i = 0; i < xGridSize; ++i )
        {
          // Set the xgrid
          float xGridValue = static_cast< Float32 > ( _grid.at( 0 ).at( i ) );

          // Set the final grid values for this iteration
          xValues.at( index ) = xGridValue * static_cast< float > ( cos( static_cast< double > ( yGridValue ) ) );
          yValues.at( index ) = xGridValue * static_cast< float > ( sin( static_cast< double > ( yGridValue ) ) );
          zValues.at( index ) = static_cast< Float32 > ( _grid.at( 1 ).at( k ) );

          // push back the index counter
          _indexGrid.at( j ).at( k ).at( i ) = index + 1;

          // increment the index
          ++index;

        }
      }
    }

    // add the last grid point
    //GridType xvalue ( xValues.at( xValues.size() - 1 ) + ( xValues.at( xValues.size() - 1 ) - xValues.at( xValues.size() - 2 ) ) );
    //xValues.push_back( xvalue );

    //GridType yvalue ( yValues.at( yValues.size() - 1 ) + ( yValues.at( yValues.size() - 1 ) - yValues.at( yValues.size() - 2 ) ) );
    //xValues.push_back( yvalue );

    //GridType zvalue ( zValues.at( zValues.size() - 1 ) + ( zValues.at( zValues.size() - 1 ) - zValues.at( zValues.size() - 2 ) ) );
    //xValues.push_back( zvalue );

    // Grid size in bytes
    Usul::Types::Uint32 sizeInBytes( count * sizeof( Float32 ) );

    // write the x grid    
    ofs->write ( reinterpret_cast< char* > ( &xValues[0] ), sizeInBytes );

    // write the y grid    
    ofs->write ( reinterpret_cast< char* > ( &yValues[0] ), sizeInBytes );

    // write the z grid    
    ofs->write ( reinterpret_cast< char* > ( &zValues[0] ), sizeInBytes );
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

  float denominator ( static_cast< float > ( _dimensions[1] - 1 ) );

  // Write out the actual elements, which are hexahedral cells
  for( unsigned int j = 0; j < _dimensions[1] - 1; ++j )
  {
    float numerator ( static_cast< float >( j ) );

    // Feedback
    std::cout << "\rWriting Element Nodes -- " << ( ( numerator * 100 ) / denominator ) << "% complete" << std::flush;

    for( unsigned int k = 0; k < _dimensions[2] - 1; ++k )
    {
      for( unsigned int i = 0; i < _dimensions[0] - 1; ++i )
      {
        // vertex 1
        Uint32 v1 ( _indexGrid.at( j ).at( k ).at( i ) );

        // vertex 2
        Uint32 v2 ( _indexGrid.at( j ).at( k ).at( i + 1 ) );

        // vertex 3
        Uint32 v3 ( _indexGrid.at( j + 1 ).at( k ).at( i ) );

        // vertex 4
        Uint32 v4 ( _indexGrid.at( j + 1 ).at( k ).at( i + 1 ) );

        // vertex 5
        Uint32 v5 ( _indexGrid.at( j ).at( k + 1 ).at( i ) );

        // vertex 6
        Uint32 v6 ( _indexGrid.at( j ).at( k + 1 ).at( i + 1 ) );

        // vertex 7
        Uint32 v7 ( _indexGrid.at( j + 1 ).at( k + 1 ).at( i ) );

        // vertex 8
        Uint32 v8 ( _indexGrid.at( j + 1 ).at( k + 1 ).at( i + 1 ) );
        
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

