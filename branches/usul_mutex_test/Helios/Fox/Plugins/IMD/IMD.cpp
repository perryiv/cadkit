
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "IMD.h"
#include "IMDHeader.h"

#include "Usul/Endian/Endian.h"

#include <cstring>
#include <cerrno>
#include <cstdlib>

using namespace OsgFox::Plugins::IMD;

///////////////////////////////////////////////////////////////////////////////
//
//  IMD constructor
//
///////////////////////////////////////////////////////////////////////////////

IMD::IMD( const std::string& host, unsigned int port ) :
_headerSize( 8 ),
_version( 2 ),
_socket(  ),
_endianism( UNKNOWN )
{
  _socket.connect( host, port );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write header to socket
//
///////////////////////////////////////////////////////////////////////////////

bool IMD::_writeHeader ( Usul::System::Buffer& buf )
{
  if( ! _socket.write( buf ) )
    return false;

  if( buf.size() != _headerSize )
    return false;

  return true;
}
 
///////////////////////////////////////////////////////////////////////////////
//
//  Send disconnect message
//
///////////////////////////////////////////////////////////////////////////////

bool IMD::disconnect() 
{
  IMDheader header ( IMD::DISCONNECT, 0 );

  char *buf = new char[_headerSize];
  header.fill( buf );

  Usul::System::Buffer buffer ( _headerSize, buf);
  
  return this->_writeHeader( buffer );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Send pause message
//
///////////////////////////////////////////////////////////////////////////////

bool IMD::pause() 
{
  IMDheader header ( IMD::PAUSE, 0 );

  char *buf = new char[_headerSize];
  header.fill( buf );

  Usul::System::Buffer buffer ( _headerSize, buf );

  return this->_writeHeader( buffer );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Send kill message
//
///////////////////////////////////////////////////////////////////////////////

bool IMD::kill() 
{
  IMDheader header ( IMD::KILL, 0 );

  char *buf = new char[_headerSize];
  header.fill( buf );

  Usul::System::Buffer buffer ( _headerSize, buf );

  return this->_writeHeader( buffer );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Send go message
//
///////////////////////////////////////////////////////////////////////////////

bool IMD::_go( ) 
{
  IMDheader header ( IMD::GO, 0 );

  char *buf = new char[_headerSize];
  header.fill( buf );

  Usul::System::Buffer buffer ( _headerSize, buf );

  bool value ( true );
  if( !_socket.write( buffer ) )
    value = false;

  if( buffer.size() != _headerSize )
    value = false;

  delete [] buf;

  return value;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Send handshake
//
///////////////////////////////////////////////////////////////////////////////

bool IMD::handshake() 
{
  IMDheader header ( IMD::HANDSHAKE, 1 );
  header.length ( _version );   // Don't swap the length

  char *buf = new char[_headerSize];
  header.fill( buf );

  Usul::System::Buffer buffer ( _headerSize, buf );

  return this->_writeHeader( buffer );
}


///////////////////////////////////////////////////////////////////////////////
//
// Set the transfer rate
//
///////////////////////////////////////////////////////////////////////////////

bool IMD::trate(int32 rate) 
{
  IMDheader header ( IMD::TRATE, rate );

  char *buf = new char[_headerSize];
  header.fill( buf );

  Usul::System::Buffer buffer ( _headerSize, buf );

  return this->_writeHeader( buffer );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Send MDcomm data
//
///////////////////////////////////////////////////////////////////////////////

bool IMD::sendMDcomm(int32 n,const int32 *indices,const float *forces) 
{
  int32 size = _headerSize + 16*n;
  size *= sizeof ( char );
  char *buf = (char *) malloc( size ); 

  IMDheader header ( IMD::MDCOMM, n );
  header.network();
  header.fill( buf );

  ::memcpy( buf+_headerSize,     indices, 4*n);
  ::memcpy( buf+_headerSize+4*n, forces, 12*n);

  Usul::System::Buffer buffer ( size, buf );

  bool value ( true );
  if( !_socket.write( buffer ) )
    value = false;

  if( buffer.size() != size )
    value = false;

  free(buf);

  return value;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Send the energies
//
///////////////////////////////////////////////////////////////////////////////

bool IMD::sendEnergies(const Energies *energies) 
{
  const unsigned int energySize ( sizeof ( Energies ) );

  int32 size ( _headerSize + energySize * sizeof ( char ) );
  char *buf = (char *) malloc( size );

  IMDheader header ( IMD::ENERGIES, 1 );
  header.network();
  header.fill( buf );

  ::memcpy(buf + _headerSize, energies, energySize );

  Usul::System::Buffer buffer ( size, buf );

  bool value ( true );

  if( !_socket.write( buffer ) )
    value = false;

  if( buffer.size() != size )
    value = false;

  free(buf);

  return value;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Send the coordinates
//
///////////////////////////////////////////////////////////////////////////////

bool IMD::sendCoords(int32 n, const float *coords) 
{
  const unsigned int coordSize ( 12 * n );
  int32 size ( _headerSize + coordSize * sizeof ( char ) );

  char *buf = (char *) malloc( size ); 

  IMDheader header ( IMD::FCOORDS, n );
  header.network();
  header.fill( buf );
  
  ::memcpy( buf + _headerSize, coords, coordSize );
  
  Usul::System::Buffer buffer ( size, buf );

  bool value ( true );
  if( !_socket.write( buffer ) )
    value = false;

  if( buffer.size() != size )
    value = false;

  free( buf );

  return value;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Receive the header.  Do not swap the length
//
///////////////////////////////////////////////////////////////////////////////

IMD::IMDType IMD::_recv_header_nolengthswap(int32 &length) 
{
  char *buf = new char[_headerSize];

  Usul::System::Buffer buffer ( _headerSize, buf );

  // Wait up to 5 seconds for the handshake
  if( !_socket.read( buffer, true ) )
    return IMD::IOERROR;

  if( buffer.size() != _headerSize )
    return IMD::IOERROR;

  IMDheader header ( buffer.buffer() );

  length = header.length();
  
  header.host();
  return (IMD::IMDType) header.type(); 
}


///////////////////////////////////////////////////////////////////////////////
//
//  Recieve the header
//
///////////////////////////////////////////////////////////////////////////////

IMD::IMDType IMD::receiveHeader(int32 &length) 
{
  char *buf = new char[_headerSize];

  Usul::System::Buffer buffer ( _headerSize, buf);

  if( !_socket.read( buffer, true ) )
    return IMD::IOERROR;

  if( buffer.size() != _headerSize )
    return IMD::IOERROR;

  IMDheader header ( buffer.buffer() );

  header.host();
  length = header.length();
  return (IMD::IMDType) header.type(); 
}


///////////////////////////////////////////////////////////////////////////////
//
//  Recieve the handshake
//
///////////////////////////////////////////////////////////////////////////////

bool IMD::receiveHandshake() 
{
  int32 buf;

  // Check to see that a valid handshake was received
  IMDType type ( this->_recv_header_nolengthswap( buf ) );
  if (type != IMD::HANDSHAKE) 
    return false;

  // buf contains a unswaped int that contains the version
  // If we reconize it with out byte swapping that means that
  // the two machines have the same endianism.
  if ( buf == _version ) 
  {
    if ( _go(  ) ) 
    {
      _endianism = SAME;
      return true;
    }
    return false;
  }

  Usul::Endian::reverseBytes ( buf );
  if ( buf == _version ) 
  {
    if ( _go(  ) ) 
    {
      _endianism = OPPOSITE;
      return true;
    }
  }
  
  // We failed to determine relative endianism
  return false; 
}


///////////////////////////////////////////////////////////////////////////////
//
//  Recieve MDcomm data
//
///////////////////////////////////////////////////////////////////////////////

bool IMD::receiveMDcomm(int32 n, int32 *indices, float *forces) 
{
  const unsigned int indicesSize ( 4 * n );
  const unsigned int forcesSize  ( 12 * n );
  Usul::System::Buffer indicesBuf ( indicesSize,  (char *)indices );
  Usul::System::Buffer forcesBuf  ( forcesSize,   (char *)forces );

  if( !_socket.read( indicesBuf ) )
    return false;

  if( indicesBuf.size() != indicesSize )
    return false;
  
  if (!_socket.read( forcesBuf ) )
    return false;

  if( forcesBuf.size() != forcesSize )
    return false;
  
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Receive the energies
//
///////////////////////////////////////////////////////////////////////////////

bool IMD::receiveEnergies(Energies *energies) const
{
  const unsigned int size ( sizeof(Energies) );

  Usul::System::Buffer buf ( size, (char *)energies, false );

  if( !_socket.read( buf ) )
    return false;

  if( buf.size() != size )
    return false;

  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Receive the coordinates
//
///////////////////////////////////////////////////////////////////////////////

bool IMD::receiveCoords(int32 n, float *coords) const
{
  const unsigned int size ( 12 * n );

  Usul::System::Buffer buf ( size, (char *) coords, false );

  if( !_socket.read( buf ) )
    return false;

  if( buf.size() != size )
    return false;

  return true;

}

