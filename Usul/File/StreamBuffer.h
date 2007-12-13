
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry Miller
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Stream buffer class with progress notification.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_FILE_STREAM_BUFFER_CLASS_
#define _USUL_FILE_STREAM_BUFFER_CLASS_

#include "Usul/Trace/Trace.h"

#include <string>
#include <fstream>
#include <stdexcept>

namespace Usul {
namespace File {


class StreamBuffer: public std::basic_filebuf < char, std::char_traits < char > >
{
public:

  typedef std::char_traits < char > CharTraits;
  typedef std::basic_filebuf < char, CharTraits > BaseClass;
  typedef BaseClass::int_type IntType;

  explicit StreamBuffer ( const std::string &file ) : BaseClass(), _file ( file ), _size ( 0 ), _count ( 0 ), _callback ( 0x0 )
  {
    if ( 0x0 == this->open ( file.c_str(), std::ios_base::in | std::ios_base::binary ) )
      throw std::runtime_error ( std::string ( "Error 2836857267: Could not open file: " ) + file );

    _size = static_cast < unsigned long > ( this->pubseekoff ( 0, std::ios_base::end, std::ios_base::in ) );
    this->pubseekoff ( 0, std::ios_base::beg, std::ios_base::in );
  }

  struct Callback
  {
    virtual void operator () ( const std::string &file, unsigned long bytes, unsigned long total ) = 0;
    virtual ~Callback () {};
  };

  void callback ( Callback *cb )
  {
    _callback = cb;
  }

protected:

  // This funcion needs to be used on linux.  TODO: Find out if it also works on windows.
#ifndef _MSC_VER
  virtual IntType underflow()
  {
    USUL_TRACE_SCOPE;

    _count += ( this->egptr() - this->eback() );
    if ( 0x0 != _callback )
      (*_callback) ( _file, _count, _size );

    return BaseClass::underflow();
  }
#endif

  virtual IntType uflow()
  {
    USUL_TRACE_SCOPE;

    IntType v = BaseClass::uflow();
    _count += ( this->egptr() - this->gptr() );
    if ( 0x0 != _callback )
      (*_callback) ( _file, _count, _size );
    return v;
  }

private:

  std::string _file;
  unsigned long _size;
  unsigned long _count;
  Callback *_callback;
};


} // File
} // Usul


#endif // _USUL_FILE_STREAM_BUFFER_CLASS_
