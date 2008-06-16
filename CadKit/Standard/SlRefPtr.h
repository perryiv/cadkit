
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  SlRefPtr.h: 
//  Pointer class that handles reference counting. You have to overload 
//  CadKit::_incrementPointerReferenceCount() and
//  CadKit::_decrementPointerReferenceCount() for the pointer type used.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_STANDARD_LIBRARY_SMART_REFERENCE_POINTER_CLASS_H_
#define _CADKIT_STANDARD_LIBRARY_SMART_REFERENCE_POINTER_CLASS_H_


namespace CadKit
{
  
///////////////////////////////////////////////////////////////////////////////
//
//  These are for class SlRefPtr. The client can overload these functions to
//  use SlRefPtr with another kind of pointer (like IUnknown). By making 
//  SlRefPtr call these functions instead of it's contained pointer's
//  ref/unref (or perhaps AddRef/Release) functions, we decouple it from the 
//  API of it's contained pointer. To use SlRefPtr with a pointer type 
//  that does not have "ref/unref" members, make a specific (non-template) 
//  overload of these functions.
//
///////////////////////////////////////////////////////////////////////////////

template <class T> inline void _incrementPointerReferenceCount ( T *p ) { p->ref(); }
template <class T> inline void _decrementPointerReferenceCount ( T *p ) { p->unref(); }


template<class T> class SlRefPtr
{
public:

  SlRefPtr();
  SlRefPtr ( const SlRefPtr<T> &p );
  SlRefPtr ( T *p );
  ~SlRefPtr();

  /// Make the internal pointer null. This is different from setValue() in 
  /// that it will not release the pointer.
  void              makeNull() { _p = 0x0; }

  /// Get the value. We return a non-const copy of the internal pointer because 
  /// there is no way the client can use that pointer to change the internal 
  /// state of this instance.
  T *               getValue() const { return _p; }

  /// Check internal status.
  bool              isNull()  const { return 0x0 == _p; }
  bool              isValid() const { return 0x0 != _p; }

  /// Typecast operators.
  operator T *()             { return _p; }
  operator const T *() const { return _p; }

  /// Misc operators.
  const T *         operator -> () const { return _p; }
  T *               operator -> ()       { return _p; }

  const T &         operator *  () const { return *_p; }
  T &               operator *  ()       { return *_p; }

  bool              operator !  ()       { return 0x0 == _p; }

  SlRefPtr<T> &     operator =  ( const SlRefPtr<T> &p ) { this->setValue ( p ); return *this; }
  SlRefPtr<T> &     operator =  ( T *p )                 { this->setValue ( p ); return *this; }

  bool              operator == ( const SlRefPtr<T> &p ) const { return _p == p._p; }
  bool              operator == ( const T *p ) const           { return _p == p; }
  bool              operator == ( T *p ) const                 { return _p == p; }

  bool              operator != ( const SlRefPtr<T> &p ) const { return _p != p._p; }
  bool              operator != ( const T *p ) const           { return _p != p; }
  bool              operator != ( T *p ) const                 { return _p != p; }

  /// Set the internal pointer.
  void              setValue ( const SlRefPtr<T> &p );
  void              setValue ( T *p );

protected:

  T *_p;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Additional operators. These are not members of the class because compilers
//  vary too much in the proper syntax for friend functions in templates. 
//  See http://gcc.gnu.org/faq.html#friend and http://www.bero.org/gcc296.html
//
///////////////////////////////////////////////////////////////////////////////

template <class P> bool operator == ( const P *p1, const SlRefPtr<P> &p2 ) { return p1 == p2._p; }
template <class P> bool operator == ( P *p1,       const SlRefPtr<P> &p2 ) { return p1 == p2._p; }
template <class P> bool operator != ( const P *p1, const SlRefPtr<P> &p2 ) { return p1 != p2._p; }
template <class P> bool operator != ( P *p1,       const SlRefPtr<P> &p2 ) { return p1 != p2._p; }


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

template <class T> inline SlRefPtr<T>::SlRefPtr() :
  _p ( 0x0 )
{
  // Empty.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

template <class T> inline SlRefPtr<T>::SlRefPtr ( const SlRefPtr &p ) :
  _p ( p._p )
{
  if ( _p )
    CadKit::_incrementPointerReferenceCount ( _p );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

template <class T> inline SlRefPtr<T>::SlRefPtr ( T *p ) :
  _p ( p )
{
  if ( _p )
    CadKit::_incrementPointerReferenceCount ( _p );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

template <class T> inline SlRefPtr<T>::~SlRefPtr()
{
  if ( _p )
    CadKit::_decrementPointerReferenceCount ( _p );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the value.
//
///////////////////////////////////////////////////////////////////////////////

template <class T> inline void SlRefPtr<T>::setValue ( T *p )
{
  // If the given pointer is the same address then just return.
  if ( _p == p )
    return;

  // Make a copy of the current one.
  T *old = _p;

  // Copy the given pointer.
  _p = p;

  // If the given pointer is not null then reference it.
  if ( _p )
    CadKit::_incrementPointerReferenceCount ( _p );

  // If the old one is not null then release it. Make sure we do this last 
  // because the given pointer "p" could be a child (indirect or direct) of 
  // "old". If we release "old" before we reference "p" then "p" may get 
  // deleted when it should not have.
  if ( old )
    CadKit::_decrementPointerReferenceCount ( old );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the value.
//
///////////////////////////////////////////////////////////////////////////////

template <class T> inline void SlRefPtr<T>::setValue ( const SlRefPtr<T> &p )
{
  // Call the other one.
  this->setValue ( p._p );
}


///////////////////////////////////////////////////////////////////////////////
//
// Safely reference/unreference the pointer. Provided as a convenience.
//
///////////////////////////////////////////////////////////////////////////////

template <class T> inline void safeRef   ( T *ptr ) { if ( ptr ) ptr->ref(); }
template <class T> inline void safeUnref ( T *ptr ) { if ( ptr ) ptr->unref(); }


}; // namespace CadKit

#endif // _CADKIT_STANDARD_LIBRARY_SMART_REFERENCE_POINTER_CLASS_H_
