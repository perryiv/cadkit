
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

#include "Referenced.h"

#include <assert.h>

using namespace InterSense;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Referenced::Referenced() : _refCount ( 0 )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Referenced::~Referenced()
{
  assert ( 0 == _refCount );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Increment the reference count by one.
//
///////////////////////////////////////////////////////////////////////////////

void Referenced::ref()
{
  // Increment the reference count.
  ++_refCount;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Release the object, delete it if you should.
//
///////////////////////////////////////////////////////////////////////////////

void Referenced::unref()
{
  assert ( _refCount > 0 );
  if ( 0 == --_refCount ) 
    delete this;
}


///////////////////////////////////////////////////////////////////////////////
//
//  For boost::intrusive_ptr.
//
///////////////////////////////////////////////////////////////////////////////

namespace boost
{
  void intrusive_ptr_add_ref ( InterSense::Referenced *ptr )
  {
    assert ( ptr );
    ptr->ref();
  }
  void intrusive_ptr_release ( InterSense::Referenced *ptr )
  {
    assert ( ptr );
    ptr->unref();
  }
};
