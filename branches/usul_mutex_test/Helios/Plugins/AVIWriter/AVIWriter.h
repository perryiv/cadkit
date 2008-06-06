
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __AVI_WRITER_H__
#define __AVI_WRITER_H__

#include <string>
#include <vector>

namespace Usul { namespace Interfaces { struct IUnknown; } }

class AVIWriter
{
public:
  typedef std::string Filename;
  typedef std::vector< Filename > Filenames;

  AVIWriter( const Filename& filename, const Filenames& filenames );

  // Write the file.
  void operator() ( Usul::Interfaces::IUnknown *caller );

private:
  /// Write the movie.
  void _write( Usul::Interfaces::IUnknown *caller );

  Filename _filename;
  Filenames _filenames;
};

#endif // __AVI_WRITER_H__
