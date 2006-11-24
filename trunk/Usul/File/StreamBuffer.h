
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

#include <string>
#include <fstream>

namespace Usul {
namespace File {


class StreamBuffer: public std::basic_filebuf < char, std::char_traits < char > >
{
public:

  typedef std::char_traits < char > CharTraits;
  typedef std::basic_filebuf < char, CharTraits > BaseClass;
  typedef BaseClass::int_type IntType;

  explicit StreamBuffer ( const std::string &file ) : BaseClass(), _size ( 0 ), _callback ( 0x0 )
  {
    if ( 0x0 == this->open ( file.c_str(), std::ios_base::in | std::ios_base::binary ) )
      throw std::runtime_error ( std::string ( "Error 2836857267: Could not open file: " ) + file );

    _size = static_cast < IntType > ( this->pubseekoff ( 0, std::ios_base::end, std::ios_base::in ) );
    this->pubseekoff ( 0, std::ios_base::beg, std::ios_base::in );
  }

  struct Callback
  {
    virtual void operator () () = 0;
  };

  void callback ( Callback *cb )
  {
    _callback = cb;
  }

protected:

  virtual IntType uflow()
  {
    IntType v = BaseClass::uflow();
    IntType count = this->egptr() - this->gptr();
    if ( 0x0 != _callback )
      (*_callback)();
    return v;
  }

private:

  IntType _size;
  Callback *_callback;
};


} // File
} // Usul


#endif // _USUL_FILE_STREAM_BUFFER_CLASS_
