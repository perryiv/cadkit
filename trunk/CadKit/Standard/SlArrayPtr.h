
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  SlArrayPtr.h: Like std::auto_ptr, but for pointers to arrays.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_STANDARD_LIBRARY_SMART_ARRAY_POINTER_CLASS_H_
#define _CADKIT_STANDARD_LIBRARY_SMART_ARRAY_POINTER_CLASS_H_


namespace CadKit
{
template<class T, class I = unsigned int> class SlArrayPtr
{
public:

  SlArrayPtr ( T *p = 0x0 ) : _p ( p ){}
  SlArrayPtr ( SlArrayPtr &p ) : _p ( p.detach() ){}
  ~SlArrayPtr() { this->attach ( 0x0 ); }

  /// Attach to the given pointer.
  void              attach ( T *p );

  /// Detach from the internal pointer.
  T *               detach();

  /// Get the internal pointer.
  T *               get() const { return _p; }

  /// Get a reference to the internal pointer.
  T *&              getReference() const { return _p; }

  /// Check internal status.
  bool              isNull()  const { return 0x0 == _p; }
  bool              isValid() const { return 0x0 != _p; }

  /// Typecast operators.
  operator T *()             { return _p; }
  operator const T *() const { return _p; }

  /// Misc operators.
  SlArrayPtr &      operator  = ( SlArrayPtr &p );
  SlArrayPtr &      operator  = ( T *p );

  const T &         operator  * () const { return *_p; }
  T &               operator  * ()       { return *_p; }

  const T **        operator  & () const { return &_p; }
  T **              operator  & ()       { return &_p; }

  const T &         operator [] ( I i ) const { return _p[i]; }
  T &               operator [] ( I i )       { return _p[i]; }

  // Reset the internal pointer.
  void              reset() { this->attach ( 0x0 ); }

private:

  T *_p;

  static void       _safeDelete ( T *&p );
};


///////////////////////////////////////////////////////////////////////////////
//
//  Additional operators. These are not members of the class because compilers
//  vary too much in the proper syntax for friend functions in templates. 
//  See http://gcc.gnu.org/faq.html#friend and http://www.bero.org/gcc296.html
//
///////////////////////////////////////////////////////////////////////////////

template <class T, class I> bool operator == ( const T *p1, const SlArrayPtr<T,I> &p2 ) { return p1 == p2._p; }
template <class T, class I> bool operator != ( const T *p1, const SlArrayPtr<T,I> &p2 ) { return p1 != p2._p; }


///////////////////////////////////////////////////////////////////////////////
//
//  Assignment.
//
///////////////////////////////////////////////////////////////////////////////

template<class T, class I> inline SlArrayPtr<T,I> &SlArrayPtr<T,I>::operator = ( SlArrayPtr<T,I> &p )
{
  this->attach ( p.detach() );
  return *this;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Assignment.
//
///////////////////////////////////////////////////////////////////////////////

template<class T, class I> inline SlArrayPtr<T,I> &SlArrayPtr<T,I>::operator = ( T *p )
{
  this->attach ( p );
  return *this;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Detach from the pointer.
//
///////////////////////////////////////////////////////////////////////////////

template<class T, class I> inline T *SlArrayPtr<T,I>::detach()
{
  T *temp = _p;
  _p = 0x0;
  return temp;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Safely delete the pointer.
//
///////////////////////////////////////////////////////////////////////////////

template<class T, class I> inline void SlArrayPtr<T,I>::_safeDelete ( T *&p )
{
  if ( p )
  {
    delete [] p;
    p = 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Reset the pointer.
//
///////////////////////////////////////////////////////////////////////////////

template<class T, class I> inline void SlArrayPtr<T,I>::attach ( T *p )
{
  if ( p != _p )
  {
    this->_safeDelete ( _p );
    _p = p;
  }
}


}; // namespace CadKit


#endif // _CADKIT_STANDARD_LIBRARY_SMART_ARRAY_POINTER_CLASS_H_
