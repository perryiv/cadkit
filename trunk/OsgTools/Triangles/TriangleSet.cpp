
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV & Adam Kubach
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
#include "OsgTools/Triangles/Constants.h"
#include "OsgTools/GlassBoundingBox.h"
#include "OsgTools/HasOption.h"

#include "Usul/MPL/StaticAssert.h"
#include "Usul/Errors/Assert.h"
#include "Usul/Policies/Update.h"
#include "Usul/Predicates/EqualVector.h"
#include "Usul/Bits/Bits.h"
#include "Usul/Functors/Increment.h"
#include "Usul/Errors/Checker.h"
#include "Usul/Resources/ProgressBar.h"
#include "Usul/Resources/StatusBar.h"
#include "Usul/Resources/EventQueue.h"
#include "Usul/Resources/TextWindow.h"
#include "Usul/Interfaces/IProgressBar.h"
#include "Usul/Interfaces/IStatusBar.h"
#include "Usul/Interfaces/IFlushEvents.h"
#include "Usul/Types/Types.h"

#include "osgUtil/IntersectVisitor"

#include "osg/Group"
#include "osg/Geode"
#include "osg/Geometry"
#include "osg/Vec4"
#include "osg/StateSet"
#include "osg/Material"

#include <algorithm>
#include <numeric>
#include <functional>
#include <limits>

using namespace OsgTools::Triangles;
using namespace Usul::Types;

USUL_IMPLEMENT_TYPE_ID ( TriangleSet );


