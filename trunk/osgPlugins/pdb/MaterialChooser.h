
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __OSG_PDB_PLUGIN_COLORCHOOSER_H__
#define __OSG_PDB_PLUGIN_COLORCHOOSER_H__

#include "osg/ref_ptr"
#include "osg/Material"

#include <string>
#include <map>

class MaterialChooser 
{
public:
	typedef osg::ref_ptr<osg::Material> Material;
	typedef std::pair< std::string, osg::ref_ptr<osg::Material> > pair;
	typedef std::map< std::string, Material> Map;

	MaterialChooser();
	MaterialChooser(unsigned int seed);
  ~MaterialChooser();
	osg::ref_ptr<osg::Material> getMaterial (const std::string) const;
  void clear() { _materialMap.clear(); }
	//bool addColor(std::string, Color) { return false; }
private:
	Material addMaterial(std::string) const;
	mutable Map _materialMap;

};


#endif
