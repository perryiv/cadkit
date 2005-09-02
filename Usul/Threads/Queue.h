
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  A queue class that is guarded with a mutex.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_THREADS_GUARDED_QUEUE_CLASSES_H_
#define _USUL_THREADS_GUARDED_QUEUE_CLASSES_H_

#include "Usul/Threads/Mutex.h"
#include "Usul/Threads/Guard.h"

#include <queue>


namespace Usul {
namespace Threads {


template < class T > class Queue
{
public:

  /////////////////////////////////////////////////////////////////////////////
  //
  //  Typedefs
  //
  /////////////////////////////////////////////////////////////////////////////

  typedef std::queue < T > queue_type;
  typedef typename queue_type::value_type value_type;
  typedef typename queue_type::size_type size_type;
  typedef Usul::Threads::Mutex MutexType;
  typedef Usul::Threads::Guard < MutexType > GuardType;


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Constructor/destructor
  //
  /////////////////////////////////////////////////////////////////////////////

  Queue() : _mutex ( Usul::Threads::Mutex::create() ), _queue()
  {
  }
  ~Queue()
  {
    delete _mutex;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Is it empty?
  //
  /////////////////////////////////////////////////////////////////////////////

  bool empty()
  {
    GuardType guard ( *_mutex );
    return _queue.empty();
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Get the front element, the one that was added first.
  //
  /////////////////////////////////////////////////////////////////////////////

  value_type front() const
  {
    GuardType guard ( *_mutex );
    const value_type value ( _queue.front() );
    return value;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Get the back element, the one that was added most recently.
  //
  /////////////////////////////////////////////////////////////////////////////

  value_type back() const
  {
    GuardType guard ( *_mutex );
    const value_type value ( _queue.back() );
    return value;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Push an element onto the queue.
  //
  /////////////////////////////////////////////////////////////////////////////

  void push ( const value_type &value )
  {
    GuardType guard ( *_mutex );
    _queue.push ( value );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Pop an element off of the queue.
  //
  /////////////////////////////////////////////////////////////////////////////

  void pop()
  {
    GuardType guard ( *_mutex );
    if ( false == _queue.empty() )
      _queue.pop();
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Get the size of the queue.
  //
  /////////////////////////////////////////////////////////////////////////////

  size_type size() const
  {
    GuardType guard ( *_mutex );
    const size_type s ( _queue.size() );
    return s;
  }


private:

  // No copying or assignment.
  Queue ( const Queue & );
  Queue &operator = ( const Queue & );

  MutexType *_mutex;
  queue_type _queue;
};


}; // namespace Threads
}; // namespace Usul


#endif // _USUL_THREADS_GUARDED_QUEUE_CLASSES_H_
