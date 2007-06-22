
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

#include "Animate/AnimateComponent.h"
#include "Animate/FrameDumpDialog.h"

#include "FoxTools/Icons/Factory.h"
#include "FoxTools/Dialogs/Message.h"
#include "FoxTools/Dialogs/FileSelection.h"
#include "FoxTools/Functions/Enable.h"
#include "FoxTools/Functions/Check.h"
#include "FoxTools/Functions/Parent.h"
#include "FoxTools/Functions/MainWindow.h"
#include "FoxTools/Functions/App.h"
#include "FoxTools/ToolBar/Button.h"
#include "FoxTools/ToolBar/ToggleButton.h"
#include "FoxTools/ToolBar/Bar.h"
#include "FoxTools/ToolBar/Separator.h"
#include "FoxTools/ToolBar/Factory.h"
#include "FoxTools/ToolBar/ComboBox.h"
#include "FoxTools/ToolBar/Slider.h"
#include "FoxTools/Registry/Registry.h"
#include "FoxTools/Menu/Bar.h"
#include "FoxTools/Menu/RadioButton.h"
#include "FoxTools/Menu/Separator.h"
#include "FoxTools/Menu/CheckButton.h"
#include "FoxTools/Headers/Icon.h"
#include "FoxTools/Headers/App.h"
#include "FoxTools/Headers/HorizontalFrame.h"
#include "FoxTools/Headers/TabBook.h"
#include "FoxTools/Headers/Label.h"
#include "FoxTools/Headers/Spinner.h"
#include "FoxTools/Headers/MainWindow.h"
#include "FoxTools/Headers/ToolBar.h"
#include "FoxTools/Headers/ComboBox.h"
#include "FoxTools/Headers/ToggleButton.h"
#include "FoxTools/Headers/VerticalFrame.h"
#include "FoxTools/Headers/TextField.h"
#include "FoxTools/Headers/DialogBox.h"
#include "FoxTools/Headers/Vec4f.h"
#include "FoxTools/Headers/RealSpinner.h"
#include "FoxTools/Headers/Separator.h"
#include "FoxTools/Headers/MenuRadio.h"
#include "FoxTools/Headers/TabItem.h"
#include "FoxTools/Headers/Slider.h"

#include "Usul/Documents/Manager.h"
#include "Usul/App/Controller.h"
#include "Usul/Components/Manager.h"
#include "Usul/Errors/Stack.h"
#include "Usul/Cast/Cast.h"
#include "Usul/File/Path.h"
#include "Usul/Scope/Reset.h"
#include "Usul/File/Temp.h"
#include "Usul/File/Remove.h"
#include "Usul/Properties/Attribute.h"
#include "Usul/Interfaces/IActiveView.h"
#include "Usul/Interfaces/ITrackball.h"
#include "Usul/Interfaces/IRedraw.h"
#include "Usul/Interfaces/IFrameDump.h"
#include "Usul/Interfaces/ICreateMovie.h"
#include "Usul/Interfaces/IGroup.h"
#include "Usul/Interfaces/IWriteMovieFile.h"
#include "Usul/Interfaces/GUI/ISaveFileDialog.h"
#include "Usul/Interfaces/GUI/ILoadFileDialog.h"
#include "Usul/Interfaces/GUI/IPreferencesManager.h"
#include "Usul/Interfaces/GUI/IFlushEvents.h"
#include "Usul/Interfaces/Fox/IFoxTabItem.h"
#include "Usul/Interfaces/Fox/IFoxTabBook.h"
#include "Usul/Interfaces/Fox/IFoxTabBook.h"
#include "Usul/Interfaces/Fox/IFoxTabItem.h"
#include "Usul/Interfaces/IModifiedSubject.h"
#include "Usul/Interfaces/ISendMessage.h"
#include "Usul/Interfaces/IScreenCapture.h"

#include "GN/Config/UsulConfig.h"
#include "GN/Splines/Curve.h"
#include "GN/Evaluate/Point.h"
#include "GN/Interpolate/Global.h"
#include "GN/Algorithms/KnotVector.h"
#include "GN/Algorithms/Fill.h"
#include "GN/Tessellate/Bisect.h"

#include "Usul/Math/Transpose.h"
#include "Usul/Errors/Assert.h"

#include "OsgTools/ShapeFactory.h"

#include "osg/Vec4"
#include "osg/Geode"
#include "osg/Geometry"
#include "osg/Material"
#include "osg/Group"
#include "osg/LineWidth"
#include "osg/LightModel"
#include "osg/MatrixTransform"
#include "osg/Image"

#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>

