

#include "PolyData.h"

#include "Usul/Math/Vector3.h"

#include "OsgTools/Triangles/TriangleSet.h"

#include "vtkPolyData.h"
#include "vtkCellArray.h"
#include "vtkTriangle.h"

using namespace VTKTools::Convert;


///////////////////////////////////////////////////////////////////////////////
//
//  Convert vtkPolyData to the TriangleSet.
//
///////////////////////////////////////////////////////////////////////////////

void PolyData::polyDataToTriangleSet ( vtkPolyData *data, OsgTools::Triangles::TriangleSet *triangleSet )
{
  // Typedefs
  typedef OsgTools::Triangles::SharedVertex SharedVertex;
  typedef std::vector < SharedVertex::ValidRefPtr > SharedVertices;
  typedef std::vector < Usul::Math::Vec3ui > Indices;
  typedef std::vector < unsigned int > Counts;
  typedef OsgTools::Triangles::TriangleSet TriangleSet;
  typedef TriangleSet::VerticesPtr::element_type Vertices;
  typedef TriangleSet::NormalsPtr::element_type Normals;

  // Clear the data that we have.
  triangleSet->clear();

  // Get the points and triangles.
  vtkPoints *points ( data->GetPoints() );
  vtkCellArray *tris ( data->GetPolys() );

  const unsigned int numVertices  ( points->GetNumberOfPoints() );
  const unsigned int numTriangles ( tris->GetNumberOfCells()    );

  SharedVertices shared;
  Indices indices;
  Counts counts;
  Vertices &vertices ( *triangleSet->vertices() );
  Normals &normalsT  ( *triangleSet->normalsT() );
  Normals &normalsV  ( *triangleSet->normalsV() );

  // Make space
  shared.reserve ( numVertices );
  indices.reserve ( numTriangles );
  counts.resize ( numVertices, 0 );
  vertices.resize( numVertices );
  normalsV.resize( numVertices );
  normalsT.reserve( numTriangles );

  // Copy the point data into vertices
  for ( int i = 0; i < points->GetNumberOfPoints(); ++i )
  {
    osg::Vec3 &t ( vertices.at ( i ) );
    double d[3];
    points->GetPoint ( i, d );
    t[0] = d[0];
    t[1] = d[1];
    t[2] = d[2];
    triangleSet->updateBounds ( t );
  }

  // Build the container of vertex-usage counts.
  vtkIdType numPoints ( 0 );
  vtkIdType* ids ( 0x0 );

  // Loop throught the triangles.
  for ( tris->InitTraversal(); tris->GetNextCell(numPoints,ids); )
  {
    USUL_ASSERT ( numPoints == 3 );

    // Add the triangle indices
    indices.push_back ( Usul::Math::Vec3ui ( ids[0], ids[1], ids[2] ) );

    // Increment the counts.
    counts.at ( ids[0] ) += 1;
    counts.at ( ids[1] ) += 1;
    counts.at ( ids[2] ) += 1;

    // Compute the normal
    double normal[3];
    vtkTriangle::ComputeNormal( points, numPoints, ids, normal );
    normalsT.push_back ( osg::Vec3 ( normal[0], normal[1], normal[2] ) );
  }

  // Build the vector of shared vertices.
  for ( unsigned int i = 0; i < numVertices; ++i )
  {
    // Declare new shared-vertex and tell it how many triangles to reserve.
    // This assumes all vertices in the file are unique.
    shared.push_back ( triangleSet->newSharedVertex ( i, counts[i] ) );
  }

  // Shortcut to the map.
  typedef TriangleSet::SharedVertices VertexMap;
  VertexMap &sharedMap ( triangleSet->sharedVertices() );

  // Build the map of shared vertices.
  for ( unsigned int i = 0; i < numVertices; ++i )
  {
    // Add it to the map.
    sharedMap.insert ( VertexMap::value_type ( vertices[i], shared[i].get() ) );
  }

  typedef TriangleSet::TriangleVector TriangleVector;
  typedef OsgTools::Triangles::Triangle Triangle;

  // Shortcut to the triangles.
  TriangleVector &triangles ( triangleSet->triangles() );

  triangles.reserve ( numTriangles );

  // Build the triangles.
  for ( unsigned int i = 0; i < numTriangles; ++i )
  {
    // Get the index.
    const Indices::value_type &index ( indices[i] );

    // Get the shared vertices.
    SharedVertex::ValidRefPtr &sv0 ( shared.at ( index[0] ) );
    SharedVertex::ValidRefPtr &sv1 ( shared.at ( index[1] ) );
    SharedVertex::ValidRefPtr &sv2 ( shared.at ( index[2] ) );

    // Append new triangle to the list.
    Triangle::ValidRefPtr t ( triangleSet->newTriangle ( sv0, sv1, sv2, i ) );
    triangles.push_back ( t.get() );

    // This is an "original" triangle.
    t->original ( true );

    // Should be true because of the way we constructed them.
    USUL_ASSERT ( sv0->capacity() >= sv0->numTriangles() );
    USUL_ASSERT ( sv1->capacity() >= sv1->numTriangles() );
    USUL_ASSERT ( sv2->capacity() >= sv2->numTriangles() );

  }

  // Build per-vertex normals.
  // Loop through the shared vertices and update the normal.
  for ( unsigned int i = 0; i < numVertices; ++i )
  {
    osg::Vec3 &n ( normalsV[i] );
    n = triangleSet->averageNormal( shared[i] );
    n.normalize();
    //normalsV[i] = triangleSet->averageNormal ( shared[i] ).normalize();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Convert a TriangleSet to vtkPolyData.
//
///////////////////////////////////////////////////////////////////////////////

void PolyData::triangleSetToPolyData ( OsgTools::Triangles::TriangleSet *triangleSet, vtkPolyData *data )
{
  unsigned int numTriangles ( triangleSet->numTriangles() );
  unsigned int numVertices  ( triangleSet->vertices()->size() );

  vtkPoints *points  ( vtkPoints::New() );
  vtkCellArray *tris ( vtkCellArray::New() );
  tris->Initialize();

  points->SetDataTypeToFloat();
  points->GetData()->SetNumberOfComponents(3);
  points->Allocate ( numVertices );
  tris->Allocate ( numTriangles );

  typedef OsgTools::Triangles::TriangleSet::TriangleVector TriangleVector;
  TriangleVector &triangles ( triangleSet->triangles() );
  osg::ref_ptr < osg::Vec3Array > vertices ( triangleSet->vertices() );

  USUL_ASSERT ( sizeof ( osg::Vec3Array::value_type::value_type ) == sizeof ( points->GetData()->GetDataTypeSize() ) );
  
  //::memcpy ( points->GetData(), &vertices->front(), vertices->size() );
  for ( osg::Vec3Array::const_iterator iter = vertices->begin(); iter != vertices->end(); ++iter )
  {
    points->InsertNextPoint ( iter->ptr() );
  }

  for ( TriangleVector::iterator iter = triangles.begin(); iter != triangles.end(); ++iter )
  {
    vtkIdType pts[3];
    pts[0] = (*iter)->vertex0()->index();
    pts[1] = (*iter)->vertex1()->index();
    pts[2] = (*iter)->vertex2()->index();
    tris->InsertNextCell(3,pts);
  }

  data->SetPoints ( points );
  data->SetPolys ( tris );

  tris->Delete();
  points->Delete();
}
