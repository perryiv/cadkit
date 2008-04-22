
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
#include "osg/ref_ptr"

class Element : public osg::Referenced
{
public:
  typedef osg::ref_ptr< Element > Ptr;

  Element(int, std::string, float);
  Element(const Element&);
  const float getRadius() const { return _radius; }
  const std::string& getElementSymbol() const { return _elementSymbol; }
  const std::string& getElementName() const { return _elementName; }
  const int getAtomicNumber() const { return _atomicNumber; }
  Element& operator=(const Element&);
protected:
  Element();
  //Use reference counting
  virtual ~Element();
private:
  int _atomicNumber;
  std::string _elementSymbol, _elementName;
  float _radius;
};


#endif
