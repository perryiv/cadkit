
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant and Perry L. Miller IV.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The application class.
//
///////////////////////////////////////////////////////////////////////////////

#include "CadViewer/Application.h"
#include "CadViewer/Constants.h"
#include "CadViewer/ErrorChecker.h"

#include "VRV/Common/Libraries.h"

#include "Usul/Bits/Bits.h"
#include "Usul/Components/Manager.h"
#include "Usul/Threads/ThreadId.h"
#include "Usul/Functors/General/If.h"
#include "Usul/Predicates/UnaryPair.h"
#include "Usul/Trace/Trace.h"
#include "Usul/Interfaces/IButtonID.h"

#include "VRV/Common/Buttons.h"

#include "osg/MatrixTransform"
#include "osg/Matrix"
#include "osg/Projection"

#include "OsgTools/Text.h"
#include "OsgTools/Group.h"
#include "OsgTools/Font.h"
#include "OsgTools/Visitor.h"

namespace CV
{
  const unsigned long COMMAND_SELECT           = VRV::BUTTON_TRIGGER;
  const unsigned long COMMAND_HIDE_SELECTED    = VRV::BUTTON_YELLOW;
  const unsigned long COMMAND_UNSELECT_VISIBLE = VRV::BUTTON_GREEN;
  const unsigned long COMMAND_SHOW_ALL         = VRV::BUTTON_RED;  

