
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant and Perry L. Miller IV.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The application callbacks.
//
///////////////////////////////////////////////////////////////////////////////

#include "Precompiled.h"
#include "Application.h"
#include "Constants.h"
#include "ErrorChecker.h"
#include "NavFactory.h"
#include "NavCallback.h"
#include "ToolFactory.h"
#include "ToolCallback.h"
#include "SceneFunctors.h"
#include "ScenePredicates.h"

#include "VRV/Functors/Matrix/IdentityMatrix.h"
#include "VRV/Functors/Navigate/Translate.h"
#include "VRV/Functors/Navigate/Rotate.h"
#include "VRV/Functors/Navigate/Direction.h"
#include "VRV/Functors/Wand/WandRotation.h"
#include "VRV/Functors/Wand/WandPosition.h"
#include "VRV/Functors/Input/AnalogInput.h"
#include "VRV/Functors/Input/JoystickHorizontal.h"
#include "VRV/Functors/Input/JoystickVertical.h"
#include "CadViewer/Functors/ScaleTool.h"
#include "CadViewer/Functors/MoveTool.h"
#include "CadViewer/Functors/ToolPair.h"

#include "CadViewer/Pick/Select.h"
#include "CadViewer/Pick/Seek.h"

#include "vrj/Kernel/Kernel.h"

#include "osg/Material"
#include "osg/MatrixTransform"
#include "osg/Matrix"
#include "osg/LineSegment"
#include "osgUtil/IntersectVisitor"

#include "OsgTools/Group.h"
#include "OsgTools/State/StateSet.h"
#include "OsgTools/Visitor.h"
#include "OsgTools/Axes.h"

#include "Usul/Functors/General/Pair.h"
#include "Usul/Functors/General/If.h"
#include "Usul/Math/Constants.h"

using namespace CV;


