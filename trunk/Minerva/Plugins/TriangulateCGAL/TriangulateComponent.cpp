
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

#include "Minerva/Plugins/TriangulateCGAL/TriangulateComponent.h"

#ifdef _MSC_VER
# pragma warning ( disable : 4996 )
#endif

#include "CGAL/Exact_predicates_inexact_constructions_kernel.h"
#include "CGAL/Constrained_Delaunay_triangulation_2.h"
#include "CGAL/Delaunay_mesher_2.h"
#include "CGAL/Delaunay_mesh_face_base_2.h"
#include "CGAL/Delaunay_mesh_size_criteria_2.h"

#include <CGAL/Triangulation_euclidean_traits_xy_3.h>
#include <CGAL/Delaunay_triangulation_2.h>

// Typedefs.
struct K : public CGAL::Exact_predicates_inexact_constructions_kernel {};
typedef CGAL::Triangulation_vertex_base_2<K> Vb;
typedef CGAL::Delaunay_mesh_face_base_2<K> Fb;
typedef CGAL::Triangulation_data_structure_2<Vb, Fb> Tds;
typedef CGAL::Constrained_Delaunay_triangulation_2<K, Tds> CDT;
typedef CGAL::Delaunay_mesh_size_criteria_2<CDT> Criteria;
typedef CDT::Vertex_handle Vertex_handle;
typedef CDT::Point Point;
typedef CGAL::Delaunay_mesher_2<CDT, Criteria> Mesher;



USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( TriangulateComponent, TriangulateComponent::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
// Constructor
//
///////////////////////////////////////////////////////////////////////////////

TriangulateComponent::TriangulateComponent()
{
}


///////////////////////////////////////////////////////////////////////////////
//
// Destructor
//
///////////////////////////////////////////////////////////////////////////////

TriangulateComponent::~TriangulateComponent() 
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *TriangulateComponent::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::ITriangulate::IID:
    return static_cast < Usul::Interfaces::ITriangulate*>(this);
  case Usul::Interfaces::IPlugin::IID:
    return static_cast < Usul::Interfaces::IPlugin* > ( this );
  default:
    return 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Trianglulate.  The z component is ignored.
//
///////////////////////////////////////////////////////////////////////////////

void TriangulateComponent::triangulate ( const Vertices3D &in, Vertices3D &verticesOut )
{
  CDT cdt;

  // Vector to store the handles.
  std::vector < Vertex_handle > handles;

  // Copy vertices into proper data structure for triangulation.
  for ( Vertices3D::const_iterator vertexIter = in.begin(); vertexIter != in.end(); ++vertexIter )
  {
    handles.push_back ( cdt.insert( Point( (*vertexIter)[0], (*vertexIter)[1] ) ) );
  }

  if( handles.front() == handles.back() )
    handles.pop_back();

  // Hook up the connectivity.
  for ( unsigned int i = 0; i < handles.size() - 1; ++ i )
  {
    Vertex_handle va ( handles.at ( i ) );
    Vertex_handle vb ( handles.at ( i + 1 ) );
    
    if ( va != vb )
      cdt.insert_constraint( va, vb );
  }

  // Connect the end to the beginning
  cdt.insert_constraint( handles.at( handles.size() - 1 ), handles.at( 0 ) );

  // Create the mesh
  Mesher mesher(cdt);
  mesher.refine_mesh();

  const unsigned int numFaces ( cdt.number_of_faces() );
  const unsigned int numVertices ( cdt.number_of_vertices() );

  // Make enough room.
  verticesOut.reserve ( numVertices );

  // Loop through the faces.
  for( CDT::Finite_faces_iterator faceIter = cdt.finite_faces_begin(); faceIter != cdt.finite_faces_end(); ++faceIter )
  {
    // Only add the triangle if it's inside the orginal polygon
    if( faceIter->is_in_domain() )
    {
      // Get the points
      Point p0 ( faceIter->vertex( 0 )->point() );
      Point p1 ( faceIter->vertex( 1 )->point() );
      Point p2 ( faceIter->vertex( 2 )->point() );

      typedef Vertices3D::value_type Vertex;

      Vertex v0 ( p0.x(), p0.y(), 0.0 );
      Vertex v1 ( p1.x(), p1.y(), 0.0 );
      Vertex v2 ( p2.x(), p2.y(), 0.0 );

      verticesOut.push_back ( v0 );
      verticesOut.push_back ( v1 );
      verticesOut.push_back ( v2 );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Usul::Interfaces::IPlugin implementation
//
///////////////////////////////////////////////////////////////////////////////

std::string TriangulateComponent::getPluginName() const 
{
  return "Triangulate CGAL";
}
