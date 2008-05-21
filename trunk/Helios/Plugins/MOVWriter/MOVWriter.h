
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MOV_WRITER_H__
#define __MOV_WRITER_H__

#include <string>
#include <vector>

namespace Usul { namespace Interfaces { struct IUnknown; } }

class MOVWriter
{
public:
  typedef std::string Filename;
  typedef std::vector< Filename > Filenames;

  MOVWriter( const Filename& filename, const Filenames& filenames );

  // Write the file.
  void operator() ( Usul::Interfaces::IUnknown *caller );

private:

  /// Filename to write to.
  Filename _filename;

  /// Filenames to use to create the movie.
  Filenames _filenames;
};

#endif // __MOV_WRITER_H__
