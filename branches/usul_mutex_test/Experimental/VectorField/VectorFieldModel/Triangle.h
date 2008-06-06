
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __VECTOR_FIELD_TRIANGLE_H__
#define __VECTOR_FIELD_TRIANGLE_H__

#include "Usul/Base/Object.h"
#include "Usul/Math/Vector2.h"
#include "Usul/Math/Vector3.h"
#include "Usul/Math/Vector4.h"
#include "Usul/Pointers/Pointers.h"

#include "osg/Array"

class Triangle : public Usul::Base::Object
{
public:
  typedef Usul::Base::Object          BaseClass;
  typedef osg::Vec3Array              Positions;
  typedef Positions::value_type       Position;
  typedef osg::ref_ptr < Positions >  PositionsPtr;
  typedef osg::Vec2Array              Vectors;
  typedef Vectors::value_type         Vector;
  typedef osg::ref_ptr < Vectors >    VectorsPtr;
  typedef Usul::Math::Vec3ui          Indices;

  enum CriticalPointType
  {
    NONE,
    SADDLE_POINT,
    ATTRACTING_NODE,
    REPELLING_NODE,
    ATTRACTING_FOCUS,
    REPELLING_FOCUS,
    CENTER
  };

  USUL_DECLARE_REF_POINTERS ( Triangle );

  Triangle ( const Indices& indices, Positions* positions, Vectors* vectors );

  const Indices&            indices() const;

  Usul::Math::Vec3d         barycentric( const Position& p ) const;
  bool                      pointInside ( const Position& p ) const;

  Vector                    vectorAtPoint ( const Position& p ) const;

  CriticalPointType         classifyCriticalPoint ( const Position& p ) const;
  Position                  findCriticalPoint () const;

protected:
  virtual ~Triangle();

  void                 _findMatrixValues();

private:
  PositionsPtr _positions;
  VectorsPtr   _vectors;
  Indices      _indices;
  Usul::Math::Vec4d _a;
  Usul::Math::Vec2d _b;
};


#endif // __VECTOR_FIELD_TRIANGLE_H__
