
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

  if( this->connect( hostname, port ) < 0 )
  {
    std::ostringstream os;
    os << "Failed to connect to"  << hostname << " using port " << port << std::endl;
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

int Socket::connect( const std::string& hostname, int port )
{
  char address[1024];
  struct hostent *host;

  host = gethostbyname( hostname.c_str() );
  if ( 0x0 == host ) 
    return -1;

  sprintf(address, "%d.%d.%d.%d",
    (unsigned char) host->h_addr_list[0][0],
    (unsigned char) host->h_addr_list[0][1],
    (unsigned char) host->h_addr_list[0][2],
    (unsigned char) host->h_addr_list[0][3]);

  memset(&(_addr), 0, sizeof(_addr));

  _addr.sin_family = PF_INET;
  _addr.sin_addr.s_addr = inet_addr(address);
  _addr.sin_port = htons(port);  

  return ::connect(_sd, (struct sockaddr *) &_addr, sizeof(_addr));
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

int Socket::read(void *buf, int len) const 
{
#if defined(_MSC_VER)
  return ::recv(_sd, (char*) buf, len, 0); // windows lacks the read() call
#else
  return ::read(_sd, buf, len);
#endif

}


///////////////////////////////////////////////////////////////////////////////
//
//  Write data to socket
//
///////////////////////////////////////////////////////////////////////////////

int Socket::write(const void *buf, int len)  const
{
#if defined(_MSC_VER)
  return ::send(_sd, (const char*) buf, len, 0);  // windows lacks the write() call
#else
  return ::write(_sd, buf, len);
#endif
}

int Socket::selectRead( int seconds )
{
  fd_set rfd;
  struct timeval tv;
  int rc;
 
  FD_ZERO(&rfd);
  FD_SET(_sd, &rfd);
  memset((void *)&tv, 0, sizeof(struct timeval));
  tv.tv_sec = seconds;
  do {
    rc = select(_sd + 1, &rfd, 0x0, 0x0, &tv);
  } while (rc < 0 && errno == EINTR);
  return rc;
}

int Socket::selectWrite( int seconds ) 
{
  fd_set wfd;
  struct timeval tv;
  int rc;
 
  FD_ZERO(&wfd);
  FD_SET(_sd, &wfd);
  memset((void *)&tv, 0, sizeof(struct timeval));
  tv.tv_sec = seconds;
  do {
    rc = select(_sd + 1, 0x0, &wfd, 0x0, &tv);
  } while (rc < 0 && errno == EINTR);
  return rc;
}
