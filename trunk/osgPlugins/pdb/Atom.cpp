
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include <sstream>
#include <algorithm> //for std::transform

#include "osg/Matrixd"

#include "Atom.h"
#include "PeriodicTable.h"


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Atom::Atom ( const char *atomString, std::string type, const PeriodicTable &periodicTable ) :
_type( type ),
_matrix ( new osg::MatrixTransform )
{
	char num[9]; //temp storage
	//get id number
  _getData(num, atomString, 6, 5);
	_id = atoi(num);

  //get name
  _getData(num, atomString, 12, 4);
	_name = num;
  std::istringstream inName(_name);
  inName >> _name;

  //get residue name
  _getData(num, atomString, 17, 3);
  _residueName = num;

  //get residue ID
  _getData(num, atomString, 22, 4);
  _residueId = atoi(num);

  //get center point
  //get X
  _getData(num, atomString, 30, 8);
  _point[0] = (float) atof( num );

  //get Y
  _getData(num, atomString, 38, 8);
  _point[1] = (float) atof(num);

  //get Z
  _getData(num, atomString, 46, 8);
  _point[2] = (float) atof(num);

  //get occupancy
  _getData(num, atomString, 54, 6);
  _occupancy = atof(num);

  //get temp Factor
  _getData(num, atomString, 60, 6);
  _tempFactor = atof(num);

  //get segment ID
  _getData(num, atomString, 72, 4);
  _segmentId = num;

  //get symbol
  _getData(num, atomString, 76, 2);
  std::string symbol = num;
  std::istringstream in (symbol);
  in >> symbol;
  std::transform ( symbol.begin(), symbol.end(), symbol.begin(), ::toupper );
  _element = periodicTable.getPointer(symbol);
  if(_element == NULL)
  {
    for(unsigned int i = 0; i < _name.size() - 1; i++)
    {
      std::string test = _name.substr(i,2);
      std::transform ( test.begin(), test.end(), test.begin(), ::toupper );
      _element = periodicTable.getPointer(test);
      if(_element != NULL)
        break;
    }
    if(_element == NULL)
    {
      for(unsigned int i = 0; i < _name.size(); i++)
      {
        std::string test = _name.substr(i,1);
        std::transform ( test.begin(), test.end(), test.begin(), ::toupper );
        _element = periodicTable.getPointer(test);
        if(_element != NULL)
          break;
      }
    }
  }

  _setMatrix();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Copy Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Atom::Atom(const Atom& atom) :
  _id(atom.getId()),
  _point(atom.getVec3()),
  _name(atom.getName()),
  _type(atom.getType()),
  _element(atom.getElement()),
  _matrix(atom.getMatrix())
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
  this->_type = atom.getType();
  this->_element = atom.getElement();
  this->_matrix = atom.getMatrix();
  return *this;
}

const float Atom::getRadius() const
{
  if(_element == NULL)
    return 0.5f;
  return _element->getRadius();
}

void Atom::_setMatrix()
{
  // Make a translation.
  osg::Matrixd T;
  T.makeTranslate ( _point );

  // Random rotation.
  osg::Matrixd R;
  float angle ( osg::PI * float ( ::rand() ) / float ( RAND_MAX ) );
  osg::Vec3 axis ( ::rand(), ::rand(), ::rand() );
  axis.normalize();
  R.makeRotate ( angle, axis );

  // Make a matrix-transform.
  _matrix->setMatrix ( R * T );
}

void Atom::_getData(char * temp, const char* string, unsigned int offset, unsigned int length)
{
  memset(temp, 0, 9 * sizeof(char));
  strncpy(temp, string+offset, length);
}
