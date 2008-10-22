
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Core/Extents.h"
#include "Minerva/Core/Functions/ParameterizeExtents.h"

#include "Usul/Math/Vector2.h"
#include "Usul/Predicates/CloseFloat.h"

#include "osg/Vec2d"

#include "gtest/gtest.h"

typedef Minerva::Core::Extents<osg::Vec2d> Extents;

inline bool operator== ( const Usul::Math::Vec4d& lhs, const Usul::Math::Vec4d& rhs )
{
  return Usul::Predicates::CloseFloat<double>::compare ( lhs[0], rhs[0], 3 ) &&
         Usul::Predicates::CloseFloat<double>::compare ( lhs[1], rhs[1], 3 ) &&
         Usul::Predicates::CloseFloat<double>::compare ( lhs[2], rhs[2], 3 ) &&
         Usul::Predicates::CloseFloat<double>::compare ( lhs[3], rhs[3], 3 );
}

inline std::ostream& operator<< ( std::ostream& os, const Usul::Math::Vec4d& v )
{
  const unsigned int w ( 15 ), p ( 10 );
  os << std::setw ( w ) << std::setprecision ( p ) << std::fixed << v[0] << " ";
  os << std::setw ( w ) << std::setprecision ( p ) << std::fixed << v[1] << " ";
  os << std::setw ( w ) << std::setprecision ( p ) << std::fixed << v[2] << " ";
  os << std::setw ( w ) << std::setprecision ( p ) << std::fixed << v[3] << " ";
  return os;
}

///////////////////////////////////////////////////////////////////////////////
//
//  Parameterize an extents within another extents.
//
///////////////////////////////////////////////////////////////////////////////

TEST(ExtentsTest,Parameterize)
{
  const Extents e0 ( 0, 0, 15, 15 );
  const Extents e1 ( 0, 0, 10, 10 );
  const Extents e2 ( 5, 5, 10, 10 );

  Usul::Math::Vec4d t0 ( Minerva::Core::Functions::paramertizeExtents ( e0, e0 ) );
  Usul::Math::Vec4d t1 ( Minerva::Core::Functions::paramertizeExtents ( e1, e2 ) );
  
  // Should be true.
  EXPECT_EQ ( Usul::Math::Vec4d ( 0.0, 1.0, 0.0, 1.0 ), t0 );
  EXPECT_EQ ( Usul::Math::Vec4d ( 0.5, 1.0, 0.5, 1.0 ), t1 );

  EXPECT_THROW ( Minerva::Core::Functions::paramertizeExtents ( e1, e0 ), std::invalid_argument );
}
