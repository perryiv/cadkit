
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
//  Hud for Minerva.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_CORE_HUD_H__
#define __MINERVA_CORE_HUD_H__

#include "Minerva/Core/Export.h"
#include "Minerva/Core/Utilities/Compass.h"

#include "osg/ref_ptr"
#include "osg/Camera"
#include "osg/Vec3d"
#include "osgText/Text"

#include <string>
#include <vector>


namespace Minerva {
namespace Core {
namespace Utilities {

  
class MINERVA_EXPORT Hud
{
public:

  typedef std::vector < std::string > Strings;

  Hud();

  // Build the scene.
  osg::Node*    buildScene();

  // Set the date text.
  void          dateFeedback ( const std::string& text );

  // Set the eye altitude.
  void          eyeAltitude ( double altitude );
  
  // Set heading, pitch, roll.
  void          hpr ( double heading, double pitch, double roll );

  // Set the meta-data string.
  void          metaString ( const std::string & );

  // Set the pointer position.
  void          position ( double lat, double lon, double height );
  
  // Get/Set the number of requests.
  void          requests ( unsigned int );
  unsigned int  requests() const;
  
  // Get/Set the number of running tasks.
  void          running ( const Strings & );
  Strings       running() const;
    
  // Show the compass be shown?
  void          showCompass ( bool b );
  bool          showCompass() const;
  
  // Show the pointer position.
  void          showPointerPosition ( bool b );
  bool          showPointerPosition() const;
  
  // Show the job feedback.
  void          showJobFeedback ( bool b );
  bool          showJobFeedback() const;

  // Show the date feedback.
  void          showDateFeedback ( bool b );
  bool          showDateFeedback() const;

  // Show the eye altitude.
  void          showEyeAltitude ( bool b );
  bool          showEyeAltitude() const;

  // Show the meta-string.
  void          showMetaString ( bool b );
  bool          showMetaString() const;
  
  // Update scene.
  void          updateScene ( unsigned int width, unsigned int height );
  
private:
  
  enum Flags
  {
    _SHOW_COMPASS          = 0x00000001,
    _SHOW_POINTER_POSITION = 0x00000002,
    _SHOW_JOB_FEEDBACK     = 0x00000004,
    _SHOW_DATE_FEEDBACK    = 0x00000008,
    _SHOW_EYE_ALTITUDE     = 0x00000010,
    _SHOW_META_STRING      = 0x00000020,
    _ALL                   = _SHOW_COMPASS | 
                             _SHOW_POINTER_POSITION | 
                             _SHOW_JOB_FEEDBACK | 
                             _SHOW_DATE_FEEDBACK |
                             _SHOW_EYE_ALTITUDE |
                             _SHOW_META_STRING
  };
  
  osg::ref_ptr < osg::Camera > _camera;
  osg::ref_ptr < osgText::Text > _feedback;
  osg::ref_ptr < osgText::Text > _position;
  osg::ref_ptr < osgText::Text > _date;
  osg::ref_ptr < osgText::Text > _eyeAltitudeText;
  osg::Vec3d _latLonHeight;
  unsigned int _requests;
  Strings _running;
  osg::ref_ptr < Compass > _compass;
  unsigned int _flags;
  double _eyeAltitude;
  std::string _metaString;
};

  
} // namespace Utilities
} // namespace Core
} // namespace Minerva

#endif // __MINERVA_CORE_HUD_H__
