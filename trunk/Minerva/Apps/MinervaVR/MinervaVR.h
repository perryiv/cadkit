
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
#include "Minerva/Interfaces/IAnimationControl.h"

#include "Usul/Threads/RecursiveMutex.h"
#include "Usul/Threads/Guard.h"
#include "Usul/Threads/Thread.h"
#include "Usul/CommandLine/Options.h"
#include "Usul/Adaptors/Random.h"
#include "Usul/Interfaces/ICommand.h"
#include "Usul/Interfaces/ICommandQueueAdd.h"

#include "Magrathea/Planet.h"

#include <list>

class MinervaVR : public VrjCore::OsgVJApp,
                  public Minerva::Interfaces::IAnimationControl,
                  public Usul::Interfaces::ICommandQueueAdd
{
public:
  // Typedefs.
  typedef VrjCore::OsgVJApp                  BaseClass;
  typedef Usul::Threads::RecursiveMutex      Mutex;
  typedef Usul::Threads::Guard<Mutex>        Guard;
  typedef Usul::Threads::Thread              Thread;
  typedef Usul::Interfaces::ICommand         Command;
  typedef Command::QueryPtr                  CommandPtr;
  typedef std::list < CommandPtr >           CommandQueue;

  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Construction/Destruction.
  MinervaVR( vrj::Kernel* kern, int& argc, char** argv );
  virtual ~MinervaVR();		

  Mutex&       mutex() const { return _mutex; }

 protected:
  virtual void appInit();
  
  virtual void preFrame();
  virtual void appSceneInit();
  virtual void appPreOsgDraw();
  virtual void draw();
  virtual void postFrame();
  virtual void addSceneLight();

  void         _initLegend();

  void         _updateScene( Usul::Threads::Thread *thread = 0x0 );
  void         _buildScene();

  void         _launchUpdateThread();

  void         _processCommands ();

  /// Minerva::Interfaces::IAnimationControl
  /// Start the animation.
  virtual void                 startAnimation ();

  /// Stop the animation.
  virtual void                 stopAnimation ();

  /// Get/Set the animate speed.
  virtual void                 animateSpeed ( double speed );
  virtual double               animateSpeed () const;

  /// Usul::Interfaces::ICommandQueueAdd
  virtual void                 addCommand ( Usul::Interfaces::ICommand* command );

 private:	

  Minerva::Core::Viz::Controller::RefPtr       _dbManager;
  Minerva::Core::Scene::SceneManager::RefPtr   _sceneManager;
  osg::ref_ptr < Magrathea::Planet >           _planet;
  Usul::CommandLine::Options                   _options;
  osg::Vec4f                                   _background;
  cluster::UserData< SharedUpdateData >        _update;
  Thread::RefPtr                               _updateThread;
  mutable Mutex                                _mutex;
  Usul::Adaptors::Random < double >            _rand;
  CommandQueue                                 _commandQueue;
};

#endif // _MINERVA_VR_H_

