///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "PeriodicTable.h"

PeriodicTable::PeriodicTable() :
  _vectorElements(),
  _mapElements()
{
}

osg::ref_ptr< Element > PeriodicTable::getPointer(std::string element)
{
  MapElements::const_iterator i = _mapElements.find(element);
  if(_mapElements.end() != i)
    return i->second;
  return NULL;
}

osg::ref_ptr< Element > PeriodicTable::getPointer(int elementNumber)
{
  return _vectorElements[elementNumber];
}