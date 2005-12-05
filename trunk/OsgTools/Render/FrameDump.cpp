
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
  _dump    ( false ), 
  _dir     ( Pref::instance().getString   ( "FrameDump Directory" ) ), 
  _base    ( Pref::instance().getString   ( "FrameDump Base"      ) ), 
  _ext     ( Pref::instance().getString   ( "FrameDump Extension" ) ), 
  _start   ( Pref::instance().getUint     ( "FrameDump Start"     ) ), 
  _digits  ( Pref::instance().getUint     ( "FrameDump Digits"    ) ), 
  _current ( _start ),
  _saveFilenames ( false ),
  _filenames ()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

FrameDump::FrameDump ( bool dump, 
                       const std::string &dir, 
                       const std::string &base, 
                       const std::string &ext, 
                       unsigned int start, 
                       unsigned int digits ) :
  _dump    ( dump ), 
  _dir     ( dir ), 
  _base    ( base ), 
  _ext     ( ext ), 
  _start   ( start ), 
  _digits  ( digits ), 
  _current ( start )
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
  _current ( f._current )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

FrameDump::~FrameDump()
{
  Pref::instance().setString ( "FrameDump Directory", _dir    );
  Pref::instance().setString ( "FrameDump Base",      _base   );
  Pref::instance().setString ( "FrameDump Extension", _ext    );
  Pref::instance().setUint   ( "FrameDump Start",     _start  );
  Pref::instance().setUint   ( "FrameDump Digits",    _digits );
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
  _current = f._current;
  return *this;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the current filename.
//
///////////////////////////////////////////////////////////////////////////////

std::string FrameDump::filename() const
{
  // Make the file name.
  std::ostringstream fn;
  fn << _dir << '/' << _base << std::setw ( _digits ) << _current << _ext;
  std::string name ( fn.str() );
  std::replace ( name.begin(), name.end(), ' ', '0' );

  // Increment the counter.
  ++_current;

  if ( _saveFilenames )
    _filenames.push_back( name );

  // Return the file name.
  return name;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filenames that were written out.
//
///////////////////////////////////////////////////////////////////////////////

const FrameDump::Filenames& FrameDump::filenames() const
{
  return _filenames;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Should the filenames be saved.
//
///////////////////////////////////////////////////////////////////////////////

void FrameDump::filenamesSave( bool b )
{
  _saveFilenames = b;

  // Not sure what if filenames should be reserved.
  // Picking 1000 for now since we rarely create more than 1000 frames.
  if ( b )
    _filenames.reserve( 1000 );
}
