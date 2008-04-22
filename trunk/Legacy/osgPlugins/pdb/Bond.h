///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////


#ifndef __OSG_PDB_PLUGIN_BOND_H__
#define __OSG_PDB_PLUGIN_BOND_H__

#include <string>

#include "osg/Referenced"
#include "osg/Vec3"
#include "osg/MatrixTransform"
#include "osg/ref_ptr"

#include "Atom.h"

namespace osgPlugins {
namespace pdb {

class Bond : public osg::Referenced
{
public:
  typedef osg::Referenced BaseClass;
  typedef osg::ref_ptr< Bond > Ptr;
  typedef osgPlugins::pdb::Atom Atom;

  Bond(Atom::Ptr, Atom::Ptr, int);
  
  const int getId() const { return _id; }
  const osg::Vec3& getPoint1() const { return _point1; }
  const osg::Vec3& getPoint2() const { return _point2; }
  const std::string toString() const;
  osg::MatrixTransform* getMatrix () const { return _matrix.get(); }
  void setMatrix(osg::MatrixTransform *m) { _matrix = m; }
protected:
  Bond();
  virtual ~Bond();
private:
  int _id;
  osg::Vec3 _point1, _point2;
  mutable osg::ref_ptr< osg::MatrixTransform > _matrix;
};

};
};

#endif
