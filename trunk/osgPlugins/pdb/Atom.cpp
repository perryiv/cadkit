
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include <sstream>

#include "Atom.h"
#include "PeriodicTable.h"


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Atom::Atom ( const char *atomString, std::string type, const PeriodicTable &periodicTable )
{
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
  _point[0] = (float) atof(num);
  memset(num, 0, 9 * sizeof(char));
  strncpy(num, atomString+38, 8);
  _point[1] = (float) atof(num);
  memset(num, 0, 9 * sizeof(char));
  strncpy(num, atomString+46, 8);
  _point[2] = (float) atof(num);
	//set radius
	_r = 0.5f;
	_type = type;
  //get symbol
  memset(num, 0, 9 * sizeof(char));
  strncpy(num, atomString + 76, 2);
  _symbol = num;
  std::istringstream in (_symbol);
  in >> _symbol;
  _element = periodicTable.getPointer(_symbol);
}


///////////////////////////////////////////////////////////////////////////////
//
//  Copy Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Atom::Atom(const Atom& atom) :
  _id(atom.getId()),
  _point(atom.getVec3()),
  _r(atom.getR()),
  _name(atom.getName()),
  _type(atom.getType()),
  _symbol(atom.getSymbol()),
  _element(atom.getElement())
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Atom::~Atom()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Returns data members as a string
//
///////////////////////////////////////////////////////////////////////////////

std::string Atom::toString() const
{
	std::ostringstream out;
	out << _type <<" " << _id << " " << _name << " " << _point[0] << " " <<  _point[1] << " " <<  _point[2];
	return std::string(out.str());
}


///////////////////////////////////////////////////////////////////////////////
//
//  Assignment operator
//
///////////////////////////////////////////////////////////////////////////////

Atom& Atom::operator =(const Atom& atom)
{
  this->_id = atom.getId();
  this->_name = atom.getName();
  this->_point = atom.getVec3();
  this->_r = atom.getR();
  this->_type = atom.getType();
  this->_symbol = atom.getSymbol();
  this->_element = atom.getElement();
  return *this;
}

const float Atom::getRadius() const
{
  if(_element == NULL)
    return 0.5f;
  return _element->getRadius();
}