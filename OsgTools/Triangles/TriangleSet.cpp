
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
#include "OsgTools/GlassBoundingBox.h"
#include "Usul/MPL/StaticAssert.h"
#include "Usul/Errors/Assert.h"
#include "Usul/Policies/Update.h"
#include "Usul/Interfaces/IProgressBar.h"
#include "Usul/Interfaces/IStatusBar.h"
#include "Usul/Interfaces/IFlushEvents.h"

#include "Usul/Resources/ProgressBar.h"
#include "Usul/Resources/StatusBar.h"
#include "Usul/Resources/EventQueue.h"
#include "Usul/MPL/StaticAssert.h"

#include "OsgTools/State.h"

#include "osg/Group"
#include "osg/Geode"
#include "osg/Geometry"
#include "osg/Vec4"
#include "osg/StateSet"
#include "osg/AlphaFunc"

#include <algorithm>
#include <functional>
#include <limits>

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
  _bb(),
  _partition (  )
{
#ifndef __APPLE__ // They are different, but it is not critical. TODO.
  USUL_STATIC_ASSERT ( 12 == sizeof ( _shared         ) );
  USUL_STATIC_ASSERT (  4 == sizeof ( _vertices       ) );
  USUL_STATIC_ASSERT (  8 == sizeof ( _normals        ) );
  USUL_STATIC_ASSERT (  4 == sizeof ( _colors         ) );
  USUL_STATIC_ASSERT (  1 == sizeof ( _dirty          ) );
  USUL_STATIC_ASSERT ( 64 == sizeof ( _partition      ) );
  USUL_STATIC_ASSERT ( 24 == sizeof ( _bb             ) );
#ifdef _WIN32
  USUL_STATIC_ASSERT ( 16 == sizeof ( _triangles      ) );
  USUL_STATIC_ASSERT ( 148 == sizeof ( TriangleSet     ) ); // Why?
#else
  USUL_STATIC_ASSERT ( 12 == sizeof ( _triangles      ) );
  USUL_STATIC_ASSERT ( 144 == sizeof ( TriangleSet     ) ); // Why?
#endif
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

  // Clear the partition first.
  _partition.clear();

  // Clear the map of shared vertices. (It is most likely already empty.)
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
  this->_normalsPerVertex().clear();
  this->_normalsPerFacet().clear();
  _colors->clear();

  //Clear the Max and Min Values
  _bb.init();
  
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
//  Flip the normal vector of the i'th triangle.
//
///////////////////////////////////////////////////////////////////////////////

void TriangleSet::flipNormal( unsigned int i )
{
  Triangle *t ( _triangles.at( i ) );

  this->_normalsPerVertex().at ( t->vertex0()->index() ) *= -1;
  this->_normalsPerVertex().at ( t->vertex1()->index() ) *= -1;
  this->_normalsPerVertex().at ( t->vertex1()->index() ) *= -1;

  this->_normalsPerFacet().at( i ) *= -1;
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

void TriangleSet::addTriangle ( const osg::Vec3f &v0, const osg::Vec3f &v1, const osg::Vec3f &v2, const osg::Vec3f &n, bool correctNormal, bool rebuild )
{
  // Get or make the shared vertices.
  SharedVertex *sv0 ( this->_sharedVertex ( v0 ) );
  SharedVertex *sv1 ( this->_sharedVertex ( v1 ) );
  SharedVertex *sv2 ( this->_sharedVertex ( v2 ) );

  //Add the triangle
  this->_addTriangle( sv0, sv1, sv2, n, correctNormal, rebuild );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a triangle.
//
///////////////////////////////////////////////////////////////////////////////

void TriangleSet::addTriangle ( const SharedVertex &v0, const SharedVertex &v1, const SharedVertex &v2, const osg::Vec3f &n, bool correctNormal, bool rebuild )
{
  SharedVertex *sv0 ( const_cast < SharedVertex * > ( &v0 ) );
  SharedVertex *sv1 ( const_cast < SharedVertex * > ( &v1 ) );
  SharedVertex *sv2 ( const_cast < SharedVertex * > ( &v2 ) );

  //Add the triangle
  this->_addTriangle( sv0, sv1, sv2, n, correctNormal, rebuild );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a triangle.
//
///////////////////////////////////////////////////////////////////////////////

void TriangleSet::_addTriangle ( SharedVertex *sv0, SharedVertex *sv1, SharedVertex *sv2, const osg::Vec3f &n, bool correctNormal, bool rebuild )
{
  //Make a copy
  osg::Vec3f copy ( n );

  // Fix the normal if we should.
  if( correctNormal )
  {
    // Get the three vertices of the triangle.
    osg::Vec3 one   ( _vertices->at( sv0->index() ) );
    osg::Vec3 two   ( _vertices->at( sv1->index() ) );
    osg::Vec3 three ( _vertices->at( sv2->index() ) );

    // Get the center of the triangle.
    osg::Vec3 center ( ( one + two + three ) / 3 );

    // Get the center of the model.
    osg::Vec3 cg ( _bb.center() );

    // Make the reference vector.
    osg::Vec3 refVector ( center - cg );

    // Flip the normal if it's pointing towards the center.
    if ( ( refVector * copy ) < 0 )
      copy *= -1;
  }

  // Make the new triangle.
  Triangle::ValidRefPtr t ( new Triangle ( sv0, sv1, sv2, _triangles.size() ) );

  // Append it to the list.
  _triangles.push_back ( t.get() );

  // Add new triangle to the shared vertices.
  sv0->add ( t.get() );
  sv1->add ( t.get() );
  sv2->add ( t.get() );

  // Append normal vector.
  this->_normalsPerFacet().push_back ( copy );

  // The primitive set will be valid after the scene is built.
  if( rebuild )
  {
    //For convienence
    osg::ref_ptr< osg::Vec3Array > normals ( &this->_normalsPerVertex() );

    //How many points do we have    
    const unsigned int numPoints ( _triangles.size() * 3 );

    // Make space.
    normals->reserve      ( _vertices->size() );
    _partition.reserve    ( numPoints );

    // Add the triangle
    _partition.add ( t.get(), *_vertices, copy, true );

    // Average the normals if we are suppose to
    if ( !normals->empty() )
    {
      // Get the normals.
      osg::Vec3 n1 ( this->_averageNormal ( sv0 ) );
      osg::Vec3 n2 ( this->_averageNormal ( sv1 ) );
      osg::Vec3 n3 ( this->_averageNormal ( sv2 ) );

      // Make sure they are normalized.
      n1.normalize();
      n2.normalize();
      n3.normalize();

      // Set the normal values.
      normals->push_back( n1 );
      normals->push_back( n2 );
      normals->push_back( n3 );
    }

    //_partition.purge();
  }
  else
  {
    // Need to rebuild per-vertex normals and indices.
    _dirty = true;
  }

  // Check the max and min values
  this->_setMaxMinValues ( sv0 );
  this->_setMaxMinValues ( sv1 );
  this->_setMaxMinValues ( sv2 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Sets the Max and Min values for XYZ if any of the values are greater than
//  or less than the existing values
//
///////////////////////////////////////////////////////////////////////////////

void TriangleSet::_setMaxMinValues ( SharedVertex *sv ) 
{
    const osg::Vec3f &v ( this->getVertex( sv->index() ) );
    if ( v.x() > _bb.xMax() ) _bb.xMax() = v.x();
    if ( v.x() < _bb.xMin() ) _bb.xMin() = v.x();
    if ( v.y() > _bb.yMax() ) _bb.yMax() = v.y();
    if ( v.y() < _bb.yMin() ) _bb.yMin() = v.y();
    if ( v.z() > _bb.zMax() ) _bb.zMax() = v.z();
    if ( v.z() < _bb.zMin() ) _bb.zMin() = v.z();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the shared-vertex map.
//
///////////////////////////////////////////////////////////////////////////////

void TriangleSet::addStart()
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

void TriangleSet::addFinish()
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
  // Make copy of the options.
  Options options ( opt );

  // Show the progress bar.
  Usul::Interfaces::IProgressBar::ShowHide showHide ( Usul::Resources::progressBar() );

  // User feedback.
  this->_setStatusBar ( "Building Scene ..." );

  // Start at zero.
  this->_setProgressBar ( true, 0, 100 );

  // The scene root.
  osg::ref_ptr<osg::Group> root ( new osg::Group );

  // Should we use averaged normals?
  const bool average ( "average" == options["normals"] );

  // Rebuild if dirty or if we need per vertex normals are there aren't any.
  if ( _dirty || ( average && this->_normalsPerVertex().empty() ) )
  {
    // Clear the partition.  Make sure it's cleared before subdivided.
    _partition.clear();

    // Subdivide the partition
    _partition.subdivide( _bb, 6 );

    //For convienence
    osg::ref_ptr< osg::Vec3Array > normals ( &this->_normalsPerVertex() );

    //How many points do we have    
    const unsigned int numPoints ( _triangles.size() * 3 );

    // Size the normals if we should.
    if ( average )
      normals->resize       ( _vertices->size() );
    
    // Make some room in the partition.
    _partition.reserve   ( numPoints );

    // Initialize counter for progress.
    unsigned int count ( 0 );

    // Update progress bar every second.
    Usul::Policies::TimeBased elapsed ( 1000 );

    // Loop through the triangles.
    for ( Triangles::const_iterator i = _triangles.begin(); i != _triangles.end(); ++i )
    {
      // Shortcuts to the triangle.
      Triangle* triangle ( i->get() );

      // Should be valid.
      USUL_ASSERT ( 0x0 != triangle );

      // Add the triangle to the partition
      _partition.add ( triangle, *_vertices, this->_normalsPerFacet().at( count ) );

      // If we are suppose to add averaged normals...
      if( average )
      {
        // Get the vertices.
        const SharedVertex *sv0 ( triangle->vertex0() );
        const SharedVertex *sv1 ( triangle->vertex1() );
        const SharedVertex *sv2 ( triangle->vertex2() );

        // Make sure we have good pointers.
        if ( !sv0 || !sv1 || !sv2 )
          throw std::runtime_error ( "Error 2040664771: null vertex found when trying to build scene" );

        // Get the normals.
        osg::Vec3 n1 ( this->_averageNormal ( sv0 ) );
        osg::Vec3 n2 ( this->_averageNormal ( sv1 ) );
        osg::Vec3 n3 ( this->_averageNormal ( sv2 ) );

        // Make sure they are normalized.
        n1.normalize();
        n2.normalize();
        n3.normalize();

        // Set the normal values.
        normals->at( sv0->index() ) = n1;
        normals->at( sv1->index() ) = n2;
        normals->at( sv2->index() ) = n3;
      }

      // Show progress.
      this->_setProgressBar ( elapsed(), count, _triangles.size() );
      ++count;
    }

    // Purge any excess memory from the partition.
    _partition.purge();

    _dirty = false;
  }

  osg::ref_ptr< osg::Vec3Array > normals;

  //Get the right normals
  if( average )
    normals = &this->_normalsPerVertex();
  else
    normals = &this->_normalsPerFacet();

  // Get the node that the partition builds.
  root->addChild ( _partition ( _vertices.get(), normals.get(), average ) );

  // Show we draw a bounding box?
  const bool boundingBox ( options["BoundingBox"] == "Show" );

  // Show we draw a glass bounding box?
  const bool showGlassBoundingBox ( options["GlassBoundingBox"] == "Show" );

  if ( boundingBox || showGlassBoundingBox )
  {
    OsgTools::GlassBoundingBox gbb ( _bb );
    gbb ( root.get(), boundingBox, showGlassBoundingBox, true );
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

osg::Vec3 TriangleSet::_averageNormal ( const SharedVertex *sv ) const
{
  osg::Vec3 normal;

  //Add the normal of all triangles connected to this shared vertex
  for ( SharedVertex::ConstTriangleItr i = sv->begin(); i != sv->end(); ++i )
  {
    normal += this->_normalsPerFacet().at( (*i)->index() );
  }

  //Return the normal.  Do not normalize.
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

// 
///////////////////////////////////////////////////////////////////////////////
//
//  Reset the on edge flag.
//
///////////////////////////////////////////////////////////////////////////////

void TriangleSet::resetOnEdge()
{
  //Go to each triangle and it's shared vertices and set the visited flag to false
  for( Triangles::iterator i = _triangles.begin(); i != _triangles.end(); ++ i )
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
//  Delete triangle at given index
//
///////////////////////////////////////////////////////////////////////////////

void TriangleSet::deleteTriangle( const osg::Drawable *d, unsigned int index )
{
  //Get the triangle to remove.
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

  // Remove doomed from our vector of triangles.
  _triangles.erase( doomed );

  // Remove the normal.
  this->_normalsPerFacet().erase( ( _normalsPerFacet().begin() + index ) );

  // Remove the triangle from the partition.
  _partition.remove( d, index );

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
  _partition.clear();
  _triangles.clear();
  _vertices->clear();
  this->_normalsPerVertex().clear();
  this->_normalsPerFacet().clear();
  _colors->clear();
  _bb.init();

  // Make enough room
  this->reserve( keepers.size() );

  // Initialize the shared vertex map
  this->addStart(  );

  this->_setStatusBar( "Adding Triangles..." );

  // Show the progress bar.
  Usul::Interfaces::IProgressBar::ShowHide showHide ( Usul::Resources::progressBar() );

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

  // Done with the shared vertex map
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
//  Turn on color and set all triangles to given color.
//
///////////////////////////////////////////////////////////////////////////////

void TriangleSet::colorOn( const osg::Vec4& color )
{
#if 0
  _geometry->setColorArray( _colors.get() );

  _colors->resize ( _triangles.size() );
  std::fill ( _colors->begin(), _colors->end(), color );
      
  _geometry->setColorBinding ( osg::Geometry::BIND_PER_PRIMITIVE );
  _geometry->dirtyDisplayList();
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Color the ith triangle with given color.
//
///////////////////////////////////////////////////////////////////////////////

void TriangleSet::color ( unsigned int index, const osg::Vec4& color )
{
#if 0
  _colors->at( index ) = color;
  _geometry->dirtyDisplayList();
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Turn off color.
//
///////////////////////////////////////////////////////////////////////////////

void TriangleSet::colorOff ()
{
#if 0
  _geometry->setColorArray ( 0x0 );
  _geometry->dirtyDisplayList();
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the display list flag.
//
///////////////////////////////////////////////////////////////////////////////

bool TriangleSet::displayList() const
{
  return _partition.displayList();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the display list flag.
//
///////////////////////////////////////////////////////////////////////////////

void TriangleSet::displayList ( bool b )
{
  _partition.displayList( b );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the first vertex of the i'th triangle in the drawable.
//
///////////////////////////////////////////////////////////////////////////////

const osg::Vec3f & TriangleSet::vertex0 ( const osg::Drawable* d, unsigned int i ) const
{
  unsigned int index ( _partition.index0 ( d, i ) );
  return _vertices->at( index );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the second vertex of the i'th triangle in the drawable.
//
///////////////////////////////////////////////////////////////////////////////

const osg::Vec3f & TriangleSet::vertex1 ( const osg::Drawable* d, unsigned int i ) const
{
  unsigned int index ( _partition.index1 ( d, i ) );
  return _vertices->at( index );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the third vertex of the i'th triangle in the drawable.
//
///////////////////////////////////////////////////////////////////////////////

const osg::Vec3f & TriangleSet::vertex2 ( const osg::Drawable* d, unsigned int i ) const
{
  unsigned int index ( _partition.index2 ( d, i ) );
  return _vertices->at( index );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the first shared vertex of the i'th triangle in the drawable.
//
///////////////////////////////////////////////////////////////////////////////

const SharedVertex* TriangleSet::sharedVertex0 ( const osg::Drawable *d, unsigned int i ) const
{
  return _partition.sharedVertex0( d, i );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the second shared vertex of the i'th triangle in the drawable.
//
///////////////////////////////////////////////////////////////////////////////

const SharedVertex* TriangleSet::sharedVertex1 ( const osg::Drawable *d, unsigned int i ) const
{
  return _partition.sharedVertex1( d, i );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the third shared vertex of the i'th triangle in the drawable.
//
///////////////////////////////////////////////////////////////////////////////

const SharedVertex* TriangleSet::sharedVertex2 ( const osg::Drawable *d, unsigned int i ) const
{
  return _partition.sharedVertex2( d, i );
}
