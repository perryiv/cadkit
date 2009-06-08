
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  For redirecting stdio and stderr.
//
///////////////////////////////////////////////////////////////////////////////

#include "AppFrameWork/Core/StreamSink.h"
#include "AppFrameWork/Core/Program.h"

#include "Usul/File/Remove.h"
#include "Usul/Predicates/FileExists.h"

#include "boost/iostreams/device/null.hpp"
#include "boost/iostreams/device/file.hpp"
#include "boost/iostreams/tee.hpp"
#include "boost/iostreams/stream.hpp"

#include <iostream>

using namespace AFW::Core;

USUL_IMPLEMENT_TYPE_ID ( StreamSink );


///////////////////////////////////////////////////////////////////////////////
//
//  Start of details.
//
///////////////////////////////////////////////////////////////////////////////

namespace AFW {
namespace Core {
namespace Detail {


///////////////////////////////////////////////////////////////////////////////
//
//  Small class that redirects stdio and stderr to the application.
//
///////////////////////////////////////////////////////////////////////////////

struct APP_FRAME_WORK_EXPORT CharStreamSink : public boost::iostreams::basic_null_sink < char >
{
  std::streamsize write ( const char *s, std::streamsize n )
  {
    // Make sure the program is valid. We do not want to instantiate 
    // the app again during shutdown.
    if ( AFW::Core::Program::valid() )
      AFW::Core::Program::instance().standardOutAppend ( s, n );
    return n;
  }
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
  BaseSink()
  {
  }
  ~BaseSink()
  {
    std::cout.rdbuf ( 0x0 );
    std::cerr.rdbuf ( 0x0 );
  }
};


///////////////////////////////////////////////////////////////////////////////
//
//  Class for two sinks.
//
///////////////////////////////////////////////////////////////////////////////

struct TwoSinks : public BaseSink
{
  TwoSinks ( const std::string &file ) : _buffer()
  {
    // Delete the file if it exists.
    if ( Usul::Predicates::FileExists() ( file ) )
      Usul::File::remove ( file );

    // Open buffer for the streams.
    _buffer.open ( TwoStreamSinks ( CharStreamSink(), boost::iostreams::file_sink ( file ) ) );

    // Tell streams to write to this buffer.
    std::cout.rdbuf ( &_buffer );
    std::cerr.rdbuf ( &_buffer );
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
  CharSink() : _buffer()
  {
    // Open buffer for the streams.
    _buffer.open ( CharStreamSink() );

    // Tell streams to write to this buffer.
    std::cout.rdbuf ( &_buffer );
    std::cerr.rdbuf ( &_buffer );
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
} // namespace Core
} // namespace AFW


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

StreamSink::StreamSink ( const std::string &file ) : BaseClass(),
  _twoSinks ( 0x0 ),
  _charSink ( 0x0 )
{
  // Allocate proper sink.
  if ( false == file.empty() )
    _twoSinks = new AFW::Core::Detail::TwoSinks ( file );
  else
    _charSink = new AFW::Core::Detail::CharSink;

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
  delete _twoSinks;
  delete _charSink;
}
