
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  SlAutoPtr.h: Like std::auto_ptr, but a bit more friendly.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_STANDARD_LIBRARY_AUTO_POINTER_CLASS_H_
#define _CADKIT_STANDARD_LIBRARY_AUTO_POINTER_CLASS_H_


namespace CadKit
{
template<class T> class SlAutoPtr
{
public:

  SlAutoPtr();
  SlAutoPtr ( const SlAutoPtr<T> &p );
  SlAutoPtr ( T *p );
  ~SlAutoPtr();

  /// Get the value. We return a non-const copy of the internal pointer because 
  /// there is no way the client can use that pointer to change the internal 
  /// state of this instance.
  T *               get() const { return _p; }

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

  SlAutoPtr<T> &    operator =  ( const SlAutoPtr<T> &p ) { this->set ( p ); return *this; }
  SlAutoPtr<T> &    operator =  ( T *p )                  { this->set ( p ); return *this; }

  bool              operator == ( const SlAutoPtr<T> &p ) const { return _p == p._p; }
  bool              operator == ( const T *p ) const            { return _p == p; }

  bool              operator != ( const SlAutoPtr<T> &p ) const { return _p != p._p; }
  bool              operator != ( const T *p ) const            { return _p != p; }

  /// Release the internal pointer and return it.
  T *               release();

  /// Set the internal pointer.
  void              set ( const SlAutoPtr<T> &p );
  void              set ( T *p );

protected:

  T *_p;

  void              _delete();

  void              _set ( T *p ) { _p = p; }
};


///////////////////////////////////////////////////////////////////////////////
//
//  Additional operators. These are not members of the class because compilers
//  vary too much in the proper syntax for friend functions in templates. 
//  See http://gcc.gnu.org/faq.html#friend and http://www.bero.org/gcc296.html
//
///////////////////////////////////////////////////////////////////////////////

template <class P> bool operator == ( const P *p1, const SlAutoPtr<P> &p2 ) { return p1 == p2._p; }
template <class P> bool operator != ( const P *p1, const SlAutoPtr<P> &p2 ) { return p1 != p2._p; }


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

template <class T> inline SlAutoPtr<T>::SlAutoPtr() :
  _p ( 0x0 )
{
  // Empty.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

template <class T> inline SlAutoPtr<T>::SlAutoPtr ( const SlAutoPtr &p ) :
  _p ( p._p )
{
  // Empty.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

template <class T> inline SlAutoPtr<T>::SlAutoPtr ( T *p ) :
  _p ( p )
{
  // Empty.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Delete the pointer.
//
///////////////////////////////////////////////////////////////////////////////

template <class T> inline void SlAutoPtr<T>::_delete()
{
  if ( 0x0 != _p )
    delete _p;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

template <class T> inline SlAutoPtr<T>::~SlAutoPtr()
{
  this->_delete();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the value.
//
///////////////////////////////////////////////////////////////////////////////

template <class T> inline void SlAutoPtr<T>::set ( T *p )
{
  // If the given pointer is the same address then just return.
  if ( _p == p )
    return;

  // Delete the current one.
  this->_delete();

  // Copy the given pointer.
  this->_set ( p );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the value.
//
///////////////////////////////////////////////////////////////////////////////

template <class T> inline void SlAutoPtr<T>::set ( const SlAutoPtr<T> &p )
{
  // Call the other one.
  this->set ( p._p );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Release the internal pointer and return it.
//
///////////////////////////////////////////////////////////////////////////////

template <class T> inline T *SlAutoPtr<T>::release()
{
  T *p = this->get();
  this->_set ( 0x0 );
  return p;
}


}; // namespace CadKit

#endif // _CADKIT_STANDARD_LIBRARY_AUTO_POINTER_CLASS_H_
