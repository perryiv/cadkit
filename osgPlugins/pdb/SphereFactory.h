
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Perry Miller
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _OPEN_SCENE_GRAPH_PROTEIN_DATA_BANK_PLUGIN_SPHERE_FACTORY_H_
#define _OPEN_SCENE_GRAPH_PROTEIN_DATA_BANK_PLUGIN_SPHERE_FACTORY_H_

#include "osg/Referenced"
#include "osg/Vec3"
#include "osg/ref_ptr"
#include "osg/Geometry"

#include <vector>
#include <map>

//namespace osg { class Geometry; };


class SphereFactory : public osg::Referenced
{
public:

  typedef osg::ref_ptr<SphereFactory> Ptr;
  typedef osg::ref_ptr<osg::Geometry> Sphere;
  typedef std::map<float, std::vector< osg::Geometry*> *> Map;
  typedef std::pair<float, std::vector< osg::Geometry*> *> Pair;
  typedef std::vector< osg::Vec3 > Tristrip;

  typedef std::vector < Sphere > Subdivided;
  typedef std::pair < unsigned int, unsigned int > MeshSize;
  typedef std::pair < float, MeshSize > Key;
  typedef std::map < Key, Sphere > LatLongMap;

  SphereFactory();

  // Create a sub-divided sphere.
  osg::Geometry*        create ( unsigned int numDivisions );
  osg::Geometry*        create ( unsigned int numDivisions, float radius );

  // Create a sphere meshed in the latitude-longitude way.
  osg::Geometry*        create ( float radius, unsigned int numLatitude, unsigned int numLongitude );

protected:

  virtual ~SphereFactory();

private:
  void _splitTristrip ( const Tristrip *old, Tristrip *newTristrip1, Tristrip *newTripstrip2);

  Map _map;
  Subdivided _subdivided;
  LatLongMap _latLongMap;
};


#endif // _OPEN_SCENE_GRAPH_PROTEIN_DATA_BANK_PLUGIN_SPHERE_FACTORY_H_

