
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __OSG_TOOLS_MATERIAL_FACTORY_H__
#define __OSG_TOOLS_MATERIAL_FACTORY_H__

#include "OsgTools/Export.h"

#include "osg/Referenced"
#include "osg/ref_ptr"
#include "osg/Material"

#include <string>
#include <map>

namespace OsgTools {

class OSG_TOOLS_EXPORT MaterialFactory : public osg::Referenced
{
public:
  typedef osg::Referenced BaseClass;
	typedef osg::ref_ptr < osg::Material > Material;
	typedef std::map     < std::string, Material> Map;
  typedef osg::ref_ptr < MaterialFactory > Ptr;

	MaterialFactory();
	MaterialFactory(unsigned int seed);

  // Create a random material.  Cached based on string name.
  osg::Material* create ( const std::string&, float min = 0.0, float max = 1.0 );

  // Create a random material.
  osg::Material* create ( float min = 0.0, float max = 1.0 );

  void clear() { _materialMap.clear(); }
protected:
  virtual ~MaterialFactory();
private:
	Map _materialMap;
};

}

#endif // __OSG_TOOLS_MATERIAL_FACTORY_H__

