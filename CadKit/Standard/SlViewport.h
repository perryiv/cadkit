
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

  bool                  isEqual    ( const SlViewport<T> &v ) const;
  bool                  isNotEqual ( const SlViewport<T> &v ) const;

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
//  Additional operators. These are not members of the class because compilers
//  vary too much in the proper syntax for friend functions in templates. 
//  See http://gcc.gnu.org/faq.html#friend and http://www.bero.org/gcc296.html
//
///////////////////////////////////////////////////////////////////////////////

template<class T> bool operator == ( const SlViewport<T> &vpA, const SlViewport<T> &vpB );
template<class T> bool operator != ( const SlViewport<T> &vpA, const SlViewport<T> &vpB );


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

template<class T> inline bool SlViewport<T>::isEqual ( const SlViewport<T> &v ) const
{
  return ( 
    this->vpA._x == this->vpB._x && 
    this->vpA._y == this->vpB._y &&
    this->vpA._width == this->vpB._width &&
    this->vpA._height == this->vpB._height );
}


///////////////////////////////////////////////////////////////////////////////
//
//  See if they're not equal.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline bool SlViewport<T>::isNotEqual ( const SlViewport<T> &v ) const
{
  return ( false == this->isEqual ( v ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  See if they're equal.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline bool operator == ( const SlViewport<T> &vpA, const SlViewport<T> &vpB )
{
  return vpA.isEqual ( vpB );
}


///////////////////////////////////////////////////////////////////////////////
//
//  See if they're not equal.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline bool operator != ( const SlViewport<T> &vpA, const SlViewport<T> &vpB )
{
  return vpA.isNotEqual ( vpB );
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
