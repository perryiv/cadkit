
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Hud for the StarSystem.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __STAR_SYSTEM_HUD_H__
#define __STAR_SYSTEM_HUD_H__

#include "StarSystem/Export.h"

#include "osg/ref_ptr"
#include "osg/Camera"
#include "osg/Vec3d"
#include "osgText/Text"

namespace StarSystem {

  
class STAR_SYSTEM_EXPORT Hud
{
public:
  Hud();

  // Build the scene.
  osg::Node*    buildScene();
  
  // Update scene.
  void          updateScene( unsigned int width, unsigned int height );
  
  // Get/Set the number of requests.
  void          requests ( unsigned int );
  unsigned int  requests() const;
  
  // Get/Set the number of running tasks.
  void          running ( unsigned int );
  unsigned int  running() const;
  
  // Set the pointer position.
  void          position ( double lat, double lon, double height );
  
private:
  osg::ref_ptr < osg::Camera > _camera;
  osg::ref_ptr < osgText::Text > _feedback;
  osg::ref_ptr < osgText::Text > _position;
  osg::Vec3d _latLonHeight;
  unsigned int _requests;
  unsigned int _running;
};

  
}

#endif // __STAR_SYSTEM_HUD_H__
