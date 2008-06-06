
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  DbStlDatabase: An STL triangle facet.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_DATABASE_STEREO_LITHOGRAPHY_LIBRARY_TRIANGLE_FACET_H_
#define _CADKIT_DATABASE_STEREO_LITHOGRAPHY_LIBRARY_TRIANGLE_FACET_H_


#include "Standard/SlVec3.h"

#include <list>


namespace CadKit {


class DbStlTriangle
{
public:

  DbStlTriangle() : 
    _vertices ( SlVec3f ( 0.0f, 0.0f, 0.0f ),
                SlVec3f ( 0.0f, 0.0f, 0.0f ),
                SlVec3f ( 0.0f, 0.0f, 0.0f ) ),
    _normal ( 0.0f, 0.0f, 0.0f )
  {
    // Empty.
  }


  DbStlTriangle ( const SlVec3f &v0, const SlVec3f &v1, const SlVec3f &v2 ) : 
    _vertices ( SlVec3f ( v0 ),
                SlVec3f ( v1 ),
                SlVec3f ( v2 ) ),
    _normal ( ( v1 - v0 ).cross ( v2 - v0 ) )
  {
    _normal.normalize();
  }


  DbStlTriangle ( float t0v0, float t0v1, float t0v2, 
               float t1v0, float t1v1, float t1v2,
               float t2v0, float t2v1, float t2v2 ) : 
    _vertices ( SlVec3f ( t0v0, t0v1, t0v2 ),
                SlVec3f ( t1v0, t1v1, t1v2 ),
                SlVec3f ( t2v0, t2v1, t2v2 ) ),
    _normal ( ( _vertices[1] - _vertices[0] ).cross ( _vertices[2] - _vertices[0] ) )
  {
    _normal.normalize();
  }

  
  DbStlTriangle ( const DbStlTriangle &facet ) :
    _vertices ( facet._vertices ),
    _normal ( facet._normal )
  {
    // Empty.
  }

  const SlVec3f &getVertex ( unsigned int which ) const { return _vertices.operator[] ( which ); }
  const SlVec3f &getNormal() const { return _normal; }

  /////////////////////////////////////////////////////////////////////////////
  //
  //  Execute the functor with all the elements. Make sure the argument is 
  //  a reference or else you will faul things up.
  //
  /////////////////////////////////////////////////////////////////////////////

  template < class Functor > void operator () ( Functor &functor ) const
  {
    functor ( _vertices[0] );
    functor ( _vertices[1] );
    functor ( _vertices[2] );
    functor ( _normal );
  }

private:

  SlVec3<SlVec3f> _vertices;
  SlVec3f _normal;
};


}; // namespace CadKit


#endif // _CADKIT_DATABASE_STEREO_LITHOGRAPHY_LIBRARY_TRIANGLE_FACET_H_
