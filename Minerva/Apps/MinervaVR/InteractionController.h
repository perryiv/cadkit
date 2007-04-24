
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_VR_INTERACTION_CONTROLLER_H__
#define __MINERVA_VR_INTERACTION_CONTROLLER_H__

#include "VrjCore/OsgInteraction.h"

#include "Magrathea/Navigator.h"

class InteractionController : public VrjCore::OsgInteraction
{
 public:
  typedef VrjCore::OsgInteraction BaseClass;

  InteractionController();

  /// Set the planet.
  void planet ( ossimPlanet * planet );

  /// Set the home position
  void home ( double lat, double lon, double height );
  void homeLook ( double yaw, double pitch, double roll );
  
  void                                  goHome();

  /// Get/ Set the matrix
  virtual void                          matrix( const osg::Matrixd& );
  virtual osg::Matrixd                  matrix();

  virtual void                          joystickAxis1GlobalAction( double );
  virtual void                          joystickAxis2GlobalAction( double );
  virtual void                          joystickAxis3GlobalAction( double );
  virtual void                          joystickAxis4GlobalAction( double );
  virtual void                          joystickAxis5GlobalAction( double );
  virtual void                          joystickAxis6GlobalAction( double );
  
  virtual void                          updateDeviceData();
  
  virtual void                          updateTransforms();

  virtual void                          resetNavigation();
  virtual void                          resetState();

 protected:
  virtual InteractionController();

 private:
  osg::ref_ptr < Magrathea::Navigator > _navigator;

  osg::Vec3 _home;
  osg::Vec3 _homeLook;
};

#endif // __MINERVA_VR_INTERACTION_CONTROLLER_H__
