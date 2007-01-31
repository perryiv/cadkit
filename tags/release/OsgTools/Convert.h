
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Converter functions.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _OSG_TOOLS_CONVERTERS_H_
#define _OSG_TOOLS_CONVERTERS_H_

#include "osg/Matrix"

#include "Usul/Math/Matrix44.h"


namespace OsgTools {
namespace Convert {


///////////////////////////////////////////////////////////////////////////////
//
//  Transpose the matrix.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  template < class FromType, class ToType > 
  void transpose ( const FromType &f, ToType &t )
  {
    t(0,0) = f(0,0);
    t(1,0) = f(0,1);
    t(2,0) = f(0,2);
    t(3,0) = f(0,3);

    t(0,1) = f(1,0);
    t(1,1) = f(1,1);
    t(2,1) = f(1,2);
    t(3,1) = f(1,3);

    t(0,2) = f(2,0);
    t(1,2) = f(2,1);
    t(2,2) = f(2,2);
    t(3,2) = f(2,3);

    t(0,3) = f(3,0);
    t(1,3) = f(3,1);
    t(2,3) = f(3,2);
    t(3,3) = f(3,3);
  }
};


///////////////////////////////////////////////////////////////////////////////
//
//  Convert the matrix. An OSG matrix is the transpose of an OpenGL matrix.
//
///////////////////////////////////////////////////////////////////////////////

template < class T, class I, class B > 
void matrix ( const Usul::Math::Matrix44<T,I,B> &f, osg::Matrixf &t )
{
  Detail::transpose ( f, t );
}
template < class T, class I, class B > 
void matrix ( const osg::Matrixf &f, Usul::Math::Matrix44<T,I,B> &t )
{
  Detail::transpose ( f, t );
}
template < class T, class I, class B > 
void matrix ( const Usul::Math::Matrix44<T,I,B> &f, osg::Matrixd &t )
{
  Detail::transpose ( f, t );
}
template < class T, class I, class B > 
void matrix ( const osg::Matrixd &f, Usul::Math::Matrix44<T,I,B> &t )
{
  Detail::transpose ( f, t );
}

template < class F, class T >
void vector ( const F& from , T& to, unsigned int size )
{
	for(unsigned int i=0; i<size; i++)
		to[i] = from[i];
}

}; // namespace Convert
}; // namespace OsgTools


#endif // _OSG_TOOLS_CONVERTERS_H_
