
////////////////////////////////////////////////////////////////////////////////////
//
//  BSD License
//  http://www.opensource.org/licenses/bsd-license.html
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without 
//  modification, are permitted provided that the following conditions are met:
//
//  - Redistributions of source code must retain the above copyright notice, 
//    this list of conditions and the following disclaimer. 
//
//  - Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution. 
//
//  - Neither the name of the CAD Toolkit nor the names of its contributors may
//    be used to endorse or promote products derived from this software without
//    specific prior written permission. 
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
//  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
//  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
//  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
//  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
//  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
//  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
//  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
//  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
//  POSSIBILITY OF SUCH DAMAGE.
//
////////////////////////////////////////////////////////////////////////////////////

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
