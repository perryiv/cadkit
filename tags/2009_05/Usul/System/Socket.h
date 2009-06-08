
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Socket Abstration
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_SYSTEM_SOCKET_H__
#define __USUL_SYSTEM_SOCKET_H__

#include "Usul/Export/Export.h"

// Disable deprecated warning in Visual Studio 8 for sprintf
#if defined ( _MSC_VER ) && _MSC_VER >= 1400
#pragma warning ( disable : 4996 )
#endif

#if defined(_MSC_VER) 
#include <winsock2.h>
#else
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <sys/socket.h>
#endif

#include <string>

namespace Usul {
namespace System {


struct Buffer
{
  Buffer ( unsigned int size ) :
  _requested( size ),
  _received( 0 ),
  _buffer ( new char[size] ),
  _success( false )
  {
  }
  Buffer ( unsigned int size, char *buf, bool copy = true ) :
  _requested( size ),
  _received( 0 ),
  _buffer ( 0x0 ),
  _success( false )
  {
    if( copy )
    {
      _buffer = new char[size];
      std::copy( buf, buf + size, _buffer );
    }
    else
      _buffer = buf;
  }


  ~Buffer() { }

  unsigned int size() const { return _received; }
  void size( unsigned int s )  { _received = s; }

  unsigned int requested() const { return _requested; }

  char* buffer() const { return _buffer; }
  void buffer ( char* b ) { _buffer = b; }

private:

  unsigned int _requested;
  unsigned int _received;
  char *_buffer;
  bool _success;
};

class USUL_EXPORT Socket
{
public:
  
  Socket();
  Socket( const std::string& hostname, int port );
  ~Socket();

  bool connect(const std::string& hostname, int port);
  bool disconnect();


  int bind(int port);
  int listen();

  bool read ( Buffer &buf, bool wait = false, int seconds = 5 ) const;
  bool write ( Buffer &buf, bool wait = false, int seconds = 5 ) const;

  bool canRead() const;

protected:

  int _read(char *buf, int len) const;
  int _write(const char *buf, int len) const;

  bool _selectRead( int seconds ) const;
  bool _selectWrite ( int seconts ) const;

  bool _canRead() const;

private:
  void _init();

  struct sockaddr_in _addr; // address of socket ( provided by bind() )
  int _addrlen;             // size of the _addr struct
  int _sd;                  // socket file descriptor
};

} //namespace System
} //namespace Usul

#endif // __USUL_SYSTEM_SOCKET_H__