///////////////////////////////////////////////////////////////////////////////
//
//  Set the trim. This assumes the user is not tilting the joystick one way 
//  or the other. It records the value at the neutral position. If the value 
//  is 0.5 (like it should be) then the "trim" will be zero.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_setAnalogTrim ( MenuKit::Message m, MenuKit::Item *item )
{
  ErrorChecker ( 1071179242u, isAppThread(), CV::NOT_APP_THREAD );

  switch ( m )
  {
    case MenuKit::MESSAGE_SELECTED:
      float x ( 0.5f - this->joystick()->horizontal() );
      float y ( 0.5f - this->joystick()->vertical() );
      this->analogTrim ( x, y );
      break;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Quit the program.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_quitCallback ( MenuKit::Message m, MenuKit::Item *item )
{
  ErrorChecker ( 1067094376u, isAppThread() );

  switch ( m )
  {
    case MenuKit::MESSAGE_SELECTED:
      vrj::Kernel::instance()->stop();
      break;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Unselect all visible geometry.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_unselectVisible ( MenuKit::Message m, MenuKit::Item *item )
{
  ErrorChecker ( 1071520141, isAppThread(), CV::NOT_APP_THREAD );

  if( MenuKit::MESSAGE_UPDATE == m )
    {
      item->enabled( 0 != this->_numSelected() );
      return;
    }

  // Only selected messages.
  if ( MenuKit::MESSAGE_SELECTED != m )
    return;

  // For readability.
  typedef CV::OSG::Functors::PopMaterial PopMaterial;
  typedef CV::OSG::Functors::Unselect Unselect;
  typedef Usul::Functors::General::Pair<PopMaterial,Unselect> Restore;
  typedef CV::OSG::Predicates::IsVisible IsVisible;
  typedef Usul::Functors::General::IfThen<IsVisible,Restore> IfThen;
  typedef OsgTools::Visitor<osg::MatrixTransform,IfThen> Visitor;

  // update the tool because different things are selected
  this->_updateSceneTool();

  // Return if the pointer isn't valid.
  if ( false == _iVisibility.valid() || false == _iMaterialStack.valid() || false == _iSelection.valid() )
    return;

  // Compose the functors. For some reason I need to declare all the functors 
  // as variables, rather than pass, for example, IsSelected().
  PopMaterial pop ( _iMaterialStack );
  Unselect unselect ( _iSelection );
  Restore restore ( pop, unselect );
  IsVisible isVisible ( _iVisibility );

  IfThen ifThen ( isVisible, restore );

  // Make the visitor.
  Visitor::Ptr visitor ( new Visitor ( ifThen ) );

  // Traverse the models-scene and unselect all the nodes.
  this->models()->accept ( *visitor );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Hide all selected geometry.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_hideSelected ( MenuKit::Message m, MenuKit::Item *item )
{
  ErrorChecker ( 1071564756, isAppThread(), CV::NOT_APP_THREAD );

  // Only selected messages.
  if ( MenuKit::MESSAGE_SELECTED != m )
    return;

  // For readability.
  typedef CV::OSG::Predicates::IsSelected IsSelected;
  typedef CV::OSG::Functors::Hide Hide;
  typedef Usul::Functors::General::IfThen<IsSelected,Hide> IfThen;
  typedef OsgTools::Visitor<osg::MatrixTransform,IfThen> Visitor;

  if( false == _iVisibility.valid() || false == _iSelection.valid() )
    return;

  // Compose the functors. For some reason I need to declare all the functors 
  // as variables, rather than pass, for example, IsSelected().
  IsSelected isSelected ( _iSelection );
  Hide hide ( _iVisibility );
  IfThen ifThen ( isSelected, hide );

  // Make the visitor.
  Visitor::Ptr visitor ( new Visitor ( ifThen ) );

  // Traverse the models-scene and hide all selected nodes.
  this->models()->accept ( *visitor );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Show all the geometry.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_showAll ( MenuKit::Message m, MenuKit::Item *item )
{
  ErrorChecker ( 1071573042, isAppThread(), CV::NOT_APP_THREAD );

  // Only selected messages.
  if ( MenuKit::MESSAGE_SELECTED != m )
    return;

  // For readability.
  typedef CV::OSG::Functors::Show Show;
  typedef OsgTools::Visitor<osg::MatrixTransform,Show> Visitor;

  if ( false == _iVisibility.valid() )
    return;

  // Make the visitor.
  Visitor::Ptr visitor ( new Visitor ( Show ( _iVisibility ) ) );

  // Traverse the models-scene and hide all selected nodes.
  this->models()->accept ( *visitor );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Export the selected geometry.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_exportSelected ( MenuKit::Message m, MenuKit::Item *item )
{
  ErrorChecker ( 4127973658u, isAppThread(), CV::NOT_APP_THREAD );

  switch ( m )
  {
    case MenuKit::MESSAGE_SELECTED:
      break; // TODO
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Export the current world's geometry.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_exportWorld ( MenuKit::Message m, MenuKit::Item *item )
{
  ErrorChecker ( 3764107981u, isAppThread(), CV::NOT_APP_THREAD );

  switch ( m )
  {
    case MenuKit::MESSAGE_SELECTED:
    {
      static unsigned int count ( 0 );
      std::string number ( this->_counter ( ++count ) );
      std::string filename ( "cv_world_" + number + ".osg" );
      this->_writeScene ( filename, this->models() );
      break;
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Export the current world's geometry.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_exportWorldBinary ( MenuKit::Message m, MenuKit::Item *item )
{
  ErrorChecker ( 3764107981u, isAppThread(), CV::NOT_APP_THREAD );

  switch ( m )
  {
    case MenuKit::MESSAGE_SELECTED:
    {
      static unsigned int count ( 0 );
      std::string number ( this->_counter ( ++count ) );
      std::string filename ( "cv_world_" + number + ".ive" );
      this->_writeScene ( filename, this->models() );
      break;
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Export the current world's entire scene geometry.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_exportScene ( MenuKit::Message m, MenuKit::Item *item )
{
  ErrorChecker ( 2629806851u, isAppThread(), CV::NOT_APP_THREAD );

  switch ( m )
  {
    case MenuKit::MESSAGE_SELECTED:
    {
      static unsigned int count ( 0 );
      std::string number ( this->_counter ( ++count ) );
      std::string filename ( "cv_scene_" + number + ".osg" );
      this->_writeScene ( filename, this->_sceneRoot() );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Export the current world's entire scene geometry as binary osg.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_exportSceneBinary ( MenuKit::Message m, MenuKit::Item *item )
{
  switch ( m )
  {
    case MenuKit::MESSAGE_SELECTED:
    {
      static unsigned int count ( 0 );
      std::string number ( this->_counter ( ++count ) );
      std::string filename ( "cv_scene_" + number + ".ive" );
      this->_writeScene ( filename, this->_sceneRoot() );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Normalize the selected geometry.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_normSelected ( MenuKit::Message m, MenuKit::Item *item )
{
  ErrorChecker ( 1176061494u, isAppThread(), CV::NOT_APP_THREAD );


  switch ( m )
  {
    case MenuKit::MESSAGE_SELECTED:
      break ; // TODO
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Normalize the scene.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_normScene ( MenuKit::Message m, MenuKit::Item *item )
{
  ErrorChecker ( 1084166743, isAppThread(), CV::NOT_APP_THREAD );

  // Process the message.
  switch ( m )
  {
    case MenuKit::MESSAGE_UPDATE:
      item->checked ( this->normalize() );
      break;

    case MenuKit::MESSAGE_SELECTED:
      this->normalize ( !this->normalize() );
      break;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make horizontal joystick input translate in the wand's x-direction.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_hTransWandPosX ( MenuKit::Message m, MenuKit::Item *item )
{
  ErrorChecker ( 1083968782u, isAppThread(), CV::NOT_APP_THREAD );

  // To shorten the lines.
  typedef VRV::Functors::JoystickHorizontal Analog;
  typedef VRV::Functors::WandRotation Mat;
  typedef VRV::Functors::Translate Nav;

  // Call the common function.
  const unsigned int id ( 1084141815u );
  float speed ( this->preferences()->translationSpeed() );
  VRV::Functors::Direction::Vector dir ( 1, 0, 0 );

  FunctorPtr functor ( this->_firstNavigator () );
  CV::NavCB<Analog,Mat,Nav>::execute ( id, m, item, functor, speed, dir, this );
  this->_firstNavigator ( functor.get () );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make horizontal joystick input translate in the global x-direction.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_hTransGlobalPosX ( MenuKit::Message m, MenuKit::Item *item )
{
  ErrorChecker ( 2438291348u, isAppThread(), CV::NOT_APP_THREAD );

  // To shorten the lines.
  typedef VRV::Functors::JoystickHorizontal Analog;
  typedef VRV::Functors::Matrix::IdentityMatrix Mat;
  typedef VRV::Functors::Translate Nav;

  // Call the common function.
  const unsigned int id ( 3196018430u );
  float speed ( this->preferences()->translationSpeed() );
  VRV::Functors::Direction::Vector dir ( 1, 0, 0 );

  FunctorPtr functor ( this->_firstNavigator () );
  CV::NavCB<Analog,Mat,Nav>::execute ( id, m, item, functor, speed, dir, this );
  this->_firstNavigator ( functor.get () );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make horizontal joystick input translate along the wand's negative x-axis.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_hTransWandNegX ( MenuKit::Message m, MenuKit::Item *item )
{
  ErrorChecker ( 1084116071u, isAppThread(), CV::NOT_APP_THREAD );

  // To shorten the lines.
  typedef VRV::Functors::JoystickHorizontal Analog;
  typedef VRV::Functors::WandRotation Mat;
  typedef VRV::Functors::Translate Nav;

  // Call the common function.
  const unsigned int id ( 2934209123u );
  float speed ( this->preferences()->translationSpeed() );
  VRV::Functors::Direction::Vector dir ( -1, 0, 0 );

  FunctorPtr functor ( this->_firstNavigator () );
  CV::NavCB<Analog,Mat,Nav>::execute ( id, m, item, functor, speed, dir, this );
  this->_firstNavigator ( functor.get () );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make horizontal joystick input translate along the global negative x-axis.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_hTransGlobalNegX ( MenuKit::Message m, MenuKit::Item *item )
{
  ErrorChecker ( 4137765741u, isAppThread(), CV::NOT_APP_THREAD );

  // To shorten the lines.
  typedef VRV::Functors::JoystickHorizontal Analog;
  typedef VRV::Functors::Matrix::IdentityMatrix Mat;
  typedef VRV::Functors::Translate Nav;

  // Call the common function.
  const unsigned int id ( 3848661010u );
  float speed ( this->preferences()->translationSpeed() );
  VRV::Functors::Direction::Vector dir ( -1, 0, 0 );

  FunctorPtr functor ( this->_firstNavigator () );
  CV::NavCB<Analog,Mat,Nav>::execute ( id, m, item, functor, speed, dir, this );
  this->_firstNavigator ( functor.get () );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make vertical joystick input translate in the wand's y-direction.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_vTransWandPosY ( MenuKit::Message m, MenuKit::Item *item )
{
  ErrorChecker ( 1084444802u, isAppThread(), CV::NOT_APP_THREAD );

  // To shorten the lines.
  typedef VRV::Functors::JoystickVertical Analog;
  typedef VRV::Functors::WandRotation Mat;
  typedef VRV::Functors::Translate Nav;

  // Call the common function.
  const unsigned int id ( 1084444803u );
  float speed ( this->preferences()->translationSpeed() );
  VRV::Functors::Direction::Vector dir ( 0, 1, 0 );

  FunctorPtr functor ( this->_secondNavigator () );
  CV::NavCB<Analog,Mat,Nav>::execute ( id, m, item, functor, speed, dir, this );
  this->_secondNavigator ( functor.get () );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make vertical joystick input translate in the wand's z-direction.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_vTransWandPosZ ( MenuKit::Message m, MenuKit::Item *item )
{
  ErrorChecker ( 1083973896u, isAppThread(), CV::NOT_APP_THREAD );

  // To shorten the lines.
  typedef VRV::Functors::JoystickVertical Analog;
  typedef VRV::Functors::WandRotation Mat;
  typedef VRV::Functors::Translate Nav;

  // Call the common function.
  const unsigned int id ( 1084143905u );
  float speed ( this->preferences()->translationSpeed() );
  VRV::Functors::Direction::Vector dir ( 0, 0, -1 );

  FunctorPtr functor ( this->_secondNavigator () );
  CV::NavCB<Analog,Mat,Nav>::execute ( id, m, item, functor, speed, dir, this );
  this->_secondNavigator ( functor.get () );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make vertical joystick input translate in the wand's z-direction.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_vTransGlobalPosZ ( MenuKit::Message m, MenuKit::Item *item )
{
  ErrorChecker ( 4190151089u, isAppThread(), CV::NOT_APP_THREAD );

  // To shorten the lines.
  typedef VRV::Functors::JoystickVertical Analog;
  typedef VRV::Functors::Matrix::IdentityMatrix Mat;
  typedef VRV::Functors::Translate Nav;

  // Call the common function.
  const unsigned int id ( 3775941120u );
  float speed ( this->preferences()->translationSpeed() );
  VRV::Functors::Direction::Vector dir ( 0, 0, -1 );

  FunctorPtr functor ( this->_secondNavigator () );
  CV::NavCB<Analog,Mat,Nav>::execute ( id, m, item, functor, speed, dir, this );
  this->_secondNavigator ( functor.get () );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make vertical joystick input translate along the wand's negative z-axis.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_vTransWandNegZ ( MenuKit::Message m, MenuKit::Item *item )
{
  ErrorChecker ( 1084116072u, isAppThread(), CV::NOT_APP_THREAD );

  // To shorten the lines.
  typedef VRV::Functors::JoystickVertical Analog;
  typedef VRV::Functors::WandRotation Mat;
  typedef VRV::Functors::Translate Nav;

  // The id for this navigator.
  const unsigned int id ( 1084143896u );
  float speed ( this->preferences()->translationSpeed() );
  VRV::Functors::Direction::Vector dir ( 0, 0, 1 );

  FunctorPtr functor ( this->_secondNavigator () );
  CV::NavCB<Analog,Mat,Nav>::execute ( id, m, item, functor, speed, dir, this );
  this->_secondNavigator ( functor.get () );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make vertical joystick input translate along the wand's negative z-axis.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_vTransGlobalNegZ ( MenuKit::Message m, MenuKit::Item *item )
{
  ErrorChecker ( 1948086238u, isAppThread(), CV::NOT_APP_THREAD );

  // To shorten the lines.
  typedef VRV::Functors::JoystickVertical Analog;
  typedef VRV::Functors::Matrix::IdentityMatrix Mat;
  typedef VRV::Functors::Translate Nav;

  // The id for this navigator.
  const unsigned int id ( 3683303793u );
  float speed ( this->preferences()->translationSpeed() );
  VRV::Functors::Direction::Vector dir ( 0, 0, 1 );

  FunctorPtr functor ( this->_secondNavigator () );
  CV::NavCB<Analog,Mat,Nav>::execute ( id, m, item, functor, speed, dir, this );
  this->_secondNavigator ( functor.get () );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make horizontal joystick input rotate about the wand's y-axis.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_hRotWandPosY ( MenuKit::Message m, MenuKit::Item *item )
{
  ErrorChecker ( 1034318421u, isAppThread(), CV::NOT_APP_THREAD );

  // To shorten the lines.
  typedef VRV::Functors::JoystickHorizontal Analog;
  typedef VRV::Functors::WandRotation Mat;
  typedef VRV::Functors::Rotate Nav;

  // The id for this navigator.
  const unsigned int id ( 2200874009u );
  float speed ( this->preferences()->rotationSpeed() );
  VRV::Functors::Direction::Vector dir ( 0, 1, 0 );

  FunctorPtr functor ( this->_firstNavigator () );
  CV::NavCB<Analog,Mat,Nav>::execute ( id, m, item, functor, speed, dir, this );
  this->_firstNavigator ( functor.get () );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make horizontal joystick input rotate about the global y-axis.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_hRotGlobalPosY ( MenuKit::Message m, MenuKit::Item *item )
{
  ErrorChecker ( 2075689222u, isAppThread(), CV::NOT_APP_THREAD );

  // To shorten the lines.
  typedef VRV::Functors::JoystickHorizontal Analog;
  typedef VRV::Functors::Matrix::IdentityMatrix Mat;
  typedef VRV::Functors::Rotate Nav;

  // The id for this navigator.
  const unsigned int id ( 2093083320u );
  float speed ( this->preferences()->rotationSpeed() );
  VRV::Functors::Direction::Vector dir ( 0, 1, 0 );

  FunctorPtr functor ( this->_firstNavigator () );
  CV::NavCB<Analog,Mat,Nav>::execute ( id, m, item, functor, speed, dir, this );
  this->_firstNavigator ( functor.get () );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make horizontal joystick input rotate about the wand's y-axis.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_vRotWandPosX ( MenuKit::Message m, MenuKit::Item *item )
{
  ErrorChecker ( 3306102879u, isAppThread(), CV::NOT_APP_THREAD );

  // To shorten the lines.
  typedef VRV::Functors::JoystickVertical Analog;
  typedef VRV::Functors::WandRotation Mat;
  typedef VRV::Functors::Rotate Nav;

  // The id for this navigator.
  const unsigned int id ( 3106060360u );
  float speed ( this->preferences()->rotationSpeed() );
  VRV::Functors::Direction::Vector dir ( 1, 0, 0 );

  FunctorPtr functor ( this->_secondNavigator () );
  CV::NavCB<Analog,Mat,Nav>::execute ( id, m, item, functor, speed, dir, this );
  this->_secondNavigator ( functor.get () );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make horizontal joystick input rotate about the global y-axis.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_vRotGlobalPosX ( MenuKit::Message m, MenuKit::Item *item )
{
  ErrorChecker ( 3834691556u, isAppThread(), CV::NOT_APP_THREAD );

  // To shorten the lines.
  typedef VRV::Functors::JoystickVertical Analog;
  typedef VRV::Functors::Matrix::IdentityMatrix Mat;
  typedef VRV::Functors::Rotate Nav;

  // The id for this navigator.
  const unsigned int id ( 2513007202u );
  float speed ( this->preferences()->rotationSpeed() );
  VRV::Functors::Direction::Vector dir ( 1, 0, 0 );

  FunctorPtr functor ( this->_secondNavigator () );
  CV::NavCB<Analog,Mat,Nav>::execute ( id, m, item, functor, speed, dir, this );
  this->_secondNavigator ( functor.get () );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make vertical joystick input translate along the global y-axis.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_vTransGlobalPosY ( MenuKit::Message m, MenuKit::Item *item )
{
  ErrorChecker ( 1084438317u, isAppThread(), CV::NOT_APP_THREAD );

  // To shorten the lines.
  typedef VRV::Functors::JoystickVertical Analog;
  typedef VRV::Functors::Matrix::IdentityMatrix Mat;
  typedef VRV::Functors::Translate Nav;

  // The id for this navigator.
  const unsigned int id ( 1084438318u );
  float speed ( this->preferences()->translationSpeed() );
  VRV::Functors::Direction::Vector dir ( 0, 1, 0 );

  FunctorPtr functor ( this->_secondNavigator () );
  CV::NavCB<Analog,Mat,Nav>::execute ( id, m, item, functor, speed, dir, this );
  this->_secondNavigator ( functor.get () );
}


///////////////////////////////////////////////////////////////////////////////
//
//  The ray-intersector callback.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_raySelector ( MenuKit::Message m, MenuKit::Item *item )
{
  ErrorChecker ( 3690175917u, isAppThread(), CV::NOT_APP_THREAD );

  // To shorten up the lines.
  typedef VRV::Functors::Direction Dir;
  typedef Dir::Vector Vec;
  typedef VRV::Functors::WandRotation MF;
  typedef CV::Pick::Select Selector;

  // Local id.
  const unsigned int id ( 3923580797u );

  // Process the message.
  switch ( m )
  {
    case MenuKit::MESSAGE_UPDATE:
      item->checked ( _intersector.valid() && _intersector->id() == id );
      break;

    case MenuKit::MESSAGE_SELECTED:
    {
      // If the navigator matches the given id...
      if ( _intersector.valid() && _intersector->id() == id )
        _intersector = 0x0;

      // Otherwise...
      else
      {
        // Properties for the selector.
        ValidUnknown unknown ( this );
        Vec dir ( 0, 0, -1 );
        MF::ValidRefPtr mf ( new MF ( unknown ) );
        Dir::ValidRefPtr df ( new Dir ( unknown, dir, mf.get() ) );

        // Make the selector.
        Selector::ValidRefPtr selector ( new Selector ( unknown, df.get(), id ) );

        // Set the selection-material.
        osg::ref_ptr<osg::Material> mat ( new osg::Material );
        const VRV::Prefs::Settings::Color &c = this->preferences()->selectionColor();
        osg::Vec4 color ( c[0], c[1], c[2], c[3] );
        mat->setAmbient ( osg::Material::FRONT, color );
        mat->setDiffuse ( osg::Material::FRONT, color );
        selector->material ( mat.get() );

        // Set the intersector.
        _intersector = selector;
      }
      break;
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
// Draw polygons with smooth shading.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_polysSmooth ( MenuKit::Message m, MenuKit::Item *item )
{
  ErrorChecker ( 1084120964u, isAppThread(), CV::NOT_APP_THREAD );

  // Process the message.
  switch ( m )
  {
    case MenuKit::MESSAGE_UPDATE:
      item->checked ( OsgTools::State::StateSet::getPolygonsFilled ( this->models(), false ) &&
                      OsgTools::State::StateSet::getPolygonsSmooth ( this->models() ) );
      break;

    case MenuKit::MESSAGE_SELECTED:
      OsgTools::State::StateSet::setPolygonsFilled ( this->models(), false );
      OsgTools::State::StateSet::setPolygonsSmooth ( this->models() );
      break;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
// Draw polygons with flat shading.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_polysFlat ( MenuKit::Message m, MenuKit::Item *item )
{
  ErrorChecker ( 1084120973u, isAppThread(), CV::NOT_APP_THREAD );

  // Process the message.
  switch ( m )
  {
    case MenuKit::MESSAGE_UPDATE:
      item->checked ( OsgTools::State::StateSet::getPolygonsFilled ( this->models(), false ) &&
                      OsgTools::State::StateSet::getPolygonsFlat   ( this->models() ) );
      break;

    case MenuKit::MESSAGE_SELECTED:
      OsgTools::State::StateSet::setPolygonsFilled ( this->models(), false );
      OsgTools::State::StateSet::setPolygonsFlat   ( this->models() );
      break;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
// Draw polygons with wireframe.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_polysWireframe ( MenuKit::Message m, MenuKit::Item *item )
{
  ErrorChecker ( 1084120981u, isAppThread(), CV::NOT_APP_THREAD );

  // Process the message.
  switch ( m )
  {
    case MenuKit::MESSAGE_UPDATE:
      item->checked ( OsgTools::State::StateSet::getPolygonsLines ( this->models(), false ) );
      break;

    case MenuKit::MESSAGE_SELECTED:
      OsgTools::State::StateSet::setPolygonsLines ( this->models(), true );
      break;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
// Draw polygons as points.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_polysPoints ( MenuKit::Message m, MenuKit::Item *item )
{
  ErrorChecker ( 1084120994u, isAppThread(), CV::NOT_APP_THREAD );

  // Process the message.
  switch ( m )
  {
    case MenuKit::MESSAGE_UPDATE:
      item->checked ( OsgTools::State::StateSet::getPolygonsPoints ( this->models(), false ) );
      break;

    case MenuKit::MESSAGE_SELECTED:
      OsgTools::State::StateSet::setPolygonsPoints ( this->models(), true );
      break;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Toggle textures TODO: fix this callback
//
///////////////////////////////////////////////////////////////////////////////

void Application::_polysTexture ( MenuKit::Message m, MenuKit::Item *item )
{
  ErrorChecker ( 1084512765u, isAppThread(), CV::NOT_APP_THREAD );

  // Local id.
  const unsigned int id ( 1084512764u );

  // Process the message.
  switch ( m )
  {
    case MenuKit::MESSAGE_UPDATE:
      //item->checked ( OsgTools::State::StateSet::getPolygonsTextures ( this->models()->getStateSet() ) );
      item->checked ( _textures );
      break;

    case MenuKit::MESSAGE_SELECTED:
      //bool state = OsgTools::State::StateSet::getPolygonsTextures ( this->models()->getStateSet() );
      //OsgTools::State::StateSet::setPolygonsTextures (  this->models()->getStateSet() , state );
      _textures = !_textures;
      OsgTools::State::StateSet::setPolygonsTextures( this->models()->getOrCreateStateSet(), _textures );
      break;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
// Set the grid visibility.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_gridVisibility ( MenuKit::Message m, MenuKit::Item *item )
{
  ErrorChecker ( 1084126008u, isAppThread(), CV::NOT_APP_THREAD );

  // TODO: fix.
  // Process the message.
  /*switch ( m )
  {
    case MenuKit::MESSAGE_UPDATE:
      item->checked ( _navBranch->containsNode ( _gridBranch.get() ) );
      break;

    case MenuKit::MESSAGE_SELECTED:
      if ( item->checked() && _navBranch->containsNode ( _gridBranch.get() ) )
        OsgTools::Group::removeAllOccurances ( _gridBranch.get(), _navBranch.get() );
      else
        _navBranch->addChild ( _gridBranch.get() );
      break;
  }*/
}


///////////////////////////////////////////////////////////////////////////////
//
// Set the status-bar visibility.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_statusBarVis ( MenuKit::Message m, MenuKit::Item *item )
{
  ErrorChecker ( 1552310702u, isAppThread(), CV::NOT_APP_THREAD );

  MenuPtr status ( this->statusBar() );

  // Process the message.
  switch ( m )
  {
    case MenuKit::MESSAGE_UPDATE:
      item->checked ( status->menu()->expanded() );
      break;

    case MenuKit::MESSAGE_SELECTED:
      status->menu()->expanded ( !status->menu()->expanded() );
      status->updateScene();
      break;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
// Default callback for menu items.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_defaultCallback ( MenuKit::Message m, MenuKit::Item *item )
{
  ErrorChecker ( 1411642884u, isAppThread(), CV::NOT_APP_THREAD );

  if( MenuKit::MESSAGE_UPDATE == m )
  {
    if( dynamic_cast< MenuKit::Button* >( item ) )
	    item->enabled( false );
  }

  // Update the status-bar.
  if ( MenuKit::MESSAGE_FOCUS_ON == m )
    this->_updateStatusBar ( item->info() );
}


///////////////////////////////////////////////////////////////////////////////
//
// Move the camera to the home position.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_viewHome ( MenuKit::Message m, MenuKit::Item *item )
{
  ErrorChecker ( 2437985028u, isAppThread(), CV::NOT_APP_THREAD );

  if ( MenuKit::MESSAGE_SELECTED == m )
  {
    this->_navigationMatrix ( _home );
    this->_setNearAndFarClippingPlanes();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
// Move the camera such that the whole world is visible.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_viewWorld ( MenuKit::Message m, MenuKit::Item *item )
{
  ErrorChecker ( 2226820120u, isAppThread(), CV::NOT_APP_THREAD );

  if ( MenuKit::MESSAGE_SELECTED == m )
  {
    // Save current model-matrix.
    osg::Matrix original ( this->models()->getMatrix() );

    // Perform the "view all" on the model's branch.
    this->viewAll ( this->models(), this->preferences()->viewAllScaleZ() );

    // Move the navigation branch.
    this->_navigationMatrix ( this->models()->getMatrix() );

    // Restore the model's matrix.
    this->models()->setMatrix ( original );

    // make sure the scene is visible
    this->_setNearAndFarClippingPlanes();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
// Move the camera such that the whole world is visible.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_viewScene ( MenuKit::Message m, MenuKit::Item *item )
{
  ErrorChecker ( 3436039617u, isAppThread(), CV::NOT_APP_THREAD );

  if ( MenuKit::MESSAGE_SELECTED == m )
  {
    this->viewAll ( this->navigationScene(), this->preferences()->viewAllScaleZ() );
    this->_setNearAndFarClippingPlanes();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
// Set the current camera position as the home view.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_setAsHome ( MenuKit::Message m, MenuKit::Item *item )
{
  ErrorChecker ( 3328782636u, isAppThread(), CV::NOT_APP_THREAD );

  if ( MenuKit::MESSAGE_SELECTED == m )
    this->_setHome();
}


///////////////////////////////////////////////////////////////////////////////
//
// Scale the world with the vertical joystick.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_vScaleWorld ( MenuKit::Message m, MenuKit::Item *item )
{
  ErrorChecker ( 2053584659u, isAppThread(), CV::NOT_APP_THREAD );

  // To shorten the lines.
  typedef VRV::Functors::JoystickVertical Analog;
  typedef CV::Functors::ScaleTool Tool;
  typedef Tool::Vector Vector;

  // The id for this navigator.
  const unsigned int id ( 2747412269u );

  // The scale "directions" and the speed.
  float speed ( this->preferences()->scaleSpeed() );
  Tool::Vector scale ( 1, 1, 1 );

  // Put the models-branch in a vector.
  Tool::Transforms vt;
  vt.push_back ( this->models() );

  // Call the convenience function.
  CV::ScaleCB<Analog,Tool>::execute ( id, m, item, _sceneTool, vt, speed, scale, this );
}


///////////////////////////////////////////////////////////////////////////////
//
// Scale the selected geometry with the vertical joystick.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_vScaleSelected ( MenuKit::Message m, MenuKit::Item *item )
{
  ErrorChecker ( 3794111793u, isAppThread(), CV::NOT_APP_THREAD );

  if( MenuKit::MESSAGE_UPDATE == m )
    item->enabled( 0 != this->_numSelected() );

  // To shorten the lines.
  typedef VRV::Functors::JoystickVertical Analog;
  typedef CV::Functors::ScaleTool Tool;
  typedef Tool::Vector Vector;

  // The id for this navigator.
  const unsigned int id ( 1612324357u );

  // The scale "directions" and the speed.
  float speed ( this->preferences()->scaleSpeed() );
  Tool::Vector scale ( 1, 1, 1 );

  // Get the vector of selected matrix-transforms.
  Tool::Transforms vt;
  if ( MenuKit::MESSAGE_SELECTED == m )
    this->_selected ( vt );

  // Call the convenience function.
  CV::ScaleCB<Analog,Tool>::execute ( id, m, item, _sceneTool, vt, speed, scale, this );
}


///////////////////////////////////////////////////////////////////////////////
//
// Move the selected geometry.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_wMoveSelLocal ( MenuKit::Message m, MenuKit::Item *item )
{
  ErrorChecker ( 2457117960u, isAppThread(), CV::NOT_APP_THREAD );

  typedef VRV::Functors::Matrix::MatrixFunctor MF;
  typedef CV::Functors::ToolPair ToolPair;
  typedef CV::Functors::MoveTool MoveTool;
  typedef VRV::Functors::JoystickHorizontal JH;
  typedef VRV::Functors::JoystickVertical JV;
  typedef VRV::Functors::WandRotation WR;
  typedef VRV::Functors::Direction Dir;
  typedef Dir::Vector Vec;

  const unsigned int id ( 3210541994u );

  switch ( m )
  {
  case MenuKit::MESSAGE_UPDATE:
    item->enabled( 0 != this->_numSelected() );
    item->checked ( _sceneTool.valid() && _sceneTool->id() == id );
    break;

  case MenuKit::MESSAGE_SELECTED:

    if ( _sceneTool.valid() && _sceneTool->id() == id )
      _sceneTool = 0x0;

    else
    {
      ValidUnknown unknown ( this );
      float speed ( this->preferences()->scaleSpeed() );

      JH::ValidRefPtr hai ( new JH ( unknown ) );
      JV::ValidRefPtr vai ( new JV ( unknown ) );

      MF::ValidRefPtr wm ( new WR ( unknown ) );

      Vec x ( 1, 0, 0 );
      Vec z ( 0, 0, 1 );

      Dir::ValidRefPtr pwx ( new Dir ( unknown, -x, wm ) );
      Dir::ValidRefPtr nwz ( new Dir ( unknown,  z, wm ) );

      MoveTool::ValidRefPtr mx ( new MoveTool ( unknown, pwx, hai, speed, id ) );
      MoveTool::ValidRefPtr my ( new MoveTool ( unknown, nwz, vai, speed, id ) );

      ToolPair::ValidRefPtr tool ( new ToolPair ( unknown, mx, my, id ) );

      MoveTool::Transforms tr;
      this->_selected ( tr );
      tool->transforms ( tr );

      _sceneTool = tool;
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
// Move the selected geometry.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_wMoveTopLocal ( MenuKit::Message m, MenuKit::Item *item )
{
  ErrorChecker ( 1084528454u, isAppThread(), CV::NOT_APP_THREAD );

  typedef CV::Functors::ToolPair ToolPair;
  typedef CV::Functors::MoveTool MoveTool;
  typedef VRV::Functors::JoystickHorizontal JH;
  typedef VRV::Functors::JoystickVertical JV;
  typedef VRV::Functors::Matrix::MatrixFunctor MF;
  typedef VRV::Functors::WandRotation WR;
  typedef VRV::Functors::Direction Dir;
  typedef Dir::Vector Vec;

  const unsigned int id ( 1084528455u );

  switch ( m )
  {
  case MenuKit::MESSAGE_UPDATE:

    item->checked ( _sceneTool.valid() && _sceneTool->id() == id );
    break;

  case MenuKit::MESSAGE_SELECTED:

    if ( _sceneTool.valid() && _sceneTool->id() == id )
      _sceneTool = 0x0;

    else
    {
      ValidUnknown unknown ( this );
      float speed ( this->preferences()->scaleSpeed() );

      JH::ValidRefPtr hai ( new JH ( unknown ) );
      JV::ValidRefPtr vai ( new JV ( unknown ) );

      MF::ValidRefPtr wm ( new WR ( unknown ) );

      Vec x ( 1, 0, 0 );
      Vec z ( 0, 0, 1 );

      Dir::ValidRefPtr pwx ( new Dir ( unknown, -x, wm ) );
      Dir::ValidRefPtr nwz ( new Dir ( unknown,  z, wm ) );

      MoveTool::ValidRefPtr mx ( new MoveTool ( unknown, pwx, hai, speed, id ) );
      MoveTool::ValidRefPtr my ( new MoveTool ( unknown, nwz, vai, speed, id ) );

      ToolPair::ValidRefPtr tool ( new ToolPair ( unknown, mx, my, id ) );

      // Put the models-branch in a vector.
      MoveTool::Transforms vt;
      vt.push_back ( this->models() );
      tool->transforms ( vt );

      _sceneTool = tool;
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
// Resize the Grid
//
///////////////////////////////////////////////////////////////////////////////

void Application::_resizeGrid ( MenuKit::Message m, MenuKit::Item *item )
{
  ErrorChecker ( 1084416658u, isAppThread(), CV::NOT_APP_THREAD );

  // Process the message.
  if( m==MenuKit::MESSAGE_SELECTED )
  {
    this->_initGrid ( this->models() );
    this->_setNearAndFarClippingPlanes();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
// Set the background color
//
///////////////////////////////////////////////////////////////////////////////

void Application::_backgroundColor ( MenuKit::Message m, MenuKit::Item *item )
{
  ErrorChecker ( 1084416659u, isAppThread(), CV::NOT_APP_THREAD );

  // Process the message.
  if ( MenuKit::MESSAGE_SELECTED == m )
  {
    const osg::Vec4& color = this->_getColor( item->text() );
    this->setBackgroundColor ( color );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
// Set the grid's color
//
///////////////////////////////////////////////////////////////////////////////

void Application::_gridColor ( MenuKit::Message m, MenuKit::Item *item )
{
  ErrorChecker ( 1084416660u, isAppThread(), CV::NOT_APP_THREAD );

  // Process the message.
  if ( MenuKit::MESSAGE_SELECTED == m )
  {
    const osg::Vec4& c = this->_getColor( item->text() );
	  for( unsigned int i = 0; i < _gridFunctors.size(); ++i )
	  {
		  _gridFunctors[i].color ( c[0], c[1], c[2], c[3] );
	  }
    this->_rebuildGrid();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
// Set the selection's color
//
///////////////////////////////////////////////////////////////////////////////

void Application::_selectionColor ( MenuKit::Message m, MenuKit::Item *item )
{
  ErrorChecker ( 108444654u, isAppThread(), CV::NOT_APP_THREAD );

  // Process the message.
  if ( MenuKit::MESSAGE_SELECTED == m )
  {
    // Get the color from the string and set the preferences.
    const osg::Vec4& c = this->_getColor ( item->text() );
    this->preferences()->selectionColor ( c[0], c[1], c[2], c[3] );

    // See if there is a selector.
    CV::Pick::Select* select = dynamic_cast<CV::Pick::Select*>( _intersector.get() );
    if ( !select )
      return;

    // Make a new material and set the colors.
    osg::ref_ptr<osg::Material> mat ( new osg::Material );
    mat->setAmbient ( osg::Material::FRONT, c );
    mat->setDiffuse ( osg::Material::FRONT, c );

    // Tell the selector to use this material.
    select->material ( mat.get() );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
// Reset the clipping planes
//
///////////////////////////////////////////////////////////////////////////////

void Application::_resetClipping ( MenuKit::Message m, MenuKit::Item *item )
{
  ErrorChecker ( 1084416661u, isAppThread(), CV::NOT_APP_THREAD );

  // Process the message.
  if( m==MenuKit::MESSAGE_SELECTED )
  {
    this->_setNearAndFarClippingPlanes();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Translate with respect to the wand's orientation
//
///////////////////////////////////////////////////////////////////////////////

void Application::_flyMode ( MenuKit::Message m, MenuKit::Item *item )
{
  ErrorChecker ( 1084438123u, isAppThread(), CV::NOT_APP_THREAD );

  // Process the message.
  switch ( m )
  {
    case MenuKit::MESSAGE_SELECTED:
      this->flyMode ();
      break;

    case MenuKit::MESSAGE_UPDATE:
      item->checked ( this->getFlyMode () );
      break;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Translate with respect to the global axes
//
///////////////////////////////////////////////////////////////////////////////

void Application::_walkMode ( MenuKit::Message m, MenuKit::Item *item )
{
  ErrorChecker (1084438119u, isAppThread(), CV::NOT_APP_THREAD );

  // Process the message.
  switch ( m )
  {
    case MenuKit::MESSAGE_SELECTED:
      this->walkMode ();
      break;

    case MenuKit::MESSAGE_UPDATE:
      item->checked ( this->getWalkMode () );
      break;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Horizontal joystick rotates, vertical changes elevation.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_poleNav ( MenuKit::Message m, MenuKit::Item *item )
{
  ErrorChecker ( 1084438121u, isAppThread(), CV::NOT_APP_THREAD );

  // Process the message.
  switch ( m )
  {
    case MenuKit::MESSAGE_SELECTED:
      this->poleMode ();
      break;

    case MenuKit::MESSAGE_UPDATE:
      item->checked ( this->getPoleMode () );
      break;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Reset the navigation to the requested view
//
///////////////////////////////////////////////////////////////////////////////

void Application::_gotoViewFront ( MenuKit::Message m, MenuKit::Item *item )
{
  ErrorChecker ( 1084522854u, isAppThread(), CV::NOT_APP_THREAD );

  // Local id.
  const unsigned int id ( 1084522855u );

  // Process the message.
  switch( m )
  {
  case MenuKit::MESSAGE_SELECTED:
    const osg::BoundingSphere& bs = this->models()->getBound();
    osg::Matrix trans( osg::Matrix::translate( osg::Vec3(0.0,0.0,-2.0*bs.radius())-bs.center() ) );
    this->_navigationMatrix ( trans );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Reset the navigation to the requested view
//
///////////////////////////////////////////////////////////////////////////////

void Application::_gotoViewBack ( MenuKit::Message m, MenuKit::Item *item )
{
  ErrorChecker ( 1084522856u, isAppThread(), CV::NOT_APP_THREAD );

  // Local id.
  const unsigned int id ( 1084522857u );

  // Process the message.
  switch( m )
  {
  case MenuKit::MESSAGE_SELECTED:
    const osg::BoundingSphere& bs = this->models()->getBound();
    osg::Matrix rot  ( osg::Matrix::rotate   ( osg::PI , osg::Y_AXIS ) );
    osg::Matrix zero ( osg::Matrix::translate( -(bs.center()) ) );
    osg::Matrix trans( osg::Matrix::translate( osg::Vec3(0.0,0.0,-2.0*bs.radius()) ) );
    this->_navigationMatrix ( zero*rot*trans );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Reset the navigation to the requested view
//
///////////////////////////////////////////////////////////////////////////////

void Application::_gotoViewTop ( MenuKit::Message m, MenuKit::Item *item )
{
  ErrorChecker ( 1084522858u, isAppThread(), CV::NOT_APP_THREAD );

  // Local id.
  const unsigned int id ( 1084522859u );

  // Process the message.
  switch( m )
  {
  case MenuKit::MESSAGE_SELECTED:
    const osg::BoundingSphere& bs = this->models()->getBound();
    osg::Matrix rot  ( osg::Matrix::rotate   ( osg::PI_2 , osg::X_AXIS ) );
    osg::Matrix zero ( osg::Matrix::translate( -(bs.center()) ) );
    osg::Matrix trans( osg::Matrix::translate( osg::Vec3(0.0,0.0,-2.0*bs.radius()) ) );
    this->_navigationMatrix ( zero*rot*trans );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Reset the navigation to the requested view
//
///////////////////////////////////////////////////////////////////////////////

void Application::_gotoViewBottom ( MenuKit::Message m, MenuKit::Item *item )
{
  ErrorChecker ( 1084522860u, isAppThread(), CV::NOT_APP_THREAD );

  // Local id.
  const unsigned int id ( 1084522861u );

  // Process the message.
  switch( m )
  {
  case MenuKit::MESSAGE_SELECTED:
    const osg::BoundingSphere& bs = this->models()->getBound();
    osg::Matrix rot  ( osg::Matrix::rotate   ( -osg::PI_2 , osg::X_AXIS ) );
    osg::Matrix zero ( osg::Matrix::translate( -(bs.center()) ) );
    osg::Matrix trans( osg::Matrix::translate( osg::Vec3(0.0,0.0,-2.0*bs.radius()) ) );
    this->_navigationMatrix ( zero*rot*trans );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Reset the navigation to the requested view
//
///////////////////////////////////////////////////////////////////////////////

void Application::_gotoViewRight ( MenuKit::Message m, MenuKit::Item *item )
{
  ErrorChecker ( 1084522862u, isAppThread(), CV::NOT_APP_THREAD );

  // Local id.
  const unsigned int id ( 1084522863u );

  // Process the message.
  switch( m )
  {
  case MenuKit::MESSAGE_SELECTED:
    const osg::BoundingSphere& bs = this->models()->getBound();
    osg::Matrix rot  ( osg::Matrix::rotate   ( -osg::PI_2 , osg::Y_AXIS ) );
    osg::Matrix zero ( osg::Matrix::translate( -(bs.center()) ) );
    osg::Matrix trans( osg::Matrix::translate( osg::Vec3(0.0,0.0,-2.0*bs.radius()) ) );
    this->_navigationMatrix ( zero*rot*trans );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Reset the navigation to the requested view
//
///////////////////////////////////////////////////////////////////////////////

void Application::_gotoViewLeft ( MenuKit::Message m, MenuKit::Item *item )
{
  ErrorChecker ( 1084522864u, isAppThread(), CV::NOT_APP_THREAD );

  // Local id.
  const unsigned int id ( 1084522865u );

  // Process the message.
  switch( m )
  {
  case MenuKit::MESSAGE_SELECTED:
    const osg::BoundingSphere& bs = this->models()->getBound();
    osg::Matrix rot  ( osg::Matrix::rotate   ( osg::PI_2 , osg::Y_AXIS ) );
    osg::Matrix zero ( osg::Matrix::translate( -(bs.center()) ) );
    osg::Matrix trans( osg::Matrix::translate( osg::Vec3(0.0,0.0,-2.0*bs.radius()) ) );
    this->_navigationMatrix ( zero*rot*trans );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Rotate the Scene about the primary axis.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_rotateWorld ( MenuKit::Message m, MenuKit::Item *item )
{
  ErrorChecker ( 1084574455u, isAppThread(), CV::NOT_APP_THREAD );

  // Process the message.
  if ( MenuKit::MESSAGE_SELECTED == m )
  {
    std::istringstream str( item->text() );
    std::string world;
    float degrees;
    char axis;
    str >> world >> axis >> degrees;

    float radians = Usul::Math::DEG_TO_RAD * degrees;
    osg::Vec3 vec;
    if ( 'X' == axis || 'x' == axis )
      vec = osg::X_AXIS;
    else if ( 'Y' == axis || 'y' == axis )
      vec = osg::Z_AXIS;  // NOT a bug =)
    else // z-axis
    {                   // NOT a bug =)
      radians *= -1.0;
      vec = osg::Y_AXIS;
    }

    const osg::BoundingSphere& bs = this->models()->getBound();
    osg::Matrix rot ( osg::Matrix::rotate ( radians, vec ) );
    this->models()->preMult ( rot );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Drop to the surface below you.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_dropToFloor ( MenuKit::Message m, MenuKit::Item *item )
{
  ErrorChecker ( 3042741392u, isAppThread(), CV::NOT_APP_THREAD );

  // Only selection events...
  if ( MenuKit::MESSAGE_SELECTED != m )
    return;

  // Make the line-segment.
  osg::ref_ptr<osg::LineSegment> segment ( new osg::LineSegment );
  float y ( 1e6f );
  osg::Vec3 beg ( 0,  y, 0 );
  osg::Vec3 end ( 0, -y, 0 );
  segment->set ( beg, end ); // TODO, These must not be in the proper coordinate space...

  // Add the line-segment to the visitor.
  osg::ref_ptr<osgUtil::IntersectVisitor> visitor ( new osgUtil::IntersectVisitor );
  visitor->addLineSegment ( segment.get() );

  // Calculate the intersections.
  this->navigationScene()->accept ( *visitor );

  // Save the info if we intersected.
  osgUtil::IntersectVisitor::HitList &hits = visitor->getHitList ( segment.get() );
  if ( hits.size() )
  {
    // Get the point.
    osg::Vec3 point ( hits[0].getWorldIntersectPoint() );
    point *= -1.0;
    
    // Translate to that point.
    Matrix44f M;
    M.makeTranslation ( point );
    this->postMultiply ( M );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Increase speed.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_increaseSpeed    ( MenuKit::Message m, MenuKit::Item *item )
{
  // Process the message.
  switch( m )
  {
  case MenuKit::MESSAGE_SELECTED:
    this->_increaseTranslateSpeed ( 2.0 );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Decrease speed.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_decreaseSpeed    ( MenuKit::Message m, MenuKit::Item *item )
{
  // Process the message.
  switch( m )
  {
  case MenuKit::MESSAGE_SELECTED:
    this->_decreaseTranslateSpeed ( 2.0 );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Increase speed.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_increaseSpeedTen    ( MenuKit::Message m, MenuKit::Item *item )
{
  // Process the message.
  switch( m )
  {
  case MenuKit::MESSAGE_SELECTED:
    this->_increaseTranslateSpeed ( 10.0 );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Decrease speed.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_decreaseSpeedTen    ( MenuKit::Message m, MenuKit::Item *item )
{
  // Process the message.
  switch( m )
  {
  case MenuKit::MESSAGE_SELECTED:
    this->_decreaseTranslateSpeed ( 10.0 );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the number of rendering passes to one.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_renderPassesOne      ( MenuKit::Message m, MenuKit::Item *item )
{
  // Process the message.
  switch( m )
  {
  case MenuKit::MESSAGE_SELECTED:
    this->numRenderPasses ( 1 );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the number of rendering passes to three.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_renderPassesThree    ( MenuKit::Message m, MenuKit::Item *item )
{
  // Process the message.
  switch( m )
  {
  case MenuKit::MESSAGE_SELECTED:
    this->numRenderPasses ( 3 );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the number of rendering passes to nine.
//
///////////////////////////////////////////////////////////////////////////////

void  Application::_renderPassesNine     ( MenuKit::Message m, MenuKit::Item *item )
{
  // Process the message.
  switch( m )
  {
  case MenuKit::MESSAGE_SELECTED:
    this->numRenderPasses ( 9 );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the number of rendering passes to tweleve.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_renderPassesTweleve  ( MenuKit::Message m, MenuKit::Item *item )
{
  // Process the message.
  switch( m )
  {
  case MenuKit::MESSAGE_SELECTED:
    this->numRenderPasses ( 12 );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Toggle dump frames.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_dumpFrames ( MenuKit::Message m, MenuKit::Item *item )
{
  // Process the message.
  switch( m )
  {
  case MenuKit::MESSAGE_UPDATE:
    item->checked ( this->frameDump() );
    break;
  case MenuKit::MESSAGE_SELECTED:
    this->frameDump ( !this->frameDump() );
    break;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//   Toggle whether or not to hide or show the scene when the menu is visible
//
///////////////////////////////////////////////////////////////////////////////

void Application::_toggleMenuSceneHideShow ( MenuKit::Message m, MenuKit::Item *item )
{
  // Process the message.
  if ( MenuKit::MESSAGE_SELECTED == m )
  {
    if( true == this->menuSceneShowHide() )
      this->toggleMenuSceneShowHide( false );
    else
      this->toggleMenuSceneShowHide( true );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Append current frame.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_appendCurrentCamera ( MenuKit::Message m, MenuKit::Item *item )
{
  if ( MenuKit::MESSAGE_SELECTED == m )
    this->appendCamera ( );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Start the animation.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_toggleStartAnimation ( MenuKit::Message m, MenuKit::Item *item )
{
  if ( MenuKit::MESSAGE_SELECTED == m )
    this->startAnimation ( );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Start the animation.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_clearAnimation ( MenuKit::Message m, MenuKit::Item *item )
{
  if ( MenuKit::MESSAGE_SELECTED == m )
    this->clearAnimation ( );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the number of animation steps to 20.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_animationSteps20 ( MenuKit::Message m, MenuKit::Item *item )
{
  // Process the message.
  switch( m )
  {
  case MenuKit::MESSAGE_UPDATE:
    item->checked ( 20 == this->animationSteps () );
    break;
  case MenuKit::MESSAGE_SELECTED:
    this->animationSteps ( 20 );
    break;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the number of animation steps to 50.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_animationSteps50 ( MenuKit::Message m, MenuKit::Item *item )
{
  // Process the message.
  switch( m )
  {
  case MenuKit::MESSAGE_UPDATE:
    item->checked ( 50 == this->animationSteps () );
    break;
  case MenuKit::MESSAGE_SELECTED:
    this->animationSteps ( 50 );
    break;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the number of animation steps to 100.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_animationSteps100 ( MenuKit::Message m, MenuKit::Item *item )
{
  // Process the message.
  switch( m )
  {
  case MenuKit::MESSAGE_UPDATE:
    item->checked ( 100 == this->animationSteps () );
    break;
  case MenuKit::MESSAGE_SELECTED:
    this->animationSteps ( 100 );
    break;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Double the number of animation steps.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_animationStepsDouble ( MenuKit::Message m, MenuKit::Item *item )
{
  if ( MenuKit::MESSAGE_SELECTED == m )
    this->animationSteps ( this->animationSteps () * 2 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Half the number of animation steps.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_animationStepsHalf ( MenuKit::Message m, MenuKit::Item *item )
{
  if ( MenuKit::MESSAGE_SELECTED == m )
    this->animationSteps ( this->animationSteps () / 2 );
}