  const unsigned long TOOL_SCALE      = VRV::BUTTON_GREEN;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Enumerations.
//
///////////////////////////////////////////////////////////////////////////////

namespace CV
{
  // Various callback ids.
  enum
  {
    INTERSECT_SELECT,
    INTERSECT_SEEK,
    INTERSECT_HIDE,
  };
};


///////////////////////////////////////////////////////////////////////////////
//
//  These bitmask values do not work well with the template functions in 
//  Bitmask.h if they are declared as enumerations.
//
///////////////////////////////////////////////////////////////////////////////

namespace CV
{
  namespace Detail
  {
    const unsigned int _TEXT_IS_INITIALIZED       = 0x00000001;
  };
};


using namespace CV;


///////////////////////////////////////////////////////////////////////////////
//
//  Static variables.
//
///////////////////////////////////////////////////////////////////////////////

unsigned long Application::_appThread  = 0;
unsigned long Application::_mainThread = 0;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Application::Application ( ) :
  BaseClass       ( ),
  _gridBranch     ( new osg::MatrixTransform ),
  _sceneTool      ( 0x0 ),
  _pickText       ( new OsgTools::Text ),
  _navText        ( new OsgTools::Text ),
  _frameText      ( new OsgTools::Text ),
  _msgText        ( new OsgTools::Text ),
  _flags          ( 0 ),
  _textures       ( true )
{
  ErrorChecker ( 1067097070u, 0 == _appThread );
  ErrorChecker ( 2970484549u, 0 == _mainThread );
  ErrorChecker ( 1067094628u, _gridBranch.valid() );
  ErrorChecker ( 1071551353u, 0x0 != _pickText.get() );
  ErrorChecker ( 1071551354u, 0x0 != _navText.get() );
  ErrorChecker ( 1071551355u, 0x0 != _frameText.get() );
  ErrorChecker ( 1071551356u, 0x0 != _msgText.get() );

  // Initialize the main thread.
  _mainThread = Usul::Threads::currentThreadId();

  // Hook up the branches.
  //_navBranch->addChild ( _gridBranch.get()   );

  // Name the branches.
  _gridBranch->setName   ( "_gridBranch"   );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Application::~Application()
{
  _gridFunctors.clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize this instance.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_init()
{
  ErrorChecker ( 1067096939, 0 == _appThread );
  ErrorChecker ( 1074061336, !isMainThread() );

  // Initialize the application thread.
  _appThread = Usul::Threads::currentThreadId();

  // Call the base class's function first.
  BaseClass::_init();
  
  // Set the global GL_NORMALIZE flag.
  //this->normalize ( this->preferences()->normalizeVertexNormalsGlobal() );

  // Note: we cannot initialize the text yet because the viewport has not been set.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize the text.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_initText()
{
#ifdef _MSC_VER
  // Removing anything we may have already.
  this->projectionGroupRemove ( "VRV_TEXT" );

  ErrorChecker ( 1071444744, isAppThread(), CV::NOT_APP_THREAD );
  ErrorChecker ( 1071444746, 0x0 != _pickText.get() );
  ErrorChecker ( 1071444747, 0x0 != _navText.get() );
  ErrorChecker ( 1071444748, 0x0 != _frameText.get() );
  ErrorChecker ( 1071444749, 0x0 != _msgText.get() );
  ErrorChecker ( 1071556909, false == Usul::Bits::has ( _flags, Detail::_TEXT_IS_INITIALIZED ) );

  if( !this->_isHeadNode() )
    return;

  osg::ref_ptr < osg::Group > group ( this->projectionGroupGet ( "VRV_TEXT" ) );

  // Create a matrix-transform relative to the global coordinate system.
  osg::ref_ptr<osg::MatrixTransform> mt ( new osg::MatrixTransform );
  ErrorChecker ( 1071452071, mt.valid() );

  mt->setReferenceFrame ( osg::Transform::ABSOLUTE_RF );
  mt->setMatrix ( osg::Matrix::identity() );
  mt->setName ( "TextBranch" );

  // Make the text branch an orthographic projection.
  osg::ref_ptr < osg::Viewport > vp ( this->viewport() );
  float x ( vp->x()      );
  float y ( vp->y()      );
  float w ( vp->width()  );
  float h ( vp->height() );

  // Set the text font.
  std::string f ( OsgTools::Font::fontfile ( "courbd" ) ); // Courier New Bold
  _pickText->font  ( f );
  _navText->font   ( f );
  _frameText->font ( f );
  _msgText->font   ( f );

  // Set the text height.
  float th ( 12 );
  _pickText->height  ( th );
  _navText->height   ( th );
  _frameText->height ( th );
  _msgText->height   ( th );

  // Set the text positions.
  float tx ( 5 ), ty ( 5 );
  _msgText->position   ( osg::Vec3 ( tx, ty, 0 ) ); ty = ty + th + 2;
  _pickText->position  ( osg::Vec3 ( tx, ty, 0 ) ); ty =  h - th - 2;
  _frameText->position ( osg::Vec3 ( tx, ty, 0 ) ); tx =  w - 220;
  _navText->position   ( osg::Vec3 ( tx, ty, 0 ) );

  // Hook up the scene.
  mt->addChild ( (*_pickText) () );
  mt->addChild ( (*_navText)  () );
  mt->addChild ( (*_frameText)() );
  mt->addChild ( (*_msgText)  () );
  //_textBranch->addChild ( mt.get() );
  group->addChild ( mt.get() );

#if 0 // Use for debugging their placement.
  _navText->text   ( "_navText"   );
  _msgText->text   ( "_msgText"   );
  _frameText->text ( "_frameText" );
  _pickText->text  ( "_pickText"  );
#endif
#endif
  // Set the flag that says the text is initialized.
  _flags = Usul::Bits::add ( _flags, Detail::_TEXT_IS_INITIALIZED );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize the grid to be under the given node.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_initGrid ( osg::Node *node )
{
  ErrorChecker ( 1067093690, isAppThread(), CV::NOT_APP_THREAD );
  ErrorChecker ( 1293823849, 0x0 != node );

  // Get the bounding sphere of the node.
  const osg::BoundingSphere &bs = node->getBound();

  // Handle zero-sized bounding spheres.
  float r = ( bs.radius() <= 1e-6 ) ? 1 : bs.radius();

  // Clean up any old grids.
  _gridFunctors.clear();

  // Set the properties.
  for( int i = 0; i < this->preferences()->numGrids(); ++i )
  {
    OsgTools::Grid grid;
    grid.numBlocks ( this->preferences()->numGridBlocks( i ) );
    grid.size ( r * this->preferences()->gridScale( i ) );
    grid.color ( this->preferences()->gridColor( i ) );
    grid.fillColor ( this->preferences()->gridFillColor( i ) );
    Usul::Math::Matrix44f o;
    o.makeRotation ( this->preferences()->gridRotationAngleRad( static_cast < int > ( i ) ), this->preferences()->gridRotationVector( i ) );
    grid.orientation ( o );

    // Move the center so that it is below the bounding sphere of the node.
    osg::Vec3 c ( bs.center() );
    if(this->preferences()->offsetGrid( static_cast < int > ( i ) )) 
      c[1] = -r;

    grid.center ( Usul::Math::Vec3f ( c[0], c[1], c[2] ) );
    _gridFunctors.push_back(grid);
  }

  _rebuildGrid();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Rebuild the grid in case the _gridFunctor's state has been updated
//
///////////////////////////////////////////////////////////////////////////////

void Application::_rebuildGrid()
{
  // Remove the old grid and add the new one.
  OsgTools::Group::removeAllChildren ( _gridBranch.get() );
  for( unsigned int i = 0; i < _gridFunctors.size(); ++i )
  {
    _gridBranch->addChild ( _gridFunctors[i]() );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called before the frame is drawn.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_latePreFrame()
{
  USUL_TRACE_SCOPE;

  ErrorChecker ( 1067093580, isAppThread(), CV::NOT_APP_THREAD );
  
  // Call the base class's function.
  BaseClass::_latePreFrame();

  // Update the frame-rate's text.
  this->_updateFrameRateDisplay();

  // Update the analog-input text.
  this->_updateAnalogText();

  // Use the scene-tool if we are supposed to.
  this->_useSceneTool();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called after the frame is drawn.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_postFrame()
{
  USUL_TRACE_SCOPE;

  ErrorChecker ( 1071439494, isAppThread(), CV::NOT_APP_THREAD );

  // Call the base class's function.
  BaseClass::_postFrame();

  // Initialize the text if we need to. We cannot call this sooner because 
  // contextInit() has to be called first.
  if ( false == Usul::Bits::has ( _flags, Detail::_TEXT_IS_INITIALIZED ) )
  {
    osg::ref_ptr < osg::FrameStamp > framestamp ( this->frameStamp() );
    if ( 0x0 != framestamp.get() && framestamp->getFrameNumber() > 10 )
      this->_initText();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Select the scene.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_select()
{
  ErrorChecker ( 1070920309, isAppThread(), CV::NOT_APP_THREAD );
#if 0
  // See if we have a selector...
  typedef CV::Pick::Select Select;
  Select::RefPtr selector ( dynamic_cast < Select * > ( _intersector.get() ) );
  if ( selector.valid() )
  {
    // Tell the selector to permanently "select" the intersected node.
    selector->select();

    // Report node name to user.
    const osg::Node *node = selector->node();
    if ( node )
      this->_updateStatusBar ( "Selected node: " + node->getName() );

    // Reset the selector.
    selector->reset();
  }
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Unselect the scene.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_unselect()
{
  ErrorChecker ( 1070922810, isAppThread(), CV::NOT_APP_THREAD );
#if 0
  // See if we have a selector...
  typedef CV::Pick::Select Select;
  Select::RefPtr selector ( dynamic_cast < Select * > ( _intersector.get() ) );
  if ( selector.valid() )
  {
    // Tell the selector to "unselect" the intersected node.
    selector->unselect();
  }
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is this the application thread? The application thread is the one that 
//  calls init(), preFrame() and postFrame().
//
///////////////////////////////////////////////////////////////////////////////

bool Application::isAppThread()
{
  ErrorChecker ( 2104785486u, 0 != _mainThread );
  return ( Usul::Threads::currentThreadId() == _appThread );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is this the main thread? The main thread is the original thread of 
//  execution that calls main().
//
///////////////////////////////////////////////////////////////////////////////

bool Application::isMainThread()
{
  ErrorChecker ( 1078956732u, 0 != _mainThread );
  return ( Usul::Threads::currentThreadId() == _mainThread );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Navigate if we are supposed to.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_navigate()
{
  USUL_TRACE_SCOPE;

  ErrorChecker ( 1068000936, isAppThread(), CV::NOT_APP_THREAD );

  // If we have a valid tool then we don't navigate.
  if ( _sceneTool.valid() )
    return;

  // Call the base class
  BaseClass::_navigate ();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Use the scene-tool if we are supposed to.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_useSceneTool()
{
  ErrorChecker ( 2860383896u, isAppThread(), CV::NOT_APP_THREAD );

  // If the menu is showing then we don't use the tool.
  if ( this->menu()->menu()->expanded() )
    return;

  // Tell it to execute.
  if ( _sceneTool.valid() )
    (*_sceneTool)();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the scene holding the text.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_update ( OsgTools::Text &t, const std::string &s )
{
  // If this is the application thread...
  if ( isAppThread() )
  {
    // Change the text if it is different.
    if ( false == t.equal ( s ) )
      t.text ( s );
  }

  // Dump text to stdout if this is the message-text or the pick-text.
  if ( &t == _pickText.get() || &t == _msgText.get() )
    std::cout << s << std::endl;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the frame-rate text.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_updateFrameRateDisplay()
{
  ErrorChecker ( 1071560060, isAppThread(), CV::NOT_APP_THREAD );

  // Make sure the frame-time is greater than zero.
  double ft ( this->frameTime() );
  if ( ft > 0 )
  {
    // Compose the string. Note: std::setprecision() doesn't 
    // work with std::ostringstream.
    const size_t size ( 128 );
    static char buffer[size];
    ::sprintf ( buffer, "Frame rate: %9.6f Hz", 1.0 / ft );

    // Set the text.
    this->_update ( *_frameText, std::string ( buffer ) );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the analog-input text.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_updateAnalogText()
{
  ErrorChecker ( 1071563205, isAppThread(), CV::NOT_APP_THREAD );

#if 0
  // Compose the string. Note: std::setprecision() doesn't 
  // work with std::ostringstream.
  const size_t size ( 128 );
  static char buffer[size];
  float h ( this->joystickHorizontal() );
  float v ( this->joystickVertical() );
  ::sprintf ( buffer, "Joystick: %9.6f %9.6f", h, v );

  // Set the text.
  this->_update ( *_navText, std::string ( buffer ) );
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Reference.
//
///////////////////////////////////////////////////////////////////////////////

void Application::ref()
{
  BaseClass::ref();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Unreference.
//
///////////////////////////////////////////////////////////////////////////////

void Application::unref ( bool allowDeletion )
{
  BaseClass::unref( allowDeletion );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *Application::queryInterface ( unsigned long iid )
{
  return BaseClass::queryInterface ( iid );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Returns the number of selected
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Application::_numSelected ()
{
  ErrorChecker ( 1084582713u, isAppThread(), CV::NOT_APP_THREAD );
#if 0
  CV::Functors::Tool::Transforms t;
  this->_selected( t );
  return t.size();
#else
  return 0;
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Fill the given vector with the selected matrix-transforms.
//
///////////////////////////////////////////////////////////////////////////////
#if 0
void Application::_selected ( CV::Functors::Tool::Transforms &vt )
{
  ErrorChecker ( 1227492927u, isAppThread(), CV::NOT_APP_THREAD );

  // For readability.
  typedef CV::Functors::Tool::TransformPtr TransformPtr;
  typedef osg::MatrixTransform MT;
  typedef CV::OSG::Predicates::IsSelected IsSelected;
  typedef CV::OSG::Predicates::IsVisible IsVisible;
  typedef Usul::Predicates::UnaryPair<IsVisible,std::logical_and<bool>,IsSelected> IsWanted;
  typedef CV::OSG::Functors::Append<MT> Append;
  typedef Usul::Functors::General::IfThen<IsSelected,Append> IfThen;
  //  typedef Usul::Functors::General::IfThen<IsWanted,Append> IfThen;
  typedef OsgTools::Visitor<MT,IfThen> Visitor;

  if( false == _iVisibility.valid() || false == _iSelection.valid () )
    return;

  // Compose the functors. For some reason I need to declare all the functors 
  // as variables, rather than pass, for example, IsSelected().
  IsSelected isSelected ( _iSelection );
  IsVisible isVisible ( _iVisibility );

  // TODO: correct UnaryPair error, use result in IfThen
  //std::logical_and<bool> operation;
  //Usul::Predicates::UnaryPair<IsVisible,std::logical_and<bool>,IsSelected> isWanted(isVisible,
  //                                                                                  operation,
  //                                                                                  isSelected);
  //IfThen ifThen ( isWanted, Append() );
  IfThen ifThen( isSelected , Append() );

  // Make the visitor.
  Visitor::Ptr visitor ( new Visitor ( ifThen ) );

  // Traverse the models-scene and append selected nodes.
  this->models()->accept ( *visitor );

  // Set the given vector.
  Append &append = visitor->op().then();
  vt.resize ( append.size() );
  std::copy ( append.begin(), append.end(), vt.begin() );
}
#endif

///////////////////////////////////////////////////////////////////////////////
//
//  Update the scene-tool, if any.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_updateSceneTool()
{
  ErrorChecker ( 2346088799u, isAppThread(), CV::NOT_APP_THREAD );
#if 0
  // If we have a scene functor...
  if ( _sceneTool.valid() )
  {
    // See if it is really a tool...
    {
      typedef CV::Functors::Tool Tool;
      Tool::RefPtr tool ( dynamic_cast < Tool * > ( _sceneTool.get() ) );
      if ( tool.valid() )
      {
        // Update the tool's list of selected nodes.
        this->_selected ( tool->transforms() );
        return;
      }
    }

    // It could be a pair...
    {
      typedef CV::Functors::ToolPair Tool;
      Tool::RefPtr tool ( dynamic_cast < Tool * > ( _sceneTool.get() ) );
      if ( tool.valid() )
      {
        // Update the tool's list of selected nodes.
        Tool::Transforms tr;
        this->_selected ( tr );
        tool->transforms ( tr );
      }
    }
  }
#endif
}

