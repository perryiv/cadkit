///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////


#ifndef __OSG_PDB_PLUGIN_ATOM_H__
#define __OSG_PDB_PLUGIN_ATOM_H__

#include <osg/Vec3>
#include <string>


class Atom 
{
public:
  typedef unsigned int ID;

  Atom() { _id = 0; }
	Atom(char*, std::string);
  Atom(const Atom&);
	const float getX() const { return _point[0]; }
	const float getY() const { return _point[1]; }
	const float getZ() const { return _point[2]; }
	const float getR() const { return _r; }
	std::string getName() const { return _name; }
	std::string getType() const { return _type; }
	const ID getId() const { return _id; }
	const osg::Vec3& getVec3() const { return _point; }
	std::string toString() const;
  bool valid() const { return _id != 0; }

  Atom& operator=(const Atom&);
private:
	ID _id;
  osg::Vec3 _point;
	float _r;
	std::string _name, _type;
};

#endif