using namespace Animate;

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( AnimateComponent , AnimateComponent::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
//  Definitions
//
///////////////////////////////////////////////////////////////////////////////

#define MENU_TOKEN reinterpret_cast < unsigned int > ( this )


///////////////////////////////////////////////////////////////////////////////
//
//  Typedefs
//
///////////////////////////////////////////////////////////////////////////////

typedef FoxTools::Menu::Button MenuButton;
typedef FoxTools::Menu::RadioButton MenuRadio;
typedef FoxTools::Menu::CheckButton MenuCheck;
typedef FoxTools::Menu::Separator MenuSeparator;
typedef FoxTools::Menu::Bar MenuBar;
typedef Usul::Properties::Attribute < unsigned int > UIntAttribute;
typedef FoxTools::ToolBar::Bar ToolBar;


///////////////////////////////////////////////////////////////////////////////
//
//  Convenience function.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  inline FX::FXComboBox* getComboBox()
  {
    // Get the camera tool bar.
    ToolBar::ValidRefPtr camera ( FoxTools::ToolBar::Factory::instance().toolbar ( "Camera" ) );

    // Find the command with name of combo box.
    FoxTools::Items::Command::RefPtr command ( camera->find ( "ComboBox" ) );

    // If we have a valid command.
    if( command.valid() )
    {
      // Is this command a combo box.
      if( FoxTools::ToolBar::ComboBox *comboBox = dynamic_cast < FoxTools::ToolBar::ComboBox * > ( command.get() ) )
      {
        return comboBox->comboBox();
      }
    }

    // If we get here, no combo box.
    return 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Message maps.
//
///////////////////////////////////////////////////////////////////////////////

FXDEFMAP ( AnimateComponent ) AnimateMap[] = 
{
  //          Message Type     ID                                   Message Handler.
  FXMAPFUNC ( FX::SEL_COMMAND, AnimateComponent::ID_PREPEND_KEY_FRAME,     AnimateComponent::onCommandPrependFrame     ),
  FXMAPFUNC ( FX::SEL_UPDATE,  AnimateComponent::ID_PREPEND_KEY_FRAME,     AnimateComponent::onUpdateMenuCommand       ),
  FXMAPFUNC ( FX::SEL_COMMAND, AnimateComponent::ID_APPEND_KEY_FRAME,      AnimateComponent::onCommandAppendFrame      ),
  FXMAPFUNC ( FX::SEL_UPDATE,  AnimateComponent::ID_APPEND_KEY_FRAME,      AnimateComponent::onUpdateMenuCommand       ),
  FXMAPFUNC ( FX::SEL_COMMAND, AnimateComponent::ID_PLAY_MOVIE_FORWARD,    AnimateComponent::onCommandPlayForward      ),
  FXMAPFUNC ( FX::SEL_UPDATE,  AnimateComponent::ID_PLAY_MOVIE_FORWARD,    AnimateComponent::onUpdatePlayForward       ),
  FXMAPFUNC ( FX::SEL_COMMAND, AnimateComponent::ID_PLAY_MOVIE_REVERSE,    AnimateComponent::onCommandPlayReverse      ),
  FXMAPFUNC ( FX::SEL_UPDATE,  AnimateComponent::ID_PLAY_MOVIE_REVERSE,    AnimateComponent::onUpdatePlayReverse       ),
  FXMAPFUNC ( FX::SEL_COMMAND, AnimateComponent::ID_STOP_MOVIE,            AnimateComponent::onCommandStopMovie        ),
  FXMAPFUNC ( FX::SEL_UPDATE,  AnimateComponent::ID_STOP_MOVIE,            AnimateComponent::onUpdateStop              ),
  FXMAPFUNC ( FX::SEL_COMMAND, AnimateComponent::ID_CLEAR_FRAMES,          AnimateComponent::onCommandClear            ),
  FXMAPFUNC ( FX::SEL_UPDATE,  AnimateComponent::ID_CLEAR_FRAMES,          AnimateComponent::onUpdateClear             ),
  FXMAPFUNC ( FX::SEL_COMMAND, AnimateComponent::ID_SAVE_ANIMATION,        AnimateComponent::onCommandSaveAnimation    ),
  FXMAPFUNC ( FX::SEL_UPDATE,  AnimateComponent::ID_SAVE_ANIMATION,        AnimateComponent::onUpdateSaveAnimation     ),
  FXMAPFUNC ( FX::SEL_COMMAND, AnimateComponent::ID_SAVE_AS_ANIMATION,     AnimateComponent::onCommandSaveAsAnimation  ),
  FXMAPFUNC ( FX::SEL_UPDATE,  AnimateComponent::ID_SAVE_AS_ANIMATION,     AnimateComponent::onUpdateSaveAnimation     ),
  FXMAPFUNC ( FX::SEL_COMMAND, AnimateComponent::ID_LOAD_ANIMATION,        AnimateComponent::onCommandLoadAnimation    ),
  FXMAPFUNC ( FX::SEL_UPDATE,  AnimateComponent::ID_LOAD_ANIMATION,        AnimateComponent::onUpdateMenuCommand       ),
  FXMAPFUNC ( FX::SEL_COMMAND, AnimateComponent::ID_CAMERA_STATE,          AnimateComponent::onCommandCameraState      ),
  FXMAPFUNC ( FX::SEL_UPDATE,  AnimateComponent::ID_CAMERA_STATE,          AnimateComponent::onUpdateCameraState       ),
  FXMAPFUNC ( FX::SEL_COMMAND, AnimateComponent::ID_SET_ANIMATION_TIME,    AnimateComponent::onCommandAnimationTime    ),
  FXMAPFUNC ( FX::SEL_COMMAND, AnimateComponent::ID_SAVE_FRAMES,           AnimateComponent::onCommandSaveFrames       ),
  FXMAPFUNC ( FX::SEL_UPDATE,  AnimateComponent::ID_SAVE_FRAMES,           AnimateComponent::onUpdateSaveAnimation     ),
  FXMAPFUNC ( FX::SEL_COMMAND, AnimateComponent::ID_SAVE_MOVIE,            AnimateComponent::onCommandSaveMovie        ),
  FXMAPFUNC ( FX::SEL_UPDATE,  AnimateComponent::ID_SAVE_MOVIE,            AnimateComponent::onUpdateSaveMovie         ),
  FXMAPFUNC ( FX::SEL_COMMAND, AnimateComponent::ID_CHANGE_ANIMATION,      AnimateComponent::onCommandChangeAnimation  ),
  FXMAPFUNC ( FX::SEL_UPDATE,  AnimateComponent::ID_CHANGE_ANIMATION,      AnimateComponent::onUpdateChangeAnimation   ),
  FXMAPFUNC ( FX::SEL_COMMAND, AnimateComponent::ID_FIRST_FRAME,           AnimateComponent::onCommandFirstFrame       ),
  FXMAPFUNC ( FX::SEL_UPDATE,  AnimateComponent::ID_FIRST_FRAME,           AnimateComponent::onUpdateFirstFrame        ),
  FXMAPFUNC ( FX::SEL_COMMAND, AnimateComponent::ID_PREVIOUS_FRAME,        AnimateComponent::onCommandPreviousFrame    ),
  FXMAPFUNC ( FX::SEL_UPDATE,  AnimateComponent::ID_PREVIOUS_FRAME,        AnimateComponent::onUpdateFirstFrame        ),
  FXMAPFUNC ( FX::SEL_COMMAND, AnimateComponent::ID_NEXT_FRAME,            AnimateComponent::onCommandNextFrame        ),
  FXMAPFUNC ( FX::SEL_UPDATE,  AnimateComponent::ID_NEXT_FRAME,            AnimateComponent::onUpdateLastFrame         ),
  FXMAPFUNC ( FX::SEL_COMMAND, AnimateComponent::ID_LAST_FRAME,            AnimateComponent::onCommandLastFrame        ),
  FXMAPFUNC ( FX::SEL_UPDATE,  AnimateComponent::ID_LAST_FRAME,            AnimateComponent::onUpdateLastFrame         ),
  FXMAPFUNC ( FX::SEL_COMMAND, AnimateComponent::ID_COMBO_BOX,             AnimateComponent::onCommandComboBox         ),
  FXMAPFUNC ( FX::SEL_UPDATE,  AnimateComponent::ID_COMBO_BOX,             AnimateComponent::onUpdateComboBox          ),
  FXMAPFUNC ( FX::SEL_CHANGED, AnimateComponent::ID_COMBO_BOX,             AnimateComponent::onChangedComboBox         ),
  FXMAPFUNC ( FX::SEL_COMMAND, AnimateComponent::ID_SET_FRAME_POSITION,    AnimateComponent::onCommandSetFramePosition ),
  FXMAPFUNC ( FX::SEL_UPDATE,  AnimateComponent::ID_SET_FRAME_POSITION,    AnimateComponent::onUpdateNeedingFrame      ),
  FXMAPFUNC ( FX::SEL_COMMAND, AnimateComponent::ID_COMMENTS,              AnimateComponent::onCommandComments         ),
  FXMAPFUNC ( FX::SEL_UPDATE,  AnimateComponent::ID_COMMENTS,              AnimateComponent::onUpdateNeedingFrame      ),
  FXMAPFUNC ( FX::SEL_COMMAND, AnimateComponent::ID_FRAME_DUMP,            AnimateComponent::onCommandFrameDump        ),
  FXMAPFUNC ( FX::SEL_UPDATE,  AnimateComponent::ID_FRAME_DUMP,            AnimateComponent::onUpdateFrameDump         ),
  FXMAPFUNC ( FX::SEL_COMMAND, AnimateComponent::ID_DELETE_FRAME,          AnimateComponent::onCommandDeleteFrame      ),
  FXMAPFUNC ( FX::SEL_UPDATE,  AnimateComponent::ID_DELETE_FRAME,          AnimateComponent::onUpdateNeedingFrame      ),
  FXMAPFUNC ( FX::SEL_COMMAND, AnimateComponent::ID_PAUSE_MOVIE,           AnimateComponent::onCommandPause            ),
  FXMAPFUNC ( FX::SEL_UPDATE,  AnimateComponent::ID_PAUSE_MOVIE,           AnimateComponent::onUpdatePause             ),
  FXMAPFUNC ( FX::SEL_COMMAND, AnimateComponent::ID_ANIMATION_PATH,        AnimateComponent::onCommandAnimationPath    ),
  FXMAPFUNC ( FX::SEL_UPDATE,  AnimateComponent::ID_ANIMATION_PATH,        AnimateComponent::onUpdateAnimationPath     ),
  FXMAPFUNC ( FX::SEL_CHANGED, AnimateComponent::ID_ANIMATION_SLIDER,      AnimateComponent::onCommandAnimationSlider  ),
  FXMAPFUNC ( FX::SEL_UPDATE,  AnimateComponent::ID_ANIMATION_SLIDER,      AnimateComponent::onUpdateAnimationSlider   ),
  FXMAPFUNC ( FX::SEL_COMMAND, AnimateComponent::ID_LOOP,                  AnimateComponent::onCommandLoop             ),
  FXMAPFUNC ( FX::SEL_UPDATE,  AnimateComponent::ID_LOOP,                  AnimateComponent::onUpdateLoop              ),
};

FOX_TOOLS_IMPLEMENT ( AnimateComponent, FX::FXObject, AnimateMap, ARRAYNUMBER ( AnimateMap ) );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

AnimateComponent::AnimateComponent() : BaseClass(),
  _caller         (),
  _current        ( 0x0 ),
  _movies         (),
  _menu           ( 0x0 ),
  _animationTime  ( 2 ),
  _slideContents  ( 0x0 )
{
  this->_createMovie( "" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

AnimateComponent::~AnimateComponent()
{
  this->_clearSlides();

  _menu = 0x0;
  _caller = static_cast < Usul::Interfaces::IUnknown* > ( 0x0 );

  for ( Movies::iterator i = _movies.begin(); i != _movies.end(); ++i )
  {
    Usul::Interfaces::IModifiedSubject::QueryPtr subject ( i->second );

    if ( subject.valid() )
      subject->removeModifiedObserver( this );

    i->second->closing( 0x0 );
  }

  _movies.clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *AnimateComponent::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::IFoxMenuPane::IID:
    return static_cast < Usul::Interfaces::IFoxMenuPane*>(this);
  case Usul::Interfaces::IPlugin::IID:
    return static_cast < Usul::Interfaces::IPlugin* > ( this );
  case Usul::Interfaces::IFoxDockBar::IID:
    return static_cast < Usul::Interfaces::IFoxDockBar* > ( this );
  case Usul::Interfaces::IAddFrame::IID:
    return static_cast < Usul::Interfaces::IAddFrame* > ( this );
  case Usul::Interfaces::IAnimate::IID:
    return static_cast < Usul::Interfaces::IAnimate* > ( this );
  case Usul::Interfaces::IDocumentCreate::IID:
    return static_cast < Usul::Interfaces::IDocumentCreate* > ( this );
  case Usul::Interfaces::IGUIDelegate::IID:
    return static_cast < Usul::Interfaces::IGUIDelegate* > ( this );
  case Usul::Interfaces::INewDocumentCreate::IID:
    return static_cast < Usul::Interfaces::INewDocumentCreate* > ( this );
  case Usul::Interfaces::IModifiedObserver::IID:
    return static_cast < Usul::Interfaces::IModifiedObserver * > ( this );
  default:
    return 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the menu.
//
///////////////////////////////////////////////////////////////////////////////

void AnimateComponent::buildMenu ( Usul::Interfaces::IUnknown *caller, FoxTools::Menu::Bar *menuBar  )
{
  // Handle bad input.
  if ( 0x0 == menuBar )
    throw std::invalid_argument ( "Error 3231413723: null menu-bar given" );

  // Set the caller.
  _caller = caller;

  // Build the menu.
  _menu = new MenuGroup ( "Camera" );
    _menu->append ( new MenuButton ( "&Prepend", "", "Prepend Key Frame", this, AnimateComponent::ID_PREPEND_KEY_FRAME ) );
    _menu->append ( new MenuButton ( "&Append",  "", "Append Key Frame",  this, AnimateComponent::ID_APPEND_KEY_FRAME  ) );
    _menu->append ( new MenuSeparator );
    _menu->append ( new MenuCheck  ( "Loop", "", "Loop the animation", this, AnimateComponent::ID_LOOP ) );
    _menu->append ( new MenuButton ( "Play &Forward", "", "Play Movie Forward", this, AnimateComponent::ID_PLAY_MOVIE_FORWARD ) );
    _menu->append ( new MenuButton ( "Play &Reverse", "", "Play Movie Reverse", this, AnimateComponent::ID_PLAY_MOVIE_REVERSE ) );
    _menu->append ( new MenuSeparator );
    _menu->append ( new MenuButton ( "&Clear",        "", "Clear Frames",                     this, AnimateComponent::ID_CLEAR_FRAMES   ) );
    _menu->append ( new MenuSeparator );
    _menu->append ( new MenuButton ( "&Load Camera File...", "", "Load a OSG FOX animation file.", this, AnimateComponent::ID_LOAD_ANIMATION ) );
    _menu->append ( new MenuButton ( "&Save Camera File",    "", "Save a OSG FOX animation file.", this, AnimateComponent::ID_SAVE_ANIMATION ) );
    _menu->append ( new MenuButton ( "&Save Camera File As...", "", "Save a OSG FOX animation file.", this, AnimateComponent::ID_SAVE_AS_ANIMATION ) );
    _menu->append ( new MenuButton ( "&Save Frames...",      "", "Save frames as images.",         this, AnimateComponent::ID_SAVE_FRAMES    ) );
    _menu->append ( new MenuButton ( "&Save Movie...",       "", "Save movie.",                    this, AnimateComponent::ID_SAVE_MOVIE     ) );
    this->_buildButtons();
  menuBar->append ( _menu );

  // Build the preferences.
  this->_buildPreferencesTab();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear the frame buttons.
//
///////////////////////////////////////////////////////////////////////////////

void AnimateComponent::_clearButtons()
{
  // Delete all items with token "this".
  _menu->purge ( MENU_TOKEN );

  FX::FXComboBox* combo ( Detail::getComboBox() );

  // Clear the combo box
  if( combo )
    combo->clearItems();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the menu.
//
///////////////////////////////////////////////////////////////////////////////

void AnimateComponent::_buildButtons()
{
  // Make sure there is a pane.
  if ( !_menu.valid() )
    return;

  // Delete the ones we have.
  this->_clearButtons();

  // Get current movie.
  Movie::RefPtr movie ( _current.get() );

  // Make the separator.
  _menu->append ( new MenuSeparator( MENU_TOKEN ) );

  for ( Movies::iterator i = _movies.begin(); i != _movies.end(); ++i )
  {
    // Make a new button and add it.
    MenuRadio::ValidRefPtr button ( new MenuRadio ( i->first.c_str(), this, AnimateComponent::ID_CHANGE_ANIMATION, MENU_TOKEN ) );
    _menu->append ( button );
  }
  
  // Set the number of visible items in the combo box.
  const unsigned int numFrames  ( movie->getNumberFrames() );
  const unsigned int numVisible ( numFrames > 15 ? 15 : numFrames );

  FX::FXComboBox* combo ( Detail::getComboBox() );
  
  // If we have a valid combo box, set the number of items visible
  if( combo )
    combo->setNumVisible ( numVisible );

  // Make the camera menu, but don't add it yet.
  MenuGroup::ValidRefPtr cameras ( new MenuGroup ( "Cameras" ) );

  // If we have cameras...
  if ( !movie->empty() )
  {
    // Add the camera menu.
    _menu->append ( new MenuSeparator( MENU_TOKEN ) );
    cameras->scroll ( true );
    _menu->append ( cameras );
  }

  // Loop through the list.
  unsigned int count ( 0 );
  for ( Movie::iterator i = movie->begin(); i != movie->end(); ++i )
  {
    // Make the text.
    std::ostringstream name, description;

    // Get the name from the frame if is has one, if not build it from frame count
    if ( i->getName().empty() )
      name << "Camera " << count;
    else
      name << i->getName();

    // Append the item to the combo box.
    if( combo )
      combo->appendItem ( name.str().c_str() );

    // Append the help string.
    description << "Switch to camera " << count;

    // Add a button.
    MenuRadio::ValidRefPtr radio ( new MenuRadio ( name.str(), "", description.str(), this, AnimateComponent::ID_CAMERA_STATE, MENU_TOKEN ) );
    radio->userData ( new UIntAttribute ( count ) );
    cameras->append ( radio );

    // Don't increment until now.
    ++count;
  }

  // Rebuild the menu
  _menu->rebuild();
  _menu->create();

}


///////////////////////////////////////////////////////////////////////////////
//
//  Prepend a frame.
//
///////////////////////////////////////////////////////////////////////////////

long AnimateComponent::onCommandPrependFrame ( FX::FXObject *, FX::FXSelector, void * )
{
  Movie::RefPtr movie ( _current.get() );

  movie->prependFrame ( this->_currentFrame() );
  //this->_buildButtons();

  // Set the current frame to this one
  movie->setCurrentFrame ( 0 );

  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Append a frame.
//
///////////////////////////////////////////////////////////////////////////////

long AnimateComponent::onCommandAppendFrame ( FX::FXObject *, FX::FXSelector, void * )
{
  Movie::RefPtr movie ( _current.get() );

  movie->appendFrame ( this->_currentFrame() );
  //this->_buildButtons();

  // Set the current frame to this one
  movie->setCurrentFrame ( movie->getNumberFrames() - 1 );

  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Play forwards.
//
///////////////////////////////////////////////////////////////////////////////

long AnimateComponent::onCommandPlayForward ( FX::FXObject *, FX::FXSelector, void * )
{
  this->_playForward();
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Play reverse.
//
///////////////////////////////////////////////////////////////////////////////

long AnimateComponent::onCommandPlayReverse ( FX::FXObject *, FX::FXSelector, void * )
{
  this->_playReverse();
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Go to this frame.
//
///////////////////////////////////////////////////////////////////////////////

void AnimateComponent::_go ( const Frame &frame )
{
  // Make a temporary movie from the current frame to the given one.
  Movie::ValidRefPtr movie ( new Movie );

  // Append the current frame.
  movie->appendFrame ( this->_currentFrame() );

  // Append where they want to go.
  movie->appendFrame ( frame );

  // Need a scope...
  {
    // Temporarily set the animation time to something small.
    typedef Movie::Parameter Parameter;
    Usul::Scope::Reset<Parameter> reset ( movie->segmentDuration(), 500, movie->segmentDuration() );

    // Play this movie.
    movie->playForward( _caller );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Go from one frame to another
//
///////////////////////////////////////////////////////////////////////////////

void AnimateComponent::_go ( const Frame &from, const Frame& to, double duration )
{
  // Make a temporary movie from the current frame to the given one.
  Movie::ValidRefPtr movie ( new Movie );

  // Append the from frame.
  movie->appendFrame ( from );

  // Append where they want to go.
  movie->appendFrame ( to );

  //Set the duration to time specified
  movie->segmentDuration( duration );

  // Play this movie.
  movie->playForward( _caller );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Play forwards.
//
///////////////////////////////////////////////////////////////////////////////

void AnimateComponent::_playForward()
{
  Movie::RefPtr movie ( _current.get() );

  // Play the animation.
  movie->playForward( _caller );

}


///////////////////////////////////////////////////////////////////////////////
//
//  Play reverse.
//
///////////////////////////////////////////////////////////////////////////////

void AnimateComponent::_playReverse()
{
  Movie::RefPtr movie ( _current.get() );

  // Play the animation.
  movie->playReverse( _caller );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Stop the animation.
//
///////////////////////////////////////////////////////////////////////////////

long AnimateComponent::onCommandStopMovie ( FX::FXObject *, FX::FXSelector, void * )
{
  _current->stop();
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear all frames.
//
///////////////////////////////////////////////////////////////////////////////

long AnimateComponent::onCommandClear ( FX::FXObject *object, FX::FXSelector, void * )
{
  bool result ( true );

#if 0

  // Not sure if this makes sense here because the user can say "No" and have 
  // it remember, which means that subsequent calls to "Clear" do nothing.

  // TODO: make it remember iff the user chooses yes. 1314169231

  FoxTools::Dialogs::Message dialog ( "clearing_cameras_cannot_be_undone", true, false );
  dialog.text ( "No Undo!", "Clearing the cameras cannot be undone. Continue?" );
  dialog.icon ( FoxTools::Icons::Factory::ICON_WARNING, object );
  dialog.buttons ( "Yes", "No" );

  result = ( "Yes" == dialog.run ( object ) );

#endif

  if ( result )
  {
    _current->clear();
    this->_clearButtons();
  }

  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the menu button.
//
///////////////////////////////////////////////////////////////////////////////

long AnimateComponent::onUpdateClear ( FX::FXObject *object, FX::FXSelector, void * )
{
  Usul::Interfaces::IActiveView::QueryPtr activeView ( _caller );
  bool active ( activeView.valid() && 0x0 != activeView->getActiveView() );
  bool movie ( _current.valid() && !_current->empty() );
  FoxTools::Functions::enable ( active && movie, object );
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Save the animation.
//
///////////////////////////////////////////////////////////////////////////////

long AnimateComponent::onCommandSaveAnimation ( FX::FXObject *, FX::FXSelector, void * )
{
  _current->save ( _caller );

  this->_buildButtons();

  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Save the animation.
//
///////////////////////////////////////////////////////////////////////////////

long AnimateComponent::onCommandSaveAsAnimation ( FX::FXObject *, FX::FXSelector, void * )
{
  _current->saveAs ( _caller );

  this->_buildButtons();

  return 1;
}

///////////////////////////////////////////////////////////////////////////////
//
//  Update the menu button.
//
///////////////////////////////////////////////////////////////////////////////

long AnimateComponent::onUpdateSaveAnimation ( FX::FXObject *object, FX::FXSelector, void * )
{
  FoxTools::Functions::enable ( _current.valid() && !_current->empty(), object );
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Load the animation.
//
///////////////////////////////////////////////////////////////////////////////

long AnimateComponent::onCommandLoadAnimation ( FX::FXObject *, FX::FXSelector, void * )
{
  typedef Usul::Interfaces::ILoadFileDialog ILoadFileDialog;
  typedef ILoadFileDialog::FileName FileName;
  typedef ILoadFileDialog::Filters Filters;
  typedef ILoadFileDialog::FileResult FileResult;

  FileName filename;

  {
    Movie::RefPtr movie ( new Movie );
    ILoadFileDialog::ValidQueryPtr loadFileDialog ( _caller );
    Filters filters ( movie->filtersOpen() );
    FileResult result ( loadFileDialog->getLoadFileName ( "Load Animation", filters ) );
    filename = result.first;
  }

  if ( false == filename.empty() )
  {
    Movie::RefPtr movie ( this->_createMovie ( filename ) );

    movie->open ( filename );
  }

  this->_buildButtons();

  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the camera state.
//
///////////////////////////////////////////////////////////////////////////////

long AnimateComponent::onCommandCameraState ( FX::FXObject *object, FX::FXSelector, void * )
{
  FX::FXMenuRadio *mr = SAFE_CAST_FOX ( FX::FXMenuRadio, object );
  if( 0x0 == mr )
    return 0;

  // Find the button.
  MenuRadio::RefPtr button ( reinterpret_cast < MenuRadio * > ( mr->getUserData() ) );
  if ( false == button.valid() )
    return 0;
 
  // Get user-data.
  UIntAttribute::RefPtr data ( dynamic_cast < UIntAttribute * > ( button->userData() ) );
  unsigned int num ( data.valid() ? data->value() : 0 );

  // Set frame for this button.
  const Frame &frame ( _current->frame ( num ) );
  _current->setCurrentFrame ( num );

  // Go to this frame.
  this->_go ( frame );

  // Message handled.
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the camera button
//
///////////////////////////////////////////////////////////////////////////////

long AnimateComponent::onUpdateCameraState ( FX::FXObject *object, FX::FXSelector sel, void *data )
{
  FX::FXMenuRadio *mr = SAFE_CAST_FOX ( FX::FXMenuRadio, object );
  if( 0x0 == mr )
    return 0;

  // Find the button.
  MenuRadio::RefPtr button ( reinterpret_cast < MenuRadio * > ( mr->getUserData() ) );
  if ( false == button.valid() )
    return 0;
  
  if ( this->_validMovieAndView() && !_current->empty() )
  {
    button->enable ( true );

    Usul::Interfaces::IActiveView::ValidQueryPtr activeView ( _caller );
    Usul::Interfaces::ITrackball::ValidQueryPtr trackball ( activeView->getActiveView() );

    // Get user-data.
    UIntAttribute::RefPtr data ( dynamic_cast < UIntAttribute * > ( button->userData() ) );
    unsigned int num ( data.valid() ? data->value() : 0 );

    // See if the trackball and frame are the same...
    const Frame &frame ( _current->frame ( num ) );
    bool result ( frame.getCenter() == trackball->getCenter() && frame.getDistance() == trackball->getDistance() && frame.getRotation() == trackball->getRotation() );
    
    button->check ( result );

  }
  else
    button->enable ( false );

  // Message handled.
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update menu command.
//
///////////////////////////////////////////////////////////////////////////////

long AnimateComponent::onUpdateMenuCommand ( FX::FXObject *object, FX::FXSelector, void * )
{
  // Get the active view
  Usul::Interfaces::IActiveView::QueryPtr activeView ( _caller );

  // Enable or Disable
  FoxTools::Functions::enable ( ( activeView.valid() && 0x0 != activeView->getActiveView() ), object );

  // Message handled.
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is the movie playing forwards?
//
///////////////////////////////////////////////////////////////////////////////

long AnimateComponent::onUpdatePlayForward ( FX::FXObject *object, FX::FXSelector, void * )
{
  bool enable ( this->_validMovieAndView() && _current->canPlay() && _current->getCurrentFrame() != ( _current->getNumberFrames() - 1 ) );
  bool result ( _current->playingForwards() );

  FoxTools::Functions::enable( enable, object );
  FoxTools::Functions::check ( result, object );

  // Message handled.
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update menu commands that depend on the number of frames.
//
///////////////////////////////////////////////////////////////////////////////

long AnimateComponent::onUpdatePlayReverse ( FX::FXObject *object, FX::FXSelector, void * )
{
  bool enable ( this->_validMovieAndView() && _current->canPlay() && _current->getCurrentFrame() != 0 );
  bool result ( _current->playingReverse() );

  FoxTools::Functions::enable( enable, object );
  FoxTools::Functions::check ( result, object );

  // Message handled.
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the current frame.
//
///////////////////////////////////////////////////////////////////////////////

AnimateComponent::Frame AnimateComponent::_currentFrame()
{
  Usul::Interfaces::IActiveView::ValidQueryPtr activeView ( _caller );
  Usul::Interfaces::ITrackball::ValidQueryPtr trackball ( activeView->getActiveView() );
  Frame frame ( trackball->getCenter(), trackball->getDistance(), trackball->getRotation() );
  return frame;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build a tab for the preferences dialog
//
///////////////////////////////////////////////////////////////////////////////

void AnimateComponent::_buildPreferencesTab( )
{
  Usul::Interfaces::IPreferencesManager::QueryPtr pref ( _caller );
  if( pref.valid() )
  {
    //Needed interfaces
    Usul::Interfaces::IFoxTabItem::QueryPtr foxTabItem ( pref->getPreferences() );
    Usul::Interfaces::IFoxTabBook::QueryPtr foxTabBook ( pref->getPreferences() );

    if( foxTabItem.valid() && foxTabBook.valid() )
    {
      //Get the last animation time from the registry
      FX::FXRegistry &reg = FoxTools::Functions::application()->reg();
      _animationTime = reg.readRealEntry ( "CameraPreferences", "Animation Time", 2 );

      //Add a new tab for camera
      foxTabItem->addTab( "Camera" );
      FX::FXTabBook *tabBook = foxTabBook->tabBook() ;

      FX::FXHorizontalFrame* pane = new FX::FXHorizontalFrame 
        ( tabBook, LAYOUT_FILL_X|LAYOUT_FILL_Y|FRAME_THICK );
      new FX::FXLabel ( pane, "Animation Time", NULL, FX::LAYOUT_LEFT );

      // Spinner to set animation time value
      FX::FXSpinner* spinner = new FX::FXSpinner 
        ( pane, 2, this, AnimateComponent::ID_SET_ANIMATION_TIME,
          JUSTIFY_RIGHT | FRAME_SUNKEN | FRAME_THICK | 
          LAYOUT_LEFT | LAYOUT_FILL_COLUMN | LAYOUT_FILL_ROW );
      spinner->setValue ( static_cast < int > ( _animationTime ) );
      spinner->setRange ( 1, 15 );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the animation time
//
///////////////////////////////////////////////////////////////////////////////

long AnimateComponent::onCommandAnimationTime ( FX::FXObject *, FX::FXSelector, void *data )
{
  // Get the new time
  const int time = reinterpret_cast < int > ( data );
  _animationTime = ( ( time > 0 ) ? static_cast < double > ( time ) : 0 );
  
  _current->segmentDuration ( _animationTime );

  // Save this value to the registry
  FX::FXRegistry &reg = FoxTools::Functions::application()->reg();
  reg.writeRealEntry ( "CameraPreferences", "Animation Time", _animationTime );

  // Message handled.
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Save the animation as a series of frames
//
///////////////////////////////////////////////////////////////////////////////

long AnimateComponent::onCommandSaveFrames ( FX::FXObject *, FX::FXSelector, void * )
{
  typedef Usul::Interfaces::IFrameDump IFrameDump;

  FrameDumpDialog dialog;
  IFrameDump *fd ( dialog.getFrameDumpProperties( _caller ) );
  if( fd )
  {
    IFrameDump::ScopedDump sd ( fd );
    this->_playForward();
  }

  // Message handled.
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Save the movie.
//
///////////////////////////////////////////////////////////////////////////////

long AnimateComponent::onCommandSaveMovie ( FX::FXObject *, FX::FXSelector, void * )
{

  typedef FoxTools::Dialogs::FileSelection FileSelection;

  //FileSelection::Filters filters ( create->getFilters() );
  FileSelection::Filters filters ( _current->filtersWrite() );

  const FileSelection::FileResult result ( FoxTools::Dialogs::FileSelection::askForFileName ( FileSelection::SAVE, "Movie Name", filters, 0x0, true ) );

  std::string filename ( result.first );

  if( filename.empty() )
    return 0;

  //Useful typedefs
  typedef Usul::Interfaces::IFrameDump IFrameDump;
  typedef Usul::Interfaces::IActiveView IActiveView;

  IActiveView::ValidQueryPtr view ( _caller );
  IFrameDump::ValidQueryPtr fd ( view->getActiveView() );

  // Create a temp name
  Usul::File::Temp temp;

  const std::string path ( temp.name() );
  const std::string dir  ( Usul::File::directory( path, false ) );
  const std::string name ( Usul::File::base ( path ) );
  const std::string ext ( ".jpg" );
  const unsigned int digits ( 10 );

  fd->frameDumpProperties ( dir, name, ext, 0, digits );
  {
    fd->dumpFrames ( true );
    IFrameDump::ScopedDump sd ( fd );
    this->_playForward();
    fd->dumpFrames ( false );
  }

  // Get the filenames.
  Movie::FileNames filenames ( fd->filenames() );

  // Create the movie
  _current->writeMovie ( filename, filenames );

  // Delete the created image files.
  for( unsigned int i = 0; i < filenames.size(); ++i )
  {
    Usul::File::remove ( filenames.at( i ) );
  }

  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update save movie menu
//
///////////////////////////////////////////////////////////////////////////////

long AnimateComponent::onUpdateSaveMovie ( FX::FXObject *object, FX::FXSelector, void * )
{
  Usul::Interfaces::IWriteMovieFile::QueryPtr create ( Usul::Components::Manager::instance().getInterface ( Usul::Interfaces::IWriteMovieFile::IID ) );

  FoxTools::Functions::enable ( this->_validMovieAndView() && create.valid(), object );

  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Change the current animation to one clicked on
//
///////////////////////////////////////////////////////////////////////////////

long AnimateComponent::onCommandChangeAnimation ( FX::FXObject *object, FX::FXSelector, void * )
{ 
  FX::FXMenuRadio *mr = SAFE_CAST_FOX ( FX::FXMenuRadio, object );
  if( 0x0 == mr )
    return 0;

  // Find the button.
  MenuRadio::RefPtr button ( reinterpret_cast < MenuRadio * > ( mr->getUserData() ) );
  if ( false == button.valid() )
    return 0;

  const std::string& filename ( button->name() );
  Movies::iterator i = _movies.find ( filename );
  if ( i != _movies.end() )
    _current = i->second;

  this->_buildGUIForCurrentMovie();

  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Should the animation button be checked?
//
///////////////////////////////////////////////////////////////////////////////

long AnimateComponent::onUpdateChangeAnimation ( FX::FXObject *object, FX::FXSelector, void * )
{
  FX::FXMenuRadio *mr = SAFE_CAST_FOX ( FX::FXMenuRadio, object );
  if( 0x0 == mr )
    return 0;

  // Find the button.
  MenuRadio::RefPtr button ( reinterpret_cast < MenuRadio * > ( mr->getUserData() ) );
  if ( false == button.valid() )
    return 0;

  // Set the check if the strings are the same.
  button->check ( _current->fileName() == button->name() );

  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the dock bar
//
///////////////////////////////////////////////////////////////////////////////

void AnimateComponent::addDockBar( Usul::Interfaces::IUnknown *caller )
{
  // Typedefs
  typedef FoxTools::ToolBar::ToggleButton ToggleButton;
  typedef FoxTools::ToolBar::Button Button;
  typedef FoxTools::ToolBar::Factory Factory;
  typedef FoxTools::Icons::Factory Icons;

  //Make the animation toolbar
  ToolBar::ValidRefPtr animateToolBar ( Factory::instance().create( "Animation", Factory::TOP ) );

  //Add buttons to animation toolbars
  animateToolBar->append ( new ToggleButton ( "Frame Dump", "Toggle Frame Dumping ( Recording ).",  Icons::ICON_RED_CIRCLE,  this, AnimateComponent::ID_FRAME_DUMP  ) );
  animateToolBar->append ( new ToggleButton ( "Animation Path", "Hide/Show Animation Path",         Icons::ICON_CURVE,       this, AnimateComponent::ID_ANIMATION_PATH  ) );
  animateToolBar->append ( new FoxTools::ToolBar::Separator() );
  animateToolBar->append ( new Button ( "First Frame",    "Jump to the first frame.",        Icons::ICON_BACKWARD_ARROW_BAR, this, AnimateComponent::ID_FIRST_FRAME  ) );
  animateToolBar->append ( new Button ( "Previous Frame", "Jump to the previous frame.",     Icons::ICON_BACKWARD_ARROW,     this, AnimateComponent::ID_PREVIOUS_FRAME  ) );
  animateToolBar->append ( new ToggleButton ( "Play backwards", "Play the movie backwards.", Icons::ICON_PLAY_BACKWARDS,     this, AnimateComponent::ID_PLAY_MOVIE_REVERSE ) );
  animateToolBar->append ( new Button ( "Stop",           "Stop the movie.",                 Icons::ICON_STOP,               this, AnimateComponent::ID_STOP_MOVIE ) );
  animateToolBar->append ( new ToggleButton ( "Play forwards",  "Play the movie forwards.",  Icons::ICON_PLAY,               this, AnimateComponent::ID_PLAY_MOVIE_FORWARD ) );
  animateToolBar->append ( new Button ( "Next Frame",     "Jump to the next frame.",         Icons::ICON_FORWARD_ARROW,      this, AnimateComponent::ID_NEXT_FRAME  ) );
  animateToolBar->append ( new Button ( "Last Frame",     "Jump to the last frame.",         Icons::ICON_FORWARD_ARROW_BAR,  this, AnimateComponent::ID_LAST_FRAME  ) );
 
  //Make the camera toolbar
  ToolBar::ValidRefPtr cameraToolBar ( Factory::instance().create ( "Camera", Factory::TOP ) );

  //Combo box for the cameras
  cameraToolBar->append ( new FoxTools::ToolBar::ComboBox ( "ComboBox", this, AnimateComponent::ID_COMBO_BOX ) );

  //Add buttons to camera toolbar
  cameraToolBar->append ( new Button ( "Set Position", "Set the selected frame to the current position", Icons::ICON_XYZ,     this, AnimateComponent::ID_SET_FRAME_POSITION ) );
  cameraToolBar->append ( new Button ( "Delete",       "Delete Selected Frame.",                         Icons::ICON_RED_X,   this, AnimateComponent::ID_DELETE_FRAME       ) );
  cameraToolBar->append ( new Button ( "Comments",     "View/Edit comments",                             Icons::ICON_WRITE,   this, AnimateComponent::ID_COMMENTS           ) );

  // Make a toolbar for the slider.
  ToolBar::ValidRefPtr sliderToolBar ( Factory::instance().create ( "Movie Progress", Factory::BOTTOM ) );
  sliderToolBar->layout ( FX::LAYOUT_FILL_X );

  FoxTools::ToolBar::Slider::ValidRefPtr slider ( new FoxTools::ToolBar::Slider ( "Slider", this, AnimateComponent::ID_ANIMATION_SLIDER ) );
  sliderToolBar->append ( slider.get() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Go to the next frame
//
///////////////////////////////////////////////////////////////////////////////

long AnimateComponent::onCommandNextFrame ( FX::FXObject *, FX::FXSelector, void * )
{
  //Go to the next frame in movie
  _current->nextFrame();

  //Get the new current frame
  const Frame& f = _current->currentFrame();

  // Go to this frame.
  this->_go ( f );

  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Gp to the previous frame
//
///////////////////////////////////////////////////////////////////////////////

long AnimateComponent::onCommandPreviousFrame ( FX::FXObject *, FX::FXSelector, void * )
{
  //Go to the previous frame in movie
  _current->previousFrame();

  //Get the new current frame
  const Frame& f = _current->currentFrame();

  // Go to this frame.
  this->_go ( f );

  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Go to the first frame
//
///////////////////////////////////////////////////////////////////////////////

long AnimateComponent::onCommandFirstFrame ( FX::FXObject *, FX::FXSelector, void * )
{
  //Go to the first frame in movie
  _current->firstFrame();

  //Get the new current frame
  const Frame& f = _current->currentFrame();

  // Go to this frame.
  this->_go ( f );

  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Go to the last frame
//
///////////////////////////////////////////////////////////////////////////////

long AnimateComponent::onCommandLastFrame ( FX::FXObject *, FX::FXSelector, void * )
{
  //Go to the last frame in movie
  _current->lastFrame();

  //Get the new current frame
  const Frame& f = _current->currentFrame();

  // Go to this frame.
  this->_go ( f );

  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the selected combo box
//
///////////////////////////////////////////////////////////////////////////////

long AnimateComponent::onCommandComboBox ( FX::FXObject *object, FX::FXSelector, void * )
{
  FX::FXComboBox *comboBox = SAFE_CAST_FOX ( FX::FXComboBox, object );
  if ( 0x0 == comboBox )
    return 0;

  //set the current frame to the one selected
  _current->setCurrentFrame( comboBox->getCurrentItem() );

  //Get the current frame
  const Frame& f = _current->currentFrame(  );

  // Go to this frame.
  this->_go ( f );

  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the selected combo box
//
///////////////////////////////////////////////////////////////////////////////

long AnimateComponent::onUpdateComboBox ( FX::FXObject *object, FX::FXSelector, void * )
{
  FX::FXComboBox *comboBox = SAFE_CAST_FOX ( FX::FXComboBox, object );
  if ( 0x0 == comboBox )
    return 0;

  if( this->_validMovieAndView() && !_current->empty() )
  {
    comboBox->enable();

    //Return if there are no items in the combo box
    if( comboBox->getNumItems() == 0 )
      return 0;

    //Get the current frame
    unsigned int current ( _current->getCurrentFrame() );

    //Set the combo box item to the current camera
    comboBox->setCurrentItem( current );

    Usul::Interfaces::IActiveView::ValidQueryPtr activeView ( _caller );
    Usul::Interfaces::ITrackball::ValidQueryPtr trackball ( activeView->getActiveView() );
    const Frame &frame ( _current->frame( _current->getCurrentFrame() ) );

    //If the trackball and frame are the same...
    if( frame.getCenter() == trackball->getCenter() && frame.getDistance() == trackball->getDistance() && frame.getRotation() == trackball->getRotation() )
    {
      FX::FXVec4f v ( 0.0, 0.0, 0.0, 1.0 );
      comboBox->setTextColor( v );
    }
    else
    {
      FX::FXVec4f v ( 1.0, 0.0, 0.0, 1.0 );
      comboBox->setTextColor( v );
    }
  }

  else
    comboBox->disable();

  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the selected combo box
//
///////////////////////////////////////////////////////////////////////////////

long AnimateComponent::onChangedComboBox ( FX::FXObject *object, FX::FXSelector, void * )
{
  FX::FXComboBox *comboBox = SAFE_CAST_FOX ( FX::FXComboBox, object );
  if ( 0x0 == comboBox )
    return 0;

  unsigned int index ( comboBox->getCurrentItem() );

  Frame& frame = _current->frame( index );

  frame.setName( ( char* ) comboBox->getText().text() );

  _current->modified( true );

  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the current frame to the current trackball data
//
///////////////////////////////////////////////////////////////////////////////

long AnimateComponent::onCommandSetFramePosition  ( FX::FXObject *, FX::FXSelector, void * )
{
  //Build a frame from the trackball data
  Frame f ( this->_currentFrame() );

  //Set the current frame to this data
  _current->currentFrame( f );

  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  View/Edit comments for selected camera
//
///////////////////////////////////////////////////////////////////////////////

long AnimateComponent::onCommandComments ( FX::FXObject *, FX::FXSelector, void * )
{
  //The dialog box
  std::auto_ptr< FX::FXDialogBox >  dialog ( new FX::FXDialogBox ( FoxTools::Functions::mainWindow(), "Comments", FX::DECOR_CLOSE|DECOR_TITLE|DECOR_BORDER,0,0,0,0, 10,10,10,10, 4,4 ) );

  //vertical frame for labels and buttons
  FX::FXVerticalFrame* theFrame ( new FX::FXVerticalFrame ( dialog.get(), LAYOUT_FILL_X|LAYOUT_FILL_Y ) );

  //Get the current frame
  Frame &f = _current->currentFrame( );

  //Icon to place before comments
  std::auto_ptr<FX::FXIcon> icon ( FoxTools::Icons::Factory::instance()->icon ( FoxTools::Icons::Factory::ICON_SMALL_DOT ) );

  //Add comments as labels
  for( Frame::iterator i = f.begin(); i != f.end(); ++i )
  {
    new FX::FXLabel( theFrame, i->c_str(), icon.get() );
  }

  //Textfield for adding comments
  FX::FXTextField *text ( new FX::FXTextField ( theFrame, 75, 0x0, 0, FRAME_SUNKEN|FRAME_THICK|LAYOUT_SIDE_TOP|LAYOUT_LEFT,0,0,0,0 ) );

  //Add and Close buttons
  FX::FXHorizontalFrame*  buttonFrame ( new FX::FXHorizontalFrame ( theFrame, LAYOUT_FILL_X|LAYOUT_FILL_Y ) );
  new FX::FXButton ( buttonFrame, "Add"  ,NULL, dialog.get(), FX::FXDialogBox::ID_ACCEPT,FX::LAYOUT_LEFT|BUTTON_INITIAL|BUTTON_DEFAULT|FRAME_RAISED|FRAME_THICK,0,0,0,0, 20,20);
  new FX::FXButton ( buttonFrame, "Close",NULL, dialog.get(), FX::FXDialogBox::ID_CANCEL,FX::LAYOUT_RIGHT|BUTTON_INITIAL|BUTTON_DEFAULT|FRAME_RAISED|FRAME_THICK,0,0,0,0, 20,20);

  dialog->create();

  //Show the comment
  if ( dialog->execute( FX::PLACEMENT_OWNER ) )
  {
    //Get the comment
    std::string comment ( ( char * ) text->getText().text() );

    //Add the comment if it is not empty
    if( !comment.empty() )
    {
      f.addComment( comment );
      _current->modified( true );
    }
  }

  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Function for toggling screen dumping
//
///////////////////////////////////////////////////////////////////////////////

long AnimateComponent::onCommandFrameDump ( FX::FXObject *, FX::FXSelector, void * )
{
  typedef Usul::Interfaces::IFrameDump IFrameDump;
  typedef Usul::Interfaces::IActiveView IActiveView;

  IActiveView::ValidQueryPtr view ( _caller );
  IFrameDump::ValidQueryPtr fd ( view->getActiveView() );

  // if we are currently dumping frames, turn it off
  if( fd->dumpFrames() )
    fd->dumpFrames ( false );

  //start to dump frames
  else
  {
    FrameDumpDialog dialog;
    IFrameDump *fd ( dialog.getFrameDumpProperties( _caller ) );

    if( fd )
      fd->dumpFrames( true );
  }

  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Function for updating screen dump button
//
///////////////////////////////////////////////////////////////////////////////

long AnimateComponent::onUpdateFrameDump ( FX::FXObject *object, FX::FXSelector, void * )
{
  typedef Usul::Interfaces::IFrameDump IFrameDump;
  typedef Usul::Interfaces::IActiveView IActiveView;

  IActiveView::QueryPtr view ( _caller );

  IFrameDump::QueryPtr fd ( view->getActiveView() );

  FoxTools::Functions::enable ( fd.valid(), object );
  bool result ( fd.valid() && fd->dumpFrames() );
  FoxTools::Functions::check( result, object );

  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Delete selected frame
//
///////////////////////////////////////////////////////////////////////////////

long AnimateComponent::onCommandDeleteFrame ( FX::FXObject *, FX::FXSelector, void * )
{
  ToolBar::ValidRefPtr camera ( FoxTools::ToolBar::Factory::instance().toolbar( "Camera" ) );

  FoxTools::Items::Command::RefPtr command ( camera->find ( "ComboBox" ) );

  if( command.valid() )
  {
    if( FoxTools::ToolBar::ComboBox *comboBox = dynamic_cast < FoxTools::ToolBar::ComboBox * > ( command.get() ) )
    {
      FX::FXComboBox* combo ( comboBox->comboBox() );

      // Clear the combo box
      if( combo )
      {
        //Delete the selected camera
        _current->removeFrame( combo->getCurrentItem() );
      }
    }
  }  

  //this->_buildButtons();

  //Get the new current frame
  const Frame& f = _current->currentFrame(  );

  // Go to this frame.
  this->_go ( f );

  _current->modified( true );

  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Enables if movie is not empty.
//
///////////////////////////////////////////////////////////////////////////////

long AnimateComponent::onUpdateNeedingFrame ( FX::FXObject *sender, FX::FXSelector, void * )
{
  //Enable or Disable
  FoxTools::Functions::enable( ( this->_validMovieAndView() && !_current->empty() ), sender );

  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Enables if movie is not at the first frame
//
///////////////////////////////////////////////////////////////////////////////

long AnimateComponent::onUpdateFirstFrame ( FX::FXObject *sender, FX::FXSelector, void * )
{
  //Is there an active view, a current movie and is the movie on the first frame?
  bool result ( this->_validMovieAndView() && !_current->empty() && ! ( _current->getCurrentFrame() == 0 ) );

  //Enable or Disable
  FoxTools::Functions::enable( result, sender );

  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Enables if movie is not at the last frame
//
///////////////////////////////////////////////////////////////////////////////

long AnimateComponent::onUpdateLastFrame ( FX::FXObject *sender, FX::FXSelector, void * )
{
  //Is there an active view, a current movie and is the movie on the last frame?
  bool result ( this->_validMovieAndView() && !_current->empty() && ! ( _current->getCurrentFrame() == _current->getNumberFrames() - 1  ) );

  //Enable or Disable
  FoxTools::Functions::enable( result, sender );

  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the stop button
//
///////////////////////////////////////////////////////////////////////////////

long AnimateComponent::onUpdateStop  ( FX::FXObject *object, FX::FXSelector, void * )
{
  // Enable or Disable
  FoxTools::Functions::enable( ( this->_validMovieAndView() && _current->canPlay() ), object );
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is there an active view and a valid movie
//
///////////////////////////////////////////////////////////////////////////////

bool AnimateComponent::_validMovieAndView( )
{
  Usul::Interfaces::IActiveView::QueryPtr activeView ( _caller );
  return ( activeView.valid() && 0x0 != activeView->getActiveView() && _current.valid() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Pause or unpause
//
///////////////////////////////////////////////////////////////////////////////

long AnimateComponent::onCommandPause ( FX::FXObject *, FX::FXSelector, void * )
{
  if( _current->paused() )
    _current->unpause();
  else
    _current->pause();

  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the pause button
//
///////////////////////////////////////////////////////////////////////////////

long AnimateComponent::onUpdatePause ( FX::FXObject *object, FX::FXSelector, void * )
{
  FoxTools::Functions::enable ( this->_validMovieAndView() && _current->canPlay(), object );
  bool result ( _current->paused() );
  FoxTools::Functions::check( result, object );

  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a frame to the movie with the group name
//
///////////////////////////////////////////////////////////////////////////////

void AnimateComponent::addFrame ( const std::string& groupName, const osg::Vec3& center, float distance, const osg::Quat& rotation )
{
  Movies::iterator iter ( _movies.find( groupName ) );

  if ( iter == _movies.end() )
  {
    Movie::ValidRefPtr movie ( new Movie );
    movie->fileName ( groupName );
    _movies.insert( Movies::value_type( movie->fileName(), movie ) );
    iter = _movies.find ( groupName );
  }

  Movie::ValidRefPtr movie ( iter->second );
  Frame frame ( center, distance, rotation );
  movie->appendFrame ( frame );
  //this->_buildButtons();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear movie with the group name
//
///////////////////////////////////////////////////////////////////////////////

void AnimateComponent::clear ( const std::string& groupName ) 
{
  Movies::iterator iter ( _movies.find( groupName ) );

  if( iter != _movies.end() )
  {
    iter->second->clear();

    //this->_buildButtons();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update current camera button.
//
///////////////////////////////////////////////////////////////////////////////

long AnimateComponent::onUpdateCurrentCamera ( FX::FXObject *object, FX::FXSelector, void * )
{
  Usul::Interfaces::IActiveView::ValidQueryPtr view ( _caller );
  FoxTools::Functions::enable ( ( view.valid() && view->getActiveView() ), object );
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Animate from current position to given position
//
///////////////////////////////////////////////////////////////////////////////

void AnimateComponent::animate ( const osg::Vec3& center, float distance, const osg::Quat& rotation )
{
  Frame frame ( center, distance, rotation );
  this->_go ( frame );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Animate from one position and orientation to another.
//
///////////////////////////////////////////////////////////////////////////////

void AnimateComponent::animate ( const osg::Vec3 &t1, const osg::Quat &r1, const osg::Vec3 &t2, const osg::Quat &r2, double duration )
{
  Frame current ( this->_currentFrame() );
  const float distance ( current.getDistance() );
  Frame from ( t1, distance, r1 );
  Frame to ( t2, distance, r2 );
  this->_go( from, to, duration );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Animate from one orientation to another.
//
///////////////////////////////////////////////////////////////////////////////

void AnimateComponent::animate ( const osg::Quat &r1, const osg::Quat &r2, double duration )
{
  Frame current ( this->_currentFrame() );
  const float distance (current.getDistance() );
  const osg::Vec3 center ( current.getCenter() );
  Frame from ( center, distance, r1 );
  Frame to ( center, distance, r2 );
  this->_go( from, to, duration );

}

 
///////////////////////////////////////////////////////////////////////////////
//
//  Usul::Interfaces::IDocumentCreate.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Documents::Document * AnimateComponent::createDocument ( Usul::Interfaces::IUnknown *caller )
{
  Movie::ValidRefPtr document ( this->_createMovie( "" ) );
  return document.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Do we handle this token
//
///////////////////////////////////////////////////////////////////////////////

bool AnimateComponent::doesHandle( const std::string& token ) const
{
  return "Camera Document" == token;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create the default GUI
//
///////////////////////////////////////////////////////////////////////////////

void AnimateComponent::createDefaultGUI ( Usul::Documents::Document *document, Usul::Interfaces::IUnknown* caller )
{
  if( Movie *movie = dynamic_cast < Movie* > ( document ) )
  {
    _movies.insert ( Movies::value_type ( document->fileName(), movie ) );
    this->_buildButtons();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
// Refresh the view
//
///////////////////////////////////////////////////////////////////////////////

void AnimateComponent::refreshView ( Usul::Documents::Document *document, Usul::Interfaces::IViewer  *viewer )
{
  this->_buildButtons();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Name of this document
//
///////////////////////////////////////////////////////////////////////////////

std::string AnimateComponent::documentTypeName()
{
  return "Camera Document";
}


///////////////////////////////////////////////////////////////////////////////
//
//  Hide/Show the animation path.
//
///////////////////////////////////////////////////////////////////////////////

long AnimateComponent::onCommandAnimationPath ( FX::FXObject *, FX::FXSelector, void * )
{
  Usul::Interfaces::IActiveView::ValidQueryPtr activeView ( _caller );
  Usul::Interfaces::IGroup::QueryPtr           group      ( activeView->getActiveView() );

  if( group.valid() )
  {
    if( group->hasGroup ( "Animation_Group" ) )
    {
        //Remove the group.
        group->removeGroup ( "Animation_Group" );
    }
    else
    {
      // Get the group.
      osg::ref_ptr< osg::Group > g ( group->getGroup ( "Animation_Group" ) );

      g->addChild( _current->buildAnimationPath() );
    }

    Usul::Interfaces::IDocument::QueryPtr document       ( Usul::Documents::Manager::instance().active() );
    Usul::Interfaces::ISendMessage::QueryPtr sendMessage ( document );
    if ( sendMessage.valid() )
      sendMessage->sendMessage( Usul::Interfaces::ISendMessage::ID_RENDER_SCENE );
  }

  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the animation path button.
//
///////////////////////////////////////////////////////////////////////////////

long AnimateComponent::onUpdateAnimationPath ( FX::FXObject *object, FX::FXSelector, void * )
{
  FoxTools::Functions::enable ( this->_validMovieAndView() && _current->canPlay() , object );

  Usul::Interfaces::IActiveView::ValidQueryPtr activeView ( _caller );
  Usul::Interfaces::IGroup::QueryPtr           group      ( activeView->getActiveView() );

  bool result ( group.valid() ? group->hasGroup ( "Animation_Group" ) : false );

  FoxTools::Functions::check ( result, object );

  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Change the current position in the movie using the slider.
//
///////////////////////////////////////////////////////////////////////////////

long AnimateComponent::onCommandAnimationSlider ( FX::FXObject *object, FX::FXSelector, void *data )
{
  FX::FXSlider *slider = SAFE_CAST_FOX( FX::FXSlider, object );
  if( 0x0 == slider )
    return 0;

  const int value = reinterpret_cast < int > ( data );

  Frame frame ( _current->lastU ( (double) value / 100 ) );

  typedef Usul::Interfaces::IActiveView        IActiveView;
  typedef Usul::Interfaces::ITrackball         ITrackball;
  typedef Usul::Interfaces::IRedraw            IRedraw;
  typedef Usul::Interfaces::IFlushEvents       IFlushEvents;

  // Get the needed interfaces.
  IActiveView::ValidQueryPtr   activeView ( _caller );
  ITrackball::ValidQueryPtr    trackball ( activeView->getActiveView() );
  IRedraw::ValidQueryPtr       redraw ( activeView->getActiveView() );

  trackball->setTrackball ( frame.getCenter(), frame.getDistance(), frame.getRotation(), true, true );

  redraw->redraw();

  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the animation slider button.
//
///////////////////////////////////////////////////////////////////////////////

long AnimateComponent::onUpdateAnimationSlider ( FX::FXObject *object, FX::FXSelector, void * )
{
  FX::FXSlider *slider = SAFE_CAST_FOX( FX::FXSlider, object );
  if( 0x0 == slider )
    return 0;

  FoxTools::Functions::enable ( this->_validMovieAndView() && _current->canPlay(), object );

  if( _current.valid() )
    slider->setValue ( _current->getPercentageDone() );

  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Toggle loop animation.
//
///////////////////////////////////////////////////////////////////////////////

long AnimateComponent::onCommandLoop ( FX::FXObject *, FX::FXSelector, void * )
{

  if( _current.valid() )
    _current->loop( !_current->loop() );

  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update loop check box.
//
///////////////////////////////////////////////////////////////////////////////

long AnimateComponent::onUpdateLoop ( FX::FXObject *object, FX::FXSelector, void * )
{
  FoxTools::Functions::enable ( this->_validMovieAndView(), object );
  FoxTools::Functions::check ( _current.valid() && _current->loop(), object );
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  The document has been modified.
//
///////////////////////////////////////////////////////////////////////////////

void AnimateComponent::subjectModified ( Usul::Interfaces::IUnknown *caller )
{
  this->_buildGUIForCurrentMovie();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the GUI for the movie
//
///////////////////////////////////////////////////////////////////////////////

void AnimateComponent::_buildGUIForCurrentMovie ( )
{
  // Rebuild the buttons
  this->_buildButtons();

  // Rebuild the slides.
  this->_buildSlides();

  Usul::Interfaces::IActiveView::ValidQueryPtr activeView ( _caller );
  Usul::Interfaces::IGroup::QueryPtr           group      ( activeView->getActiveView() );

  if( group.valid() )
  {
    if( group->hasGroup ( "Animation_Group" ) )
    {
      osg::ref_ptr< osg::Group > g ( group->getGroup ( "Animation_Group" ) );

      g->removeChild( 0, g->getNumChildren() );

      g->addChild( _current->buildAnimationPath() );

      Usul::Interfaces::IDocument::QueryPtr document       ( Usul::Documents::Manager::instance().active() );
      Usul::Interfaces::ISendMessage::QueryPtr sendMessage ( document );
      if ( sendMessage.valid() )
        sendMessage->sendMessage( Usul::Interfaces::ISendMessage::ID_RENDER_SCENE );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create a movie.
//
///////////////////////////////////////////////////////////////////////////////

Helios::Animate::Movie* AnimateComponent::_createMovie( const std::string& file )
{
  Movie::RefPtr movie ( new Movie );

  _current = movie;

  std::string filename ( file );

  if( file.empty() )
    filename = movie->fileName();

  _movies.insert ( Movies::value_type ( movie->fileName(), movie.get() ) );

  // Add this as an observer
  Usul::Interfaces::IModifiedSubject::QueryPtr subject ( movie );
  if ( subject.valid() )
    subject->addModifiedObserver( this );

  Usul::Documents::Manager::instance().add( movie.get() );

  return movie.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear the slides.
//
///////////////////////////////////////////////////////////////////////////////

void AnimateComponent::_clearSlides()
{
  if( _slideContents )
    delete _slideContents;

  _slideContents = 0x0;

  Usul::Interfaces::IFoxTabItem::QueryPtr tabItem ( _caller );

  if( tabItem.valid( ) )
    tabItem->deleteTab( "Slides" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the slides.
//
///////////////////////////////////////////////////////////////////////////////

void AnimateComponent::_buildSlides()
{
  this->_clearSlides();

  // Get current movie.
  Movie::RefPtr movie ( _current.get() );

  //Query for interfaces
  Usul::Interfaces::IFoxTabItem::QueryPtr tabItem    ( _caller );
  Usul::Interfaces::IFoxTabBook::QueryPtr foxTabBook ( _caller );
  Usul::Interfaces::IScreenCapture::QueryPtr screenCapture ( Usul::App::Controller::instance().activeView() );

  if ( movie->empty() || !tabItem.valid() || !foxTabBook.valid() || !screenCapture.valid() )
    return;

  tabItem->addTab ( "Slides" );
  FX::FXTabItem * slidesTab ( tabItem->tabItem( "Slides" ) );

  slidesTab->create();

  FX::FXTabBook *tabBook ( foxTabBook->tabBook() );

  _slideContents = new FX::FXVerticalFrame ( tabBook, LAYOUT_FILL_X|LAYOUT_FILL_Y );

  FX::FXScrollWindow *scrollArea ( new FX::FXScrollWindow( _slideContents, FX::LAYOUT_FILL_X | FX::LAYOUT_FILL_Y ) );

  FX::FXVerticalFrame *frame ( new FX::FXVerticalFrame ( scrollArea, LAYOUT_FILL_X|LAYOUT_FILL_Y ) );

  // Loop through the list.
  unsigned int count ( 0 );
  for ( Movie::iterator i = movie->begin(); i != movie->end(); ++i )
  {
    // Make the text.
    std::ostringstream name, description;

    // Get the name from the frame if is has one, if not build it from frame count
    if ( i->getName().empty() )
      name << "Camera " << count;
    else
      name << i->getName();

    // Add frame to the slides tab item.
    if ( _slideContents )
    {
      new FX::FXLabel( frame, name.str().c_str() );
      
      const unsigned int width ( 200 );
      const unsigned int height ( 200 );
      osg::ref_ptr< osg::Image > image ( screenCapture->screenCapture( i->getCenter(), i->getDistance(), i->getRotation(), height, width ) );

      FX::FXIcon *foxImage ( new FX::FXIcon( FoxTools::Functions::application(), 0x0, FXRGB ( 0, 0, 0 ), FX::IMAGE_OWNED, width, height ) );

      // Convert Osg image to fox image.  TODO put this in a library or plugin.
      for( int y = 0; y < image->t(); ++y )
      {
        for( int x = 0; x < image->s(); ++x )
        {
          unsigned char* source = image->data(x,y,0);

          FXColor color ( FXRGB ( *source, *(source+1), *(source+2) ) );
          foxImage->setPixel ( x, image->t() - y - 1, color );
        }
      }

      new FX::FXButton ( frame, "", foxImage, 0x0, 0, FX::LAYOUT_FILL_Y | FX::LAYOUT_FILL_X );
      
    }    

    // Append the help string.
    description << "Switch to camera " << count;

    // Don't increment until now.
    ++count;
  }

  _slideContents->create();

  // Need this to have the slides tab to always show up after the first frame is added.
  tabBook->layout();
}
