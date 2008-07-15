
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
#include "Usul/Interfaces/IAnimatePath.h"
#include "Usul/Interfaces/IMenuAdd.h"
#include "Usul/Interfaces/IPlugin.h"
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
                               public Usul::Interfaces::IUpdateListener
{
public:

  // Typedefs.
  typedef Usul::Base::Object BaseClass;
  typedef Usul::Interfaces::IUnknown IUnknown;
  typedef Usul::Interfaces::IActiveViewListener IActiveViewListener;
  typedef Usul::Interfaces::IAnimatePath IAnimatePath;
  typedef std::vector < CameraPath::RefPtr > Paths;
  typedef std::list < CurvePlayer::RefPtr > Players;

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

  // Usul::Interfaces::IAnimatePath.
  virtual void                  animatePath ( const IAnimatePath::PackedMatrices & );
  virtual void                  animatePath ( const IAnimatePath::PackedMatrices &, unsigned int steps );
  virtual bool                  isPlaying() const;
  virtual void                  stopPlaying();

  // Usul::Interfaces::IPlugin
  virtual std::string           getPluginName() const { return "Path Animation"; }

  // Usul::Interfaces::IMenuAdd
  virtual void                  menuAdd ( MenuKit::Menu& menu, Usul::Interfaces::IUnknown * caller = 0x0 );

  // Usul::Interfaces::IUpdateListener
  virtual void                  updateNotify ( IUnknown *caller );

protected:

  // Use reference counting.
  virtual ~PathAnimationComponent();

  void                          _activateRenderLoop();

  void                          _buildPathsMenu();
  void                          _buildCameraMenu();
  osg::Node *                   _buildCurve() const;

  bool                          _canClosePath() const;
  bool                          _canPlay() const;
  bool                          _canOpenPath ( Usul::Interfaces::IUnknown * caller ) const;
  bool                          _canSavePath ( Usul::Interfaces::IUnknown * caller ) const;
  void                          _closeCameraPath();
  void                          _currentCameraAppend();
  void                          _currentCameraInsert();
  void                          _currentCameraPrepend();
  void                          _currentCameraRemove();
  void                          _currentCameraReplace();

  void                          _exportMovie ( Usul::Interfaces::IUnknown::QueryPtr );

  void                          _goToNextCamera();
  void                          _goToPrevCamera();

  bool                          _hasCurrentPath() const;

  bool                          _isCurrentPath ( CameraPath::RefPtr ) const;
  bool                          _isCurrentPathModified() const;
  bool                          _isDegree ( unsigned int ) const;
  bool                          _isLooping() const;
  bool                          _isNumSteps ( unsigned int steps ) const;
  bool                          _isShowingPath() const;

  void                          _loadPath ( const std::string& name, Usul::Interfaces::IUnknown::QueryPtr caller );

  void                          _newPath();
  void                          _openPath ( Usul::Interfaces::IUnknown::QueryPtr );

  void                          _playBackward();
  void                          _playForward();
  void                          _playPathBackward ( const CameraPath *path, unsigned int steps, bool loop );
  void                          _playPathForward ( const CameraPath *path, unsigned int steps, bool loop );

  void                          _saveCurrentPath ( Usul::Interfaces::IUnknown::QueryPtr );
  void                          _saveAsCurrentPath ( Usul::Interfaces::IUnknown::QueryPtr );
  void                          _setCameraPosition ( unsigned int );
  void                          _setCurrentPath ( CameraPath::RefPtr );
  void                          _setDegree ( unsigned int );
  void                          _setLooping ( bool );
  void                          _setNumSteps ( unsigned int steps );
  void                          _setShowPath ( bool );

  void                          _updateScene();

  void                          _writeMovieFile ( Usul::Interfaces::IUnknown *caller );

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
  Usul::Interfaces::IUnknown::QueryPtr _movieWriter;
  Usul::Interfaces::IUnknown::QueryPtr _caller;
  unsigned int _numSteps;
  osg::ref_ptr<osg::Group> _root;
  bool _showPath;
  bool _looping;
  bool _dirtyScene;
  unsigned int _currentCamera;
  bool _renderLoop;
};


#endif // _PATH_ANIMATION_COMPONENT_CLASS_H_
