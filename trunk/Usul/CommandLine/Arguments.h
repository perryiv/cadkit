
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
  typedef Args::const_iterator Itr;

  // Return the number of arguments.
  unsigned int                argc() const;

  // Get the argument.
  const std::string &         argv ( unsigned int ) const;

  // Iterator access.
  Itr                         begin() const { return _args.begin(); }

  // Iterator access.
  Itr                         end() const { return _args.end(); }

  // Return the directory that the program lives in.
  std::string                 directory() const;

  // Return the single instance.
  static Arguments &          instance();

  // Return the name the program.
  const std::string &         program() const;

  // Set the arguments.
  template < class I > void set ( I first, I last )
  {
    _args.clear();
    _args.insert ( _args.end(), first, last );
  }

private:

  // Constructor and destructor.
  Arguments();
  ~Arguments();

  // No copying or assignment.
  Arguments &operator = ( const Arguments & );
  Arguments ( const Arguments & );

  Args _args;
};


}; // namespace Shared
}; // namespace Usul


#endif // _USUL_COMMAND_LINE_ARGUMENTS_CLASS_H_
