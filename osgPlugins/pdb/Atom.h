
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
#include "osg/MatrixTransform"
#include "osg/ref_ptr"
#include "Element.h"
#include <string>


class PeriodicTable;

//TODO inherit from osg::Referenced?
class Atom 
{
public:
  typedef unsigned int ID;

  Atom(const char*, std::string, const PeriodicTable &);
  Atom(const Atom&);
  ~Atom();
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
  osg::MatrixTransform* getMatrix () const { return _matrix.get(); }
  void setMatrix(osg::MatrixTransform *m) { _matrix = m; }
  const ID getResidueId() const { return _residueId; }
  const std::string getResidueName() const { return _residueName; }
  const std::string getSegmentId() const { return _segmentId; }
  const float getOccupancy() const { return _occupancy; }
  const float getTempFactor() const { return _tempFactor; }
private:
  Atom();

  void _setMatrix();
  void _getData(char* temp, const char* string, unsigned int offset, unsigned int length);

  const Element * _element;
  ID _id, _residueId;
  osg::Vec3 _point;
  std::string _type, _name, _residueName, _segmentId;
  float _occupancy, _tempFactor;
  mutable osg::ref_ptr< osg::MatrixTransform > _matrix;
};

#endif
