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
#include <osg/Vec4>
#include <string>
#include <sstream>

class Atom {
public:
	Atom(std::istringstream&, std::string);
	Atom(char*, std::string);
	const float getX() const { return _x; }
	const float getY() const { return _y; }
	const float getZ() const { return _z; }
	const float getR() const { return _r; }
	std::string getName() const { return _name; }
	std::string getType() const { return _type; }
	int getId() { return _id; }
	osg::Vec3 getVec3() { return osg::Vec3(_x, _y, _z); }
	std::string toString()const;
private:
	int _id;
	float _x,_y,_z, _r;
	std::string _name, _type;
};

#endif