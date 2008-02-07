
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
#include "StarSystem/Compass.h"

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
  
  // Show the compass be shown?
  void          showCompass ( bool b );
  bool          showCompass() const;
  
  // Set heading, pitch, roll.
  void          hpr ( double heading, double pitch, double roll );
  
private:
  osg::ref_ptr < osg::Camera > _camera;
  osg::ref_ptr < osgText::Text > _feedback;
  osg::ref_ptr < osgText::Text > _position;
  osg::Vec3d _latLonHeight;
  unsigned int _requests;
  unsigned int _running;
  osg::ref_ptr < Compass > _compass;
  bool _showCompass;
};

  
}

#endif // __STAR_SYSTEM_HUD_H__
