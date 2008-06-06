
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Flags given on the command line.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_COMMAND_LINE_FLAGS_H__
#define __USUL_COMMAND_LINE_FLAGS_H__

#include <set>
#include <string>

namespace Usul {
namespace CommandLine {


class Flags
{
 public:
  typedef std::set < std::string > Set;
  Flags();

  void insert ( const std::string& );

  bool flag ( const std::string& ) const;

 private:
  Set _flags;
};

}
}


#endif // USUL_COMMAND_LINE_FLAGS_H__
