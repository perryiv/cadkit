
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Registry functions.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _OPEN_SCENE_GRAPH_FOX_BINDING_REGISTRY_FUNCTIONS_H_
#define _OPEN_SCENE_GRAPH_FOX_BINDING_REGISTRY_FUNCTIONS_H_

#include "osg/Vec4d"
#include "osg/Vec3d"
#include "osg/Vec2d"

#include <string>


namespace Helios {
namespace Registry {


// Read the vector.
osg::Vec4f read ( const std::string &section, const std::string &key, const osg::Vec4f &vec );
osg::Vec4d read ( const std::string &section, const std::string &key, const osg::Vec4d &vec );
osg::Vec3f read ( const std::string &section, const std::string &key, const osg::Vec3f &vec );
osg::Vec3d read ( const std::string &section, const std::string &key, const osg::Vec3d &vec );
osg::Vec2f read ( const std::string &section, const std::string &key, const osg::Vec2f &vec );
osg::Vec2d read ( const std::string &section, const std::string &key, const osg::Vec2d &vec );


// Write the vector.
void write ( const std::string &section, const std::string &key, const osg::Vec2f &vec );
void write ( const std::string &section, const std::string &key, const osg::Vec3f &vec );
void write ( const std::string &section, const std::string &key, const osg::Vec4f &vec );


}; // namespace Registry
}; // namespace Helios


#endif // _OPEN_SCENE_GRAPH_FOX_BINDING_REGISTRY_FUNCTIONS_H_
