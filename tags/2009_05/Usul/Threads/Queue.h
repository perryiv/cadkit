
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


namespace Usul {
namespace Threads {


template < class Config_ > class Queue
{
public:

  /////////////////////////////////////////////////////////////////////////////
  //
  //  Typedefs
  //
  /////////////////////////////////////////////////////////////////////////////

  typedef Config_ Config;
  typedef typename Config::mutex_type mutex_type;
  typedef typename Config::guard_type guard_type;
  typedef typename Config::queue_type queue_type;
  typedef typename queue_type::value_type value_type;
  typedef typename queue_type::size_type size_type;


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Constructor/destructor
  //
  /////////////////////////////////////////////////////////////////////////////

  Queue() : _mutex(), _queue()
  {
  }
  ~Queue()
  {
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Clear the queue.
  //
  /////////////////////////////////////////////////////////////////////////////

  void clear()
  {
    guard_type guard ( _mutex );
    while ( false == _queue.empty() )
    {
      _queue.pop();
    }
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Is it empty?
  //
  /////////////////////////////////////////////////////////////////////////////

  bool empty() const
  {
    guard_type guard ( _mutex );
    return _queue.empty();
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Get the next item on the queue and pop it off the queue.
  //
  /////////////////////////////////////////////////////////////////////////////

  value_type next()
  {
    guard_type guard ( _mutex );
    value_type value ( _queue.front() );
    _queue.pop();
    return value;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Add an element onto the queue.
  //
  /////////////////////////////////////////////////////////////////////////////

  void add ( const value_type &value )
  {
    guard_type guard ( _mutex );
    _queue.push ( value );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Get the size of the queue.
  //
  /////////////////////////////////////////////////////////////////////////////

  size_type size() const
  {
    guard_type guard ( _mutex );
    return _queue.size();
  }

private:

  // No copying or assignment.
  Queue ( const Queue & );
  Queue &operator = ( const Queue & );

  mutable mutex_type _mutex;
  queue_type _queue;
};


} // namespace Threads
} // namespace Usul


///////////////////////////////////////////////////////////////////////////////
//
//  Macro for declaring config structs.
//
///////////////////////////////////////////////////////////////////////////////

#define USUL_DECLARE_QUEUE_CONFIG(struct_name,the_mutex_type,the_guard_type,the_queue_type) \
struct struct_name \
{ \
  typedef the_mutex_type mutex_type; \
  typedef the_guard_type guard_type; \
  typedef the_queue_type queue_type; \
} \



#endif // _USUL_THREADS_GUARDED_QUEUE_CLASSES_H_