
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  For storing the command line.
//
///////////////////////////////////////////////////////////////////////////////

#include "Usul/CommandLine/Arguments.h"
#include "Usul/File/Path.h"

// Disable deprecated warning in Visual Studio 8 for strdup
#if defined ( _MSC_VER ) && _MSC_VER >= 1400
#pragma warning ( disable : 4996 )
#endif

using namespace Usul;
using namespace Usul::CommandLine;


///////////////////////////////////////////////////////////////////////////////
//
//  Return the single instance.
//
///////////////////////////////////////////////////////////////////////////////

namespace Usul
{
  namespace CommandLine
  {
    namespace Detail
    {
      Usul::CommandLine::Arguments *_single ( 0x0 );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Arguments::Arguments() : _argc ( 0 ), _argv ( 0x0 )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Arguments::~Arguments()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the single instance.
//
///////////////////////////////////////////////////////////////////////////////

Arguments &Arguments::instance()
{
  if ( 0x0 == Usul::CommandLine::Detail::_single )
    Usul::CommandLine::Detail::_single = new Arguments;
  return *(Usul::CommandLine::Detail::_single);
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the argument.
//
///////////////////////////////////////////////////////////////////////////////

std::string Arguments::argv ( unsigned int i ) const
{
  const unsigned int argc ( static_cast < unsigned int > ( _argc ) );
  return std::string ( ( i < argc ) ? _argv[i] : "" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the name the program.
//
///////////////////////////////////////////////////////////////////////////////

std::string Arguments::program() const
{
  return this->argv ( 0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the directory that the program lives in.
//
///////////////////////////////////////////////////////////////////////////////

std::string Arguments::directory() const
{
  return Usul::File::directory ( this->program(), false );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the arguments.
//
///////////////////////////////////////////////////////////////////////////////

Arguments::Args Arguments::args() const
{
  Args args ( _argv, _argv + _argc );
  return args;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the arguments.
//
///////////////////////////////////////////////////////////////////////////////

void Arguments::set ( int argc, char **argv )
{
  _argc = argc;
  _argv = argv;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return a copy of the arguments.
//
///////////////////////////////////////////////////////////////////////////////

char **Arguments::copy() const
{
  // Handle empty arguments.
  const unsigned int argc ( static_cast < unsigned int > ( _argc ) );
  if ( 0 == argc )
    return 0x0;

  // Allocate one extra for the terminating null.
  char **argv = new char *[argc + 1];

  // Add a copy of the individual strings.
  for ( unsigned int i = 0; i < argc; ++i )
    argv[i] = ::strdup ( _argv[i] );

  // Important!
  argv[argc] = 0x0;

  // Return new copy.
  return argv;
}
