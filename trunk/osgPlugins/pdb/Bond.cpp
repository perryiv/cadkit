
#include "Bond.h"
#include "Atom.h"

#include <sstream>

Bond::Bond(const Atom& lhs, const Atom& rhs, int id)
{
  _id = id;
  _r = 0.15f;
  _h = this->_getHeight(lhs.getVec3(), rhs.getVec3());
  _point1 = lhs.getVec3();
  _point2 = rhs.getVec3();
}

const std::string Bond::toString() const
{
  std::ostringstream out;
	out << _id <<" " << _h << " " << _r << " " << _point1[0] << " " <<  _point1[1] << " " <<  _point1[2];
	return std::string(out.str());
}



float Bond::_getHeight(const osg::Vec3& v1, const osg::Vec3& v0)
{
  float deltaX = v1[0] - v0[0];
  float deltaY = v1[1] - v0[1];
  float deltaZ = v1[2] - v0[2];
  return (float) sqrt((deltaX * deltaX) + (deltaY * deltaY) + (deltaZ * deltaZ));
}