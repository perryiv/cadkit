
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////


#ifndef __OSG_PDB_PLUGIN_ELEMENT_H__
#define __OSG_PDB_PLUGIN_ELEMENT_H__

#include <string>
#include "osg/Referenced"

class Element : public osg::Referenced
{
public:
  Element();
  Element(int, std::string, float);
  Element(const Element&);
  const float getRadius() const { return _radius; }
  const std::string getElement() const { return _element; }
  const int getElementNumber() const { return _elementNumber; }
  Element& operator=(const Element&);
private:
  int _elementNumber;
  std::string _element;
  float _radius;
};


#endif