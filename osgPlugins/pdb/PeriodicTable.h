
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

class Element;


class PeriodicTable 
{
public:
  typedef std::pair< std::string, osg::ref_ptr< Element > > Element_pair;
  typedef std::vector< osg::ref_ptr<Element> > VectorElements;
  typedef std::map< std::string, osg::ref_ptr<Element> > MapElements;
  PeriodicTable();
  const Element * getPointer(const std::string&) const;
  const Element * getPointer(int) const;
private:
  VectorElements _vectorElements;
  MapElements _mapElements;
};

#endif
