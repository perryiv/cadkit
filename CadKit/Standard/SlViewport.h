
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  SlViewport.h: A class that represent a viewport.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_STANDARD_LIBRARY_VIEWPORT_CLASS_H_
#define _CADKIT_STANDARD_LIBRARY_VIEWPORT_CLASS_H_


namespace CadKit
{
template<class T> class SlViewport
{
public:

  SlViewport();
  SlViewport ( const T &x, const T &y, const T &width, const T &height );

  const T &             getX()      const { return _x; }
  const T &             getY()      const { return _y; }
  const T &             getWidth()  const { return _width; }
  const T &             getHeight() const { return _height; }

  // Friend function operators. See http://gcc.gnu.org/faq.html#friend 
  // and http://www.bero.org/gcc296.html
#if __GNUC__ >= 2
  template <class P> friend bool operator == ( const SlViewport<P> &vpA, const SlViewport<P> &vpB );
  template <class P> friend bool operator != ( const SlViewport<P> &vpA, const SlViewport<P> &vpB );
#else
  friend bool           operator == ( const SlViewport &vpA, const SlViewport &vpB );
  friend bool           operator != ( const SlViewport &vpA, const SlViewport &vpB );
#endif

  void                  setValue ( const SlViewport<T> &vp );
  void                  setValue ( const T &x, const T &y, const T &width, const T &height );

protected:

  T _x;
  T _y;
  T _width;
  T _height;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline SlViewport<T>::SlViewport() :
  _x ( 0 ),
  _y ( 0 ),
  _width ( 0 ),
  _height ( 0 )
{
  // Dummy initial values.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline SlViewport<T>::SlViewport ( const T &x, const T &y, const T &width, const T &height ) :
  _x ( x ),
  _y ( y ),
  _width ( width ),
  _height ( height )
{
  // Empty.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the value.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline void SlViewport<T>::setValue ( const T &x, const T &y, const T &width, const T &height )
{
  _x = x;
  _y = y;
  _width = width;
  _height = height;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the value.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline void SlViewport<T>::setValue ( const SlViewport<T> &vp )
{
  this->setValue ( vp._x, vp._y, vp._width, vp._height );
}


///////////////////////////////////////////////////////////////////////////////
//
//  See if they're equal.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline bool operator == ( const SlViewport<T> &vpA, const SlViewport<T> &vpB )
{
  return ( 
    vpA._x == vpB._x && 
    vpA._y == vpB._y &&
    vpA._width == vpB._width &&
    vpA._height == vpB._height );
}


///////////////////////////////////////////////////////////////////////////////
//
//  See if they're not equal.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline bool operator != ( const SlViewport<T> &vpA, const SlViewport<T> &vpB )
{
  return !( vpA == vpB );
}


///////////////////////////////////////////////////////////////////////////////
//
//	Common types.
//
///////////////////////////////////////////////////////////////////////////////

typedef SlViewport<long>  SlViewportl;
typedef SlViewport<int>   SlViewporti;
typedef SlViewport<short> SlViewports;


}; // End of namespace CadKit

#endif // _CADKIT_STANDARD_LIBRARY_VIEWPORT_CLASS_H_
