
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Frame-dump info.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _OPEN_SCENE_GRAPH_TOOLS_FRAME_DUMP_INFO_H_
#define _OPEN_SCENE_GRAPH_TOOLS_FRAME_DUMP_INFO_H_

#include "OsgTools/Export.h"

#include <string>
#include <vector>

namespace OsgTools {
namespace Render {


class OSG_TOOLS_EXPORT FrameDump
{
public:

  // Typedefs.
  typedef std::string Filename;
  typedef std::vector< Filename > Filenames;

  FrameDump();
  FrameDump ( bool dump, const std::string &dir, const std::string &base, const std::string &ext, unsigned int start = 0, unsigned int digits = 10, unsigned int width = 600, unsigned int height = 400 );
  FrameDump ( const FrameDump &f );
  ~FrameDump();

  FrameDump &operator = ( const FrameDump &f );

  const std::string &       base() const { return _base; }
  void                      base ( const std::string &b ) { _base = b; }

  unsigned int              current () const { return _current; }

  unsigned int              digits() const { return _digits; }
  void                      digits ( unsigned int d ) { _digits = d; }

  const std::string &       dir() const { return _dir; }
  void                      dir ( const std::string &d ) { _dir = d; }

  void                      dump ( bool state ) { _dump = state; }
  bool                      dump() const { return _dump; }

  std::string               filename() const;

  const Filenames&          filenames() const;

  void                      filenamesSave ( bool );

  const std::string &       ext() const { return _ext; }
  void                      ext ( const std::string &e ) { _ext = e; }

  unsigned int              height() const { return _height; }
  void                      height ( unsigned int h ) { _height = h; }

  void                      reset() { _current = _start; }

  unsigned int              start() const { return _start; }
  void                      start ( unsigned int s ) { _start = s; }

  bool                      useMySize() const { return _useSize; }
  void                      useMySize ( bool b ) { _useSize = b; }

  unsigned int              width() const { return _width; }
  void                      width ( unsigned int w ) { _width = w; }

private:

  bool _dump;
  std::string _dir;
  std::string _base;
  std::string _ext;
  unsigned int _start;
  unsigned int _digits;
  unsigned int _width;
  unsigned int _height;
  bool _useSize;
  mutable unsigned int _current;
  bool _saveFilenames;
  mutable Filenames _filenames;
};


} // namespace Render
} // namespace OsgTools


#endif // _OPEN_SCENE_GRAPH_TOOLS_FRAME_DUMP_INFO_H_
