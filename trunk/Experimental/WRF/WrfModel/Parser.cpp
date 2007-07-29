
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Parser.h"

#include "OsgTools/Images/Matrix.h"
#include "Usul/Exceptions/Thrower.h"

#include <stdexcept>

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Parser::Parser(  ) : 
  _filename       ( ),
  _fp             ( 0x0 ),
  _xSize          ( 0 ),
  _ySize          ( 0 ),
  _zSize          ( 0 ),
  _numTimesteps   ( 0 ),
  _numChannels    ( 0 )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Parser::Parser( const std::string & filename ) : 
  _filename       ( filename ),
  _fp             ( 0x0 ),
  _xSize          ( 0 ),
  _ySize          ( 0 ),
  _zSize          ( 0 ),
  _numTimesteps   ( 0 ),
  _numChannels    ( 0 )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Copy Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Parser::Parser( const Parser& parser ) : 
  _filename       ( parser._filename ),
  _fp             ( 0x0 ),
  _xSize          ( parser._xSize ),
  _ySize          ( parser._ySize ),
  _zSize          ( parser._zSize ),
  _numTimesteps   ( parser._numTimesteps ),
  _numChannels    ( parser._numChannels )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Parser::~Parser ()
{
  if ( 0x0 != _fp )
  {
    ::fclose ( _fp );
    _fp = 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the filename.
//
///////////////////////////////////////////////////////////////////////////////

void Parser::filename ( const std::string& filename )
{
  _filename = filename;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filename.
//
///////////////////////////////////////////////////////////////////////////////

const std::string& Parser::filename () const
{
  return _filename;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the sizes of the data.
//
///////////////////////////////////////////////////////////////////////////////

void Parser::setSizes ( unsigned int x, unsigned int y, unsigned int z )
{
  _xSize = x;
  _ySize = y;
  _zSize = z;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the number of timesteps.
//
///////////////////////////////////////////////////////////////////////////////

void Parser::timesteps ( unsigned int t )
{
  _numTimesteps = t;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the number of timesteps.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Parser::timesteps () const
{
  return _numTimesteps;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the number of channels.
//
///////////////////////////////////////////////////////////////////////////////

void Parser::channels ( unsigned int c )
{
  _numChannels = c;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the number of channels.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Parser::channels () const
{
  return _numChannels;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the requested data.
//
///////////////////////////////////////////////////////////////////////////////

void Parser::data ( Data& data, unsigned int timestep, unsigned int channel )
{
  // Open a file.
  if ( 0x0 == _fp )
    _fp = ::fopen ( _filename.c_str(), "rb" );

  if ( 0x0 == _fp )
    Usul::Exceptions::Thrower < std::runtime_error > ( "Error 2490086597: Could not open file: ", _filename );

  unsigned int sliceSize ( _xSize * _ySize );

  /// The file has a 4 byte footer and header per 2D slice.
  unsigned int sliceSizeBytes ( sliceSize * sizeof ( Usul::Types::Float32 ) + 8 );

  // Calculate the size of a channel.
  unsigned int channelSizeBytes ( sliceSizeBytes * _zSize );

  // Calculate the size of one timestep.
  unsigned int timestepSize ( channelSizeBytes * _numChannels + ( sizeof ( Usul::Types::Float32 ) * sliceSize + 8 ) );

  // Calculate the file offset.
  Usul::Types::Uint64 offset ( ( timestepSize * timestep ) + ( channelSizeBytes * channel ) );

  // Jump to the correct location.
#ifdef _MSC_VER
  Usul::Types::Uint64 result ( ::_fseeki64 ( _fp, offset, SEEK_SET ) );
#else
  Usul::Types::Uint64 result ( ::fseek ( _fp, offset, SEEK_SET ) );
#endif

  if ( result != 0 )
    Usul::Exceptions::Thrower < std::runtime_error > ( "Error 1556671836: Could not seek in file: ", _filename, " to location: ", offset );

  // Make enough room.
  data.resize ( sliceSize * _zSize );

  for ( unsigned int z = 0; z < _zSize; ++z )
  {
    unsigned int header ( 0 );
    unsigned int footer ( 0 );

    // Read the header.
    ::fread ( &header, sizeof ( unsigned int ), 1, _fp );

    unsigned int sliceOffset ( sliceSize * z );

    // Read.
    ::fread ( &data.front() + sliceOffset, sizeof ( Usul::Types::Float32 ), sliceSize, _fp );

    // Read the footer.
    ::fread ( &footer, sizeof ( unsigned int ), 1, _fp );
  }
}
