
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////


#ifndef __OSG_PDB_PLUGIN_ATOM_H__
#define __OSG_PDB_PLUGIN_ATOM_H__

#include "osg/Referenced"
#include "osg/Vec3"
#include "osg/MatrixTransform"
#include "osg/ref_ptr"

#include "Element.h"
#include "Export.h"

#include <string>
#include <iostream>
#include <iomanip>

class PeriodicTable;

namespace osgPlugins {
namespace pdb {

class OSG_PDB_EXPORT Atom : public osg::Referenced
{
public:
  typedef osg::Referenced BaseClass;
  typedef osg::ref_ptr< Atom > Ptr;
  typedef unsigned int ID;

  Atom(const char*, const std::string&, const PeriodicTable &);
  Atom(const Atom&);
  Atom& operator=(const Atom&);
  
  //Getters for ATOM information
  const std::string& getType() const { return _type; }
  const ID getId() const { return _id; }
  const std::string& getName() const { return _name; }
  const char getAlternateLocation() const { return _altLocation; }
  const std::string& getResidueName() const { return _residueName; } 
  const ID getChainId() const { return _chainId; }
  const ID getResidueId() const { return _residueId; }
  const char getInsertion() const { return _insertion; }
  const osg::Vec3& getCenter() const { return _point; }
  const float getOccupancy() const { return _occupancy; }
  const float getTempFactor() const { return _tempFactor; }
  const std::string& getSegmentId() const { return _segmentId; }
  const std::string& getSymbol() const { return _element->getElementSymbol(); }
  const std::string& getCharge() const { return _charge; }

  //set/get fixed atom
  bool fixed();
  void fixed ( bool );

  //get the atom information as a string
  std::string toString() const;
  //Is this a valid atom
  bool valid() const { return _id != 0; }
  //Get the radius of this atom
  const float getRadius() const;
  
  //Get/Set the matrix transform
  osg::MatrixTransform* getMatrix () const { return _matrix.get(); }
  void setMatrix(osg::MatrixTransform *m) { _matrix = m; }

  //friend std::ostream& operator<<( std::ostream& os, const Atom& );
 //friend std::ostream& operator<<( std::ostream& os, const Atom::Ptr );
  friend std::ostream& operator<<( std::ostream& os, const Atom& atom )
  {
    osg::Vec3 center ( atom.getMatrix()->getMatrix().getTrans() );
    
    os.setf( std::ios::left );
    os << std::setw(6) << atom.getType();
    os.unsetf ( std::ios::left );
    os << std::setw(5) << atom.getId();
    os.setf ( std::ios::left );
    os << " "
      << std::setw(4) << atom.getName()
      << atom.getAlternateLocation()
      << std::setw(3) << atom.getResidueName()
      << " "
      << atom.getChainId()
      << std::setw(4) << atom.getResidueId()
      << atom.getInsertion()
      << "   "
      << std::setw(8) << center[0]
      << std::setw(8) << center[1]
      << std::setw(8) << center[2]
      << std::setw(6) << atom.getOccupancy()
      << std::setw(6) << atom.getTempFactor()
      << "      "
      << std::setw(4) << atom.getSegmentId();
    os.unsetf( std::ios::left );
    os << std::setw(2) << atom.getSymbol();
    os.setf( std::ios::left );
    os << std::setw(2) << atom.getCharge();

    return os;
  }

protected:
  Atom();
  virtual ~Atom();
private:
  void _setMatrix();
  void _getData(char* temp, const char* string, unsigned int offset, unsigned int length);

  Element::Ptr _element;
  ID _id, _residueId, _chainId;
  osg::Vec3 _point;
  std::string _type, _name, _residueName, _segmentId, _charge;
  float _occupancy, _tempFactor;
  char _insertion, _altLocation;
  mutable osg::ref_ptr< osg::MatrixTransform > _matrix;
};

}; // namespace pdb
}; // namespace osgPlugins

#endif
