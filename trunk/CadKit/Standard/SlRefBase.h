
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  SlRefBase.h: 
//  Inherit from this class to get reference counting members. When the
//  reference number is decremented to 0 the destructor is called.
//  
//  Note: by not having a default constructor we force the client to decide 
//  what the initial reference count is. Typically, it is either 0 or 1.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_STANDARD_LIBRARY_REFERENCE_OBJECT_CLASS_H_
#define _CADKIT_STANDARD_LIBRARY_REFERENCE_OBJECT_CLASS_H_

#include "SlBase.h"
#include "SlRefPtr.h"


namespace CadKit
{
class SlRefBase : public SlBase
{
public:

  /// Get the reference count.
  unsigned long   getRefCount() const { return _refCount; }

  /// Increment/decrement the reference count.
  void            ref();
  void            unref();

protected:

  unsigned long _refCount;

  #ifdef _DEBUG
  SlRefBase ( const unsigned long &refCount );
  #else
  SlRefBase ( const unsigned long &refCount ) : SlBase(), _refCount ( refCount ){}
  #endif

  virtual ~SlRefBase();

  SL_DECLARE_CLASS ( SlRefBase, 1032912262 );
};


}; // namespace CadKit


// Declare a typecast for SlRefPtr. Put this in your class declaration.
#define SL_DECLARE_REFERENCE_POINTER(class_name) \
  public: \
  typedef SlRefPtr < class_name > Ptr


#endif // _CADKIT_STANDARD_LIBRARY_REFERENCE_OBJECT_CLASS_H_
