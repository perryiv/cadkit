

#ifndef __IMD_HEADER_H__
#define __IMD_HEADER_H__

#include "Usul/Types/Types.h"


namespace OsgFox {
namespace Plugins {
namespace IMD {

typedef Usul::Types::Int32 int32;

///////////////////////////////////////////////////////////////////////////////
//
//  IMD communication protocol message header structure
//
///////////////////////////////////////////////////////////////////////////////
struct IMDheader
{
  IMDheader () { }
  IMDheader ( IMD::IMDType type, int32 length ) :
  _type   ( ::htonl ( type   ) ),
  _length ( ::htonl ( length ) )
  { }

  IMDheader( char *buf )
  {
    ::memcpy( &_type, buf, 4 );
    ::memcpy( &_length, buf + 4, 4 );
  }

  //put data into network format
  void network ()
  {
    this->_type   = ::htonl ( _type );
    this->_length = ::htonl ( _length );
  }

  //put data into host format
  void host ()
  {
    this->_type   = ::ntohl ( _type );
    this->_length = ::ntohl ( _length );
  }


  void type( int32 type ) { _type = type; }
  int32 type() const { return _type; }

  void length( int32 length ) { _length = length; }
  int32 length() const { return _length; }

  void fill( char *buf ) 
  { 
    //copy type
    ::memcpy( buf, &_type, 4 );
    //copy length
    ::memcpy( buf + 4, &_length, 4 );
  }

private:
  int32 _type;
  int32 _length;
};

}
}
}

#endif //__IMD_HEADER_H__

