
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Document for triangle models.
//
///////////////////////////////////////////////////////////////////////////////

#include "OsgTools/Triangles/TriangleSet.h"

#include "Usul/MPL/StaticAssert.h"
#include "Usul/Errors/Assert.h"
#include "Usul/Policies/Update.h"
#include "Usul/Interfaces/IProgressBar.h"
#include "Usul/Interfaces/IStatusBar.h"
#include "Usul/Interfaces/IFlushEvents.h"

#include <algorithm>
#include <functional>

using namespace OsgTools::Triangles;

USUL_IMPLEMENT_TYPE_ID ( TriangleSet );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

TriangleSet::TriangleSet() : BaseClass(),
  _shared    (),
  _triangles (),
  _vertices  ( new osg::Vec3Array ),
  _normals   ( new osg::Vec3Array, new osg::Vec3Array ),
  _colors    ( new osg::Vec4Array ),
  _dirty     ( true ),
  _geometry  ( new osg::Geometry );
{
  USUL_STATIC_ASSERT ( 12 == sizeof ( _shared         ) );
  USUL_STATIC_ASSERT ( 16 == sizeof ( _triangles      ) );
  USUL_STATIC_ASSERT (  4 == sizeof ( _vertices       ) );
  USUL_STATIC_ASSERT (  8 == sizeof ( _normals        ) );
  USUL_STATIC_ASSERT (  4 == sizeof ( _colors         ) );
  USUL_STATIC_ASSERT (  1 == sizeof ( _dirty          ) );
  USUL_STATIC_ASSERT ( 60 == sizeof ( TriangleSet     ) ); // Why?
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

TriangleSet::~TriangleSet()
{
  // Explicitely clear because of circular references.
  this->clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear the data.
//
///////////////////////////////////////////////////////////////////////////////

void TriangleSet::clear()
{
  // Clear the map of shared vertices. (It is most likely already empty.)
  _shared.clear();

  // Because shared vertices and triangles reference each other, we have to 
  // explicitely tell each triangle to unreference its vertices.
  std::for_each ( _triangles.begin(), _triangles.end(), std::mem_fun ( Triangle::clear ) );

  // All triangles should have a reference count of one.
  for ( Triangles::const_iterator i = _triangles.begin(); i != _triangles.end(); ++i )
    USUL_ASSERT ( 1 == (*i)->refCount() );

  // Now clear the sequence of triangles. The triangles should get deleted.
  _triangles.clear();

  // Clear the vertices, normals, and colors.
  _vertices->clear();
  this->_normalsPerVertex().clear();
  this->_normalsPerFacet().clear();
  _colors->clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make space for the triangles.
//
///////////////////////////////////////////////////////////////////////////////

void TriangleSet::reserve ( unsigned int num )
{
  // Reserve triangle, vertices, and per-facet normals.
  _triangles.reserve ( num );
  _vertices->reserve ( 3 * num );
  this->_normalsPerFacet().reserve ( num );

  // Note: Colors are user-defined, and per-vertex normals are calculated 
  // when done adding triangles.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Flip the normal vectors.
//
///////////////////////////////////////////////////////////////////////////////

void TriangleSet::flipNormals()
{
  // Loop through the normals and negate them.
  std::transform ( this->_normalsPerVertex().begin(), this->_normalsPerVertex().end(), this->_normalsPerVertex().begin(), std::negate<osg::Vec3f>() );
  std::transform ( this->_normalsPerFacet().begin(),  this->_normalsPerFacet().end(),  this->_normalsPerFacet().begin(),  std::negate<osg::Vec3f>() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the vertex of the i'th triangle.
//
///////////////////////////////////////////////////////////////////////////////

const osg::Vec3f &TriangleSet::vertex0 ( unsigned int i ) const
{
  unsigned int v ( _triangles.at(i)->vertex0()->index() );
  return _vertices->at ( v );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the vertex of the i'th triangle.
//
///////////////////////////////////////////////////////////////////////////////

const osg::Vec3f &TriangleSet::vertex1 ( unsigned int i ) const
{
  unsigned int v ( _triangles.at(i)->vertex1()->index() );
  return _vertices->at ( v );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the vertex of the i'th triangle.
//
///////////////////////////////////////////////////////////////////////////////

const osg::Vec3f &TriangleSet::vertex2 ( unsigned int i ) const
{
  unsigned int v ( _triangles.at(i)->vertex2()->index() );
  return _vertices->at ( v );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the normal of the i'th triangle.
//
///////////////////////////////////////////////////////////////////////////////

const osg::Vec3f &TriangleSet::normal ( unsigned int i ) const
{
  return this->_normalsPerFacet().at(i);
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add the triangles.
//
///////////////////////////////////////////////////////////////////////////////

void TriangleSet::addTriangle ( const osg::Vec3f &v0, const osg::Vec3f &v1, const osg::Vec3f &v2, const osg::Vec3f &n )
{
  // Get or make the shared vertices.
  SharedVertex *sv0 ( this->_sharedVertex ( v0 ) );
  SharedVertex *sv1 ( this->_sharedVertex ( v1 ) );
  SharedVertex *sv2 ( this->_sharedVertex ( v2 ) );

  // Make the new triangle.
  Triangle::ValidRefPtr t ( new Triangle ( sv0, sv1, sv2 ) );

  // Append it to the list.
  _triangles.push_back ( t.get() );

  // Add new triangle to the shared vertices.
  sv0->add ( t.get() );
  sv1->add ( t.get() );
  sv2->add ( t.get() );

  // Append normal vector.
  this->_normalsPerFacet().push_back ( n );

  // Need to rebuild per-vertex normals and indices.
  _dirty = true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the shared-vertex map.
//
///////////////////////////////////////////////////////////////////////////////

void TriangleSet::addStart ( Usul::Interfaces::IUnknown *caller )
{
  typedef Usul::Interfaces::IProgressBar IProgressBar;
  typedef Usul::Interfaces::IStatusBar IStatusBar;

  // Get the interfaces.
  IProgressBar::QueryPtr progress ( caller );
  IStatusBar::QueryPtr status ( caller );

  // Update every second.
  Usul::Policies::TimeBased update ( 1000 );

  // Set status bar.
  if ( status.valid() )
    status->setStatusBarText ( "Initializing map of shared vertices...", true );

  // Loop through the triangles.
  for ( Triangles::const_iterator i = _triangles.begin(); i != _triangles.end(); ++i )
  {
    // Get the triangle.
    Triangle::ValidRefPtr t ( i->get() );

    // Get the shared vertices.
    SharedVertex::ValidRefPtr sv0 ( t->vertex0() );
    SharedVertex::ValidRefPtr sv1 ( t->vertex1() );
    SharedVertex::ValidRefPtr sv2 ( t->vertex2() );

    // Get the indices.
    const unsigned int i0 ( sv0->index() );
    const unsigned int i1 ( sv1->index() );
    const unsigned int i2 ( sv2->index() );

    // Set the map's key to the shared vertex.
    _shared[_vertices->at(i0)] = sv0;
    _shared[_vertices->at(i1)] = sv1;
    _shared[_vertices->at(i2)] = sv2;

    // Feedback.
    if ( progress.valid() && update() )
      progress->updateProgressBar ( ( i - _triangles.begin() ) / _triangles.size() );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear the shared-vertex map.
//
///////////////////////////////////////////////////////////////////////////////

void TriangleSet::addFinish ( Usul::Interfaces::IUnknown *caller )
{
  // Get the interface.
  typedef Usul::Interfaces::IStatusBar IStatusBar;
  IStatusBar::QueryPtr status ( caller );

  // Set status bar.
  if ( status.valid() )
    status->setStatusBarText ( "Clearing map of shared vertices...", true );

  // Clear the map.
  _shared.clear();

  // Set status bar.
  if ( status.valid() )
    status->setStatusBarText ( "Done clearing map of shared vertices", true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get or make the shared vertex.
//
///////////////////////////////////////////////////////////////////////////////

SharedVertex *TriangleSet::_sharedVertex ( const osg::Vec3f &v )
{
  // Look for the shared vertex.
  SharedVertices::iterator i = _shared.find ( v );
  if ( _shared.end() != i )
    return i->second.get();

  // If we get to here then append to the vertices.
  _vertices->push_back ( v );

  // Make shared vertex with proper index.
  SharedVertex::ValidRefPtr sv ( new SharedVertex ( _vertices->size() - 1  ) );
  _shared.insert ( SharedVertices::value_type ( v, sv.get() ) );

  // Return the new shared vertex.
  return sv.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene. TODO: use indexed arrays.
//  See http://www.nps.navy.mil/cs/sullivan/osgtutorials/osgGeometry.html
//
///////////////////////////////////////////////////////////////////////////////

osg::Node *TriangleSet::buildScene ( const BaseClass::Options &opt, Unknown *caller )
{
  // Make copy of the options.
  Options options ( opt );

  // User feedback.
  this->setStatusBar ( "Building Scene ...", caller );

  // Start at zero.
  this->setProgressBar ( true, 0, 100, caller );

  // The scene root.
  osg::ref_ptr<osg::Group> root ( new osg::Group );
#if 0
  // A single geode.
  osg::ref_ptr<osg::Geode> geode ( new osg::Geode );
  root->addChild ( geode.get() );

  // Make geometry and add to geode.
  osg::ref_ptr<osg::Geometry> geom  ( new osg::Geometry );
  geode->addDrawable ( geom.get() );  

  // Make vertices and normals for the geometry.
  osg::ref_ptr<osg::Vec3Array> vertices ( new osg::Vec3Array );
  osg::ref_ptr<osg::Vec3Array> normals  ( new osg::Vec3Array );

  // Set vertices and normals.
  geom->setVertexArray ( vertices.get() );
  geom->setNormalArray ( normals.get() );

  // Need per-vertex normals for osg::Geometry's "fast path".
  geom->setNormalBinding ( osg::Geometry::BIND_PER_VERTEX );

  // Make space.
  const unsigned int numPoints ( _triangles->size() * 3 );
  vertices->reserve ( numPoints );
  normals->reserve  ( numPoints );

  // Shortcut to triangles.
  const Triangles::Polygons &triangles = _triangles->polygons();

  // Interpret every three osg::Vec3 as a triangle.
  geom->addPrimitiveSet ( new osg::DrawArrays ( osg::PrimitiveSet::TRIANGLES, 0, 3 * triangles.size() ) );

  // Should we use averaged normals?
  const bool average ( "average" == options["normals"] );

  // Initialize counter for progress.
  unsigned int count ( 0 );

  // Update progress bar every second.
  Usul::Policies::TimeBased elapsed ( 1000 );

  // Loop through the triangles.
  for ( Triangles::Polygons::const_iterator i = triangles.begin(); i != triangles.end(); ++i )
  {
    // Shortcuts to the triangle.
    const Triangle::ValidRefPtr triangle ( *i );

    // Get the vertices.
    const SharedVertex *v1 ( triangle->vertex1() );
    const SharedVertex *v2 ( triangle->vertex2() );
    const SharedVertex *v3 ( triangle->vertex3() );

    // Make sure we have good pointers.
    if ( !v1 || !v2 || !v3 )
      throw std::runtime_error ( "Error 2040664771: null vertex found when trying to build scene" );

    // Set the vertex values.
    vertices->push_back ( v1->value() );
    vertices->push_back ( v2->value() );
    vertices->push_back ( v3->value() );

    // Get the normals.
    NormalType n1 ( ( average ) ? v1->normal() : triangle->normal() );
    NormalType n2 ( ( average ) ? v2->normal() : triangle->normal() );
    NormalType n3 ( ( average ) ? v3->normal() : triangle->normal() );

    // Make sure they are normalized.
    n1.normalize();
    n2.normalize();
    n3.normalize();

    // Set the normal values.
    normals->push_back ( n1 );
    normals->push_back ( n2 );
    normals->push_back ( n3 );

    // Show progress.
    this->setProgressBar ( elapsed(), count, triangles.size(), caller );
    ++count;
  }

  // Return the root.
  return root.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Convenience function to set status bar and flush events.
//
///////////////////////////////////////////////////////////////////////////////

void TriangleSet::setStatusBar ( const std::string &text, Unknown *caller )
{
  Usul::Interfaces::IStatusBar::QueryPtr status ( caller );
  if ( status.valid() )
    status->setStatusBarText ( text, true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Convenience function to show progress and flush events.
//
///////////////////////////////////////////////////////////////////////////////

void TriangleSet::setProgressBar ( bool state, unsigned int numerator, unsigned int denominator, Unknown *caller )
{
  // If we should...
  if ( state )
  {
    // Report progress.
    Usul::Interfaces::IProgressBar::QueryPtr progress ( caller );
    if ( progress.valid() )
    {
      float n ( static_cast < float > ( numerator ) );
      float d ( static_cast < float > ( denominator ) );
      float fraction ( n / d );
      progress->updateProgressBar ( static_cast < unsigned int > ( fraction * 100 ) );
    }

    // Give user opportunity to cancel.
    Usul::Interfaces::IFlushEvents::QueryPtr flush ( caller );
    if ( flush.valid() )
      flush->flushEventQueue();
  }
}
