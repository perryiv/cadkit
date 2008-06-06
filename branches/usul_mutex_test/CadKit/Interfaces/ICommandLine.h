
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

#include <list>
#include <string>


namespace CadKit
{
class ICommandLine : public IUnknown
{
public:

  // Id for this interface.
  enum { IID = 1032754743 };

  // Parse the command-line arguments and execute.
  virtual bool            execute ( int argc, char **argv, IUnknown *source, IUnknown *target ) = 0;

  // Get the usage string.
  virtual std::string     getUsageString ( const std::string &program, bool extended ) = 0;
};

}; // namespace CadKit

#endif // _CADKIT_INTERFACE_COMMAND_LINE_H_
