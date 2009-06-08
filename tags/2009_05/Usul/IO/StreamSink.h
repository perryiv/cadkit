
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  For redirecting stdio and stderr to file, a callback function, or both.
//  Note: Including this header files introduces a dependency on boost.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_STANDARD_STREAM_SINK_CLASS_H_
#define _USUL_STANDARD_STREAM_SINK_CLASS_H_

#include "Usul/File/Remove.h"
#include "Usul/Interfaces/IStreamListener.h"
#include "Usul/Predicates/FileExists.h"

#include "boost/iostreams/device/null.hpp"
#include "boost/iostreams/device/file.hpp"
#include "boost/iostreams/tee.hpp"
#include "boost/iostreams/stream.hpp"
#include "boost/shared_ptr.hpp"

#include <iostream>
#include <list>
#include <string>

namespace Usul { namespace IO { namespace Detail { struct TwoSinks; struct CharSink; } } }


///////////////////////////////////////////////////////////////////////////////
//
//  Start of primary namespace.
//
///////////////////////////////////////////////////////////////////////////////

namespace Usul {
namespace IO {


///////////////////////////////////////////////////////////////////////////////
//
//  The stream sink class.
//
///////////////////////////////////////////////////////////////////////////////

class StreamSink
{
public:

  // Typedefs.
  typedef boost::shared_ptr<StreamSink> RefPtr;
  typedef Usul::Interfaces::IStreamListenerChar IStreamListener;
  typedef std::list<IStreamListener::RefPtr> Listeners;

  // Constructor.
  StreamSink ( const std::string &file );

  // Needs to be public for boost::shared_ptr. Use reference counting.
  virtual ~StreamSink();

  // Add/remove a listener.
  void                    add    ( Usul::Interfaces::IUnknown * );
  void                    remove ( Usul::Interfaces::IUnknown * );

  // Flush the stream(s).
  void                    flush();

  // Notify listeners.
  void                    notify ( const char *s, unsigned int n );

  // Helper class to add/remove a listener.
  struct ScopedListener
  {
    template < class T > ScopedListener ( StreamSink &sink, T *t ) : 
      _listener ( t ), 
      _sink ( sink )
    {
      if ( true == _listener.valid() )
      {
        _sink.add ( _listener.get() );
      }
    }
    ~ScopedListener()
    {
      if ( true == _listener.valid() )
      {
        _sink.remove ( _listener.get() );
      }
    }
  private:
    IStreamListener::QueryPtr _listener;
    StreamSink &_sink;
  };

private:

  // No copying.
  StreamSink ( const StreamSink & );
  StreamSink &operator = ( const StreamSink & );

