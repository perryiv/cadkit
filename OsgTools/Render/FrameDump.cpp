
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

#ifdef __sgi
#define _CPP_CMATH 1
#endif

#include "OsgTools/Render/FrameDump.h"

#include "Usul/Shared/Preferences.h"

#include <sstream>
#include <iomanip>
#include <algorithm>

using namespace OsgTools;
using namespace OsgTools::Render;

typedef Usul::Shared::Preferences Pref;


///////////////////////////////////////////////////////////////////////////////
//
//  Default constructor.
//
///////////////////////////////////////////////////////////////////////////////

FrameDump::FrameDump() : 
  _dump    ( Usul::Interfaces::IFrameDump::NEVER_DUMP ), 
  _dir     ( Pref::instance().getString   ( "FrameDump Directory"   ) ), 
  _base    ( Pref::instance().getString   ( "FrameDump Base"        ) ), 
  _ext     ( Pref::instance().getString   ( "FrameDump Extension"   ) ), 
  _start   ( Pref::instance().getUint     ( "FrameDump Start"       ) ), 
  _digits  ( Pref::instance().getUint     ( "FrameDump Digits"      ) ), 
  _scale   ( Pref::instance().getFloat    ( "FrameDump Scale", 1.0f ) ), 
  _current ( _start ),
  _names   ( 0x0 )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

FrameDump::FrameDump ( Usul::Interfaces::IFrameDump::DumpState dump, 
                       const std::string &dir, 
                       const std::string &base, 
                       const std::string &ext, 
                       unsigned int start, 
                       unsigned int digits, 
                       float scale ) :
  _dump    ( dump ), 
  _dir     ( dir ), 
  _base    ( base ), 
  _ext     ( ext ), 
  _start   ( start ), 
  _digits  ( digits ), 
  _scale   ( scale ),
  _current ( start ),
  _names   ( 0x0 )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Copy constructor.
//
///////////////////////////////////////////////////////////////////////////////

FrameDump::FrameDump ( const FrameDump &f ) : 
  _dump    ( f._dump ), 
  _dir     ( f._dir ), 
  _base    ( f._base ),
  _ext     ( f._ext ), 
  _start   ( f._start ), 
  _digits  ( f._digits ), 
  _scale   ( f._scale ), 
  _current ( f._current ),
  _names   ( f._names )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

FrameDump::~FrameDump()
{
  Pref::instance().setString ( "FrameDump Directory", _dir     );
  Pref::instance().setString ( "FrameDump Base",      _base    );
  Pref::instance().setString ( "FrameDump Extension", _ext     );
  Pref::instance().setUint   ( "FrameDump Start",     _start   );
  Pref::instance().setUint   ( "FrameDump Digits",    _digits  );
  Pref::instance().setFloat  ( "FrameDump Scale",     _scale   );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Assignment.
//
///////////////////////////////////////////////////////////////////////////////

FrameDump &FrameDump::operator = ( const FrameDump &f )
{
  _dump    = f._dump;
  _dir     = f._dir;
  _base    = f._base;
  _ext     = f._ext;
  _start   = f._start;
  _digits  = f._digits;
  _scale   = f._scale;
  _current = f._current;
  _names   = f._names;
  return *this;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the current filename.
//
///////////////////////////////////////////////////////////////////////////////

std::string FrameDump::file() const
{
  // Make the zero-padded number.
  std::string number;
  {
    std::ostringstream out;
    out << std::setw ( _digits ) << _current;
    number = out.str();
    std::replace ( number.begin(), number.end(), ' ', '0' );
  }

  // Make the full path.
  std::string path;
  {
    std::ostringstream out;
    out << _dir << '/' << number << _base << _ext;
    path = out.str();
  }

  // Increment the counter.
  ++_current;

  // Store file names (e.g., for movies) if we are supposed to.
  if ( _names.valid() )
    _names->value().push_back ( path );

  // Return the file name.
  return path;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the current filename.
//
///////////////////////////////////////////////////////////////////////////////

void FrameDump::collection ( FrameDump::Collection *n )
{
  _names = n;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the scale.
//
///////////////////////////////////////////////////////////////////////////////

void FrameDump::scale ( float p )
{
  if ( p > 0.0f )
    _scale = p;
}
