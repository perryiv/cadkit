
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_LOOPS_DIRECTION_H__
#define __USUL_LOOPS_DIRECTION_H__

#include "Usul/Math/Functions.h"

namespace Usul {
namespace Loops {

  namespace Detail {

    template < class Vertex > double getAngle( const Vertex &v1, const Vertex &v2, const Vertex &v3 )
    {
      Vertex t1 ( v2 - v1 );
      Vertex t2 ( v3 - v2 );

      t1.normalize();
      t2.normalize();

      return ::acos( t1 * t2 );
    }
  }

template < class Iter >
bool isCounterClockwise ( Iter begin, Iter end )
{
  if( *begin == *end )
    throw std::logic_error ( "Error 3524416433: Beginning point must not equal ending point." );

  double angle( 0.0 );

  for( Iter i = begin; i < ( end - 2 ); ++i )
  {
    angle += Detail::getAngle ( *i, *( i + 1 ), *( i + 2 ) );
  }

  angle += Detail::getAngle ( *( end - 2 ), *( end - 1 ), *( end ) );

  return ( angle > 0 );
}

}
}


#endif // __USUL_LOOPS_DIRECTION_H__
