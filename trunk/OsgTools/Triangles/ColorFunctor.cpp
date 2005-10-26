
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV & Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Functors for setting the color.
//
///////////////////////////////////////////////////////////////////////////////

#include "OsgTools/Triangles/ColorFunctor.h"
#include "OsgTools/Triangles/Constants.h"
#include "OsgTools/Triangles/TriangleSet.h"

#include "Usul/Math/Constants.h"
#include "Usul/Math/MinMax.h"

#include "osg/Array"

using namespace OsgTools::Triangles;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

ColorFunctor::ColorFunctor() : BaseClass()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

ColorFunctor::~ColorFunctor()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the name.
//
///////////////////////////////////////////////////////////////////////////////

std::string ColorFunctor::name() const
{
  return "Default Color";
}


///////////////////////////////////////////////////////////////////////////////
//
//  Calculate the triangle color.
//
///////////////////////////////////////////////////////////////////////////////

osg::Vec4f ColorFunctor::color ( const TriangleSet *, const Triangle * ) const
{
  return OsgTools::Triangles::DEFAULT_COLOR;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Calculate the vertex color.
//
///////////////////////////////////////////////////////////////////////////////

osg::Vec4f ColorFunctor::color ( const TriangleSet *, const SharedVertex * ) const
{
  return OsgTools::Triangles::DEFAULT_COLOR;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Macro for declaring other color-functors.
//
///////////////////////////////////////////////////////////////////////////////

#define DECLARE_COLOR_FUNCTOR(base_class,class_name,functor_name) \
class OSG_TOOLS_EXPORT class_name : public base_class \
{ \
public: \
  typedef base_class BaseClass; \
  USUL_DECLARE_REF_POINTERS ( class_name ); \
  class_name() : BaseClass(){} \
  virtual std::string   name() const { return functor_name; } \
  virtual osg::Vec4f    color ( const TriangleSet *ts, const Triangle *t ) const; \
  virtual osg::Vec4f    color ( const TriangleSet *ts, const SharedVertex *sv ) const; \
protected: \
  class_name ( const class_name & ); \
  class_name &operator = ( const class_name & ); \
  virtual ~class_name(){} \
}


///////////////////////////////////////////////////////////////////////////////
//
//  Color-functor that colors triangles and vertices according to the 
//  "thickness" of the material.
//
///////////////////////////////////////////////////////////////////////////////

DECLARE_COLOR_FUNCTOR ( ColorFunctor, SmallAngleColorFunctor, "Angles <= 15 Degrees" );
osg::Vec4f SmallAngleColorFunctor::color ( const TriangleSet *ts, const SharedVertex *sv ) const
{
  return BaseClass::color ( ts, sv );
}
osg::Vec4f SmallAngleColorFunctor::color ( const TriangleSet *ts, const Triangle *t ) const
{
  // Handle bad input.
  if ( 0x0 == ts || 0x0 == t )
    return BaseClass::color ( ts, t );

  // Get all vertices.
  const osg::Vec3Array *vertices ( ts->vertices() );

  // Get the triangle's vertices.
  const osg::Vec3f &v0 ( vertices->at ( t->vertex0()->index() ) );
  const osg::Vec3f &v1 ( vertices->at ( t->vertex1()->index() ) );
  const osg::Vec3f &v2 ( vertices->at ( t->vertex2()->index() ) );

  // Get the edges.
  const Usul::Math::Vec3f e01 ( v1[0] - v0[0], v1[1] - v0[1], v1[2] - v0[2] );
  const Usul::Math::Vec3f e02 ( v2[0] - v0[0], v2[1] - v0[1], v2[2] - v0[2] );
  const Usul::Math::Vec3f e12 ( v2[0] - v1[0], v2[1] - v1[1], v2[2] - v1[2] );

  // Get the angles between the edges.
  const float a0 ( e01.angle (  e02 ) );
  const float a1 ( e01.angle ( -e02 ) );
  const float a2 ( e12.angle (  e02 ) );

  // Get the minimum angle.
  const float angle ( Usul::Math::RAD_TO_DEG * Usul::Math::minimum ( a0, a1, a2 ) );
  const float minAngle ( 15 );

  // The two possible colors.
  static const osg::Vec4f bad  ( 0.8f, 0.0f, 0.0f, 1.0f );

  // Return rgba color.
  return ( ( angle < minAngle ) ? bad : OsgTools::Triangles::DEFAULT_COLOR );
}
