
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  SlRefBase.cpp: 
//  Inherit from this class to get reference counting members. When the
//  reference number is decremented to 0 the destructor is called.
//  
//  Note: by not having a default constructor we force the client to decide 
//  what the initial reference count is. Typically, it is either 0 or 1.
//
///////////////////////////////////////////////////////////////////////////////

#include "SlPrecompiled.h"
#include "SlRefBase.h"
#include "SlAssert.h"
#include "SlPrint.h"
#include "SlThread.h"

using namespace CadKit;

SL_IMPLEMENT_CLASS(SlRefBase,SlBase);


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG

SlRefBase::SlRefBase ( const unsigned long &refCount ) : SlBase(), _refCount ( refCount )
{
  SL_PRINT4 ( "SlRefBase::SlRefBase(), this = %X, _refCount = %d, class name = %s\n", this, _refCount, this->getClassName() );
}

#endif

///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

SlRefBase::~SlRefBase()
{
  SL_PRINT4 ( "SlRefBase::~SlRefBase(), this = %X, _refCount = %d, class name = %s\n", this, _refCount, this->getClassName() );
  SL_ASSERT ( 0 == _refCount );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Increment the reference count by one.
//
///////////////////////////////////////////////////////////////////////////////

void SlRefBase::_incrementReferenceCount()
{
  SL_PRINT4 ( "SlRefBase::_incrementReferenceCount(), this = %X, _refCount = %d, class name = %s\n", this, _refCount, this->getClassName() );
  SL_ASSERT ( this );

  // Increment the reference count.
  CadKit::threadSafeIncrement ( _refCount );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Release the object, delete it if you should.
//
///////////////////////////////////////////////////////////////////////////////

void SlRefBase::_decrementReferenceCount()
{
  SL_PRINT4 ( "SlRefBase::_decrementReferenceCount(), this = %X, _refCount = %d, class name = %s\n", this, _refCount, this->getClassName() );
  SL_ASSERT ( this );

  // Decrement the reference count.
  CadKit::threadSafeDecrement ( _refCount );

  // Delete this instance if the count is now zero.
  if ( 0 == _refCount ) 
    delete this;
}


namespace CadKit {


///////////////////////////////////////////////////////////////////////////////
//
//  This function makes SlRefBase work with SlRefPtr.
//
///////////////////////////////////////////////////////////////////////////////

void _incrementPointerReferenceCount ( SlRefBase *p )
{
  p->_incrementReferenceCount();
}


///////////////////////////////////////////////////////////////////////////////
//
//  This function makes SlRefBase work with SlRefPtr.
//
///////////////////////////////////////////////////////////////////////////////

void _decrementPointerReferenceCount ( SlRefBase *p )
{
  p->_decrementReferenceCount();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Safely reference the pointer.
//
///////////////////////////////////////////////////////////////////////////////

void safeReference ( SlRefBase *p )
{
  if ( p )
    p->_incrementReferenceCount();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Safely unreference the pointer.
//
///////////////////////////////////////////////////////////////////////////////

void safeDereference ( SlRefBase *p )
{
  if ( p )
    p->_decrementReferenceCount();
}


}; // namespace CadKit
