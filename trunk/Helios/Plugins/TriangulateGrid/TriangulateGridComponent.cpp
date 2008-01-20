
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Triangulate component.
//
///////////////////////////////////////////////////////////////////////////////

#include "Helios/Plugins/TriangulateGrid/TriangulateGridComponent.h"

#include "Usul/Components/Factory.h"
#include "Usul/Predicates/CloseFloat.h"

#include "OsgTools/Triangles/TriangleSet.h"

USUL_DECLARE_COMPONENT_FACTORY ( TriangulateGridComponent );
USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( TriangulateGridComponent, TriangulateGridComponent::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
// Constructor
//
///////////////////////////////////////////////////////////////////////////////

TriangulateGridComponent::TriangulateGridComponent()
{
}


///////////////////////////////////////////////////////////////////////////////
//
// Destructor
//
///////////////////////////////////////////////////////////////////////////////

TriangulateGridComponent::~TriangulateGridComponent() 
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *TriangulateGridComponent::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::ITriangulateGrid::IID:
    return static_cast < Usul::Interfaces::ITriangulateGrid*>(this);
  case Usul::Interfaces::IPlugin::IID:
    return static_cast < Usul::Interfaces::IPlugin* > ( this );
  default:
    return 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build a triangle set from a grid of data.  GridResolution is the size of a grid cell in the x and y directions.
//
///////////////////////////////////////////////////////////////////////////////

OsgTools::Triangles::TriangleSet*  TriangulateGridComponent::triangulateGrid ( const Grid &grid, const GridResolution& resolution, ValueType noDataValue )
{
  // Make the triangle set.
  OsgTools::Triangles::TriangleSet::RefPtr triangleSet ( new OsgTools::Triangles::TriangleSet );

  // Close float predicate.
  Usul::Predicates::CloseFloat<ValueType> close ( 5 );

  const unsigned int rows    ( grid.rows() );
  const unsigned int columns ( grid.columns() );
  
  // Walk the grid.
  for ( unsigned int i = 0; i < rows - 1; ++i )
  {
    for ( unsigned int j = 0; j < columns - 1 ; ++j )
    {
      // Get the values of the square.
      const ValueType a ( grid.at ( i,     j     ) );
      const ValueType b ( grid.at ( i,     j + 1 ) );
      const ValueType c ( grid.at ( i + 1, j     ) );
      const ValueType d ( grid.at ( i + 1, j + 1 ) );

      // Check which corners are valid.
      const bool aIsValid ( !close ( a, noDataValue ) );
      const bool bIsValid ( !close ( b, noDataValue ) );
      const bool cIsValid ( !close ( c, noDataValue ) );
      const bool dIsValid ( !close ( d, noDataValue ) );

      const osg::Vec3::value_type startX ( j * resolution[0] );
      const osg::Vec3::value_type startY ( i * resolution[1] );

      osg::Vec3 v0 ( startX,                 startY,                 a ); // Point a.
      osg::Vec3 v1 ( startX + resolution[0], startY,                 b ); // Point b.
      osg::Vec3 v2 ( startX,                 startY + resolution[1], c ); // Point c.
      osg::Vec3 v3 ( startX + resolution[0], startY + resolution[1], d ); // Point d.

      // See if we should add a triangle for these 3 corners.
      if ( aIsValid && bIsValid && cIsValid )
      {
        osg::Vec3 t0 ( v2 - v1 );
        osg::Vec3 t1 ( v0 - v1 );

        osg::Vec3 normal ( t0 ^ t1 );
        normal.normalize();

        triangleSet->addTriangle ( v0, v1, v2, normal, false, true );
      }

      // See if we should add a triangle for these 3 corners.
      if ( bIsValid && cIsValid && dIsValid )
      {
        osg::Vec3 t0 ( v3 - v2 );
        osg::Vec3 t1 ( v1 - v2 );

        osg::Vec3 normal ( t0 ^ t1 );
        normal.normalize();

        triangleSet->addTriangle ( v1, v2, v3, normal * -1, false, true );
      }
    }
  }

  return triangleSet.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Usul::Interfaces::IPlugin implementation
//
///////////////////////////////////////////////////////////////////////////////

std::string TriangulateGridComponent::getPluginName() const 
{
  return "Triangulate Grid  ";
}
