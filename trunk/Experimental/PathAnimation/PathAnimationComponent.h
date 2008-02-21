
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
#include "Usul/Interfaces/IActiveViewListener.h"
#include "Usul/Interfaces/IAnimatePath.h"
#include "Usul/Interfaces/IMenuAdd.h"
#include "Usul/Interfaces/IPlugin.h"
#include "Usul/Interfaces/IUpdateListener.h"

#include "MenuKit/Menu.h"

#include "osg/Group"
#include "osg/ref_ptr"

#include <vector>


class PathAnimationComponent : public Usul::Base::Object,
                               public Usul::Interfaces::IPlugin,
                               public Usul::Interfaces::IMenuAdd,
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

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( PathAnimationComponent );

  // Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  // Default construction.
  PathAnimationComponent();

  // Usul::Interfaces::IActiveViewListener
  virtual void                  activeViewChanged ( IUnknown *oldView, IUnknown *newView );

  // Usul::Interfaces::IAnimatePath.
  virtual void                  animatePath ( const IAnimatePath::PackedMatrices & );
  virtual void                  animatePath ( const IAnimatePath::PackedMatrices &, double step );

  // Usul::Interfaces::IPlugin
  virtual std::string           getPluginName() const { return "Path Animation"; }

  // Usul::Interfaces::IMenuAdd
  virtual void                  menuAdd ( MenuKit::Menu& menu, Usul::Interfaces::IUnknown * caller = 0x0 );

  // Usul::Interfaces::IUpdateListener
  virtual void                  updateNotify ( IUnknown *caller );

protected:

  // Use reference counting.
  virtual ~PathAnimationComponent();

  void                          _buildPathsMenu();
  void                          _buildCameraMenu();

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

  bool                          _hasCurrentPath() const;

  bool                          _isCurrentPath ( CameraPath::RefPtr ) const;
  bool                          _isCurrentPathModified() const;
  bool                          _isDegree ( unsigned int ) const;
  bool                          _isPlaying() const;
  bool                          _isPaused() const;
  bool                          _isStepSize ( double step ) const;
  bool                          _isShowPath() const;

  void                          _newPath();
  void                          _openPath ( Usul::Interfaces::IUnknown::QueryPtr );

  void                          _pause ( bool );
  void                          _playBackward();
  void                          _playForward();
  void                          _playPathBackward ( const CameraPath *path, double step );
  void                          _playPathForward ( const CameraPath *path, double step );

  void                          _saveCurrentPath ( Usul::Interfaces::IUnknown::QueryPtr );
  void                          _saveAsCurrentPath ( Usul::Interfaces::IUnknown::QueryPtr );
  void                          _setCameraPosition ( unsigned int );
  void                          _setCurrentPath ( CameraPath::RefPtr );
  void                          _setDegree ( unsigned int );
  void                          _setShowPath ( bool );
  void                          _setStepSize ( double step );
  void                          _stopPlaying();

  void                          _writeMovieFile( Usul::Interfaces::IUnknown *caller );

private:

  // Do not copy.
  PathAnimationComponent ( const PathAnimationComponent & );
  PathAnimationComponent &operator = ( const PathAnimationComponent & );

  MenuKit::Menu::RefPtr _pathsMenu;
  MenuKit::Menu::RefPtr _cameraMenu;
  CameraPath::RefPtr _currentPath;
  Paths _paths;
  CurvePlayer::RefPtr _player;
  bool _paused;
  unsigned int _degree;
  bool _writeMovie;
  std::string _movieFilename;
  Usul::Interfaces::IUnknown::QueryPtr _movieWriter;
  Usul::Interfaces::IUnknown::QueryPtr _caller;
  double _stepSize;
  osg::ref_ptr<osg::Group> _root;
  bool _showPath;
};


#endif // _PATH_ANIMATION_COMPONENT_CLASS_H_
