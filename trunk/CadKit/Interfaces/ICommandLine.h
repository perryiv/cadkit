
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  ICommandLine: Interface for checking and parsing the command-line.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_INTERFACE_COMMAND_LINE_H_
#define _CADKIT_INTERFACE_COMMAND_LINE_H_

#include "IUnknown.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include <string>
# include <list>
#endif


namespace CadKit
{
class ICommandLine : public IUnknown
{
public:

  // Id for this interface.
  enum { IID = 1032754743 };

  // Convenient typedef.
  typedef std::list<std::string> Args;

  // Check the arguments.
  virtual bool            checkArguments ( const int &argc, const char **argv ) const = 0;

  // Parse the command-line arguments and execute.
  virtual bool            execute ( int argc, char **argv, IUnknown *source, IUnknown *target ) = 0;

  // Get the usage string.
  virtual std::string     getUsageString ( const std::string &program, const std::string &ext, bool extended ) const = 0;

  // Parse the arguments.
  virtual bool            parseArguments ( const int &argc, const char **argv, Args &args ) = 0;
};

}; // namespace CadKit

#endif // _CADKIT_INTERFACE_COMMAND_LINE_H_
