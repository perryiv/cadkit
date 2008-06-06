
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "Molecule.h"
#include "BondFinder.h"
#include "Enum.h"

#include "osg/Geometry"
#include "osg/MatrixTransform"
#include "osg/Geode"
#include "osg/LOD"
#include "osg/Shape"
#include "osg/ShapeDrawable"
#include "osg/Material"
#include "osg/Vec3"

#include <limits>
#include <vector>
#include <iostream>
#include <stdexcept>
#if _DEBUG
#define NOMINMAX

#if _WIN32
# include <windows.h>
#else
#include <sys/types.h>
#include <dirent.h>
#include "Usul/File/Path.h"
#endif

#include <sstream>
#include <ctime>
#include <float.h>
#endif

using namespace osgPlugins;
using namespace osgPlugins::pdb;

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Molecule::Molecule ( MaterialFactory *mc, ShapeFactory *sf, unsigned int flags, unsigned int numAtoms ) : 
  _atoms             (),
  _bonds             (),
  _maxDistanceFactor ( 100 ),
  _lastRangeMax      ( std::numeric_limits<float>::max() ),
  _numLodChildren    ( 5 ),
  _stepFactor        ( 10 ),
  _lodDistancePower  (  2 ),
  _materialFactory   ( mc ),
  _shapeFactory      ( sf ),
  _minNumSegsLat     (  3 ),
  _maxNumSegsLat     ( 50 ),
  _minNumSegsLong    (  6 ),
  _maxNumSegsLong    ( 50 ),
  _flags             ( flags )
{
  _atoms.reserve( numAtoms );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Molecule::~Molecule()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add an atom.
//
///////////////////////////////////////////////////////////////////////////////

void Molecule::addAtom(Atom::Ptr atom)
{
  _atoms.push_back( atom );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a bond.
//
///////////////////////////////////////////////////////////////////////////////

void Molecule::addBond(Atom::ID id1, Atom::ID id2)
{
#if 0
  // Find the atoms with given ids.
  Map::const_iterator i1 = _atoms.find ( id1 );
  Map::const_iterator i2 = _atoms.find ( id2 );

  // Check to see if the atom ids exist, if so add bond to the list.
  if(_atoms.end() != i1 && _atoms.end() != i2)
    _bonds.push_back( new Bond ( i1->second, i2->second, _bonds.size() + 1));
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the molecule
//
///////////////////////////////////////////////////////////////////////////////

osg::Group *Molecule::_build() const
{
#if _DEBUG
  std::ostringstream message;
  clock_t start;
  double total_second;
  start = ::clock();
#endif

  // If no bonds exist use nearest neighbor to guess where they are.
#if 0 //TODO should this be a plugin?
  BondFinder bf;
  if ( _bonds.empty() && ( PDB::LOAD_BONDS == ( _flags & PDB::LOAD_BONDS ) ) )
    bf.findBonds ( bf.mapToVector ( _atoms ), _bonds );
#endif

#if _DEBUG
  total_second = (double)(::clock() - start) / CLOCKS_PER_SEC;
  message << "Time to find bonds: " << total_second << std::endl;
  start = ::clock();
#endif
  
  // The scene root.
  osg::ref_ptr<osg::Group> root ( new osg::Group );

  // If we are supposed to load the atoms...
  if ( PDB::LOAD_ATOMS == ( _flags & PDB::LOAD_ATOMS ) )
  {
    // Loop through all the atoms.
    for ( Atoms::const_iterator i = _atoms.begin(); i != _atoms.end(); ++i )
    {
      // Add the node for this atom.
      root->addChild ( this->_makeAtom ( *i ) );
    }
  }

#if _DEBUG
  total_second = (double)(::clock() - start) / CLOCKS_PER_SEC;
  message << "Time to build atoms: " << total_second << std::endl;
  start = ::clock();
#endif


#if 1 // TODO, bonds are not working real well...

  // If we are supposed to load the bonds...
  if ( PDB::LOAD_BONDS == ( _flags & PDB::LOAD_BONDS ) )
  {
    // Loop through the bonds.
    for ( Bonds::const_iterator i = _bonds.begin(); i != _bonds.end(); ++i )
    {
      // Add the node for this bond.
      root->addChild ( this->_makeBond ( *i ) );
    }
  }
  
#if _DEBUG
  total_second = (double)(::clock() - start) / CLOCKS_PER_SEC;
  message << "Time to build bonds: " << total_second << std::endl;
#endif

#endif

#if _DEBUG
  message << "Number of Atoms: " << _atoms.size() << std::endl;
  message << "Number of Bonds: " << _bonds.size() << std::endl;
 // GCC balks at the next line - '::OutputDebugString' has not been declared'
 // ::OutputDebugString( message.str().c_str() );
#endif

  // Return the root.
  return root.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make an bond.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node *Molecule::_makeBond (const Bond::Ptr bond ) const
{
  osg::ref_ptr<osg::LOD> lod ( new osg::LOD );
  lod->setName( bond->toString() );
  
  // Set the lod's material. This will effect all the children.
  osg::ref_ptr<osg::StateSet> ss ( lod->getOrCreateStateSet() );
  ss->setAttribute ( _materialFactory->create( "Bond" ) );

  // Get Matrix Transform for this bond
  osg::ref_ptr<osg::MatrixTransform> mt ( bond->getMatrix() );
  mt->addChild( lod.get() );

  osg::Vec3 point1 ( bond->getPoint1() );
  osg::Vec3 point2 ( bond->getPoint2() );

  // Add several cylinders
  unsigned int numBondLods( 5 );
  for(unsigned int i = 0; i < numBondLods  - 1; ++i)
  {
    unsigned int sides = 5 + ( numBondLods - i - 2) * (_stepFactor / 2);
    lod->addChild (this->_makeCylinder( point1, point2, 0.20f, sides));
  }
  
  // Make last LOD a line.
  osg::ref_ptr<osg::Geode> geode ( new osg::Geode );
  osg::ref_ptr<osg::Geometry> geom ( new osg::Geometry );
  osg::ref_ptr<osg::Vec3Array> p ( new osg::Vec3Array );

  p->resize ( 2 );
  (*p)[0] = osg::Vec3(0.0, 0.0, 0.0);
  (*p)[1] = osg::Vec3(0.0, 1.0, 0.0);

  geom->setVertexArray ( p.get() );
  geom->addPrimitiveSet ( new osg::DrawArrays ( osg::PrimitiveSet::LINES, 0, 2 ) );
  geode->addDrawable ( geom.get() );

  lod->addChild( geode.get() );

  osg::Vec3 center;
  center[0] = (point1[0] + point2[0])/2.0;
  center[1] = (point1[1] + point2[1])/2.0;
  center[2] = (point1[2] + point2[2])/2.0;

  // Set the range.
  this->_setCentersAndRanges ( lod.get(), center );

  return mt.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make a cylinder.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node * Molecule::_makeCylinder ( const osg::Vec3 &point1, const osg::Vec3 &point2, float radius, unsigned int sides ) const
{
  // Make a cylinder of length one with given radius and number of sides.
  osg::ref_ptr<osg::Geometry> geom ( _shapeFactory->cylinder ( radius, sides ) );

  // Add cylinder to the scene.
  osg::ref_ptr<osg::Geode> geode ( new osg::Geode );
  geode->addDrawable ( geom.get() );

  return geode.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make an atom.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node *Molecule::_makeAtom ( Atom::Ptr atom ) const
{
  // The lod holding the various representations.
  osg::ref_ptr<osg::LOD> lod ( new osg::LOD );

  // The matrix-transform holding the atom.
  osg::ref_ptr<osg::MatrixTransform> mt ( atom->getMatrix() );
  mt->addChild ( lod.get() );

  // Set the lod's material. This will effect all the children.
  osg::ref_ptr<osg::StateSet> ss ( mt->getOrCreateStateSet() );
  ss->setAttribute ( _materialFactory->create ( atom->getSymbol() ) );

  //mt->setUserData( atom.get() );

  // Get the atom's numbers.
  const osg::Vec3 center ( atom->getCenter() );
  const float radius ( atom->getRadius() );

  // Name the matrix transform with the data from the atom.
  mt->setName ( atom->toString() );

  // Add several spheres.
  float denominator ( _numLodChildren - 1 );
  for ( unsigned int i = 0; i < _numLodChildren - 1; ++i )
  {
    float loop ( i );
    float detail ( ::pow ( 1.0f - loop / denominator, _lodDistancePower ) );
    lod->addChild ( this->_makeSphere ( center, radius * .75, osg::Vec2 ( detail, detail ) ) );
    lod->getChild( i )->setUserData( atom.get() );
  }

  // Last child.
  lod->addChild ( this->_makeCube ( center, radius ) );

  // Set the centers and ranges.
  this->_setCentersAndRanges ( lod.get() );

  // Return the lod.
  return mt.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make a sphere.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node *Molecule::_makeSphere ( const osg::Vec3 &center, float radius, const osg::Vec2 &detail ) const
{
  // Determine the number of latitudinal and longitudinal segments.
  unsigned int latitude  ( _minNumSegsLat  + detail[0] * ( _maxNumSegsLat  - _minNumSegsLat  ) );
  unsigned int longitude ( _minNumSegsLong + detail[1] * ( _maxNumSegsLong - _minNumSegsLong ) );

  // Make a sphere.
  ShapeFactory::MeshSize size ( latitude, longitude );
  ShapeFactory::LatitudeRange  latRange  ( 89.9f, -89.9f );
  ShapeFactory::LongitudeRange longRange (  0.0f, 360.0f );
  osg::ref_ptr<osg::Geometry> geom ( _shapeFactory->sphere ( radius, size, latRange, longRange ) );

  // Add the geometry to a geode.
  osg::ref_ptr<osg::Geode> geode ( new osg::Geode );
  geode->addDrawable ( geom.get() );

  return geode.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make a cube.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node *Molecule::_makeCube ( const osg::Vec3 &center, float size ) const
{
  // Make the cube.
  osg::Vec3 sides ( size, size, size );
  osg::ref_ptr<osg::Geometry> geom ( _shapeFactory->cube ( sides ) );

  // Add the cube to a geode.
  osg::ref_ptr<osg::Geode> geode ( new osg::Geode );
  geode->addDrawable ( geom.get() );

  return geode.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the lod center and ranges.
//
///////////////////////////////////////////////////////////////////////////////

void Molecule::_setCentersAndRanges ( osg::LOD *lod ) const
{
  // If there are no children then just return.
  if ( 0 == lod->getNumChildren() )
    return;

  // Get the first child.
  osg::Node *child = lod->getChild ( 0 );

  // Get the bounding sphere for the first child.
  const osg::BoundingSphere &boundingSphere = child->getBound();
  if ( boundingSphere.radius() <= 0 )
    throw ( std::runtime_error ( "Error 1614663463, bounding sphere has zero radius" ) );

  // Get the center of the bounding sphere.
  const osg::Vec3 &center ( boundingSphere.center() );

  // Set the center of this lod to be the center of the bounding sphere.
  lod->setCenter ( center );

  // The minimum of the range we set.
  float rangeMin ( 0 );
  // The maximum distance for the lod ranges.
  float maxDist ( _maxDistanceFactor * boundingSphere.radius() );

  this->_setRanges( lod, rangeMin, maxDist );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the lod center and ranges for cylinder.
//
///////////////////////////////////////////////////////////////////////////////

void Molecule::_setCentersAndRanges ( osg::LOD *lod, const osg::Vec3 &center ) const
{
  // If there are no children then just return.
  if ( 0 == lod->getNumChildren() )
    return;

  // Set the center of this lod to be the center of the bounding sphere.
  lod->setCenter ( center );

  // The minimum of the range we set.
  float rangeMin ( 0 );
  // The maximum distance for the lod ranges.
  float maxDist ( _maxDistanceFactor * 0.50f );

  this->_setRanges( lod, rangeMin, maxDist );
}

void Molecule::_setRanges( osg::LOD *lod, float rangeMin, float maxDist  ) const
{
  // Loop through all of the children except the last one.
  // Note: Unlike previous versions of OSG, there is one LOD "range" for 
  // each LOD "child". Each "range" has a min and max value.
  unsigned int numChildren ( lod->getNumChildren() );
  for ( unsigned int i = 0; i < numChildren - 1; ++i )
  {
    // Set the range.
    float rangeMax = ( ( (float) i + 1 ) * maxDist ) / ( (float) ( numChildren - 1 ) );
    lod->setRange ( i, rangeMin, rangeMax );
    rangeMin = rangeMax;
  }

  // Set the range for the last child.
  lod->setRange ( numChildren - 1, rangeMin, _lastRangeMax );
}

