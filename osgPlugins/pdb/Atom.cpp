///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include <iostream>

#include "atom.h"

Atom::Atom(std::istringstream &in, std::string type)
{
	std::string word;
	//get id number
	in >> _id;
	//get name
	in >> _name;
	//discard next 3
	in >> word >> word >> word;
	//set center point
	in >> _x;
	in >> _y;
	in >> _z;
	//set radius
	_r = 0.5f;
	_type = type;

}

Atom::Atom(char *atomString, std::string type) {
	char num[9];
	//get id number
	memset(num, 0, 9 * sizeof(char));
	strncpy(num, atomString + 6, 5);
	_id = atoi(num);
	//get name
	memset(num, 0, 9 * sizeof(char));
	strncpy(num, atomString + 12, 4);
	_name = num;
	//get center point
    memset(num, 0, 9 * sizeof(char));
    strncpy(num, atomString+30, 8);
    _x = (float) atof(num);
    memset(num, 0, 9 * sizeof(char));
    strncpy(num, atomString+38, 8);
    _y = (float) atof(num);
    memset(num, 0, 9 * sizeof(char));
    strncpy(num, atomString+46, 8);
    _z = (float) atof(num);
	//set radius
	_r = 0.5f;
	_type = type;
}

std::string Atom::toString() const
{
	std::ostringstream out;
	out << _type << _id << _name << _x << _y << _z;
	return std::string(out.str());
}