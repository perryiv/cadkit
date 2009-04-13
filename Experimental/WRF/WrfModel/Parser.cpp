
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifdef __linux
#define __USE_LARGEFILE64
#endif

#include "Experimental/WRF/WrfModel/Parser.h"

#include "Usul/Exceptions/Thrower.h"
#include "Usul/Strings/Format.h"
#include "Usul/System/LastError.h"

#include <stdexcept>

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Parser::Parser() : 
  _filename       (),
  _fp             ( 0x0 ),
  _xSize          ( 0 ),
  _ySize          ( 0 ),
  _zSize          ( 0 ),
  _numTimesteps   ( 0 ),
  _numChannels    ( 0 ),
  _numFields2D    ( 0 ),
  _headers        ( false )
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
  _numChannels    ( 0 ),
  _numFields2D    ( 0 ),
  _headers        ( true )
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
  _numChannels    ( parser._numChannels ),
  _numFields2D    ( parser._numFields2D ),
  _headers        ( parser._headers )
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
//  Assignment.
//
///////////////////////////////////////////////////////////////////////////////

Parser& Parser::operator = ( const Parser& parser )
{
  // Set data members.  Do not set file pointer.  Will get opened with data is read.
  // This is so we can have many threads reading from the file without blocking when reading.
  _filename     = parser._filename;
  _fp           = 0x0;
  _xSize        = parser._xSize;
  _ySize        = parser._ySize;
  _zSize        = parser._zSize;
  _numTimesteps = parser._numTimesteps;
  _numChannels  = parser._numChannels;
  _numFields2D  = parser._numFields2D;
  _headers      = parser._headers;

  return *this;
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
//  Set the nubmer of 2D fields.
//
///////////////////////////////////////////////////////////////////////////////

void Parser::numFields2D ( unsigned int value )
{
  _numFields2D = value;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the nubmer of 2D fields.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Parser::numFields2D () const
{
  return _numFields2D;
}

///////////////////////////////////////////////////////////////////////////////
//
//  Set if the file has headers.
//
///////////////////////////////////////////////////////////////////////////////

void Parser::headers ( bool b )
{
  _headers = b;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get if the file has headers
//
///////////////////////////////////////////////////////////////////////////////

bool Parser::headers () const
{
  return _headers;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the requested data.
//
///////////////////////////////////////////////////////////////////////////////

void Parser::data ( Data& data, unsigned int timestep, unsigned int channel )
{
  // Open the file.
  this->_open ();

  const unsigned int sliceSize ( _xSize * _ySize );

  /// The file has a 4 byte footer and header per 2D slice.
  const unsigned int sliceSizeBytes ( sliceSize * sizeof ( Usul::Types::Float32 ) + this->_headerSize () );

  // Calculate the size of a channel.
  const Usul::Types::Uint64 channelSizeBytes ( sliceSizeBytes * _zSize );

  // Calculate the size of one timestep.
  const Usul::Types::Uint64 timestepSize ( ( channelSizeBytes * _numChannels ) + ( sliceSizeBytes * _numFields2D ) );

  // Calculate the offset to the given timestep.
  const Usul::Types::Uint64 timestepOffset ( timestepSize * static_cast < Usul::Types::Uint64 > ( timestep ) );

  // Calculate the offset from the start of the timestep to the given channel.
  const Usul::Types::Uint64 channelOffset ( channelSizeBytes * static_cast < Usul::Types::Uint64 > ( channel ) );

  // Calculate the file offset.
  const Usul::Types::Uint64 offset ( timestepOffset + channelOffset );

  // Jump to the correct location.
  this->_seek ( offset );

  // Make enough room.
  data.resize ( sliceSize * _zSize );
  
  // Read all the slices.
  for ( unsigned int z = 0; z < _zSize; ++z )
  {
    // Offset for our current slice.
    const unsigned int sliceOffset ( sliceSize * z );

    // Read a slice.
    this->_readSlice ( &data.front() + sliceOffset );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the topography for the data set.
//
///////////////////////////////////////////////////////////////////////////////

void Parser::field2D ( Data& data, unsigned int i )
{
  // Open the file.
  this->_open ();

  const unsigned int sliceSize ( _xSize * _ySize );

  /// The file has a 4 byte footer and header per 2D slice.
  const unsigned int sliceSizeBytes ( sliceSize * sizeof ( Usul::Types::Float32 ) + this->_headerSize () );

  // Calculate the size of a channel.
  const unsigned int channelSizeBytes ( sliceSizeBytes * _zSize );

  // Calculate the file offset.
  const Usul::Types::Uint64 offset ( ( channelSizeBytes * _numChannels ) + ( sliceSizeBytes * i ) );

  // Jump to the correct location.
  this->_seek ( offset );

  // Make enough room.
  data.resize ( sliceSize );

  // Read the slice.
  this->_readSlice ( &data.front() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Open the file.
//
///////////////////////////////////////////////////////////////////////////////

void Parser::_open ()
{
  Usul::System::LastError::init();

  if ( 0x0 == _fp )
  {
#ifdef __linux
    _fp = ::fopen64 ( _filename.c_str(), "rb" );
#else
    _fp = ::fopen ( _filename.c_str(), "rb" );
#endif
  }

  if ( 0x0 == _fp )
  {
    const std::string error ( ( true == Usul::System::LastError::has() ) ? Usul::System::LastError::message() : "" );
    const std::string message ( Usul::Strings::format ( "Error 2490086597: Could not open file: ", _filename, ". ", error ) );
    throw std::runtime_error ( message );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Seek.
//
///////////////////////////////////////////////////////////////////////////////

void Parser::_seek ( Usul::Types::Int64 offset )
{
  this->_seek ( offset, SEEK_SET );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Seek.
//
///////////////////////////////////////////////////////////////////////////////

void Parser::_seek ( Usul::Types::Int64 offset, int whence )
{
#ifdef _MSC_VER

  Usul::Types::Int64 result ( ::_fseeki64 ( _fp, offset, whence ) );

#elif __APPLE__

  // 64 bit offset for seeking in the file.
  // See: http://lists.apple.com/archives/scitech/2005/Jan/msg00271.html and http://www.opengroup.org/onlinepubs/007908799/xsh/fseek.html
  Usul::Types::Int32 result ( ::fseeko ( _fp, offset, whence ) );

#else // __linux

  Usul::Types::Int32 result ( ::fseeko64 ( _fp, offset, whence ) );

#endif

  if ( result != 0 )
    Usul::Exceptions::Thrower < std::runtime_error > ( "Error 1556671836: Could not seek in file: ", _filename, " to location: ", offset );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read a 2D slice.
//
///////////////////////////////////////////////////////////////////////////////

void Parser::_readSlice ( Data::value_type* buffer )
{
  // How big is a slice?
  const unsigned int sliceSize ( _xSize * _ySize );

  // Header and footer for the slice.
  typedef Usul::Types::Uint32 HeaderFooterType;
  HeaderFooterType header ( 0 );
  HeaderFooterType footer ( 0 );

  // Read the header.
  if ( this->headers () )
    ::fread ( &header, sizeof ( HeaderFooterType ), 1, _fp );

  // Read.
  ::fread ( buffer, sizeof ( Usul::Types::Float32 ), sliceSize, _fp );

  // Read the footer.
  if ( this->headers () )
    ::fread ( &footer, sizeof ( HeaderFooterType ), 1, _fp );

  if ( this->headers () && header != footer )
  {
    Usul::Exceptions::Thrower < std::runtime_error > ( "Error 3056859569: Header and footers are of different sizes." );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the size of a header.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Parser::_headerSize () const
{
  return this->headers () ? 8 : 0;
}
