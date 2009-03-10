
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Unix (Linux, OS X) implementation of the Process class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_SYSTEM_PROCESS_CLASS_UNIX_IMPL_H_
#define _USUL_SYSTEM_PROCESS_CLASS_UNIX_IMPL_H_

#include <iosfwd>
#include <string>
#include <unistd.h>
#include <vector>

namespace Usul {
namespace System {
namespace Detail {

class ProcessImpl
{
public:
  
  // Useful typedefs.
  typedef pid_t ID;
  typedef std::vector<char> Arguments;
  
  ProcessImpl();
  ~ProcessImpl();
  
  // Get the current process's id.
  static ID               currentProcessId();
  
  // Get the system's ID for this object.
  ID                      id() const;
  
  // Is the process running?
  bool                    isRunning() const;
  
  // Start the process.
  bool                    start ( const std::string &executable, 
                                 const std::string & arguments );
  
  // Stop the process.
  bool                    stop();
  
  // Wait for the process to finish.
  void                    wait ( unsigned long milliseconds = 0xFFFFFFFF );
  
private:
  
  // No copying or assigning.
  ProcessImpl ( const ProcessImpl & );
  ProcessImpl &operator = ( const ProcessImpl & );

  pid_t _id;
};


} // namespace Detail
} // namespace System
} // namespace Usul


#endif // _USUL_SYSTEM_PROCESS_CLASS_UNIX_IMPL_H_
