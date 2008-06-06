
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Single-threaded policy class (inspired by Andrei Alexandrescu's Loki).
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_STANDARD_LIBRARY_SINGLE_THREADED_POLICY_CLASS_H_
#define _CADKIT_STANDARD_LIBRARY_SINGLE_THREADED_POLICY_CLASS_H_


namespace CadKit {
namespace Threads {


template <typename Host> class SingleThreaded
{
public:
  class Lock
  {
  public:
    Lock(){}
    Lock ( Host &host ){}
  protected:
    ~Lock(){} // See Section 1.7 of "Modern C++ Design".
  };
};


}; // namespace Threads
}; // namespace CadKit


#endif // _CADKIT_STANDARD_LIBRARY_THREADS_H_
