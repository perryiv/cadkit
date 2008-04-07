
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

Arguments::Arguments() : _args()
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
//  Return the number of arguments.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Arguments::argc() const
{
  return _args.size();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the argument.
//
///////////////////////////////////////////////////////////////////////////////

const std::string &Arguments::argv ( unsigned int i ) const
{
  return _args.at(i);
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the name the program.
//
///////////////////////////////////////////////////////////////////////////////

const std::string &Arguments::program() const
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