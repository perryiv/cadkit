
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "Usul/System/Socket.h"

#include <sstream>
#include <stdexcept>

#ifndef _MSC_VER 
#include <errno.h>
#include <sys/types.h>
#include <cstring>
#endif

#if defined ( _MSC_VER ) && _MSC_VER >= 1400
#include <errno.h>
#endif

using namespace Usul::System;

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

Socket::Socket()
{
  this->_init();
}

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

Socket::Socket( const std::string& hostname, int port )
{
  this->_init();

  if( !this->connect( hostname, port ) )
  {
    std::ostringstream os;
    os << "Failed to connect to "  << hostname << " using port " << port << std::endl;
    throw std::runtime_error ( os.str() );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

Socket::~Socket()
{
#if defined(_MSC_VER)
  shutdown(_sd, SD_SEND);
  closesocket(_sd);
#else
  shutdown(_sd, 1);
  close(_sd);
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize the socket
//
///////////////////////////////////////////////////////////////////////////////

void Socket::_init()
{
#if defined(_MSC_VER)
  //make sure windows sockets are initialized

  WSADATA wsdata;
  if( WSAStartup(MAKEWORD(1,1), &wsdata) < 0 ) 
    throw std::runtime_error ( "Failed to initialize socket." );
#endif

  if ((_sd = socket(PF_INET, SOCK_STREAM, 0)) == -1)
    throw std::runtime_error ( "Failed to open socket." );

}


///////////////////////////////////////////////////////////////////////////////
//
//  Connect to host on port
//
///////////////////////////////////////////////////////////////////////////////

bool Socket::connect( const std::string& hostname, int port )
{
  char address[1024];
  struct hostent *host;

  host = gethostbyname( hostname.c_str() );
  if ( 0x0 == host ) 
    return false;

  sprintf(address, "%d.%d.%d.%d",
    (unsigned char) host->h_addr_list[0][0],
    (unsigned char) host->h_addr_list[0][1],
    (unsigned char) host->h_addr_list[0][2],
    (unsigned char) host->h_addr_list[0][3]);

  memset(&(_addr), 0, sizeof(_addr));

  _addr.sin_family = PF_INET;
  _addr.sin_addr.s_addr = inet_addr(address);
  _addr.sin_port = htons(port);  

  return ( ::connect(_sd, (struct sockaddr *) &_addr, sizeof(_addr)) != -1 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Bind socket to given port number
//
///////////////////////////////////////////////////////////////////////////////

int Socket::bind(int port) 
{
  memset(&(_addr), 0, sizeof(_addr)); 
  _addr.sin_family = PF_INET;
  _addr.sin_port = htons(port);

  return ::bind(_sd, (struct sockaddr *) &_addr, sizeof(_addr));
}


///////////////////////////////////////////////////////////////////////////////
//
//  Listen on port for data
//
///////////////////////////////////////////////////////////////////////////////

int Socket::listen() 
{
  return ::listen(_sd, 5);
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read data from socket
//
///////////////////////////////////////////////////////////////////////////////

int Socket::_read(char *buf, int len) const 
{
#if defined(_MSC_VER)
  return ::recv( _sd, buf, len, 0 ); // windows lacks the read() call
#else
  return ::read(_sd, buf, len);
#endif

}


///////////////////////////////////////////////////////////////////////////////
//
//  Write data to socket
//
///////////////////////////////////////////////////////////////////////////////

int Socket::_write(const char *buf, int len)  const
{
#if defined(_MSC_VER)
  return ::send( _sd, buf, len, 0 );  // windows lacks the write() call
#else
  return ::write(_sd, buf, len);
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Check for specified time to see if socket is readable.
//
///////////////////////////////////////////////////////////////////////////////

bool Socket::_selectRead( int seconds ) const
{
  fd_set rfd;
  struct timeval tv;
  int rc;
 
  //clear file descriptor set
  FD_ZERO(&rfd);

  //set the file descriptor field to _sd
  FD_SET(_sd, &rfd);

  memset((void *)&tv, 0, sizeof(struct timeval));
  tv.tv_sec = seconds;
  do {
    rc = select(_sd + 1, &rfd, 0x0, 0x0, &tv);
  } while (rc < 0 && errno == EINTR);
  
  //If FD_ISSET returns zero, then _sd is not ready for reading
  return (FD_ISSET(_sd, &rfd ) != 0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Check for specified time to see if socket is writable
//
///////////////////////////////////////////////////////////////////////////////

bool Socket::_selectWrite( int seconds ) const
{
  fd_set wfd;
  struct timeval tv;
  int rc;
 
  //clear file descriptor set
  FD_ZERO(&wfd);

  //set the file descriptor field to _sd
  FD_SET(_sd, &wfd);

  memset((void *)&tv, 0, sizeof(struct timeval));
  tv.tv_sec = seconds;
  do {
    rc = select(_sd + 1, 0x0, &wfd, 0x0, &tv);
  } while (rc < 0 && errno == EINTR);
  
  //If FD_ISSET returns zero, then _sd is not ready for writing
  return (FD_ISSET(_sd, &wfd ) != 0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read from the socket
//
///////////////////////////////////////////////////////////////////////////////

bool Socket::read ( Buffer &buf, bool wait, int seconds ) const
{
  if( wait )
    if ( !this->_selectRead( seconds )  )
      return false;

  int numRead( 0 );
  int numLeft( buf.requested() );
  char *buffer ( buf.buffer() );

  if( !this->_canRead() )
    return false;

  while ( numLeft > 0 )
  {
    numRead = this->_read( buffer, numLeft );
    if( numRead < 0 )
      return false;
    numLeft -= numRead;
    buffer += numRead;
    buf.size( buf.size() + numRead );
  }

  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write to the socket
//
///////////////////////////////////////////////////////////////////////////////

bool Socket::write ( Buffer &buf, bool wait, int seconds ) const
{
  if( wait )
    if ( !this->_selectWrite( seconds ) )
      return false;

  int numRead( 0 );
  int numLeft( buf.requested() );
  char *buffer ( buf.buffer() );

  while ( numLeft > 0 )
  {
    numRead = this->_write( buffer, numLeft );
    if ( numRead < 0 ) 
      return false;
    numLeft -= numRead;
    buffer += numRead;

    buf.size( buf.size() + numRead );
  }


  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is there data to be read on the socket?
//
///////////////////////////////////////////////////////////////////////////////

bool Socket::_canRead() const 
{
  char c;
  int data;

  data = ::recv(_sd, &c, 1, MSG_PEEK);

  if( data > 0 )
    return true;
  return false;

}