  Detail::TwoSinks *_twoSinks;
  Detail::CharSink *_charSink;
  Listeners _listeners;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Start of details.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail {


///////////////////////////////////////////////////////////////////////////////
//
//  Small class that redirects stdio and stderr to the application.
//
///////////////////////////////////////////////////////////////////////////////

struct CharStreamSink : public boost::iostreams::basic_null_sink < char >
{
  CharStreamSink ( Usul::IO::StreamSink &sink ) : _sink ( sink )
  {
  }
  CharStreamSink ( const CharStreamSink &sink ) : _sink ( sink._sink )
  {
  }
  ~CharStreamSink()
  {
  }
  std::streamsize write ( const char *s, std::streamsize n )
  {
    _sink.notify ( s, static_cast<unsigned int> ( ( n >= 0 ) ? n : 0 ) );
    return n;
  }
private:
  CharStreamSink &operator = ( const CharStreamSink &sink ); // No assignment.
  Usul::IO::StreamSink &_sink;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Local typedefs.
//
///////////////////////////////////////////////////////////////////////////////

typedef boost::iostreams::tee_device < CharStreamSink, boost::iostreams::file_sink > TwoStreamSinks;
typedef boost::iostreams::stream_buffer < TwoStreamSinks > TwoStreamSinksBuffer;
typedef boost::iostreams::stream_buffer < CharStreamSink > CharStreamSinkBuffer;


///////////////////////////////////////////////////////////////////////////////
//
//  Base class for sink-wrappers.
//
///////////////////////////////////////////////////////////////////////////////

struct BaseSink
{
  BaseSink ( Usul::IO::StreamSink &sink ) : _sink ( sink )
  {
  }
  ~BaseSink()
  {
    std::cout.rdbuf ( 0x0 );
    std::cerr.rdbuf ( 0x0 );
  }
protected:
  Usul::IO::StreamSink &_sink;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Class for two sinks.
//
///////////////////////////////////////////////////////////////////////////////

struct TwoSinks : public BaseSink
{
  TwoSinks ( Usul::IO::StreamSink &sink, const std::string &file ) : BaseSink ( sink ), _buffer()
  {
    // Delete the file if it exists.
    if ( Usul::Predicates::FileExists() ( file ) )
      Usul::File::remove ( file );

    // Open buffer for the streams.
    _buffer.open ( TwoStreamSinks ( CharStreamSink ( sink ), boost::iostreams::file_sink ( file ) ) );

    // Tell streams to write to this buffer.
    std::cout.rdbuf ( &_buffer );
    std::cerr.rdbuf ( &_buffer );
  }
  ~TwoSinks()
  {
  }
  void flush()
  {
    std::cout << std::flush;
    std::cerr << std::flush;
  }

private:

  TwoStreamSinksBuffer _buffer;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Class for function sink.
//
///////////////////////////////////////////////////////////////////////////////

struct CharSink : public BaseSink
{
  CharSink ( Usul::IO::StreamSink &sink ) : BaseSink ( sink ), _buffer()
  {
    // Open buffer for the streams.
    _buffer.open ( CharStreamSink ( sink ) );

    // Tell streams to write to this buffer.
    std::cout.rdbuf ( &_buffer );
    std::cerr.rdbuf ( &_buffer );
  }
  ~CharSink()
  {
  }
  void flush()
  {
    std::cout << std::flush;
    std::cerr << std::flush;
  }

private:

  CharStreamSinkBuffer _buffer;
};


///////////////////////////////////////////////////////////////////////////////
//
//  End of details.
//
///////////////////////////////////////////////////////////////////////////////

} // namespace Detail


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

StreamSink::StreamSink ( const std::string &file ) : 
  _twoSinks ( 0x0 ),
  _charSink ( 0x0 ),
  _listeners()
{
  // Allocate proper sink.
  if ( false == file.empty() )
  {
    _twoSinks = new Usul::IO::Detail::TwoSinks ( *this, file );
  }
  else
  {
    _charSink = new Usul::IO::Detail::CharSink ( *this );
  }

  // Sync with printf().
  std::ios::sync_with_stdio();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

StreamSink::~StreamSink()
{
  delete _twoSinks; _twoSinks = 0x0;
  delete _charSink; _charSink = 0x0;
  _listeners.clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a listener.
//
///////////////////////////////////////////////////////////////////////////////

void StreamSink::add ( Usul::Interfaces::IUnknown *unknown )
{
  IStreamListener::QueryPtr listener ( unknown );
  if ( true == listener.valid() )
  {
    _listeners.push_back ( IStreamListener::RefPtr ( listener.get() ) );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove a listener.
//
///////////////////////////////////////////////////////////////////////////////

void StreamSink::remove ( Usul::Interfaces::IUnknown *unknown )
{
  IStreamListener::QueryPtr listener ( unknown );
  if ( true == listener.valid() )
  {
    _listeners.remove ( IStreamListener::RefPtr ( listener.get() ) );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Flush the stream(s).
//
///////////////////////////////////////////////////////////////////////////////

void StreamSink::flush()
{
  if ( 0x0 != _twoSinks )
  {
    _twoSinks->flush();
  }
  if ( 0x0 != _charSink )
  {
    _charSink->flush();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Notify listeners.
//
///////////////////////////////////////////////////////////////////////////////

void StreamSink::notify ( const char *s, unsigned int n )
{
  for ( Listeners::iterator i = _listeners.begin(); i != _listeners.end(); ++i )
  {
    IStreamListener::RefPtr listener ( *i );
    if ( true == listener.valid() )
    {
      listener->notify ( 0x0, s, n );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  End of primary namespace.
//
///////////////////////////////////////////////////////////////////////////////

} // namespace IO
} // namespace Usul


#endif //_USUL_STANDARD_STREAM_SINK_CLASS_H_