///////////////////////////////////////////////////////////////////////////////
//
//  Constants for this file.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  const osg::Vec4f _defaultPerVertexColor ( 0.5f, 0.5f, 0.5f, 1.0f );
  const unsigned int _milliseconds ( 250 );
  const unsigned int _averageTrianglesPerBlock ( 5000 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

TriangleSet::TriangleSet() : BaseClass(),
  _shared    ( LessVector ( CloseFloat() ) ),
  _triangles (),
  _vertices  ( new osg::Vec3Array ),
  _normalsV  ( new osg::Vec3Array ),
  _normalsT  ( new osg::Vec3Array ),
  _colorsV   ( new osg::Vec4Array ),
  _flags     ( Dirty::NORMALS_V | Dirty::COLORS_V | Dirty::BLOCKS ),
  _bbox      (),
  _factory   ( new Factory ),
  _blocks    ( 0x0 ),
  _progress  ( 0, 1 ),
  _color     ( new ColorFunctor )
{
#ifdef _MSC_VER
  // Keeping tabs on memory consumption...
  USUL_STATIC_ASSERT ( sizeof ( TriangleSet ) < 200 );
#endif
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

void TriangleSet::clear ( Usul::Interfaces::IUnknown *caller )
{
  // Try to get a progress.
  Usul::Interfaces::IProgressBar::QueryPtr progress ( caller );

  // Update every second.
  Usul::Policies::TimeBased update ( 1000 );

  // Delete the blocks.
  _blocks = 0x0;
  this->dirtyBlocks ( true );

  // Clear the map of shared vertices.
  _shared.clear();

  // Used for progress feedback.
  const unsigned int numTriangles ( _triangles.size() );
  const unsigned int maxProgress ( numTriangles * 2 );
  unsigned int count ( 0 );

  // Loop through all the triangles.
  for ( unsigned int i = 0; i < numTriangles; ++i )
  {
    // Because shared vertices and triangles reference each other, we have to 
    // explicitely tell each triangle to unreference its vertices.
    _triangles[i]->clear();

    // Feedback.
    if ( progress.valid() && update() )
      progress->updateProgressBar ( (unsigned int) ( float ( count ) / float ( maxProgress ) * 100 ) );
    ++count;
  }

  // Loop through all the triangles. Yes, we need two loops.
  for ( unsigned int i = 0; i < numTriangles; ++i )
  {
    // Should be true.
    USUL_ASSERT ( 1 == _triangles[i]->refCount() );

    // This will delete the triangle.
    _triangles[i] = 0x0;

    // Feedback.
    if ( progress.valid() && update() )
      progress->updateProgressBar ( (unsigned int) ( float ( count ) / float ( maxProgress ) * 100 ) );
    ++count;
  }

  // Now clear the list of triangles.
  _triangles.clear();

  // Clear the vertices, normals, and colors.
  _vertices->clear();
  this->normalsV()->clear();
  this->normalsT()->clear();
  _colorsV->clear();

  // Reset the bounding box.
  _bbox.init();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make space for the triangles.
//
///////////////////////////////////////////////////////////////////////////////

void TriangleSet::reserve ( unsigned int num )
{
  // Reserve space. Since there are almost never more vertices than 
  // triangles, we reserve the same number.
  _triangles.reserve ( num );
  _vertices->reserve ( num );
  this->normalsT()->reserve ( num );
  _colorsV->reserve ( num );
  _factory->reserveTriangles ( num );
  _factory->reserveSharedVertices ( num );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Flip the normal vectors.
//
///////////////////////////////////////////////////////////////////////////////

void TriangleSet::flipNormals()
{
  // Loop through the normals and negate them.
  std::transform ( this->normalsV()->begin(), this->normalsV()->end(), this->normalsV()->begin(), std::negate<osg::Vec3f>() );
  std::transform ( this->normalsT()->begin(), this->normalsT()->end(), this->normalsT()->begin(), std::negate<osg::Vec3f>() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Flip the normal vector of the i'th triangle.
//
///////////////////////////////////////////////////////////////////////////////

void TriangleSet::flipNormal ( unsigned int i )
{
  Triangle *t ( _triangles.at ( i ) );

  this->normalsV()->at ( t->vertex0()->index() ) *= -1;
  this->normalsV()->at ( t->vertex1()->index() ) *= -1;
  this->normalsV()->at ( t->vertex2()->index() ) *= -1;

  this->normalsT()->at ( i ) *= -1;
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

const osg::Vec3f &TriangleSet::triangleNormal ( unsigned int i ) const
{
  return this->normalsT()->at(i);
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return a correct normal vector.
//
///////////////////////////////////////////////////////////////////////////////

void TriangleSet::correctNormal ( const Triangle *t, osg::Vec3f &normal ) const
{
  this->correctNormal ( t->vertex0(), t->vertex1(), t->vertex2(), normal );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return a correct normal vector.
//
///////////////////////////////////////////////////////////////////////////////

void TriangleSet::correctNormal ( const SharedVertex *sv0, const SharedVertex *sv1, const SharedVertex *sv2, osg::Vec3f &normal ) const
{
  // Get the three vertices of the triangle.
  const osg::Vec3f one   ( _vertices->at ( sv0->index() ) );
  const osg::Vec3f two   ( _vertices->at ( sv1->index() ) );
  const osg::Vec3f three ( _vertices->at ( sv2->index() ) );

  // Get the center of the triangle.
  const osg::Vec3f center ( ( one + two + three ) / 3 );

  // Get the center of the model.
  const osg::Vec3f cg ( _bbox.center() );

  // Make the reference vector.
  const osg::Vec3f refVector ( center - cg );

  // Flip the normal if it's pointing towards the center. 
  // The '*' operator is overloaded as the dot product for osg::Vec3.
  if ( ( refVector * normal ) < 0 )
    normal = -normal;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add the triangles.
//
///////////////////////////////////////////////////////////////////////////////

void TriangleSet::addTriangle ( const osg::Vec3f &v0, const osg::Vec3f &v1, const osg::Vec3f &v2, const osg::Vec3f &n, bool update )
{
  // Get or make the shared vertices.
  SharedVertex *sv0 ( this->addSharedVertex ( v0 ) );
  SharedVertex *sv1 ( this->addSharedVertex ( v1 ) );
  SharedVertex *sv2 ( this->addSharedVertex ( v2 ) );

  // Add the triangle
  this->addTriangle ( sv0, sv1, sv2, n, update );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a triangle.
//
///////////////////////////////////////////////////////////////////////////////

void TriangleSet::addTriangle ( SharedVertex *sv0, SharedVertex *sv1, SharedVertex *sv2, const osg::Vec3f &n, bool update )
{
  // Make the new triangle.
  Triangle::ValidRefPtr t ( this->newTriangle ( sv0, sv1, sv2, _triangles.size() ) );

  // Append it to the list.
  _triangles.push_back ( t.get() );

  // Add normal vector for this triangle. We have to add this now.
  this->normalsT()->push_back ( n );

  // Update the bounds.
  this->updateBounds ( t );

  // Set triangle's flag if appropriate.
  t->problem ( sv0->problem() || sv1->problem() || sv2->problem() );

  // If we are supposed to update everything now...
  if ( update )
    this->_updateDependencies ( t );

  // Otherwise, these things are dirty.
  else
  {
    this->dirtyBlocks   ( true );
    this->dirtyNormalsV ( true );
    this->dirtyColorsV  ( true );
    sv0->dirtyColor ( true );
    sv1->dirtyColor ( true );
    sv2->dirtyColor ( true );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove the specified triangle.
//
///////////////////////////////////////////////////////////////////////////////

void TriangleSet::removeTriangle ( const osg::Drawable *d, unsigned int i )
{
  // Get the triangle.
  // TODO: Not sure if this gives correct block in all cases because you use 
  // vertex0 when initially putting triangle into blocks...
  Block::RefPtr block ( ( d ) ? _blocks->block ( d->getBound().center() ) : 0x0 );
  Triangle::RefPtr t ( ( block.valid() ) ? block->triangle ( i ) : 0x0 );
  if ( false == t.valid() )
    return;

  // Remove the triangle from the sequence.
  _triangles.erase ( _triangles.begin() + t->index() );

  // Remove the corresponding normal.
  this->normalsT()->erase ( this->normalsT()->begin() + t->index() );

  // Remove the triangle from the block.
  block->removeTriangle ( i );

  // Tell shared-vertices to remove this triangle.
  t->vertex0()->removeTriangle ( t );
  t->vertex1()->removeTriangle ( t );
  t->vertex2()->removeTriangle ( t );

  // Need to update these.
  this->dirtyNormalsV ( true );
  this->dirtyColorsV ( true );
  this->dirtyColorsV ( true );
  t->vertex0()->dirtyColor ( true );
  t->vertex1()->dirtyColor ( true );
  t->vertex2()->dirtyColor ( true );

  // Remove references to help it get deleted.
  t->clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update everything that depends on this triangle.
//
///////////////////////////////////////////////////////////////////////////////

void TriangleSet::_updateDependencies ( Triangle *t )
{
  // Handle bad input.
  if ( 0x0 == t )
    return;

  // Make sure the per-triangle normal is correct.
  this->correctNormal ( t, this->normalsT()->at ( t->index() ) );

  // Get the shared-vertices.
  SharedVertex *sv0 ( t->vertex0() );
  SharedVertex *sv1 ( t->vertex1() );
  SharedVertex *sv2 ( t->vertex2() );

  // Make sure the per-vertex normals are correct.
  this->_updateNormalV ( sv0 );
  this->_updateNormalV ( sv1 );
  this->_updateNormalV ( sv2 );

  // Make sure the per-vertex colors are correct.
  this->_updateColorV ( sv0 );
  this->_updateColorV ( sv1 );
  this->_updateColorV ( sv2 );

  // Get the vertices.
  const osg::Vec3f &v0 ( _vertices->at ( sv0->index() ) );
  const osg::Vec3f &v1 ( _vertices->at ( sv1->index() ) );
  const osg::Vec3f &v2 ( _vertices->at ( sv2->index() ) );

  // Make sure our bounding box is updated.
  this->updateBounds ( v0 );
  this->updateBounds ( v1 );
  this->updateBounds ( v2 );

  // Add the triangle to the blocks. Note: blocks should be able to handle 
  // vertices that fall outside of their bounds.
  if ( _blocks.valid() )
    _blocks->addTriangle ( this, t );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the bounding box.
//
///////////////////////////////////////////////////////////////////////////////

void TriangleSet::updateBounds ( Triangle *t ) 
{
  this->updateBounds ( t->vertex0() );
  this->updateBounds ( t->vertex1() );
  this->updateBounds ( t->vertex2() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the bounding box.
//
///////////////////////////////////////////////////////////////////////////////

void TriangleSet::updateBounds ( SharedVertex *sv ) 
{
  this->updateBounds ( this->getVertex ( sv->index() ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the bounding box.
//
///////////////////////////////////////////////////////////////////////////////

void TriangleSet::updateBounds ( const osg::Vec3f &v ) 
{
  _bbox.expandBy ( v );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make the shared vertex.
//
///////////////////////////////////////////////////////////////////////////////

SharedVertex* TriangleSet::addSharedVertex ( const osg::Vec3f &v, bool look )
{
  // Should always be true.
  USUL_ASSERT ( _shared.size() == _vertices->size() );
  USUL_ASSERT ( _shared.size() == _normalsV->size() );
  USUL_ASSERT ( _shared.size() == _colorsV->size() );

  // Look for an existing shared vertex if we are supposed to.
  if ( look )
  {
    SharedVertices::iterator i = _shared.find ( v );
    if ( _shared.end() != i )
      return i->second.get();
  }

  // Should always be true.
  USUL_ASSERT ( _shared.size() == _vertices->size() );
  USUL_ASSERT ( _shared.size() == _normalsV->size() );
  USUL_ASSERT ( _shared.size() == _colorsV->size() );

  // If we get to here then make shared vertex with proper index.
  SharedVertex::ValidRefPtr sv ( this->newSharedVertex ( _vertices->size() ) );

  // Insert the new shared-vertex into the map.
  const InsertResult result ( this->_insertSharedVertex ( v, sv.get() ) );

  // If insertion worked (and it should have).
  if ( true == result.second )
  {
    // Append to sequence.
    _vertices->push_back ( v );

    // Add dummy normal value because this keeps the vector sizes consistant, 
    // and prevents accessing values off the end. We don't want to add the 
    // real value because this requires averaging the normals from all the 
    // triangles that contain this vector, which is expensive.
    _normalsV->push_back ( OsgTools::Triangles::DEFAULT_NORMAL );

    // For similar reasons as above, add default color.
    _colorsV->push_back ( OsgTools::Triangles::DEFAULT_COLOR );

    // Set appropriate dirty-flags.
    sv->dirtyColor  ( true );
    sv->dirtyNormal ( true );
  }

  // In rare cases, insertion fails even though the "find" above was 
  // executed. I've seen this using the CloseFloat and LessVector combination, 
  // but not with the default std::less comparison predicate.
  else
  {
    std::cout << "Warning 1435177723: Failed to find existing shared-vertex for "
              << '(' << v[0] << ',' << v[1] << ',' << v[2] << ')'
              << ", however, when attempting to insert a new shared-vertex, "
              << "an existing one was returned" << std::endl;

    // Re-assign the shared-vertex to the one returned.
    sv = result.first->second;

    // Should be true.
    USUL_ASSERT ( sv->index() < _vertices->size() );
    USUL_ASSERT ( sv->index() < _colorsV->size() );

    // Flag it.
    sv->problem ( true );
  }

  // Should always be true.
  USUL_ASSERT ( _shared.size() == _vertices->size() );
  USUL_ASSERT ( _shared.size() == _normalsV->size() );
  USUL_ASSERT ( _shared.size() == _colorsV->size() );

  // Return the new shared vertex.
  return sv.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Insert the given shared-vertex into the correct map.
//
///////////////////////////////////////////////////////////////////////////////

TriangleSet::InsertResult TriangleSet::_insertSharedVertex ( const osg::Vec3f &v, SharedVertex *sv )
{
  // Using default heap-allocator for now...
  return _shared.insert ( SharedVertices::value_type ( v, sv ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Convenience function to set status bar and flush events.
//
///////////////////////////////////////////////////////////////////////////////

void TriangleSet::_setStatusBar ( const std::string &text )
{
  Usul::Interfaces::IStatusBar::QueryPtr status ( Usul::Resources::statusBar() );
  if ( status.valid() )
    status->setStatusBarText ( text, true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Convenience function to show progress and flush events.
//
///////////////////////////////////////////////////////////////////////////////

void TriangleSet::_setProgressBar ( bool state, unsigned int numerator, unsigned int denominator )
{
  // If we should...
  if ( state )
  {
    // Report progress.
    Usul::Interfaces::IProgressBar::QueryPtr progress ( Usul::Resources::progressBar() );
    if ( progress.valid() )
    {
      float n ( static_cast < float > ( numerator ) );
      float d ( static_cast < float > ( denominator ) );
      float fraction ( n / d );
      progress->updateProgressBar ( static_cast < unsigned int > ( fraction * 100 ) );
    }

    // Give user opportunity to cancel.
    Usul::Interfaces::IFlushEvents::QueryPtr flush ( Usul::Resources::flushEvents() );
    if ( flush.valid() )
      flush->flushEventQueue();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the averaged normal for the shared vertex
//
///////////////////////////////////////////////////////////////////////////////

osg::Vec3f TriangleSet::averageNormal ( const SharedVertex *sv ) const
{
  // Initialize.
  osg::Vec3f normal ( 0, 0, 0 );

  // Add the normal of all triangles connected to this shared vertex
  for ( SharedVertex::ConstTriangleItr i = sv->begin(); i != sv->end(); ++i )
  {
    normal += this->normalsT()->at( (*i)->index() );
  }

  // Return the normal. Do not normalize.
  return normal;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set all polyons and shared vertices as unvisited
//
///////////////////////////////////////////////////////////////////////////////

void TriangleSet::setAllUnvisited()
{
  //Go to each triangle and it's shared vertices and set the visited flag to false
  for( TriangleVector::iterator i = _triangles.begin(); i != _triangles.end(); ++ i )
  {
    (*i)->visited( false );
    (*i)->vertex0()->visited( false );
    (*i)->vertex1()->visited( false );
    (*i)->vertex2()->visited( false );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the triangles as visited.
//
///////////////////////////////////////////////////////////////////////////////

void TriangleSet::setVisited(Indices &keepers)
{
    Uint32 numTri = keepers.size();
    Uint32 index = 0;
  //Go to each triangle and it's shared vertices in the list and set the visited flag to false
  for( index = 0; index < numTri; index++)
  {
      Triangle::ValidRefPtr t ( _triangles.at ( keepers[index] ) );
      t->visited( true );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Returns the first triangle flagged as visited=FALSE or -1 if all the indices
//   have their visited flags marked as true.
//
///////////////////////////////////////////////////////////////////////////////

Int32 TriangleSet::firstUnvisited() 
{
  for( TriangleVector::iterator i = _triangles.begin(); i != _triangles.end(); ++ i )
  {
    if ( (*i)->visited() == false) 
    {
        std::cout << "Triangle Not Visited: " << (*i)->index() << std::endl;
       return (*i)->index();
    }
  }
  return -1; //-1 works good as it is unambiguous if an index exists or not.
}

///////////////////////////////////////////////////////////////////////////////
//
//  Reset the on edge flag.
//
///////////////////////////////////////////////////////////////////////////////

void TriangleSet::resetOnEdge()
{
  //Go to each triangle and it's shared vertices and set the visited flag to false
  for( TriangleVector::iterator i = _triangles.begin(); i != _triangles.end(); ++ i )
  {
    (*i)->onEdge( false );
    (*i)->vertex0()->onEdge( false );
    (*i)->vertex1()->onEdge( false );
    (*i)->vertex2()->onEdge( false );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the vertex at the index
//
///////////////////////////////////////////////////////////////////////////////

const osg::Vec3f& TriangleSet::getVertex ( unsigned int index ) const
{
  return _vertices->at ( index );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the vertex at the index
//
///////////////////////////////////////////////////////////////////////////////

void TriangleSet::checkStatus() const
{
  USUL_ERROR_CHECKER ( _shared.size() == _vertices->size() );
  USUL_ERROR_CHECKER ( _shared.size() == _colorsV->size() );
  USUL_ERROR_CHECKER ( _shared.size() == _normalsV->size() );

  // Check every triangle's vertices.
  {
    const unsigned int numVertices  ( _vertices->size() );
    const unsigned int numTriangles ( _triangles.size() );
    for ( unsigned int i = 0; i < numTriangles; ++i )
    {
      const Triangle *t ( _triangles[i] );
      const SharedVertex *sv0 ( t->vertex0() );
      const SharedVertex *sv1 ( t->vertex1() );
      const SharedVertex *sv2 ( t->vertex2() );

      // Check for null.
      USUL_ERROR_CHECKER ( 0x0 != sv0 );
      USUL_ERROR_CHECKER ( 0x0 != sv1 );
      USUL_ERROR_CHECKER ( 0x0 != sv2 );

      // Check reference count.
      USUL_ERROR_CHECKER ( sv0->refCount() == sv0->numTriangles() + 1 );
      USUL_ERROR_CHECKER ( sv1->refCount() == sv1->numTriangles() + 1 );
      USUL_ERROR_CHECKER ( sv2->refCount() == sv2->numTriangles() + 1 );

      // Make sure triangle contains the shared-vertex only once.
      const unsigned int num0 ( std::count_if ( sv0->begin(), sv0->end(), TriangleVector::value_type::IsEqual ( t ) ) );
      const unsigned int num1 ( std::count_if ( sv1->begin(), sv1->end(), TriangleVector::value_type::IsEqual ( t ) ) );
      const unsigned int num2 ( std::count_if ( sv2->begin(), sv2->end(), TriangleVector::value_type::IsEqual ( t ) ) );
      USUL_ERROR_CHECKER ( 1 == num0 );
      USUL_ERROR_CHECKER ( 1 == num1 );
      USUL_ERROR_CHECKER ( 1 == num2 );

      // Get index.
      const unsigned int i0 ( sv0->index() );
      const unsigned int i1 ( sv1->index() );
      const unsigned int i2 ( sv2->index() );

      // Make sure the index is in range.
      USUL_ERROR_CHECKER ( i0 < numVertices );
      USUL_ERROR_CHECKER ( i1 < numVertices );
      USUL_ERROR_CHECKER ( i2 < numVertices );

      // Make sure it is in the map.
      USUL_ERROR_CHECKER ( _shared.end() != _shared.find ( _vertices->at ( i0 ) ) );
      USUL_ERROR_CHECKER ( _shared.end() != _shared.find ( _vertices->at ( i1 ) ) );
      USUL_ERROR_CHECKER ( _shared.end() != _shared.find ( _vertices->at ( i2 ) ) );
    }
  }

  // Check every shared-vertex.
  {
    const unsigned int numVertices  ( _vertices->size() );
    for ( SharedVertices::const_iterator i = _shared.begin(); i != _shared.end(); ++i )
    {
      const SharedVertex *sv ( i->second );
      const unsigned int index ( sv->index() );
      USUL_ERROR_CHECKER ( index < numVertices );
      USUL_ERROR_CHECKER ( sv->numTriangles() > 0 );
      USUL_ERROR_CHECKER ( sv->numTriangles() + 1 == sv->refCount() );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Keep only these triangles.
//
///////////////////////////////////////////////////////////////////////////////

void TriangleSet::keepTriangles ( const Indices &keepers, Usul::Interfaces::IUnknown *caller )
{
  USUL_ASSERT ( _shared.size() == _vertices->size() );
  USUL_ASSERT ( _shared.size() == _colorsV->size() );
  USUL_ASSERT ( _shared.size() == _normalsV->size() );

  // Handle trivial case.
  if ( keepers.size() == _triangles.size() )
    return;

  // For progress.
  _progress.first = 0;
  _progress.second = 3 * _shared.size() + keepers.size();
  Usul::Policies::TimeBased update ( 1000 );

#ifdef _DEBUG
  this->checkStatus();
#endif

  // Tell all shared vertices to remove their triangles.
  {
    this->_setStatusBar ( "Removing Triangles from Shared Vertices..." );
    for ( SharedVertices::iterator i = _shared.begin(); i != _shared.end(); ++i )
    {
      // Save the current number of triangles and reserve them again.
      SharedVertex *sv ( i->second );
      const unsigned int num ( sv->numTriangles() );
      sv->removeAllTriangles();
      sv->reserve ( num );
      USUL_ASSERT ( num + 1 == sv->refCount() );
      this->_incrementProgress ( update() );
    }
  }

  // Build new triangles and per-triangle normal vectors.
  {
    this->_setStatusBar ( "Adding new Triangles..." );
    TriangleVector triangles;
    NormalsPtr normalsT ( new osg::Vec3Array );
    const unsigned int numKeepers ( keepers.size() );
    triangles.reserve ( numKeepers );
    normalsT->reserve ( numKeepers );
    for ( unsigned int i = 0; i < numKeepers; ++i )
    {
      // Push the triangle on to the new sequence.
      Triangle::ValidRefPtr t ( _triangles.at ( keepers[i] ) );
      t->index ( triangles.size() );
      triangles.push_back ( t.get() );

      // Push the per-triangle normal vector.
      normalsT->push_back ( this->normalsT()->at ( keepers[i] ) );

      // Tell the shared-vertices to add this triangle to themselves.
      t->vertex0()->addTriangle ( t.get() );
      t->vertex1()->addTriangle ( t.get() );
      t->vertex2()->addTriangle ( t.get() );

      // Feedback.
      this->_incrementProgress ( update() );
    }
    _triangles.swap ( triangles ); // Important!
    _normalsT = normalsT.get();
    USUL_ASSERT ( keepers.size() == _triangles.size() );
    USUL_ASSERT ( keepers.size() == this->normalsT()->size() );
  }

  // Purge all shared-vertices that do not have any triangles.
  {
    this->_setStatusBar ( "Purging Shared Vertices..." );
    CloseFloat closeFloatPred;
    LessVector lessVectorPred ( closeFloatPred );
    SharedVertices shared ( lessVectorPred );
    for ( SharedVertices::iterator i = _shared.begin(); i != _shared.end(); ++i )
    {
      if ( i->second->numTriangles() > 0 )
        shared.insert ( shared.end(), SharedVertices::value_type ( i->first, i->second ) );
      this->_incrementProgress ( update() );
    }
    _shared.swap ( shared ); // Important!
    USUL_ASSERT ( _shared.size() < shared.size() );
    USUL_ASSERT ( _shared.size() < keepers.size() * 3 );
  }

  // Update the vertex pool, the per-vertex normals and colors, the shared-vertices'
  // indices, and the bounding box.
  {
    this->_setStatusBar ( "Updating Vertex Pool and Per-Vertex Normal Vectors..." );
    _vertices->clear();
    _vertices->reserve ( _shared.size() );
    ColorsPtr colors ( new osg::Vec4Array );
    colors->reserve ( _shared.size() );
    NormalsPtr normalsV ( new osg::Vec3Array );
    normalsV->reserve ( _shared.size() );
    _bbox.init();
    for ( SharedVertices::iterator i = _shared.begin(); i != _shared.end(); ++i )
    {
      // Order is important here... 
      const osg::Vec3f &v ( i->first );
      SharedVertex *sv ( i->second );

      // Add normal vector and color using original index.
      normalsV->push_back ( this->normalsV()->at ( sv->index() ) );
      colors->push_back ( _colorsV->at ( sv->index() ) );

      // Update the shared-vertex's index.
      i->second->index ( _vertices->size() );

      // Add the shared-vertex's key (the 3D vector) to the vertex pool.
      _vertices->push_back ( v );

      // Expand the bounding box and display progress.
      this->updateBounds ( v );
      this->_incrementProgress ( update() );
    }
    _normalsV = normalsV.get(); // Important!
    _colorsV = colors.get();    // Important!
    this->dirtyNormalsV ( false );
    this->dirtyColorsV ( false );
  }

#ifdef _DEBUG
  this->checkStatus();
#endif

  // These things are now dirty.
  this->dirtyBlocks ( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove these triangles.
//
///////////////////////////////////////////////////////////////////////////////

void TriangleSet::removeTriangles ( Indices &doomed, Usul::Interfaces::IUnknown *caller )
{
  // Get number of triangles.
  const unsigned int numTriangles ( _triangles.size() );

  // Handle trivial cases.
  if ( doomed.empty() || 0 == numTriangles )
    return;

  // Sort given sequence.
  std::sort ( doomed.begin(), doomed.end() );

  // Build indices of existing triangles.
  Indices existing ( numTriangles );
  std::generate ( existing.begin(), existing.end(), Usul::Functors::Increment<Indices::value_type> ( 0 ) );

  // We are going to keep these ones.
  Indices keepers ( numTriangles );

  // Set difference operation: keepers = existing - doomed
  Indices::iterator end ( std::set_difference ( existing.begin(), existing.end(), doomed.begin(), doomed.end(), keepers.begin() ) );
  keepers.erase ( end, keepers.end() );

  // We do this because keeping is faster than removing.
  this->keepTriangles ( keepers, caller );
}

///////////////////////////////////////////////////////////////////////////////
//
//  Create a new TriangleSet that contains only the triangles listed in 
//   keepers
//
///////////////////////////////////////////////////////////////////////////////

void TriangleSet::createSubset ( const Indices &keepers, TriangleSet *triSet, Usul::Interfaces::IUnknown *caller )
{
    Uint32 numKeepers = keepers.size();
    //TriangleSet triSet; //Create a new TriangleSet Object
    for ( unsigned int i = 0; i < numKeepers; ++i )
    {
        // Push the triangle on to the new sequence.
        Triangle::ValidRefPtr t ( _triangles.at ( keepers[i] ) );
        //Add this triangle to the new TriangleSet
       // SharedVertex *sv0 = new SharedVertex(t->vertex0() );
		osg::Vec3 v0 ( this->vertex0 ( t->index() ) );
		osg::Vec3 v1 ( this->vertex1 ( t->index() ) );
		osg::Vec3 v2 ( this->vertex2 ( t->index() ) );

        triSet->addTriangle( v0, v1, v2, this->normalsT()->at ( keepers[i] )  , false);
    }
    //return triSet;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the display list flag.
//
///////////////////////////////////////////////////////////////////////////////

bool TriangleSet::displayList() const
{
  return _blocks->displayList();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the display list flag.
//
///////////////////////////////////////////////////////////////////////////////

void TriangleSet::displayList ( bool b )
{
  _blocks->displayList ( b );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the normal of the i'th vertex.
//
///////////////////////////////////////////////////////////////////////////////

const osg::Vec3f & TriangleSet::vertexNormal ( unsigned int i ) const
{
  return ( ( normalsV()->empty() ) ? normalsT()->at ( i / 3 ) : normalsV()->at ( i ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the bounding box
//
///////////////////////////////////////////////////////////////////////////////

osg::BoundingBox TriangleSet::getBoundingBox() const 
{
  return _bbox;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return a new shared vertex.
//
///////////////////////////////////////////////////////////////////////////////

OsgTools::Triangles::SharedVertex *TriangleSet::newSharedVertex ( unsigned int index, unsigned int numTrianglesToReserve )
{
  return _factory->newSharedVertex ( index, numTrianglesToReserve );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return a new triangle.
//
///////////////////////////////////////////////////////////////////////////////

OsgTools::Triangles::Triangle *TriangleSet::newTriangle ( SharedVertex *v0, SharedVertex *v1, SharedVertex *v2, unsigned int index )
{
  return _factory->newTriangle ( v0, v1, v2, index );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get vertex pool. Use with caution.
//
///////////////////////////////////////////////////////////////////////////////

const osg::Vec3Array *TriangleSet::vertices() const
{
  return _vertices.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get vertex pool. Use with caution.
//
///////////////////////////////////////////////////////////////////////////////

osg::Vec3Array *TriangleSet::vertices()
{
  return _vertices.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Access the container of colors. Use with caution.
//
///////////////////////////////////////////////////////////////////////////////

const osg::Vec4Array *TriangleSet::colorsV() const
{
  return _colorsV.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Access the container of colors. Use with caution.
//
///////////////////////////////////////////////////////////////////////////////

osg::Vec4Array *TriangleSet::colorsV()
{
  return _colorsV.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Access the container of normals. Use with caution.
//
///////////////////////////////////////////////////////////////////////////////

const osg::Vec3Array *TriangleSet::normalsT() const
{
  return _normalsT.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Access the container of normals. Use with caution.
//
///////////////////////////////////////////////////////////////////////////////

osg::Vec3Array *TriangleSet::normalsT()
{
  return _normalsT.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Access the container of normals. Use with caution.
//
///////////////////////////////////////////////////////////////////////////////

const osg::Vec3Array *TriangleSet::normalsV() const
{
  return _normalsV.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Access the container of normals. Use with caution.
//
///////////////////////////////////////////////////////////////////////////////

osg::Vec3Array *TriangleSet::normalsV()
{
  return _normalsV.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the dirty flag.
//
///////////////////////////////////////////////////////////////////////////////

void TriangleSet::dirtyNormalsV ( bool state )
{
  const unsigned int bit ( Dirty::NORMALS_V );
  _flags = ( ( state ) ? Usul::Bits::add ( _flags, bit ) : Usul::Bits::remove ( _flags, bit ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the dirty flag.
//
///////////////////////////////////////////////////////////////////////////////

bool TriangleSet::dirtyNormalsV() const
{
    return Usul::Bits::has ( _flags, Dirty::NORMALS_V );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the dirty flag.
//
///////////////////////////////////////////////////////////////////////////////

void TriangleSet::dirtyColorsV ( bool state )
{
  const unsigned int bit ( Dirty::COLORS_V );
  _flags = ( ( state ) ? Usul::Bits::add ( _flags, bit ) : Usul::Bits::remove ( _flags, bit ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the dirty flag.
//
///////////////////////////////////////////////////////////////////////////////

bool TriangleSet::dirtyColorsV() const
{
  return Usul::Bits::has ( _flags, Dirty::COLORS_V );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the dirty flag.
//
///////////////////////////////////////////////////////////////////////////////

void TriangleSet::dirtyBlocks ( bool state )
{
  const unsigned int bit ( Dirty::BLOCKS );
  _flags = ( ( state ) ? Usul::Bits::add ( _flags, bit ) : Usul::Bits::remove ( _flags, bit ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the dirty flag.
//
///////////////////////////////////////////////////////////////////////////////

bool TriangleSet::dirtyBlocks() const
{
  return Usul::Bits::has ( _flags, Dirty::BLOCKS );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the blocks if needed. 
//
///////////////////////////////////////////////////////////////////////////////

void TriangleSet::_updateBlocks()
{
  USUL_ASSERT ( _bbox.valid() );

  // Return now if we are not dirty, or if there are no triangles.
  if ( false == this->dirtyBlocks() || true == _triangles.empty() )
    return;

  // User feedback.
  Usul::Policies::TimeBased update ( Detail::_milliseconds );
  this->_setStatusBar ( "Updating Blocks ..." );

  // Needed below.
  const unsigned int numTriangles ( _triangles.size() );

  // Figure out how many times we need to subdivide so that the number of 
  // triangles in each block is not too high.
  unsigned int divisions ( 1 );
  while ( numTriangles / Usul::Math::pow<double> ( 2, divisions ) > Detail::_averageTrianglesPerBlock )
    ++divisions;

  // Make new blocks. Subdivide sufficient number of times.
  _blocks = new Blocks ( _bbox, divisions, numTriangles );

  // Loop through triangles.
  for ( unsigned int i = 0; i < numTriangles; ++i )
  {
    // Add a triangle.
    _blocks->addTriangle ( this, _triangles[i] );

    // Progress.
    this->_incrementProgress ( update() );
  }

  // Purge excess memory.
  _blocks->purge();

  // No longer dirty.
  this->dirtyBlocks ( false );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the normal vectors if needed. 
//
///////////////////////////////////////////////////////////////////////////////

void TriangleSet::_updateNormalsV()
{
  // Return now if we are not dirty.
  if ( false == this->dirtyNormalsV() )
    return;

  // User feedback.
  Usul::Policies::TimeBased update ( Detail::_milliseconds );
  this->_setStatusBar ( "Updating Per-Vertex Normal Vectors ..." );

  // Make room.
  const unsigned int numVertices ( _shared.size() );
  this->normalsV()->resize ( numVertices );

  // Loop through the shared vertices and update the normals.
  for ( SharedVertices::iterator i = _shared.begin(); i != _shared.end(); ++i )
  {
    // This only updates the individual normals that are dirty.
    this->_updateNormalV ( i->second );

    // Progress.
    this->_incrementProgress ( update() );
  }

  // No longer dirty.
  this->dirtyNormalsV ( false );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the per-vertex normal vector if needed. 
//
///////////////////////////////////////////////////////////////////////////////

void TriangleSet::_updateNormalV ( SharedVertex *sv )
{
  // Return now if we are not dirty.
  if ( 0x0 == sv || false == sv->dirtyNormal() )
    return;

  // Update the normal.
  osg::Vec3f &n = this->normalsV()->at ( sv->index() );
  n = this->averageNormal ( sv );
  n.normalize();

  // No longer dirty.
  sv->dirtyNormal ( false );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the per-vertex colors if needed. 
//
///////////////////////////////////////////////////////////////////////////////

void TriangleSet::_updateColorsV()
{
  // Return now if we are not dirty.
  if ( false == this->dirtyColorsV() )
    return;

  // User feedback.
  Usul::Policies::TimeBased update ( Detail::_milliseconds );
  this->_setStatusBar ( "Updating Per-Vertex Colors ..." );

  // Make room.
  const unsigned int numVertices ( _shared.size() );
  _colorsV->resize ( numVertices, OsgTools::Triangles::DEFAULT_COLOR );

  // Loop through the shared vertices and update the colors.
  for ( SharedVertices::iterator i = _shared.begin(); i != _shared.end(); ++i )
  {
    // This only updates the individual normals that are dirty.
    this->_updateColorV ( i->second );

    // Progress.
    this->_incrementProgress ( update() );
  }

  // No longer dirty.
  this->dirtyColorsV ( false );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the per-vertex color if needed. 
//
///////////////////////////////////////////////////////////////////////////////

void TriangleSet::_updateColorV ( SharedVertex *sv )
{
  // Return now if we are not dirty.
  if ( 0x0 == sv || false == sv->dirtyColor() )
    return;

  // Update the color.
  osg::Vec4f &c = _colorsV->at ( sv->index() );
  c = this->color ( sv );

  // No longer dirty.
  sv->dirtyColor ( false );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build any decorations. 
//
///////////////////////////////////////////////////////////////////////////////

void TriangleSet::_buildDecorations ( const Options &options, osg::Group *root ) const
{
  // Should we draw a bounding box?
  const bool boundingBox ( OsgTools::Options::has ( options, "BoundingBox",      "Show" ) );
  const bool showGlass   ( OsgTools::Options::has ( options, "GlassBoundingBox", "Show" ) );

  // Draw the bounding box if we should.
  if ( boundingBox || showGlass )
  {
    OsgTools::GlassBoundingBox gbb ( _bbox );
    gbb ( root, boundingBox, showGlass, true );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene. 
//
///////////////////////////////////////////////////////////////////////////////

osg::Node *TriangleSet::buildScene ( const Options &options, Unknown * )
{
  // The scene root  
  osg::ref_ptr<osg::Group> root ( new osg::Group );

  // Handle trivial case.
  if ( _triangles.empty() )
    return root.release();

  // Show the progress bar.
  Usul::Interfaces::IProgressBar::ShowHide showHide ( Usul::Resources::progressBar() );

  // Set the progress counter and max.
  _progress.first = 0;
  const unsigned int numTriangles ( this->dirtyBlocks()   ? _triangles.size() : 0 );
  const unsigned int numColors    ( this->dirtyColorsV()  ? _shared.size()  : 0 );
  const unsigned int numNormals   ( this->dirtyNormalsV() ? _shared.size() : 0 );
  _progress.second = numTriangles + numColors + numNormals;

  // Start at zero.
  this->_setProgressBar ( true, 0, 100 );

  // Make sure we have per-vertex normal vectors if we need them.
  this->_updateNormalsV();

  // Make sure we have per-vertex colors if we need them.
  this->_updateColorsV();

  // Build the blocks if we should.
  this->_updateBlocks();
  USUL_ASSERT ( _blocks.valid() );

  // Have the blocks build their scenes.
  root->addChild ( _blocks->buildScene ( options, this ) );

  // Add any extra decorations.
  this->_buildDecorations ( options, root.get() );
   
  // Return the root.
  return root.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the shared vertex 0 of the i'th triangle.
//
///////////////////////////////////////////////////////////////////////////////

const OsgTools::Triangles::SharedVertex* TriangleSet::sharedVertex0 ( const osg::Drawable *d, unsigned int i ) const
{
  const Triangle *t ( this->triangle ( d, i ) );
  return ( ( t ) ? t->vertex0() : 0x0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the shared vertex 0 of the i'th triangle.
//
///////////////////////////////////////////////////////////////////////////////

OsgTools::Triangles::SharedVertex* TriangleSet::sharedVertex0 ( const osg::Drawable *d, unsigned int i )
{
  Triangle *t ( this->triangle ( d, i ) );
  return ( ( t ) ? t->vertex0() : 0x0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the shared vertex 1 of the i'th triangle.
//
///////////////////////////////////////////////////////////////////////////////

const OsgTools::Triangles::SharedVertex* TriangleSet::sharedVertex1 ( const osg::Drawable *d, unsigned int i ) const
{
  const Triangle *t ( this->triangle ( d, i ) );
  return ( ( t ) ? t->vertex1() : 0x0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the shared vertex 1 of the i'th triangle.
//
///////////////////////////////////////////////////////////////////////////////

OsgTools::Triangles::SharedVertex* TriangleSet::sharedVertex1 ( const osg::Drawable *d, unsigned int i )
{
  Triangle *t ( this->triangle ( d, i ) );
  return ( ( t ) ? t->vertex1() : 0x0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the shared vertex 2 of the i'th triangle.
//
///////////////////////////////////////////////////////////////////////////////

const OsgTools::Triangles::SharedVertex* TriangleSet::sharedVertex2 ( const osg::Drawable *d, unsigned int i ) const
{
  const Triangle *t ( this->triangle ( d, i ) );
  return ( ( t ) ? t->vertex2() : 0x0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the shared vertex 2 of the i'th triangle.
//
///////////////////////////////////////////////////////////////////////////////

OsgTools::Triangles::SharedVertex* TriangleSet::sharedVertex2 ( const osg::Drawable *d, unsigned int i )
{
  Triangle *t ( this->triangle ( d, i ) );
  return ( ( t ) ? t->vertex2() : 0x0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the vertex 0 of the i'th triangle.
//
///////////////////////////////////////////////////////////////////////////////

const osg::Vec3f &TriangleSet::vertex0 ( const osg::Drawable *d, unsigned int i ) const
{
  const SharedVertex *sv ( this->sharedVertex0 ( d, i ) );
  return _vertices->at ( sv->index() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the vertex 1 of the i'th triangle.
//
///////////////////////////////////////////////////////////////////////////////

const osg::Vec3f &TriangleSet::vertex1 ( const osg::Drawable *d, unsigned int i ) const
{
  const SharedVertex *sv ( this->sharedVertex1 ( d, i ) );
  return _vertices->at ( sv->index() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the vertex 2 of the i'th triangle.
//
///////////////////////////////////////////////////////////////////////////////

const osg::Vec3f &TriangleSet::vertex2 ( const osg::Drawable *d, unsigned int i ) const
{
  const SharedVertex *sv ( this->sharedVertex2 ( d, i ) );
  return _vertices->at ( sv->index() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  The hit information gives us the index of the primitive with respect to 
//  the drawable intersected. Since each block has its own drawable, translate 
//  this primitive index to a triangle index.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int TriangleSet::index ( const osgUtil::Hit &hit ) const
{
  const Triangle *t ( this->triangle ( hit._drawable.get(), hit._primitiveIndex ) );
  if ( 0x0 == t )
    throw std::runtime_error ( "Error 4259806184: No triangle found for given hit information" );
  return ( t->index() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the triangle given a drawable and primitive index.
//
///////////////////////////////////////////////////////////////////////////////

const Triangle *TriangleSet::triangle ( const osg::Drawable *d, unsigned int num ) const
{
  const Block *b ( ( d ) ? _blocks->block ( d->getBound().center() ) : 0x0 );
  const Triangle *t ( ( b ) ? b->triangle ( num ) : 0x0 );
  return t;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the triangle given a drawable and primitive index.
//
///////////////////////////////////////////////////////////////////////////////

Triangle *TriangleSet::triangle ( const osg::Drawable *d, unsigned int num )
{
  Block *b ( ( d ) ? _blocks->block ( d->getBound().center() ) : 0x0 );
  Triangle *t ( ( b ) ? b->triangle ( num ) : 0x0 );
  return t;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the center of the triangle.
//
///////////////////////////////////////////////////////////////////////////////

osg::Vec3f TriangleSet::triangleCenter ( unsigned int i ) const
{
  const float third ( static_cast < float > ( 1 ) / static_cast < float > ( 3 ) );
  const Triangle *t ( _triangles.at ( i ) );
  const osg::Vec3f &v0 ( _vertices->at ( t->vertex0()->index() ) );
  const osg::Vec3f &v1 ( _vertices->at ( t->vertex1()->index() ) );
  const osg::Vec3f &v2 ( _vertices->at ( t->vertex2()->index() ) );
  const osg::Vec3f center ( ( v0 + v1 + v2 ) * third );
  return center;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Purge memory.
//
///////////////////////////////////////////////////////////////////////////////

void TriangleSet::purge()
{
  // Purge triangles.
  if ( false == _triangles.empty() )
  {
    TriangleVector temp ( _triangles );
    _triangles.swap ( temp );
  }

  // Purge vertices.
  if ( _vertices.valid() && false == _vertices->empty() )
  {
    osg::ref_ptr < osg::Vec3Array > temp ( new osg::Vec3Array ( *_vertices ) );
    _vertices = temp;
  }

  // Purge normals.
  if ( _normalsV.valid() && false == _normalsV->empty() )
  {
    osg::ref_ptr < osg::Vec3Array > temp ( new osg::Vec3Array ( *_normalsV ) );
    _normalsV = temp;
  }

  // Purge other normals.
  if ( _normalsT.valid() && false == _normalsT->empty() )
  {
    osg::ref_ptr < osg::Vec3Array > temp ( new osg::Vec3Array ( *_normalsT ) );
    _normalsT = temp;
  }

  // Purge per-vertex colors.
  if ( _colorsV.valid() && false == _colorsV->empty() )
  {
    osg::ref_ptr < osg::Vec4Array > temp ( new osg::Vec4Array ( *_colorsV ) );
    _colorsV = temp;
  }

  // Purge the blocks.
  if ( _blocks.valid() )
    _blocks->purge();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the progress bar.
//
///////////////////////////////////////////////////////////////////////////////

void TriangleSet::_incrementProgress ( bool state )
{
  unsigned int &numerator   ( _progress.first  );
  unsigned int &denominator ( _progress.second );
  this->_setProgressBar ( state, numerator, denominator );
  ++numerator;
  USUL_ASSERT ( numerator <= denominator );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Calculate the color for this triangle.
//
///////////////////////////////////////////////////////////////////////////////

osg::Vec4f TriangleSet::color ( const Triangle *t ) const
{
  return ( ( t && _color.valid() ) ? 
           ( _color->color ( this, t ) ) : 
           ( OsgTools::Triangles::DEFAULT_COLOR ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Calculate the color for this vertex.
//
///////////////////////////////////////////////////////////////////////////////

osg::Vec4f TriangleSet::color ( const SharedVertex *sv ) const
{
  return ( ( sv && _color.valid() ) ? 
           ( _color->color ( this, sv ) ) : 
           ( OsgTools::Triangles::DEFAULT_COLOR ) );
}
