
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
#include "Usul/Interfaces/ICoordinateTransform.h"
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

OsgTools::Triangles::TriangleSet* TriangulateGridComponent::triangulateGrid ( const Grid &grid, 
                                                                              const GridResolution& resolution, 
                                                                              const Usul::Math::Vec3d& o, 
                                                                              ValueType noDataValue,
                                                                              Usul::Interfaces::IUnknown* caller )
{
  // Query for needed interface.
  Usul::Interfaces::ICoordinateTransformOSG::QueryPtr transform ( caller );

  // Make the triangle set.
  unsigned int unitsInLastPlace ( 10 );
  OsgTools::Triangles::TriangleSet::RefPtr triangleSet ( new OsgTools::Triangles::TriangleSet ( unitsInLastPlace ) );

  // Close float predicate.
  Usul::Predicates::CloseFloat<ValueType> close ( unitsInLastPlace );

  // Keep a local cache of shared vertices. This works better because we know it's a regular grid.
  typedef std::pair < unsigned int, unsigned int > Indices;
  typedef OsgTools::Triangles::SharedVertex::RefPtr SharedVertexPtr;
  typedef std::map < Indices, SharedVertexPtr > SharedVertices;
  SharedVertices sv;

  const unsigned int rows    ( grid.rows() );
  const unsigned int columns ( grid.columns() );

  // Offset for each vertex.
  const osg::Vec3f offset ( o[0], o[1], o[2] );

  // Walk the grid.
  for ( unsigned int i = 0; i < rows - 1; ++i )
  {
    for ( unsigned int j = 0; j < columns - 1 ; ++j )
    {
      // Make indices.
      const Indices ia ( i,     j     );
      const Indices ib ( i,     j + 1 );
      const Indices ic ( i + 1, j     );
      const Indices id ( i + 1, j + 1 );

      // Get the values of the square.
      const ValueType a ( grid.at ( ia.first, ia.second ) );
      const ValueType b ( grid.at ( ib.first, ib.second ) );
      const ValueType c ( grid.at ( ic.first, ic.second ) );
      const ValueType d ( grid.at ( id.first, id.second ) );

      // Check which corners are valid.
      const bool aIsValid ( !close ( a, noDataValue ) );
      const bool bIsValid ( !close ( b, noDataValue ) );
      const bool cIsValid ( !close ( c, noDataValue ) );
      const bool dIsValid ( !close ( d, noDataValue ) );

      const osg::Vec3::value_type startX ( j * resolution[0] );
      const osg::Vec3::value_type startY ( i * resolution[1] );

      osg::Vec3f v0 ( offset + osg::Vec3f ( startX,                 startY,                 a  ) ); // Point a.
      osg::Vec3f v1 ( offset + osg::Vec3f ( startX + resolution[0], startY,                 b  ) ); // Point b.
      osg::Vec3f v2 ( offset + osg::Vec3f ( startX,                 startY + resolution[1], c ) ); // Point c.
      osg::Vec3f v3 ( offset + osg::Vec3f ( startX + resolution[0], startY + resolution[1], d ) ); // Point d.

      if ( transform.valid() )
      {
        transform->transformCoordinate ( v0 );
        transform->transformCoordinate ( v1 );
        transform->transformCoordinate ( v2 );
        transform->transformCoordinate ( v3 );
      }

      // See if we should add a triangle for these 3 corners.
      if ( aIsValid && bIsValid && cIsValid )
      {
        const osg::Vec3 t0 ( v2 - v1 );
        const osg::Vec3 t1 ( v0 - v1 );
        osg::Vec3 normal ( t0 ^ t1 );
        normal.normalize();

        // Get the shared vertices.
        SharedVertexPtr sv0 ( ( true == sv[ia].valid() ) ? sv[ia].get() : triangleSet->addSharedVertex ( v0, false ) );
        SharedVertexPtr sv1 ( ( true == sv[ib].valid() ) ? sv[ib].get() : triangleSet->addSharedVertex ( v1, false ) );
        SharedVertexPtr sv2 ( ( true == sv[ic].valid() ) ? sv[ic].get() : triangleSet->addSharedVertex ( v2, false ) );

        // Add the triangle.
        triangleSet->addTriangle ( sv0, sv1, sv2, normal, false );

        // Put shared vertices in our map. This is redundant but that's ok.
        sv[ia] = sv0;
        sv[ib] = sv1;
        sv[ic] = sv2;
      }

      // See if we should add a triangle for these 3 corners.
      if ( bIsValid && cIsValid && dIsValid )
      {
        const osg::Vec3 t0 ( v3 - v2 );
        const osg::Vec3 t1 ( v1 - v2 );
        osg::Vec3 normal ( t1 ^ t0 );
        normal.normalize();

        // Get the shared vertices.
        SharedVertexPtr sv1 ( ( true == sv[ib].valid() ) ? sv[ib].get() : triangleSet->addSharedVertex ( v1, false ) );
        SharedVertexPtr sv2 ( ( true == sv[ic].valid() ) ? sv[ic].get() : triangleSet->addSharedVertex ( v2, false ) );
        SharedVertexPtr sv3 ( ( true == sv[id].valid() ) ? sv[id].get() : triangleSet->addSharedVertex ( v3, false ) );

        // Add the triangle. Note the order of vertices.
        triangleSet->addTriangle ( sv1, sv3, sv2, normal, false );

        // Put shared vertices in our map. This is redundant but that's ok.
        sv[ib] = sv1;
        sv[ic] = sv2;
        sv[id] = sv3;
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
  return "Triangulate Grid";
}
