
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

void SlRefBase::ref()
{
  SL_PRINT4 ( "SlRefBase::ref(), this = %X, _refCount = %d, class name = %s\n", this, _refCount, this->getClassName() );
  SL_ASSERT ( this );

  // Increment the reference count.
  CadKit::Threads::safeIncrement ( _refCount );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Release the object, delete it if you should.
//
///////////////////////////////////////////////////////////////////////////////

void SlRefBase::unref()
{
  SL_PRINT4 ( "SlRefBase::unref(), this = %X, _refCount = %d, class name = %s\n", this, _refCount, this->getClassName() );
  SL_ASSERT ( this );

  // Decrement the reference count.
  CadKit::Threads::safeDecrement ( _refCount );

  // Delete this instance if the count is now zero.
  if ( 0 == _refCount ) 
    delete this;
}
