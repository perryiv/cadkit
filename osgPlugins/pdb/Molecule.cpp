
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "Molecule.h"
#include "Cylinder.h"

#include "osg/MatrixTransform"
#include "osg/Geometry"
#include "osg/LineWidth"

#include "osg/Geode"
#include "osg/LOD"
#include "osg/Shape"
#include "osg/ShapeDrawable"

#include "osg/material"

#include <limits>


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Molecule::Molecule(MaterialChooser* mc) : 
  _atoms(),
  _bonds(),
  _maxDistanceFactor ( 75 ),
  _lastRangeMax ( std::numeric_limits<float>::max() ),
  _numLodChildren ( 5 ),
  _lodDistancePower ( 2 )
{
  _materialChooser = mc;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Molecule::~Molecule()
{
}


void Molecule::addAtom(const Atom& atom)
{
  _atoms.insert( Map::value_type (atom.getId(), atom) );
}


void Molecule::addBond(Atom::ID id1, Atom::ID id2)
{
  //find the atoms with given ids
  Map::const_iterator i1 = _atoms.find ( id1 );
  Map::const_iterator i2 = _atoms.find ( id2 );
  //check to see if the atom ids exist, if so add bond to the list
  if(_atoms.end() != i1 && _atoms.end() != i2)
    _bonds.push_back( Bond ( i1->second, i2->second, _bonds.size() + 1));
}

///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene.
//
///////////////////////////////////////////////////////////////////////////////

osg::Group *Molecule::_build() const
{
  // The scene root.
  osg::ref_ptr<osg::Group> root ( new osg::Group );

  // Loop through all the atoms.
  for ( Atoms::const_iterator i = _atoms.begin(); i != _atoms.end(); ++i )
  {
    // Get the atom.
    const Atom &atom = i->second;
 
     // make the geometry for this point.
    osg::ref_ptr<osg::LOD> lod ( this->_makeAtom ( atom ) );
    // Add the lod to the root.
    root->addChild ( lod.get() );
 
  }

  //loop through the bonds
  for (Bonds::const_iterator i = _bonds.begin(); i != _bonds.end(); ++i)
  {
    const Bond &bond = *i;
    //make the geometry
    osg::ref_ptr<osg::LOD> lod (this->_makeBond( bond ) );
    //add the lod to the root
    root->addChild ( lod.get() );
  }

  // Return the root.
  return root.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make an bond.
//  TODO, make several branches in the lod. 
//
///////////////////////////////////////////////////////////////////////////////

osg::LOD *Molecule::_makeBond (const Bond &bond ) const
{
  osg::ref_ptr<osg::LOD> lod ( new osg::LOD );

  Cylinder c(bond.getPoint1(), bond.getPoint2());

  //add several cylinders
  for(unsigned int i = 0; i < _numLodChildren; ++i)
  {
    float detail = 10.0 + (10 * i);
    osg::ref_ptr< osg::Geode > geode (new osg::Geode);
    geode->addDrawable( (c.getGeometry( _materialChooser->getMaterial( "Bond" ).get(), detail) ));
    lod->addChild(geode.release());
  }

  lod->setName( bond.toString());

  // Set the range.
  //lod->setRange ( 0, 0, std::numeric_limits<float>::max() ); // TODO, use bounding-box to set ranges.
  this->_setCentersAndRanges ( lod.get() );

  return lod.release();
}

///////////////////////////////////////////////////////////////////////////////
//
//  Make an atom.
//
///////////////////////////////////////////////////////////////////////////////

osg::LOD *Molecule::_makeAtom ( const Atom &atom ) const
{
  // The lod holding the various representations.
  osg::ref_ptr<osg::LOD> lod ( new osg::LOD );

  // Set the lod's material. This will effect all the children.
  osg::ref_ptr<osg::StateSet> ss ( lod->getOrCreateStateSet() );
  osg::ref_ptr<osg::Material> m ( _materialChooser->getMaterial ( atom.getSymbol() ) );
  ss->setAttribute ( m.get() );

  // Get the atom's numbers.
  const osg::Vec3 center ( atom.getX(), atom.getY(), atom.getZ() );
  const float radius ( atom.getR() );

  // Name the lod with the data from the atom.
  lod->setName ( atom.toString() );

  // Add several spheres.
  for ( unsigned int i = 0; i < _numLodChildren - 1; ++i )
  {
    float detail ( ::pow ( 1.0f - (float) i / ( _numLodChildren - 2 ), _lodDistancePower ) );
    lod->addChild ( this->_makeSphere ( center, radius, detail ) );
  }

  // Last child is a cube.
  //lod->addChild ( this->_makeCube ( center, radius * 1.5 ) );

  // Set the centers and ranges.
  this->_setCentersAndRanges ( lod.get() );

  // Return the lod.
  return lod.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make a sphere. TODO, use osg::Geometry for vertex arrays. 
//  Perhaps make soccer ball.
//
///////////////////////////////////////////////////////////////////////////////

osg::Geode *Molecule::_makeSphere ( const osg::Vec3 &center, float radius, float detail ) const
{
  // Make the sphere.
  osg::ref_ptr<osg::Sphere> sphere ( new osg::Sphere ( center, radius ) );
  osg::ref_ptr<osg::ShapeDrawable> drawable ( new osg::ShapeDrawable ( sphere.get() ) );

  // Adjust the number of triangles.
  osg::ref_ptr<osg::TessellationHints> hints ( new osg::TessellationHints() );
  hints->setDetailRatio ( detail );
  drawable->setTessellationHints ( hints.get() );

  // TODO, make this an option. Display lists crash with really big files.
  drawable->setUseDisplayList ( false );

  // Add the sphere to a geode.
  osg::ref_ptr<osg::Geode> geode ( new osg::Geode );
  geode->addDrawable ( drawable.get() );

  // Return the geode.
  return geode.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make a cube. TODO, use osg::Geometry for vertex arrays. 
//
///////////////////////////////////////////////////////////////////////////////

osg::Geode *Molecule::_makeCube ( const osg::Vec3 &center, float size ) const
{
  // Make the sphere.
  osg::ref_ptr<osg::Box> cube ( new osg::Box ( center, size ) );
  osg::ref_ptr<osg::ShapeDrawable> drawable ( new osg::ShapeDrawable ( cube.get() ) );

  // Add the cube to a geode.
  osg::ref_ptr<osg::Geode> geode ( new osg::Geode );
  geode->addDrawable ( drawable.get() );

  // Return the geode.
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

  // The maximum distance for the lod ranges.
  float maxDist ( _maxDistanceFactor * boundingSphere.radius() );

  // Get the center of the bounding sphere.
  const osg::Vec3 &center = boundingSphere.center();

  // Set the center of this lod to be the center of the bounding sphere.
  lod->setCenter ( center );

  // The minimum of the range we set.
  float rangeMin ( 0 );

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