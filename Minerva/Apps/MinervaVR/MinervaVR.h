
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_VR_H__
#define __MINERVA_VR_H__


#include "VrjCore/OsgVJApp.h"

#include "Minerva/Core/Viz/Controller.h"
#include "Minerva/Scene/SceneManager.h"


#include "Magrathea/Planet.h"


class MinervaVR : public VrjCore::OsgVJApp 
{
 public:

  MinervaVR( vrj::Kernel* kern, int& argc, char** argv );
  virtual ~MinervaVR();		

 protected:
  virtual void appInit();
  
  virtual void appSceneInit();
  virtual void appPreOsgDraw();
  virtual void addSceneLight();

  void _updateScene();

 private:	

  Minerva::Core::Viz::Controller::RefPtr _dbManager;
  Minerva::Scene::SceneManager::RefPtr   _sceneManager;
  osg::ref_ptr < Magrathea::Planet >     _planet;
};

#endif //: _MINERVA_VR_H_

