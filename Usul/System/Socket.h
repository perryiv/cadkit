
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

#if defined(_MSC_VER) 
#include <winsock2.h>
#else
//TODO
#endif

#include <string>

namespace Usul {
namespace System {

class USUL_EXPORT Socket
{
public:
  Socket();
  Socket( const std::string& hostname, int port );
  ~Socket();

  int connect(const std::string& hostname, int port);
  int bind(int port);
  int listen();
  int read(void *buf, int len) const;
  int write(const void *buf, int len) const;

  int selectRead( int seconds );
  int selectWrite ( int seconts );
  
private:
  void _init();

  struct sockaddr_in _addr; // address of socket ( provided by bind() )
  int _addrlen;             // size of the _addr struct
  int _sd;                  // socket file descriptor
};

} //namespace System
} //namespace Usul

#endif // __USUL_SYSTEM_SOCKET_H__

