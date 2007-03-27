///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __OSG_PDB_PLUGIN_MOLECULE_H__
#define __OSG_PDB_PLUGIN_MOLECULE_H__

#include "Export.h"
#include "Atom.h"
#include "Bond.h"

#include "OsgTools/ShapeFactory.h"
#include "OsgTools/MaterialFactory.h"

#include "osg/Referenced"

namespace osg { class Group; class LOD; class Geode; }

#include <map>
#include <list>

namespace osgPlugins {
namespace pdb {

class OSG_PDB_EXPORT Molecule : public osg::Referenced
{
public:
  typedef osgPlugins::pdb::Atom Atom;
  typedef osgPlugins::pdb::Bond Bond;
  //typedef std::map<Atom::ID, Atom::Ptr> Map;
  typedef std::vector<Atom::Ptr> Atoms;
  typedef std::list<Bond::Ptr> Bonds;
  typedef OsgTools::ShapeFactory ShapeFactory;
  typedef OsgTools::MaterialFactory MaterialFactory;
  
  Molecule ( MaterialFactory *mc, ShapeFactory *sf, unsigned int flags, unsigned int numAtoms );
  osg::Group* build() const { return _build(); }
  void addAtom(Atom::Ptr );
  void addBond(Atom::ID, Atom::ID);

  Atoms& atoms() { return _atoms; }
  const Atoms& atoms() const { return _atoms; }
  Bonds& bonds() { return _bonds; }

protected:
  virtual ~Molecule();
  osg::Group*             _build() const;
  osg::Node *             _makeAtom ( Atom::Ptr atom ) const;
  osg::Node *             _makeBond ( const Bond::Ptr bond) const;
  osg::Node *             _makeCylinder ( const osg::Vec3 &point1, const osg::Vec3 &point2, float radius, unsigned int sides ) const;
  osg::Node *             _makeSphere ( const osg::Vec3 &center, float radius, float detail ) const;
  osg::Node *             _makeSphere ( const osg::Vec3 &center, float radius, const osg::Vec2 &detail ) const;
  osg::Node *             _makeCube   ( const osg::Vec3 &center, float size ) const;
  void                    _setCentersAndRanges ( osg::LOD *lod ) const;
  void                    _setCentersAndRanges ( osg::LOD *lod, const osg::Vec3 &center ) const;
  void                    _setRanges( osg::LOD *lod, float rangeMin, float maxDist ) const;
private:

  Molecule();

  Atoms _atoms;
  mutable Bonds _bonds;
  float _maxDistanceFactor;
  float _lastRangeMax;
  unsigned int _numLodChildren;
  unsigned int _stepFactor;
  float _lodDistancePower;
  mutable MaterialFactory::Ptr _materialFactory;
  mutable ShapeFactory::Ptr _shapeFactory;
  unsigned int _minNumSegsLat;
  unsigned int _maxNumSegsLat;
  unsigned int _minNumSegsLong;
  unsigned int _maxNumSegsLong;
  unsigned int _flags;
};

};
};


#endif
