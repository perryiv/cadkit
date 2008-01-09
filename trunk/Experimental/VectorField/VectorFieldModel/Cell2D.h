
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __VECTOR_FIELD_CELL_TWO_D_H__
#define __VECTOR_FIELD_CELL_TWO_D_H__

#include "Triangle.h"

#include "Usul/Base/Object.h"
#include "Usul/Pointers/Pointers.h"
#include "Usul/Math/Vector4.h"

class Cell2D : public Usul::Base::Object
{
public:
  typedef Usul::Base::Object      BaseClass;
  typedef Triangle::Positions     Positions;
  typedef Triangle::Position      Position;
  typedef Triangle::PositionsPtr  PositionsPtr;
  typedef Triangle::Vectors       Vectors;
  typedef Triangle::Vector        Vector;
  typedef Triangle::VectorsPtr    VectorsPtr;
  typedef Usul::Math::Vec4ui      Indices;

  USUL_DECLARE_REF_POINTERS( Cell2D );

  enum
  {
    UPPER_LEFT = 0,
    UPPER_RIGHT = 1,
    LOWER_LEFT = 2,
    LOWER_RIGHT = 3
  };

  Cell2D( const Indices& indices, Positions* positions, Vectors* vectors );

  Triangle*         upper();
  const Triangle*   upper() const;

  Triangle*         lower();
  const Triangle*   lower() const;

  const Indices&    indices() const;

protected:
  // Use reference counting.
  virtual ~Cell2D();

private:
  Indices _indices;
  PositionsPtr _positions;
  VectorsPtr _vectors;
  Triangle::RefPtr _t0;
  Triangle::RefPtr _t1;
};


#endif // __VECTOR_FIELD_CELL_TWO_D_H__
