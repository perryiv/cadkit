
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

#ifndef _USUL_COMMAND_LINE_ARGUMENTS_CLASS_H_
#define _USUL_COMMAND_LINE_ARGUMENTS_CLASS_H_

#include "Usul/Export/Export.h"

#include <vector>
#include <string>


namespace Usul {
namespace CommandLine {


class USUL_EXPORT Arguments
{
public:

  // Typedefs.
  typedef std::vector < std::string > Args;

  // Get the number of arguments.
  const int &                 argc() const { return _argc; }
  int &                       argc()       { return _argc; }

  // Get list of arguments.
  Args                        args() const;

  // Get the argument(s).
  std::string                 argv ( unsigned int ) const;
  char ** const               argv() const { return _argv; }
  char **                     argv()       { return _argv; }

  // Get a copy of the arguments. Caller is responsible for deleting.
  char **                     copy() const;

  // Return the directory that the program lives in.
  std::string                 directory() const;

  // Return the single instance.
  static Arguments &          instance();

  // Return the name of the program.
  std::string                 program() const;

  // Set the arguments.
  void                        set ( int argc, char **argv );

private:

  // Constructor and destructor.
  Arguments();
  ~Arguments();

  // No copying or assignment.
  Arguments &operator = ( const Arguments & );
  Arguments ( const Arguments & );

  int _argc;
  char **_argv;
};


} // namespace Shared
} // namespace Usul


#endif // _USUL_COMMAND_LINE_ARGUMENTS_CLASS_H_
