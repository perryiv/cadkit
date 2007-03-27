
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
  _vectorElements( 105 ),
  _mapElements()
{
  //Element data from http://www.ktf-split.hr/periodni/en/
  //Hydrogen
  Element::Ptr H (new Element(1, "H", 0.373f));
  _vectorElements.at(1) = H;
  _mapElements["H"] = H;
  //Helium
  Element::Ptr He (new Element(2, "He", 1.28f));
  _vectorElements.at(2) = He;
  _mapElements["HE"] = He;
  //Lithium
  Element::Ptr Li (new Element(3, "Li", 1.52f));
  _vectorElements.at(3) = Li;
  _mapElements["LI"] = Li;
  //Beryllium
  Element::Ptr Be (new Element(4, "Be", 1.11f));
  _vectorElements.at(4) = Be;
  _mapElements["BE"] = Be;
  //Boron
  Element::Ptr B (new Element(5, "B", 0.79f));
  _vectorElements.at(5) = B;
  _mapElements["B"] = B;
  //Carbon
  Element::Ptr C (new Element(6, "C", .77f));
  _vectorElements.at(6) = C;
  _mapElements["C"] = C;
  //Nitrogen
  Element::Ptr N (new Element(7, "N", .54f));
  _vectorElements.at(7) = N;
  _mapElements["N"] = N;
  //Oxygen
  Element::Ptr O (new Element(8, "O", 0.604f));
  _vectorElements.at(8) = O;
  _mapElements["O"]= O;
  //Fluorine
  Element::Ptr F (new Element(9, "F", 0.709f));
  _vectorElements.at(9) = F;
  _mapElements["F"] = F;
  //Neon
  Element::Ptr Ne (new Element(10, "Ne", 1.54f));
  _vectorElements.at(10) = Ne;
  _mapElements["NE"] = Ne;
   //Sodium
  Element::Ptr Na (new Element(11, "Ne", 1.858f));
  _vectorElements.at(11) = Na;
  _mapElements["NA"] = Na;
  //Magnesium
  Element::Ptr Mg (new Element(12, "Mg", 1.599f));
  _vectorElements.at(12) = Mg;
  _mapElements["MG"] = Mg;
  //Aluminium
  Element::Ptr Al (new Element(13, "Al", 1.432f));
  _vectorElements.at(13) = Al;
  _mapElements["AL"] = Al;
  //Silicon
  Element::Ptr Si (new Element(14, "Si", 1.176f));
  _vectorElements.at(14) = Si;
  _mapElements["SI"] = Si;
  //Phosphorus
  Element::Ptr P (new Element(15, "P", 1.105f));
  _vectorElements.at(15) = P;
  _mapElements["P"] = P;
  //Sulfur
  Element::Ptr S (new Element(16, "S", 1.035f));
  _vectorElements.at(16) = S;
  _mapElements["S"] = S;
  //Chlorine
  Element::Ptr Cl (new Element(17, "Cl", 0.994f));
  _vectorElements.at(17) = Cl;
  _mapElements["CL"] = Cl;
  //Argon
  Element::Ptr Ar (new Element(18, "Ar", 1.88f));
  _vectorElements.at(18) = Ar;
  _mapElements["AR"] = Ar;
  //Potassium
  Element::Ptr K (new Element(19, "K", 2.272f));
  _vectorElements.at(19) = K;
  _mapElements["K"] = K;
  //Calcium
  Element::Ptr Ca (new Element(20, "Ca", 1.974f));
  _vectorElements.at(20) = Ca;
  _mapElements["CA"] = Ca;
  //Scandium
  Element::Ptr Sc (new Element(21, "Sc", 1.606f));
  _vectorElements.at(21) = Sc;
  _mapElements["SC"] = Sc;
  //Titanium
  Element::Ptr Ti (new Element(22, "Ti", 1.448f));
  _vectorElements.at(22) = Ti;
  _mapElements["TI"] = Ti;
  //Vanadium
  Element::Ptr V (new Element(23, "V", 1.311f));
  _vectorElements.at(23) = V;
  _mapElements["V"] = V;
  //Chromium
  Element::Ptr Cr (new Element(24, "Cr", 1.249f));
  _vectorElements.at(24) = Cr;
  _mapElements["CR"] = Cr;
  //Manganese
  Element::Ptr Mn (new Element(25, "Mn", 1.367f));
  _vectorElements.at(25) = Mn;
  _mapElements["MN"] = Mn;
  //Iron
  Element::Ptr Fe (new Element(26, "Fe", 1.241f));
  _vectorElements.at(26) = Fe; 
  _mapElements["FE"] = Fe;
  //Cobalt
  Element::Ptr Co (new Element(27, "Co", 1.253f));
  _vectorElements.at(27) = Co;
  _mapElements["CO"] = Co;
  //Nickel
  Element::Ptr Ni (new Element(28, "Ni", 1.246f));
  _vectorElements.at(28) = Ni;
  _mapElements["NI"] = Ni;
  //Copper
  Element::Ptr Cu (new Element(29, "Cu", 1.278f));
  _vectorElements.at(29) = Cu;
  _mapElements["CU"] = Cu;
  //Zinc
  Element::Ptr Zn (new Element(30, "Zn", 1.335f));
  _vectorElements.at(30) = Zn;
  _mapElements["ZN"] = Zn;
  //Galliumn
  Element::Ptr Ga (new Element(31, "Ga", 1.221f));
  _vectorElements.at(31) = Ga;
  _mapElements["GA"] = Ga;
  //Germanium
  Element::Ptr Ge (new Element(32, "Ge", 1.225f));
  _vectorElements.at(32) = Ge;
  _mapElements["GE"] = Ge;
  //Arsenic
  Element::Ptr As (new Element(33, "As", 1.245f));
  _vectorElements.at(33) = As;
  _mapElements["AS"] = As;
  //Selenium
  Element::Ptr Se (new Element(34, "Se", 1.16f));
  _vectorElements.at(34) = Se;
  _mapElements["SE"] = Se;
  //Bromine
  Element::Ptr Br (new Element(35, "Br", 1.145f));
  _vectorElements.at(35) = Br;
  _mapElements["BR"] = Br;
  //Krypton
  Element::Ptr Kr (new Element(36, "Kr", 2.02f));
  _vectorElements.at(36) = Kr;
  _mapElements["KR"] = Kr;
}

///////////////////////////////////////////////////////////////////////////////
//
//  Get a pointer to the element for the given element symbol.
//
///////////////////////////////////////////////////////////////////////////////
Element::Ptr PeriodicTable::getPointer(const std::string& element) const 
{
  MapElements::const_iterator i = _mapElements.find(element);
  if(_mapElements.end() != i)
    return i->second;
  return NULL;
}

///////////////////////////////////////////////////////////////////////////////
//
//  Get a pointer to the element for the given atomic number.
//  TODO add checks to make sure given number is valid.
//
///////////////////////////////////////////////////////////////////////////////
Element::Ptr PeriodicTable::getPointer(int atomicNumber) const
{
  return _vectorElements[atomicNumber];
}
