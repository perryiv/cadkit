
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

#include "Usul/MPL/StaticAssert.h"
#include "Usul/Errors/Assert.h"
#include "Usul/Policies/Update.h"
#include "Usul/Interfaces/IProgressBar.h"
#include "Usul/Interfaces/IStatusBar.h"
#include "Usul/Interfaces/IFlushEvents.h"

#include "Usul/Resources/ProgressBar.h"
#include "Usul/Resources/StatusBar.h"
#include "Usul/Resources/EventQueue.h"

#include "osg/Group"
#include "osg/Geode"
#include "osg/Geometry"

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
  _geometry  ( new osg::Geometry ),
  _primitiveSet ( new osg::DrawElementsUInt( osg::PrimitiveSet::TRIANGLES, 0 ) )
{
  USUL_STATIC_ASSERT ( 12 == sizeof ( _shared         ) );
  USUL_STATIC_ASSERT (  4 == sizeof ( _vertices       ) );
  USUL_STATIC_ASSERT (  8 == sizeof ( _normals        ) );
  USUL_STATIC_ASSERT (  4 == sizeof ( _colors         ) );
  USUL_STATIC_ASSERT (  1 == sizeof ( _dirty          ) );
  USUL_STATIC_ASSERT (  4 == sizeof ( _geometry       ) );
  USUL_STATIC_ASSERT (  4 == sizeof ( _primitiveSet   ) );
#ifdef __WIN32
  USUL_STATIC_ASSERT ( 16 == sizeof ( _triangles      ) ;)
  USUL_STATIC_ASSERT ( 68 == sizeof ( TriangleSet     ) ); // Why?
#else
  USUL_STATIC_ASSERT ( 12 == sizeof ( _triangles      ) );
  USUL_STATIC_ASSERT ( 64 == sizeof ( TriangleSet     ) ); // Why?
#endif

  // Add the vertices
  _geometry->setVertexArray( _vertices.get() );

  // Add the PrimitiveSet
  _geometry->addPrimitiveSet( _primitiveSet.get() );
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
  std::for_each ( _triangles.begin(), _triangles.end(), std::mem_fun ( &Triangle::clear ) );

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

  _primitiveSet = 0x0;
  _geometry = 0x0;
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

  //Add the triangle
  this->_addTriangle( sv0, sv1, sv2, n );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a triangle.
//
///////////////////////////////////////////////////////////////////////////////

void TriangleSet::addTriangle ( const SharedVertex &v0, const SharedVertex &v1, const SharedVertex &v2, const osg::Vec3f &n )
{
  SharedVertex *sv0 ( const_cast < SharedVertex * > ( &v0 ) );
  SharedVertex *sv1 ( const_cast < SharedVertex * > ( &v1 ) );
  SharedVertex *sv2 ( const_cast < SharedVertex * > ( &v2 ) );

#if 0
  //This is my first attempt at making sure the normal is in the same direction as it's neighbors
  //Make a copy
  osg::Vec3f copy ( n );

  osg::Vec3f test ( this->normal( (*sv0->begin())->index() ) );

  float dot ( test * copy );

  float arccos ( ::acos ( dot ) );

  float angle ( (float) ( arccos * (float) 180 ) / (float) 3.14159265 );

  if( angle > 180 )
    copy = -copy;

  this->_addTriangle( sv0, sv1, sv2, copy );
#else
  //Add the triangle
  this->_addTriangle( sv0, sv1, sv2, n );
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a triangle.
//
///////////////////////////////////////////////////////////////////////////////

void TriangleSet::_addTriangle ( SharedVertex *sv0, SharedVertex *sv1, SharedVertex *sv2, const osg::Vec3f &n )
{
  // Make the new triangle.
  Triangle::ValidRefPtr t ( new Triangle ( sv0, sv1, sv2, _triangles.size() ) );

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

void TriangleSet::addStart (  )
{
  typedef Usul::Interfaces::IProgressBar IProgressBar;
  typedef Usul::Interfaces::IStatusBar IStatusBar;

  // Get the interfaces.
  IProgressBar::QueryPtr progress ( Usul::Resources::progressBar() );
  IStatusBar::QueryPtr   status   ( Usul::Resources::statusBar()  );

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
    _shared.insert( SharedVertices::value_type ( _vertices->at(i0), sv0.get() ) );
    _shared.insert( SharedVertices::value_type ( _vertices->at(i1), sv1.get() ) );
    _shared.insert( SharedVertices::value_type ( _vertices->at(i2), sv2.get() ) );

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

void TriangleSet::addFinish (  )
{
  // Get the interface.
  typedef Usul::Interfaces::IStatusBar IStatusBar;
  IStatusBar::QueryPtr status ( Usul::Resources::statusBar() );

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
//  Build the scene. 
//
///////////////////////////////////////////////////////////////////////////////

osg::Node *TriangleSet::buildScene ( const Options &opt, Unknown *caller )
{
  if ( !_primitiveSet.valid() )
  {
     _primitiveSet = new osg::DrawElementsUInt( osg::PrimitiveSet::TRIANGLES, 0 );
  }

  if ( !_geometry.valid() )
  {
    _geometry = new osg::Geometry;

    // Add the vertices
    _geometry->setVertexArray( _vertices.get() );

    // Add the PrimitiveSet
    _geometry->addPrimitiveSet( _primitiveSet.get() );
  }

  // Make copy of the options.
  Options options ( opt );

  // User feedback.
  this->_setStatusBar ( "Building Scene ..." );

  // Start at zero.
  this->_setProgressBar ( true, 0, 100 );

  // The scene root.
  osg::ref_ptr<osg::Group> root ( new osg::Group );

  // A single geode.
  osg::ref_ptr<osg::Geode> geode ( new osg::Geode );
  root->addChild ( geode.get() );

  // Make geometry and add to geode.
  geode->addDrawable ( _geometry.get() );  

  // Should we use averaged normals?
  const bool average ( "average" == options["normals"] );

  if( _dirty )
  {
    //For convienence
    osg::ref_ptr< osg::Vec3Array > normals ( &this->_normalsPerVertex() );

    //How many points do we have    
    const unsigned int numPoints ( _triangles.size() * 3 );

    // Make space.
    normals->resize       ( _vertices->size() );
    _primitiveSet->resize ( numPoints );

    // Initialize counter for progress.
    unsigned int count ( 0 );

    // Update progress bar every second.
    Usul::Policies::TimeBased elapsed ( 1000 );

    // Loop through the triangles.
    for ( Triangles::const_iterator i = _triangles.begin(); i != _triangles.end(); ++i )
    {
      // Shortcuts to the triangle.
      const Triangle::ValidRefPtr triangle ( i->get() );

      // Get the vertices.5
      const SharedVertex *v1 ( triangle->vertex0() );
      const SharedVertex *v2 ( triangle->vertex1() );
      const SharedVertex *v3 ( triangle->vertex2() );

      // Make sure we have good pointers.
      if ( !v1 || !v2 || !v3 )
        throw std::runtime_error ( "Error 2040664771: null vertex found when trying to build scene" );

      const unsigned int triNum ( count * 3 );

      // Add the indices to the primitive set
      _primitiveSet->at( triNum )     = ( v1->index() );
      _primitiveSet->at( triNum + 1 ) = ( v2->index() );
      _primitiveSet->at( triNum + 2 ) = ( v3->index() );

      // If we are suppose to add averaged normals...
      if( average )
      {
        // Get the normals.
        osg::Vec3 n1 ( this->_averageNormal ( v1 ) );
        osg::Vec3 n2 ( this->_averageNormal ( v2 ) );
        osg::Vec3 n3 ( this->_averageNormal ( v3 ) );

        // Make sure they are normalized.
        n1.normalize();
        n2.normalize();
        n3.normalize();

        // Set the normal values.
        normals->at( v1->index() ) = n1;
        normals->at( v2->index() ) = n2;
        normals->at( v3->index() ) = n3;
      }

      // Show progress.
      this->_setProgressBar ( elapsed(), count, _triangles.size() );
      ++count;
    }

    _geometry->dirtyBound();
    _geometry->dirtyDisplayList();

    _dirty = false;
  }

  //Set the right number of normals
  if( average )
  {
    //Set the normals
    _geometry->setNormalArray ( &this->_normalsPerVertex() );

    // Need per-vertex normals for osg::Geometry's "fast path".
    _geometry->setNormalBinding ( osg::Geometry::BIND_PER_VERTEX );
  }
  else
  {
    //Delete the per vertex normals
    this->_normalsPerVertex().clear();

    //Set the normals
    _geometry->setNormalArray ( &this->_normalsPerFacet() );

    //Set the normal binding
    _geometry->setNormalBinding ( osg::Geometry::BIND_PER_PRIMITIVE );
  }

  // Return the root.
  return root.release();
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

osg::Vec3 TriangleSet::_averageNormal ( const SharedVertex *sv )
{
  osg::Vec3 normal;

  //Add the normal of all triangles connected to this shared vertex
  for ( SharedVertex::ConstTriangleItr i = sv->begin(); i != sv->end(); ++i )
  {
    normal += this->_normalsPerFacet().at( (*i)->index() );
  }

  //Return the normal.  Do not normalize
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
  for( Triangles::iterator i = _triangles.begin(); i != _triangles.end(); ++ i )
  {
    (*i)->visited( false );
    (*i)->vertex0()->visited( false );
    (*i)->vertex1()->visited( false );
    (*i)->vertex2()->visited( false );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the vertex at the index
//
///////////////////////////////////////////////////////////////////////////////

const osg::Vec3f& TriangleSet::getVertex( unsigned int index ) const
{
  return _vertices->at( index );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Delete triangle at given index
//
///////////////////////////////////////////////////////////////////////////////

void TriangleSet::deleteTriangle( unsigned int index )
{
  //Get the triangle to remove
  Triangles::iterator doomed ( _triangles.begin() + index );

  // Decrement the index of all triangles after doomed.
  for( Triangles::iterator i = doomed + 1; i != _triangles.end(); ++i  )
  {
    unsigned int t ( (*i)->index() - 1 );
    (*i)->index( t );
  }

  // Get the shared vertices.
  SharedVertex *sv0 ( (*doomed)->vertex0() );
  SharedVertex *sv1 ( (*doomed)->vertex1() );
  SharedVertex *sv2 ( (*doomed)->vertex2() );

  //Remove the triangle from the shared vertices
  sv0->remove( doomed->get() );
  sv1->remove( doomed->get() );
  sv2->remove( doomed->get() );

  // Unref doomed.  Not needed, but can't hurt
  (*doomed) = 0x0;

  // Remove doomed from our vector of triangles
  _triangles.erase( doomed );

  //Remove the normal
  this->_normalsPerFacet().erase( ( _normalsPerFacet().begin() + index ) );

  // Positions of the indices of doomed
  osg::DrawElementsUInt::iterator first ( _primitiveSet->begin() + ( index * 3 ) );
  osg::DrawElementsUInt::iterator last  ( _primitiveSet->begin() + ( index * 3 ) + 3 );

  //Remove the indices from the primitive set
  _primitiveSet->erase( first, last );

  //Display lists need to be recalculated
  _geometry->dirtyDisplayList();

  //Scene needs to be rebuilt
  _dirty = true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Keep only these triangles.
//
///////////////////////////////////////////////////////////////////////////////

void TriangleSet::keep ( const std::vector<unsigned int>& keepers, Usul::Interfaces::IUnknown *caller )
{
  //Make a copy of the triangles
  Triangles triangles ( _triangles );

  //Make a copy of the normals
  NormalsPtr normals ( new osg::Vec3Array ( this->_normalsPerFacet().size() ) );
  std::copy ( this->_normalsPerFacet().begin(), this->_normalsPerFacet().end(), normals->begin() );

  //Make a copy of the vertices
  VerticesPtr vertices ( new osg::Vec3Array ( _vertices->size() ) );
  std::copy ( _vertices->begin(), _vertices->end(), vertices->begin() );

  // Clear every thing we have.  Don't use this->clear() because triangles ref count is not 1
  _triangles.clear();
  _vertices->clear();
  this->_normalsPerVertex().clear();
  this->_normalsPerFacet().clear();
  _colors->clear();

  // Make enough room
  this->reserve( keepers.size() );

  // Initialize the shared vertex map
  this->addStart(  );

  this->_setStatusBar( "Adding Triangles..." );

  Usul::Policies::TimeBased update ( 1000 );

  // Go through the triangles that we need to keep
  for( std::vector<unsigned int>::const_iterator i = keepers.begin(); i != keepers.end(); ++i )
  {
    // Get the triangle
    Triangle::ValidRefPtr t ( triangles.at( *i ) );

    // Get it's vertices
    osg::Vec3f v0 ( vertices->at( t->vertex0()->index() ) );
    osg::Vec3f v1 ( vertices->at( t->vertex1()->index() ) );
    osg::Vec3f v2 ( vertices->at( t->vertex2()->index() ) );

    // Get it's normal
    osg::Vec3f n ( normals->at( t->index() ) );

    // Add the triangle
    this->addTriangle( v0, v1, v2, n );

    // Let the user know how much we have done
    this->_setProgressBar ( update(), ( i - keepers.begin() ), keepers.size() );
  }

  // Don't with the shared vertex map
  this->addFinish( );

  //The scene needs to be rebuilt
  _dirty = true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove these triangles.
//
///////////////////////////////////////////////////////////////////////////////

void TriangleSet::remove ( std::vector<unsigned int>& remove, Usul::Interfaces::IUnknown *caller )
{
  // Sort sequence
  std::sort( remove.begin(), remove.end(), std::less_equal< unsigned int >() );

  //Our current position in remove
  std::vector<unsigned int>::const_iterator current ( remove.begin() );

  //We are going to keep these one
  std::vector< unsigned int > keep;
  
  //Reserve enough room
  keep.reserve( _triangles.size() - remove.size() );

  //Go through each index and decide if we are going to keep or not
  for( unsigned int i = 0; i < _triangles.size(); ++i )
  {
    //If i is not going to be removed...
    if( i != *current )
      keep.push_back( i );
    //If i is going to be remove, don't add to keepers and move to next triangle to be removed
    else
      ++current;
  }

  //It's faster to rebuild than remove
  this->keep( keep, caller );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Turn on color and set all trianlges to given color.
//
///////////////////////////////////////////////////////////////////////////////

void TriangleSet::colorOn( const osg::Vec4& color )
{
  _geometry->setColorArray( _colors.get() );

  _colors->resize ( _triangles.size() );
  std::fill ( _colors->begin(), _colors->end(), color );
      
  _geometry->setColorBinding ( osg::Geometry::BIND_PER_PRIMITIVE );

  _geometry->dirtyDisplayList();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Color the ith triangle with given color.
//
///////////////////////////////////////////////////////////////////////////////

void TriangleSet::color ( unsigned int index, const osg::Vec4& color )
{
  _colors->at( index ) = color;

  _geometry->dirtyDisplayList();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Turn off color.
//
///////////////////////////////////////////////////////////////////////////////

void TriangleSet::colorOff ()
{
  _geometry->setColorArray( 0x0 );

  _geometry->dirtyDisplayList();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the display list flag.
//
///////////////////////////////////////////////////////////////////////////////

bool TriangleSet::displayList ( ) const
{
  return _geometry->getUseDisplayList ( );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the display list flag.
//
///////////////////////////////////////////////////////////////////////////////

void TriangleSet::displayList ( bool b )
{
  _geometry->setUseDisplayList ( b );
}

