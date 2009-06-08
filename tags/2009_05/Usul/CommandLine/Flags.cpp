
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "Flags.h"

using namespace Usul::CommandLine;



Flags::Flags() : _flags()
{
}

void Flags::insert ( const std::string& flag )
{
  _flags.insert( flag );
}

bool Flags::flag( const std::string& flag ) const
{
  Set::const_iterator iter = _flags.find( flag );
  return iter != _flags.end();
}
