
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
#include "SharedUpdateData.h"

#include "Minerva/Core/Viz/Controller.h"
#include "Minerva/Core/Scene/SceneManager.h"

#include "Usul/CommandLine/Options.h"

#include "Magrathea/Planet.h"


class MinervaVR : public VrjCore::OsgVJApp 
{
public:
  typedef VrjCore::OsgVJApp BaseClass;

  MinervaVR( vrj::Kernel* kern, int& argc, char** argv );
  virtual ~MinervaVR();		

 protected:
  virtual void appInit();
  
  virtual void preFrame();
  virtual void appSceneInit();
  virtual void appPreOsgDraw();
  virtual void addSceneLight();

  void _updateScene();

 private:	

  Minerva::Core::Viz::Controller::RefPtr       _dbManager;
  Minerva::Core::Scene::SceneManager::RefPtr   _sceneManager;
  osg::ref_ptr < Magrathea::Planet >           _planet;
  Usul::CommandLine::Options                   _options;
  osg::Vec4f                                   _background;
  cluster::UserData< SharedUpdateData >        _update;
  unsigned int                                 _numFramesBuild;
  unsigned int                                 _frameBuild;
};

#endif //: _MINERVA_VR_H_

