
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

#ifndef _PF_TOOLS_CONVERTERS_H_
#define _PF_TOOLS_CONVERTERS_H_

#include "Performer/pr/pfLinMath.h"

#include "Usul/Math/Matrix44.h"


namespace PfTools {
namespace Convert {


///////////////////////////////////////////////////////////////////////////////
//
//  A Performer matrix is the transpose of an OpenGL matrix.
//
///////////////////////////////////////////////////////////////////////////////

void matrix ( const Usul::Math::Matrix44f &f, ::pfMatrix &t )
{
  t.setRow ( 0, f(0,0), f(1,0), f(2,0), f(3,0) );
  t.setRow ( 1, f(0,1), f(1,1), f(2,1), f(3,1) );
  t.setRow ( 2, f(0,2), f(1,2), f(2,2), f(3,2) );
  t.setRow ( 3, f(0,3), f(1,3), f(2,3), f(3,3) );
}
void matrix ( const Usul::Math::Matrix44d &f, ::pfMatrix &t )
{
  Usul::Math::Matrix44f temp ( f );
  PfTools::Convert::matrix ( temp, t );
}
void matrix ( const ::pfMatrix &f, Usul::Math::Matrix44f &t )
{
  f.getRow ( 0, &(t(0,0)), &(t(1,0)), &(t(2,0)), &(t(3,0)) );
  f.getRow ( 1, &(t(0,1)), &(t(1,1)), &(t(2,1)), &(t(3,1)) );
  f.getRow ( 2, &(t(0,2)), &(t(1,2)), &(t(2,2)), &(t(3,2)) );
  f.getRow ( 3, &(t(0,3)), &(t(1,3)), &(t(2,3)), &(t(3,3)) );
}
void matrix ( const ::pfMatrix &f, Usul::Math::Matrix44d &t )
{
  Usul::Math::Matrix44f temp ( t );
  PfTools::Convert::matrix ( f, temp );
}


}; // namespace Convert
}; // namespace PfTools


#endif // _PF_TOOLS_CONVERTERS_H_
