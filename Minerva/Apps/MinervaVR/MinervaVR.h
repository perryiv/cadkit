
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

#include "Minerva/Document/MinervaDocument.h"
#include "Minerva/Interfaces/IAnimationControl.h"

#include "Usul/Threads/RecursiveMutex.h"
#include "Usul/Threads/Guard.h"
#include "Usul/CommandLine/Options.h"
#include "Usul/Adaptors/Random.h"
#include "Usul/Interfaces/ICommand.h"
#include "Usul/Interfaces/ICommandQueueAdd.h"

#include <list>

class MinervaVR : public VrjCore::OsgVJApp,
                  public Minerva::Interfaces::IAnimationControl,
                  public Usul::Interfaces::ICommandQueueAdd
{
public:
  // Typedefs.
  typedef VrjCore::OsgVJApp                  BaseClass;
  typedef std::list<std::string>             Args;
  typedef Usul::Threads::RecursiveMutex      Mutex;
  typedef Usul::Threads::Guard<Mutex>        Guard;
  typedef Usul::Interfaces::ICommand         Command;
  typedef Command::QueryPtr                  CommandPtr;
  typedef std::list < CommandPtr >           CommandQueue;

  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Construction/Destruction.
  MinervaVR( const Args& args );
  virtual ~MinervaVR();

  Mutex&                       mutex() const { return _mutex; }

  // Run the program.
  void                         run();

  // Print the usage string.
  static void                  usage ( const std::string &exe, std::ostream &out );

 protected:
  virtual void                 appInit();
  
  virtual void                 preFrame();
  virtual void                 appSceneInit();
  virtual void                 appPreOsgDraw();
  virtual void                 draw();
  virtual void                 postFrame();
  virtual void                 addSceneLight();

  void                         _initLegend();

  void                         _processCommands ();

  /// Minerva::Interfaces::IAnimationControl
  /// Start the animation.
  virtual void                 startAnimation ();

  /// Stop the animation.
  virtual void                 stopAnimation ();

  /// Pause the animation.
  virtual void                 pauseAnimation ();

  /// Get/Set the animate speed.
  virtual void                 animateSpeed ( double speed );
  virtual double               animateSpeed () const;

  /// Usul::Interfaces::ICommandQueueAdd
  virtual void                 addCommand ( Usul::Interfaces::ICommand* command );

private:	

  Minerva::Document::MinervaDocument::RefPtr   _document;
  Usul::CommandLine::Options                   _options;
  osg::Vec4f                                   _background;
  mutable Mutex                                _mutex;
  CommandQueue                                 _commandQueue;
};

#endif // _MINERVA_VR_H_

