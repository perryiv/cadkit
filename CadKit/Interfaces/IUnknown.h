
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  IUnknown: Interface for reference counting and querying other interfaces.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_INTERFACE_UNKNOWN_H_
#define _CADKIT_INTERFACE_UNKNOWN_H_


namespace CadKit
{
class IUnknown
{
public:

  // Id for this interface.
  enum { IID = 1032561430 };

  // See if the interface is supported.
  virtual IUnknown *      queryInterface ( const unsigned long &iid ) = 0;

  // Reference/unreference the interface.
  virtual void            ref() = 0;
  virtual void            unref() = 0;

  // Get the reference count.
  virtual unsigned long   getRefCount() const = 0;
};

// So that SlRefPtr works with IUnknown.
//inline void _incrementPointerReferenceCount ( IUnknown *p ) { p->ref(); }
//inline void _decrementPointerReferenceCount ( IUnknown *p ) { p->unref(); }

}; // namespace CadKit

#endif // _CADKIT_INTERFACE_UNKNOWN_H_
