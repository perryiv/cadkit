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
  //Test molecule with r = 1
  osg::ref_ptr< Element > AD (new Element(1, "AD", 1.0f));
  _vectorElements.push_back(AD);
  _mapElements.insert( Element_pair("AD", AD));
  //Helium
  //Element data from http://www.ktf-split.hr/periodni/en/
  //Hydrogen
  osg::ref_ptr< Element > H (new Element(1, "H", 0.373f));
  _vectorElements.push_back(H);
  _mapElements.insert( Element_pair("H", H));
  //Helium
  osg::ref_ptr< Element > He (new Element(2, "He", 1.28f));
  _vectorElements.push_back(He);
  _mapElements.insert( Element_pair("HE", He));
  //Lithium
  osg::ref_ptr< Element > Li (new Element(3, "Li", 1.52f));
  _vectorElements.push_back(Li);
  _mapElements.insert( Element_pair("LI", Li));
  //Beryllium
  osg::ref_ptr< Element > Be (new Element(4, "Be", 1.11f));
  _vectorElements.push_back(Be);
  _mapElements.insert( Element_pair("BE", Be));
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
  _mapElements.insert( Element_pair("NE", Ne));
   //Sodium
  osg::ref_ptr< Element > Na (new Element(11, "Ne", 1.858f));
  _vectorElements.push_back(Na);
  _mapElements.insert( Element_pair("NA", Na));
  //Magnesium
  osg::ref_ptr< Element > Mg (new Element(12, "Mg", 1.599f));
  _vectorElements.push_back(Mg);
  _mapElements.insert( Element_pair("MG", Mg));
  //Aluminium
  osg::ref_ptr< Element > Al (new Element(13, "Al", 1.432f));
  _vectorElements.push_back(Al);
  _mapElements.insert( Element_pair("AL", Al));
  //Silicon
  osg::ref_ptr< Element > Si (new Element(14, "Si", 1.176f));
  _vectorElements.push_back(Si);
  _mapElements.insert( Element_pair("SI", Si));
  //Phosphorus
  osg::ref_ptr< Element > P (new Element(15, "P", 1.105f));
  _vectorElements.push_back(P);
  _mapElements.insert( Element_pair("P", P));
  //Sulfur
  osg::ref_ptr< Element > S (new Element(16, "S", 1.035f));
  _vectorElements.push_back(S);
  _mapElements.insert( Element_pair("S", S));
  //Chlorine
  osg::ref_ptr< Element > Cl (new Element(17, "Cl", 0.994f));
  _vectorElements.push_back(Cl);
  _mapElements.insert( Element_pair("CL", Cl));
  //Argon
  osg::ref_ptr< Element > Ar (new Element(18, "Ar", 1.88f));
  _vectorElements.push_back(Ar);
  _mapElements.insert( Element_pair("AR", Ar));
  //Potassium
  osg::ref_ptr< Element > K (new Element(19, "K", 2.272f));
  _vectorElements.push_back(K);
  _mapElements.insert( Element_pair("K", K));
  //Calcium
  osg::ref_ptr< Element > Ca (new Element(20, "Ca", 1.974f));
  _vectorElements.push_back(Ca);
  _mapElements.insert( Element_pair("CA", Ca));
  //Scandium
  osg::ref_ptr< Element > Sc (new Element(21, "Sc", 1.606));
  _vectorElements.push_back(Sc);
  _mapElements.insert( Element_pair("SC", Sc));
  //Titanium
  osg::ref_ptr< Element > Ti (new Element(22, "Ti", 1.448f));
  _vectorElements.push_back(Ti);
  _mapElements.insert( Element_pair("TI", Ti));
  //Vanadium
  osg::ref_ptr< Element > V (new Element(23, "V", 1.311f));
  _vectorElements.push_back(V);
  _mapElements.insert( Element_pair("V", V));
  //Chromium
  osg::ref_ptr< Element > Cr (new Element(24, "Cr", 1.249f));
  _vectorElements.push_back(Cr);
  _mapElements.insert( Element_pair("CR", Cr));
  //Manganese
  osg::ref_ptr< Element > Mn (new Element(25, "Mn", 1.367f));
  _vectorElements.push_back(Mn);
  _mapElements.insert( Element_pair("MN", Mn));
  //Iron
  osg::ref_ptr< Element > Fe (new Element(26, "Fe", 1.241f));
  _vectorElements.push_back(Fe);
  _mapElements.insert( Element_pair("FE", Fe));
  //Cobalt
  osg::ref_ptr< Element > Co (new Element(27, "Co", 1.253f));
  _vectorElements.push_back(Co);
  _mapElements.insert( Element_pair("CO", Co));
  //Nickel
  osg::ref_ptr< Element > Ni (new Element(28, "Ni", 1.246f));
  _vectorElements.push_back(Ni);
  _mapElements.insert( Element_pair("NI", Ni));
  //Copper
  osg::ref_ptr< Element > Cu (new Element(29, "Cu", 1.278f));
  _vectorElements.push_back(Cu);
  _mapElements.insert( Element_pair("CU", Cu));
  //Zinc
  osg::ref_ptr< Element > Zn (new Element(30, "Zn", 1.335f));
  _vectorElements.push_back(Zn);
  _mapElements.insert( Element_pair("ZN", Zn));
  //Gallium
  osg::ref_ptr< Element > Ga (new Element(31, "Ga", 1.221));
  _vectorElements.push_back(Ga);
  _mapElements.insert( Element_pair("GA", Ga));
  //Germanium
  osg::ref_ptr< Element > Ge (new Element(32, "Ge", 1.225f));
  _vectorElements.push_back(Ge);
  _mapElements.insert( Element_pair("GE", Ge));
  //Arsenic
  osg::ref_ptr< Element > As (new Element(33, "As", 1.245f));
  _vectorElements.push_back(As);
  _mapElements.insert( Element_pair("AS", As));
  //Selenium
  osg::ref_ptr< Element > Se (new Element(34, "Se", 1.16f));
  _vectorElements.push_back(Se);
  _mapElements.insert( Element_pair("SE", Se));
  //Bromine
  osg::ref_ptr< Element > Br (new Element(35, "Br", 1.145f));
  _vectorElements.push_back(Br);
  _mapElements.insert( Element_pair("BR", Br));
  //Krypton
  osg::ref_ptr< Element > Kr (new Element(36, "Kr", 2.02f));
  _vectorElements.push_back(Kr);
  _mapElements.insert( Element_pair("KR", Kr));
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