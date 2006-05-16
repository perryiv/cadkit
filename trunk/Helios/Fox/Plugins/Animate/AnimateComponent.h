
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The component class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _ANIMATE_COMPONENT_CLASS_H_
#define _ANIMATE_COMPONENT_CLASS_H_

#include "Animate/CompileGuard.h"
#include "Animate/Movie.h"

#include "FoxTools/Headers/Object.h"
#include "FoxTools/Menu/Group.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Interfaces/IUnknown.h"
#include "Usul/Interfaces/Fox/IFoxMenuPane.h"
#include "Usul/Interfaces/Fox/IFoxDockBar.h"
#include "Usul/Interfaces/INotifyClose.h"
#include "Usul/Interfaces/IPlugin.h"
#include "Usul/Interfaces/IAddFrame.h"
#include "Usul/Interfaces/IAnimate.h"
#include "Usul/Interfaces/IDocumentCreate.h"
#include "Usul/Interfaces/GUI/IGUIDelegate.h"
#include "Usul/Interfaces/INewDocumentCreate.h"

#include <map>

namespace Animate {


class AnimateComponent : public Usul::Base::Referenced,
                  public Usul::Interfaces::IFoxMenuPane,
                  public Usul::Interfaces::INotifyClose,
                  public Usul::Interfaces::IPlugin,
                  public Usul::Interfaces::IFoxDockBar,
                  public Usul::Interfaces::IAddFrame,
                  public Usul::Interfaces::IAnimate,
                  public Usul::Interfaces::IDocumentCreate,
                  public Usul::Interfaces::IGUIDelegate,
                  public Usul::Interfaces::INewDocumentCreate,
                  public FX::FXObject
{
public:

  // Message values.
  enum 
  {
    ID_PREPEND_KEY_FRAME = 1,
    ID_APPEND_KEY_FRAME,
    ID_PLAY_MOVIE_FORWARD,
    ID_PLAY_MOVIE_REVERSE,
    ID_STOP_MOVIE,
    ID_PAUSE_MOVIE,
    ID_CLEAR_FRAMES,
    ID_SAVE_ANIMATION,
    ID_SAVE_AS_ANIMATION,
    ID_LOAD_ANIMATION,
    ID_SAVE_FRAMES,
    ID_SAVE_MOVIE,
    ID_CAMERA_STATE,
    ID_SET_ANIMATION_TIME,
    ID_CHANGE_ANIMATION,
    ID_NEXT_FRAME,
    ID_PREVIOUS_FRAME,
    ID_FIRST_FRAME,
    ID_LAST_FRAME,
    ID_COMBO_BOX,
    ID_SET_FRAME_POSITION,
    ID_COMMENTS,
    ID_FRAME_DUMP,
    ID_DELETE_FRAME,
    ID_CURRENT_CAMERA,
    ID_ANIMATION_PATH,
    ID_ANIMATION_SLIDER,
    ID_LOOP,
    ID_LAST
  };

  /// Typedefs.
  typedef Usul::Base::Referenced BaseClass;
  typedef Usul::Interfaces::IUnknown IUnknown;
  typedef Movie::Frame Frame;
  typedef FoxTools::Menu::Group MenuGroup;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( AnimateComponent );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Default construction.
  AnimateComponent();

  //  Usul::Interfaces::IPlugin
  virtual std::string       getPluginName (  ) const { return "Camera"; }

  //  Usul::Interfaces::IFoxMenuPane
  virtual void              buildMenu ( Usul::Interfaces::IUnknown *caller, FoxTools::Menu::Bar *menuBar );

  //  Usul::Interfaces::INotifyClose
  virtual bool              notifyClose ( Usul::Interfaces::IUnknown *caller );

  //  Usul::Interfaces::IFoxDockBar
  virtual void addDockBar( Usul::Interfaces::IUnknown *caller );

  //Usul::Interfaces::IAddFrame
  virtual void addFrame ( const std::string& groupName, const osg::Vec3& center, float distance, const osg::Quat& rotation );
  virtual void clear    ( const std::string& groupName );

  //Usul::Interfaces::IAnimate
  //Animate from current position to given position
  virtual void animate ( const osg::Vec3& center, float distance, const osg::Quat& rotation );

  //Animate from one position and orientation to another.
  virtual void animate ( const osg::Vec3 &t1, const osg::Quat &r1, const osg::Vec3 &t2, const osg::Quat &r2, double duration );

  //Animate from one orientation to another.
  virtual void animate ( const osg::Quat &r1, const osg::Quat &r2, double duration );

  /// Command message maps.
  long                      onCommandPrependFrame      ( FX::FXObject *, FX::FXSelector, void * );
  long                      onCommandAppendFrame       ( FX::FXObject *, FX::FXSelector, void * );
  long                      onCommandPlayForward       ( FX::FXObject *, FX::FXSelector, void * );
  long                      onCommandPlayReverse       ( FX::FXObject *, FX::FXSelector, void * );
  long                      onCommandStopMovie         ( FX::FXObject *, FX::FXSelector, void * );
  long                      onCommandClear             ( FX::FXObject *, FX::FXSelector, void * );
  long                      onCommandSaveAnimation     ( FX::FXObject *, FX::FXSelector, void * );
  long                      onCommandSaveAsAnimation   ( FX::FXObject *, FX::FXSelector, void * );
  long                      onCommandLoadAnimation     ( FX::FXObject *, FX::FXSelector, void * );
  long                      onCommandCameraState       ( FX::FXObject *, FX::FXSelector, void * );
  long                      onCommandAnimationTime     ( FX::FXObject *, FX::FXSelector, void * );
  long                      onCommandSaveFrames        ( FX::FXObject *, FX::FXSelector, void * );
  long                      onCommandSaveMovie         ( FX::FXObject *, FX::FXSelector, void * );
  long                      onCommandChangeAnimation   ( FX::FXObject *, FX::FXSelector, void * );
  long                      onCommandNextFrame         ( FX::FXObject *, FX::FXSelector, void * );
  long                      onCommandPreviousFrame     ( FX::FXObject *, FX::FXSelector, void * );
  long                      onCommandFirstFrame        ( FX::FXObject *, FX::FXSelector, void * );
  long                      onCommandLastFrame         ( FX::FXObject *, FX::FXSelector, void * );
  long                      onCommandComboBox          ( FX::FXObject *, FX::FXSelector, void * );
  long                      onCommandSetFramePosition  ( FX::FXObject *, FX::FXSelector, void * );
  long                      onCommandComments          ( FX::FXObject *, FX::FXSelector, void * );
  long                      onCommandFrameDump         ( FX::FXObject *, FX::FXSelector, void * );
  long                      onCommandDeleteFrame       ( FX::FXObject *, FX::FXSelector, void * );
  long                      onCommandPause             ( FX::FXObject *, FX::FXSelector, void * );
  long                      onCommandCurrentCamera     ( FX::FXObject *, FX::FXSelector, void * );
  long                      onCommandAnimationPath     ( FX::FXObject *, FX::FXSelector, void * );
  long                      onCommandAnimationSlider   ( FX::FXObject *, FX::FXSelector, void * );
  long                      onCommandLoop              ( FX::FXObject *, FX::FXSelector, void * );

  /// Update message maps.
  long                      onUpdateMenuCommand        ( FX::FXObject *, FX::FXSelector, void * );
  long                      onUpdatePlayForward        ( FX::FXObject *, FX::FXSelector, void * );
  long                      onUpdatePlayReverse        ( FX::FXObject *, FX::FXSelector, void * );
  long                      onUpdateClear              ( FX::FXObject *, FX::FXSelector, void * );
  long                      onUpdateSaveAnimation      ( FX::FXObject *, FX::FXSelector, void * );
  long                      onUpdateChangeAnimation    ( FX::FXObject *, FX::FXSelector, void * );
  long                      onUpdateComboBox           ( FX::FXObject *, FX::FXSelector, void * );
  long                      onUpdateFrameDump          ( FX::FXObject *, FX::FXSelector, void * );
  long                      onUpdateNeedingFrame       ( FX::FXObject *, FX::FXSelector, void * );
  long                      onUpdateFirstFrame         ( FX::FXObject *, FX::FXSelector, void * );
  long                      onUpdateLastFrame          ( FX::FXObject *, FX::FXSelector, void * );
  long                      onUpdateStop               ( FX::FXObject *, FX::FXSelector, void * );
  long                      onUpdatePause              ( FX::FXObject *, FX::FXSelector, void * );
  long                      onUpdateCurrentCamera      ( FX::FXObject *, FX::FXSelector, void * );
  long                      onUpdateCameraState        ( FX::FXObject *, FX::FXSelector, void * );
  long                      onUpdateSaveMovie          ( FX::FXObject *, FX::FXSelector, void * );
  long                      onUpdateAnimationPath      ( FX::FXObject *, FX::FXSelector, void * );
  long                      onUpdateAnimationSlider    ( FX::FXObject *, FX::FXSelector, void * );
  long                      onUpdateLoop               ( FX::FXObject *, FX::FXSelector, void * );

  /// Changed message maps
  long                      onChangedComboBox          ( FX::FXObject *, FX::FXSelector, void * );

protected:

  // Do not copy.
  AnimateComponent ( const AnimateComponent & );
  AnimateComponent &operator = ( const AnimateComponent & );

  /// Use reference counting.
  virtual ~AnimateComponent();

  Frame                     _currentFrame();

  void                      _clearButtons();
  void                      _buildButtons();

  void                      _go ( const Frame &frame );
  void                      _go ( const Frame &from, const Frame& to, double duration );

  void                      _playForward();
  void                      _playReverse();

  void                      _buildPreferencesTab( );

  bool                      _validMovieAndView( );

  /// Usul::Interfaces::IDocumentCreate
  virtual Usul::Documents::Document* createDocument ( Usul::Interfaces::IUnknown *caller = 0x0 );

  /// Usul::Interfaces::IGUIDelegate
  virtual bool                doesHandle( const std::string& token ) const;
  virtual void                createDefaultGUI ( Usul::Documents::Document *document, Usul::Interfaces::IUnknown* caller );
  virtual void                refreshView      ( Usul::Documents::Document *document, Usul::Interfaces::IViewer  *viewer );

  /// Usul::Interfaces::INewDocumentCreate
  /// Name of the document.
  virtual std::string         documentTypeName();

  typedef std::map < std::string, Movie::ValidRefPtr > Movies;

  IUnknown::QueryPtr                _caller;
  Movie::ValidAccessRefPtr          _current;
  Movies                            _movies;
  MenuGroup::ValidAccessRefPtr      _menu;
  Movie::Parameter                  _animationTime;

  FXDECLARE( AnimateComponent )
};


}; // namespace Animate


#endif // _ANIMATE_COMPONENT_CLASS_H_
