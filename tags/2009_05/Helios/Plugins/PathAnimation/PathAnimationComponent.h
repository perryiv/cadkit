
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The component class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _PATH_ANIMATION_COMPONENT_CLASS_H_
#define _PATH_ANIMATION_COMPONENT_CLASS_H_

#include "PathAnimation/CameraPath.h"
#include "PathAnimation/CurvePlayer.h"

#include "Usul/Base/Object.h"
#include "Usul/Interfaces/IActiveDocumentListener.h"
#include "Usul/Interfaces/IActiveViewListener.h"
#include "Usul/Interfaces/IAnimateNurbsCurve.h"
#include "Usul/Interfaces/IAnimatePath.h"
#include "Usul/Interfaces/IMenuAdd.h"
#include "Usul/Interfaces/IPlugin.h"
#include "Usul/Interfaces/ITimerNotify.h"
#include "Usul/Interfaces/IUpdateListener.h"

#include "MenuKit/Menu.h"

#include "osg/Group"
#include "osg/ref_ptr"

#include <list>
#include <vector>


class PathAnimationComponent : public Usul::Base::Object,
                               public Usul::Interfaces::IPlugin,
                               public Usul::Interfaces::IMenuAdd,
                               public Usul::Interfaces::IActiveDocumentListener,
                               public Usul::Interfaces::IActiveViewListener,
                               public Usul::Interfaces::IAnimatePath,
                               public Usul::Interfaces::IAnimateNurbsCurve,
                               public Usul::Interfaces::IUpdateListener,
                               public Usul::Interfaces::ITimerNotify
{
public:

  // Typedefs.
  typedef Usul::Base::Object BaseClass;
  typedef Usul::Interfaces::IUnknown IUnknown;
  typedef Usul::Interfaces::IActiveViewListener IActiveViewListener;
  typedef Usul::Interfaces::IAnimatePath IAnimatePath;
  typedef std::vector < CameraPath::RefPtr > Paths;
  typedef std::list < CurvePlayer::RefPtr > Players;
  typedef Usul::Interfaces::IAnimateNurbsCurve::DependentType DependentType;
  typedef Usul::Interfaces::IAnimateNurbsCurve::ControlPoints ControlPoints;
  typedef Usul::Interfaces::IAnimateNurbsCurve::KnotVector KnotVector;
  typedef Usul::Interfaces::IAnimateNurbsCurve::Parameters Parameters;
  typedef Usul::Interfaces::ITimerNotify::TimerID TimerID;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( PathAnimationComponent );

  // Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  // Default construction.
  PathAnimationComponent();

  // Usul::Interfaces::IActiveDocumentListener
  virtual void                  activeDocumentChanged ( IUnknown *oldDoc, IUnknown *newDoc );

  // Usul::Interfaces::IActiveViewListener
  virtual void                  activeViewChanged ( IUnknown *oldView, IUnknown *newView );

  // Usul::Interfaces::IAnimateNurbsCurve.
  virtual void                  animateNurbsCurve ( const ControlPoints &, const KnotVector &, unsigned int degree, const Parameters & );
  virtual void                  animateNurbsCurve ( const ControlPoints &, const KnotVector &, unsigned int degree, unsigned int steps );

  // Usul::Interfaces::IAnimatePath.
  virtual void                  animatePath ( const IAnimatePath::PackedMatrices &, IUnknown::RefPtr notify );
  virtual void                  animatePath ( const IAnimatePath::PackedMatrices &, unsigned int steps, IUnknown::RefPtr notify );
  virtual bool                  isPlaying() const;
  virtual void                  stopPlaying();

  // Usul::Interfaces::IPlugin
  virtual std::string           getPluginName() const { return "Path Animation"; }

  // Usul::Interfaces::IMenuAdd
  virtual void                  menuAdd ( MenuKit::Menu& menu, IUnknown *caller = 0x0 );

  // Called when the timer fires (ITimerNotify).
  virtual void                  timerNotify ( TimerID );
  
  // Usul::Interfaces::IUpdateListener
  virtual void                  updateNotify ( IUnknown *caller );

protected:

  // Use reference counting.
  virtual ~PathAnimationComponent();

  void                          _buildPathsMenu();
  void                          _buildCameraMenu();
  osg::Node *                   _buildCurve() const;

  bool                          _canClosePath() const;
  bool                          _canPlay() const;
  bool                          _canOpenPath ( IUnknown::RefPtr caller ) const;
  bool                          _canSavePath ( IUnknown::RefPtr caller ) const;
  void                          _closeCameraPath();
  void                          _currentCameraAppend();
  void                          _currentCameraInsert();
  void                          _currentCameraPrepend();
  void                          _currentCameraRemove();
  void                          _currentCameraReplace();

  void                          _exportMovie ( IUnknown::QueryPtr );

  void                          _goToNextCamera();
  void                          _goToPrevCamera();

  bool                          _hasCurrentPath() const;

  bool                          _isCurrentPath ( CameraPath::RefPtr ) const;
  bool                          _isCurrentPathModified() const;
  bool                          _isDegree ( unsigned int ) const;
  bool                          _isLooping() const;
  bool                          _isNumSteps ( unsigned int steps ) const;
  bool                          _isShowingPath() const;

  void                          _loadPath ( const std::string& name, IUnknown::QueryPtr caller );

  void                          _newPath();
  void                          _openPath ( IUnknown::QueryPtr );

  void                          _playBackward();
  void                          _playForward();
  void                          _playPathBackward ( const CameraPath *path, unsigned int steps, bool loop, IUnknown::RefPtr caller );
  void                          _playPathForward ( const CameraPath *path, unsigned int steps, bool loop, IUnknown::RefPtr caller );

  void                          _timerStart();
  void                          _timerStop();

  void                          _saveCurrentPath ( IUnknown::QueryPtr );
  void                          _saveAsCurrentPath ( IUnknown::QueryPtr );
  void                          _setCameraPosition ( unsigned int );
  void                          _setCurrentPath ( CameraPath::RefPtr );
  void                          _setDegree ( unsigned int );
  void                          _setLooping ( bool );
  void                          _setNumSteps ( unsigned int steps );
  void                          _setShowPath ( bool );

  void                          _updateScene();
  void                          _updatePath ( IUnknown *caller );

  void                          _writeMovieFile ( IUnknown::RefPtr caller );

private:

  // Do not copy.
  PathAnimationComponent ( const PathAnimationComponent & );
  PathAnimationComponent &operator = ( const PathAnimationComponent & );

  MenuKit::Menu::RefPtr _pathsMenu;
  MenuKit::Menu::RefPtr _cameraMenu;
  CameraPath::RefPtr _currentPath;
  Paths _paths;
  Players _players;
  unsigned int _degree;
  bool _writeMovie;
  std::string _movieFilename;
  IUnknown::QueryPtr _movieWriter;
  IUnknown::QueryPtr _caller;
  unsigned int _numSteps;
  osg::ref_ptr<osg::Group> _root;
  bool _showPath;
  bool _looping;
  bool _dirtyScene;
  unsigned int _currentCamera;
  bool _renderLoop;
  TimerID _timer;
  double _milliSeconds;
};


#endif // _PATH_ANIMATION_COMPONENT_CLASS_H_
