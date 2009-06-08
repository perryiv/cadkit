
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////


#ifndef __OSG_PDB_PLUGIN_PERIODIC_TABLE_H__
#define __OSG_PDB_PLUGIN_PERIODIC_TABLE_H__

#include <map>
#include <vector>
#include <string>

#include "osg/ref_ptr"

#include "Element.h"


class PeriodicTable 
{
public:
  typedef std::pair< std::string, Element::Ptr > Element_pair;
  typedef std::vector< Element::Ptr > VectorElements;
  typedef std::map< std::string, Element::Ptr > MapElements;
  PeriodicTable();
  Element::Ptr getPointer(const std::string&) const;
  Element::Ptr getPointer(int) const;
private:
  VectorElements _vectorElements;
  MapElements _mapElements;
};

#endif
