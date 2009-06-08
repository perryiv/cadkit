
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

#include "Usul/Interfaces/IFrameDump.h"
#include "Usul/Properties/Attribute.h"

#include <string>
#include <vector>


namespace OsgTools {
namespace Render {


class OSG_TOOLS_EXPORT FrameDump
{
public:

  // Typedefs.
  typedef std::string Filename;
  typedef std::vector<Filename> Names;
  typedef Usul::Properties::Attribute<Names> Collection;
  typedef Usul::Interfaces::IFrameDump IFrameDump;

  FrameDump();
  FrameDump ( IFrameDump::DumpState dump, 
              const std::string &dir, 
              const std::string &base, 
              const std::string &ext, 
              unsigned int start = 0, 
              unsigned int digits = 10, 
              float scale = 1.0f );
  FrameDump ( const FrameDump &f );
  ~FrameDump();

  FrameDump &operator = ( const FrameDump &f );

  const std::string &       base() const { return _base; }
  void                      base ( const std::string &b ) { _base = b; }

  const Collection *        collection() const { return _names.get(); }
  Collection *              collection() { return _names.get(); }
  void                      collection ( Collection * );

  unsigned int              current() const { return _current; }

  unsigned int              digits() const { return _digits; }
  void                      digits ( unsigned int d ) { _digits = d; }

  const std::string &       dir() const { return _dir; }
  void                      dir ( const std::string &d ) { _dir = d; }

  void                      dump ( IFrameDump::DumpState state ) { _dump = state; }
  IFrameDump::DumpState     dump() const { return _dump; }

  std::string               file() const;

  const std::string &       ext() const { return _ext; }
  void                      ext ( const std::string &e ) { _ext = e; }

  float                     scale() const { return _scale; }
  void                      scale ( float p );

  void                      reset() { _current = _start; }

  unsigned int              start() const { return _start; }
  void                      start ( unsigned int s ) { _start = s; }

private:

  IFrameDump::DumpState _dump;
  std::string _dir;
  std::string _base;
  std::string _ext;
  unsigned int _start;
  unsigned int _digits;
  float _scale;
  mutable unsigned int _current;
  mutable Collection::RefPtr _names;
};


} // namespace Render
} // namespace OsgTools


#endif // _OPEN_SCENE_GRAPH_TOOLS_FRAME_DUMP_INFO_H_
