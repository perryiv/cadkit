
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////


#ifndef __OSG_PDB_PLUGIN_ATOM_H__
#define __OSG_PDB_PLUGIN_ATOM_H__

#include "osg/Vec3"
#include "Element.h"
#include <string>


class PeriodicTable;

class Atom 
{
public:
  typedef unsigned int ID;

  Atom(const char*, std::string, const PeriodicTable &);
  Atom(const Atom&);
  ~Atom();
  const float getX() const { return _point[0]; }
  const float getY() const { return _point[1]; }
  const float getZ() const { return _point[2]; }
  const std::string getName() const { return _name; }
  const std::string getType() const { return _type; }
  const std::string getSymbol() const { return _element->getElementSymbol(); }
  const ID getId() const { return _id; }
  const osg::Vec3& getVec3() const { return _point; }
  const float length2() const { return _point.length2(); }
  const float length() const { return _point.length(); }
  std::string toString() const;
  bool valid() const { return _id != 0; }
  const float getRadius() const;
  Atom& operator=(const Atom&);
  const Element* getElement() const { return _element; }
private:
  Atom();
  const Element * _element;
  ID _id;
  osg::Vec3 _point;
  std::string _type, _name;
};

#endif
