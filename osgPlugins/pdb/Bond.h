
#ifndef __OSG_PDB_PLUGIN_BOND_H__
#define __OSG_PDB_PLUGIN_BOND_H__

#include <string>

#include "osg/Vec3"

class Atom;


class Bond 
{
public:
  Bond() { _id = -1; }
  Bond(const Atom&, const Atom&, int);
  const float getX() const { return _point1[0]; }
  const float getY() const { return _point1[1]; }
  const float getZ() const { return _point1[2]; }
  const float getH() const { return _h; }
  const float getR() const { return _r; }
  const int getId() const { return _id; }
  const osg::Vec3& getPoint1() const { return _point1; }
  const osg::Vec3& getPoint2() const { return _point2; }
  const std::string toString() const;
  bool valid() const { return _id != -1; }
private:
  float _getHeight(const osg::Vec3&, const osg::Vec3&);
  int _id;
  osg::Vec3 _point1, _point2;
  float _h, _r;
};

#endif