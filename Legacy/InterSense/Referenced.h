
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  For reference counting.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _INTERSENSE_TRACKER_LIBRARY_REFERENCED_OBJECT_CLASS_H_
#define _INTERSENSE_TRACKER_LIBRARY_REFERENCED_OBJECT_CLASS_H_


namespace InterSense {


class Referenced
{
public:

  // Increment/decrement the reference count.
  void            ref();
  void            unref();

protected:

  Referenced();
  virtual ~Referenced();

  unsigned int _refCount;
};


}; // namespace InterSense


namespace boost
{
  void intrusive_ptr_add_ref ( InterSense::Referenced *ptr );
  void intrusive_ptr_release ( InterSense::Referenced *ptr );
};


#endif // _INTERSENSE_TRACKER_LIBRARY_REFERENCED_OBJECT_CLASS_H_
