
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __OSG_PDB_PLUGIN_MOLECULE_H__
#define __OSG_PDB_PLUGIN_MOLECULE_H__

#include "Atom.h"
#include "Bond.h"
#include "MaterialChooser.h"
#include "SphereFactory.h"

#include "osg/Referenced"


namespace osg { class Group; class LOD; class Geode; }

#include <map>
#include <list>


class Molecule : public osg::Referenced
{
public:
  typedef std::map<Atom::ID, Atom> Map;
  typedef Map Atoms;
  typedef std::list<Bond> Bonds;

  Molecule ( MaterialChooser *mc, SphereFactory *sf );
  osg::Group* build() const { return _build(); }
  void addAtom(const Atom&);
  void addBond(Atom::ID, Atom::ID);

protected:
  virtual ~Molecule();
  osg::Group*             _build() const;
  osg::Node *             _makeAtom ( const Atom &atom ) const;
  osg::Node *             _makeBond ( const Bond &bond) const;
  osg::Node *             _makeSphere ( const osg::Vec3 &center, float radius, float detail ) const;
  osg::Node *             _makeSphere ( const osg::Vec3 &center, float radius, unsigned int numSubDivisions ) const;
  osg::Node *             _makeCube   ( const osg::Vec3 &center, float size ) const;
  void                    _setCentersAndRanges ( osg::LOD *lod ) const;
private:

  Molecule();

  Atoms _atoms;
  Bonds _bonds;
  float _maxDistanceFactor;
  float _lastRangeMax;
  unsigned int _numLodChildren;
  float _lodDistancePower;
  MaterialChooser *_materialChooser;
  mutable SphereFactory::Ptr _sphereFactory;
};


#endif