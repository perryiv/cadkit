
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  SlArrayPtr.h: Like std::SlArrayPtr, but for array pointers.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_STANDARD_LIBRARY_SMART_ARRAY_POINTER_CLASS_H_
#define _CADKIT_STANDARD_LIBRARY_SMART_ARRAY_POINTER_CLASS_H_


namespace CadKit
{
template<class T> class SlArrayPtr
{
public:

  SlArrayPtr ( T *p = 0x0 ) : _p ( p ){}
  SlArrayPtr ( SlArrayPtr &p ) : _p ( p.release() ){}
  ~SlArrayPtr() { this->reset ( 0x0 ); }

  T *               get() const { return _p; }

  SlArrayPtr &      operator = ( SlArrayPtr &p );
  T &               operator  * () const { return *_p; }
  T *               operator -> () const { return  _p; }

  T *               release();
  void              reset ( T *p = 0x0 );

private:
  
  T *_p;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Additional operators. These are not members of the class because compilers
//  vary too much in the proper syntax for friend functions in templates. 
//  See http://gcc.gnu.org/faq.html#friend and http://www.bero.org/gcc296.html
//
///////////////////////////////////////////////////////////////////////////////

template <class T> bool operator == ( const T *p1, const SlArrayPtr<T> &p2 ) { return p1 == p2._p; }
template <class T> bool operator != ( const T *p1, const SlArrayPtr<T> &p2 ) { return p1 != p2._p; }


///////////////////////////////////////////////////////////////////////////////
//
//  Assignment.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline SlArrayPtr &SlArrayPtr<T>::operator = ( SlArrayPtr<T> &p )
{
  if ( &p != this )
  {
    delete [] _p;
    _p = p.release();
  }

  return *this;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Release the pointer.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline T *SlArrayPtr<T>::release()
{
  T *temp = _p;
  _p = 0x0;
  return tmp;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Reset the pointer.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline void SlArrayPtr<T>::reset ( T *p )
{
  if ( _p )
    delete [] _p;
  _p = p;
}


}; // namespace CadKit


#endif // _CADKIT_STANDARD_LIBRARY_SMART_ARRAY_POINTER_CLASS_H_
