///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

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

#include "osg/Vec3"

class Atom;


class Bond 
{
public:
  Bond(const Atom&, const Atom&, int);
  ~Bond();
  const int getId() const { return _id; }
  const osg::Vec3& getPoint1() const { return _point1; }
  const osg::Vec3& getPoint2() const { return _point2; }
  const std::string toString() const;
private:
  Bond();
  int _id;
  osg::Vec3 _point1, _point2;
};

#endif