///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "PeriodicTable.h"
#include "Element.h"

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////
PeriodicTable::PeriodicTable() :
  _vectorElements(),
  _mapElements()
{
  //Element data from http://www.ktf-split.hr/periodni/en/
  //Hydrogen
  osg::ref_ptr< Element > H (new Element(1, "H", 0.373f));
  _vectorElements.push_back(H);
  _mapElements.insert( Element_pair("H", H));
  //Helium
  osg::ref_ptr< Element > He (new Element(2, "He", 1.28f));
  _vectorElements.push_back(He);
  _mapElements.insert( Element_pair("He", He));
  //Lithium
  osg::ref_ptr< Element > Li (new Element(3, "Li", 1.52f));
  _vectorElements.push_back(Li);
  _mapElements.insert( Element_pair("Li", Li));
  //Beryllium
  osg::ref_ptr< Element > Be (new Element(4, "Be", 1.11f));
  _vectorElements.push_back(Be);
  _mapElements.insert( Element_pair("Be", Be));
  //Boron
  osg::ref_ptr< Element > B (new Element(5, "B", 0.79f));
  _vectorElements.push_back(B);
  _mapElements.insert( Element_pair("B", He));
  //Carbon
  osg::ref_ptr< Element > C (new Element(6, "C", .77f));
  _vectorElements.push_back(C);
  _mapElements.insert( Element_pair("C", C));
  //Nitrogen
  osg::ref_ptr< Element > N (new Element(7, "N", .54f));
  _vectorElements.push_back(N);
  _mapElements.insert( Element_pair("N", N));
  //Oxygen
  osg::ref_ptr< Element > O (new Element(8, "O", 0.604f));
  _vectorElements.push_back(O);
  _mapElements.insert( Element_pair("O", O));
  //Fluorine
  osg::ref_ptr< Element > F (new Element(9, "F", 0.709f));
  _vectorElements.push_back(F);
  _mapElements.insert( Element_pair("F", F));
  //Neon
  osg::ref_ptr< Element > Ne (new Element(10, "Ne", 1.54f));
  _vectorElements.push_back(Ne);
  _mapElements.insert( Element_pair("Ne", Ne));

}

///////////////////////////////////////////////////////////////////////////////
//
//  Get a pointer to the element for the given element symbol.
//
///////////////////////////////////////////////////////////////////////////////
const Element* PeriodicTable::getPointer(std::string element) const 
{
  MapElements::const_iterator i = _mapElements.find(element);
  if(_mapElements.end() != i)
    return i->second.get();
  return NULL;
}

///////////////////////////////////////////////////////////////////////////////
//
//  Get a pointer to the element for the given atomic number.
//  TODO add checks to make sure given number is valid.
//
///////////////////////////////////////////////////////////////////////////////
const Element* PeriodicTable::getPointer(int atomicNumber) const
{
  return _vectorElements[atomicNumber - 1].get();
}