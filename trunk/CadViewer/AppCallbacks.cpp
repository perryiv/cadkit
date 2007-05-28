
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

#include "CadViewer/Functors/Translate.h"
#include "CadViewer/Functors/Rotate.h"
#include "CadViewer/Functors/Direction.h"
#include "CadViewer/Functors/AnalogInput.h"
#include "CadViewer/Functors/WandRotation.h"
#include "CadViewer/Functors/WandPosition.h"
#include "CadViewer/Functors/IdentityMatrix.h"
#include "CadViewer/Functors/JoystickHorizontal.h"
#include "CadViewer/Functors/JoystickVertical.h"
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
#include "osgDB/ReadFile"
#include "osgFX/Scribe"

#include "OsgTools/Group.h"
#include "OsgTools/State/StateSet.h"
#include "OsgTools/Visitor.h"
#include "OsgTools/Axes.h"

#include "Usul/Functors/Pair.h"
#include "Usul/Functors/If.h"
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
      _analogTrim[0] = 0.5f - _joystick->horizontal();
      _analogTrim[1] = 0.5f - _joystick->vertical();
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
  typedef Usul::Functors::Pair<PopMaterial,Unselect> Restore;
  typedef CV::OSG::Predicates::IsVisible IsVisible;
  typedef Usul::Functors::IfThen<IsVisible,Restore> IfThen;
  typedef OsgTools::Visitor<osg::MatrixTransform,IfThen> Visitor;

  // update the tool because different things are selected
  this->_updateSceneTool();

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
  _models->accept ( *visitor );
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
  typedef Usul::Functors::IfThen<IsSelected,Hide> IfThen;
  typedef OsgTools::Visitor<osg::MatrixTransform,IfThen> Visitor;

  // Compose the functors. For some reason I need to declare all the functors 
  // as variables, rather than pass, for example, IsSelected().
  IsSelected isSelected ( _iSelection );
  Hide hide ( _iVisibility );
  IfThen ifThen ( isSelected, hide );

  // Make the visitor.
  Visitor::Ptr visitor ( new Visitor ( ifThen ) );

  // Traverse the models-scene and hide all selected nodes.
  _models->accept ( *visitor );
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

  // Make the visitor.
  Visitor::Ptr visitor ( new Visitor ( Show ( _iVisibility ) ) );

  // Traverse the models-scene and hide all selected nodes.
  _models->accept ( *visitor );
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
      this->_writeScene ( filename, _models.get() );
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
      this->_writeScene ( filename, _root.get() );
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
  typedef CV::Functors::JoystickHorizontal Analog;
  typedef CV::Functors::WandRotation Mat;
  typedef CV::Functors::Translate Nav;

  // Call the common function.
  const unsigned int id ( 1084141815u );
  float speed ( _prefs->translationSpeed() );
  CV::Functors::Direction::Vector dir ( 1, 0, 0 );
  CV::NavCB<Analog,Mat,Nav>::execute ( id, m, item, _navigatorH, speed, dir, this );

  if ( MenuKit::MESSAGE_SELECTED == m )
  {
    OsgTools::Group::removeAllChildren( _cursorActiveWithRot.get() );
    OsgTools::Group::removeAllChildren( _cursorActiveNoRot.get() );
    if ( _navigatorH.valid() && id == _navigatorH->id() )
    {
      OsgTools::Axes horizTrans;
      horizTrans.state( OsgTools::Axes::POSITIVE_X |
                        OsgTools::Axes::NEGATIVE_X );
      _cursorActiveWithRot->addChild( horizTrans() );
    }
  }
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
  typedef CV::Functors::JoystickHorizontal Analog;
  typedef CV::Functors::IdentityMatrix Mat;
  typedef CV::Functors::Translate Nav;

  // Call the common function.
  const unsigned int id ( 3196018430u );
  float speed ( _prefs->translationSpeed() );
  CV::Functors::Direction::Vector dir ( 1, 0, 0 );
  CV::NavCB<Analog,Mat,Nav>::execute ( id, m, item, _navigatorH, speed, dir, this );

  if ( MenuKit::MESSAGE_SELECTED == m )
  {
    OsgTools::Group::removeAllChildren( _cursorActiveWithRot.get() );
    OsgTools::Group::removeAllChildren( _cursorActiveNoRot.get() );
    if ( _navigatorH.valid() && id == _navigatorH->id() )
    {
      OsgTools::Axes horizTrans;
      horizTrans.state( OsgTools::Axes::POSITIVE_X |
                        OsgTools::Axes::NEGATIVE_X );
      _cursorActiveNoRot->addChild( horizTrans() );
    }
  }
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
  typedef CV::Functors::JoystickHorizontal Analog;
  typedef CV::Functors::WandRotation Mat;
  typedef CV::Functors::Translate Nav;

  // Call the common function.
  const unsigned int id ( 2934209123u );
  float speed ( _prefs->translationSpeed() );
  CV::Functors::Direction::Vector dir ( -1, 0, 0 );
  CV::NavCB<Analog,Mat,Nav>::execute ( id, m, item, _navigatorH, speed, dir, this );

  if ( MenuKit::MESSAGE_SELECTED == m )
  {
    OsgTools::Group::removeAllChildren( _cursorActiveWithRot.get() );
    OsgTools::Group::removeAllChildren( _cursorActiveNoRot.get() );
    if ( _navigatorH.valid() && id == _navigatorH->id() )
    {
      OsgTools::Axes horizTrans;
      horizTrans.state( OsgTools::Axes::POSITIVE_X |
                        OsgTools::Axes::NEGATIVE_X );
      _cursorActiveWithRot->addChild( horizTrans() );
    }
  }
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
  typedef CV::Functors::JoystickHorizontal Analog;
  typedef CV::Functors::IdentityMatrix Mat;
  typedef CV::Functors::Translate Nav;

  // Call the common function.
  const unsigned int id ( 3848661010u );
  float speed ( _prefs->translationSpeed() );
  CV::Functors::Direction::Vector dir ( -1, 0, 0 );
  CV::NavCB<Analog,Mat,Nav>::execute ( id, m, item, _navigatorH, speed, dir, this );

  if ( MenuKit::MESSAGE_SELECTED == m )
  {
    OsgTools::Group::removeAllChildren( _cursorActiveWithRot.get() );
    OsgTools::Group::removeAllChildren( _cursorActiveNoRot.get() );
    if ( _navigatorH.valid() && id == _navigatorH->id() )
    {
      OsgTools::Axes horizTrans;
      horizTrans.state( OsgTools::Axes::POSITIVE_X |
                        OsgTools::Axes::NEGATIVE_X );
      _cursorActiveNoRot->addChild( horizTrans() );
    }
  }
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
  typedef CV::Functors::JoystickVertical Analog;
  typedef CV::Functors::WandRotation Mat;
  typedef CV::Functors::Translate Nav;

  // Call the common function.
  const unsigned int id ( 1084444803u );
  float speed ( _prefs->translationSpeed() );
  CV::Functors::Direction::Vector dir ( 0, 1, 0 );
  CV::NavCB<Analog,Mat,Nav>::execute ( id, m, item, _navigatorV, speed, dir, this );

  if ( MenuKit::MESSAGE_SELECTED == m )
  {
    OsgTools::Group::removeAllChildren( _cursorActiveWithRot.get() );
    OsgTools::Group::removeAllChildren( _cursorActiveNoRot.get() );
    if ( _navigatorV.valid() && id == _navigatorV->id() )
    {
      OsgTools::Axes vertTrans;
      vertTrans.state( OsgTools::Axes::POSITIVE_Y |
                       OsgTools::Axes::NEGATIVE_Y );
      _cursorActiveWithRot->addChild( vertTrans() );
    }
  }
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
  typedef CV::Functors::JoystickVertical Analog;
  typedef CV::Functors::WandRotation Mat;
  typedef CV::Functors::Translate Nav;

  // Call the common function.
  const unsigned int id ( 1084143905u );
  float speed ( _prefs->translationSpeed() );
  CV::Functors::Direction::Vector dir ( 0, 0, -1 );
  CV::NavCB<Analog,Mat,Nav>::execute ( id, m, item, _navigatorV, speed, dir, this );

  if ( MenuKit::MESSAGE_SELECTED == m )
  {
    OsgTools::Group::removeAllChildren( _cursorActiveWithRot.get() );
    OsgTools::Group::removeAllChildren( _cursorActiveNoRot.get() );
    if ( _navigatorV.valid() && id == _navigatorV->id() )
    {
      OsgTools::Axes vertTrans;
      vertTrans.state( OsgTools::Axes::POSITIVE_Z |
                       OsgTools::Axes::NEGATIVE_Z );
      _cursorActiveWithRot->addChild( vertTrans() );
    }
  }
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
  typedef CV::Functors::JoystickVertical Analog;
  typedef CV::Functors::IdentityMatrix Mat;
  typedef CV::Functors::Translate Nav;

  // Call the common function.
  const unsigned int id ( 3775941120u );
  float speed ( _prefs->translationSpeed() );
  CV::Functors::Direction::Vector dir ( 0, 0, -1 );
  CV::NavCB<Analog,Mat,Nav>::execute ( id, m, item, _navigatorV, speed, dir, this );

  if ( MenuKit::MESSAGE_SELECTED == m )
  {
    OsgTools::Group::removeAllChildren( _cursorActiveWithRot.get() );
    OsgTools::Group::removeAllChildren( _cursorActiveNoRot.get() );
    if ( _navigatorV.valid() && id == _navigatorV->id() )
    {
      OsgTools::Axes vertTrans;
      vertTrans.state( OsgTools::Axes::POSITIVE_Z |
                       OsgTools::Axes::NEGATIVE_Z );
      _cursorActiveNoRot->addChild( vertTrans() );
    }
  }
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
  typedef CV::Functors::JoystickVertical Analog;
  typedef CV::Functors::WandRotation Mat;
  typedef CV::Functors::Translate Nav;

  // The id for this navigator.
  const unsigned int id ( 1084143896u );
  float speed ( _prefs->translationSpeed() );
  CV::Functors::Direction::Vector dir ( 0, 0, 1 );
  CV::NavCB<Analog,Mat,Nav>::execute ( id, m, item, _navigatorV, speed, dir, this );

  if ( MenuKit::MESSAGE_SELECTED == m )
  {
    OsgTools::Group::removeAllChildren( _cursorActiveWithRot.get() );
    OsgTools::Group::removeAllChildren( _cursorActiveNoRot.get() );
    if ( _navigatorV.valid() && id == _navigatorV->id() )
    {
      OsgTools::Axes vertTrans;
      vertTrans.state( OsgTools::Axes::POSITIVE_Z |
                       OsgTools::Axes::NEGATIVE_Z );
      _cursorActiveWithRot->addChild( vertTrans() );
    }
  }
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
  typedef CV::Functors::JoystickVertical Analog;
  typedef CV::Functors::IdentityMatrix Mat;
  typedef CV::Functors::Translate Nav;

  // The id for this navigator.
  const unsigned int id ( 3683303793u );
  float speed ( _prefs->translationSpeed() );
  CV::Functors::Direction::Vector dir ( 0, 0, 1 );
  CV::NavCB<Analog,Mat,Nav>::execute ( id, m, item, _navigatorV, speed, dir, this );

  if ( MenuKit::MESSAGE_SELECTED == m )
  {
    OsgTools::Group::removeAllChildren( _cursorActiveWithRot.get() );
    OsgTools::Group::removeAllChildren( _cursorActiveNoRot.get() );
    if ( _navigatorV.valid() && id == _navigatorV->id() )
    {
      OsgTools::Axes vertTrans;
      vertTrans.state( OsgTools::Axes::POSITIVE_Z |
                       OsgTools::Axes::NEGATIVE_Z );
      _cursorActiveNoRot->addChild( vertTrans() );
    }
  }
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
  typedef CV::Functors::JoystickHorizontal Analog;
  typedef CV::Functors::WandRotation Mat;
  typedef CV::Functors::Rotate Nav;

  // The id for this navigator.
  const unsigned int id ( 2200874009u );
  float speed ( _prefs->rotationSpeed() );
  CV::Functors::Direction::Vector dir ( 0, 1, 0 );
  CV::NavCB<Analog,Mat,Nav>::execute ( id, m, item, _navigatorH, speed, dir, this );

  if ( MenuKit::MESSAGE_SELECTED == m )
  {
    OsgTools::Group::removeAllChildren( _cursorActiveWithRot.get() );
    OsgTools::Group::removeAllChildren( _cursorActiveNoRot.get() );
    if ( _navigatorH.valid() && id == _navigatorH->id() )
    {
      OsgTools::Axes horizRot;
      horizRot.state( OsgTools::Axes::ROTATE_Y );
      _cursorActiveWithRot->addChild( horizRot() );
    }
  }
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
  typedef CV::Functors::JoystickHorizontal Analog;
  typedef CV::Functors::IdentityMatrix Mat;
  typedef CV::Functors::Rotate Nav;

  // The id for this navigator.
  const unsigned int id ( 2093083320u );
  float speed ( _prefs->rotationSpeed() );
  CV::Functors::Direction::Vector dir ( 0, 1, 0 );
  CV::NavCB<Analog,Mat,Nav>::execute ( id, m, item, _navigatorH, speed, dir, this );

  if ( MenuKit::MESSAGE_SELECTED == m )
  {
    OsgTools::Group::removeAllChildren( _cursorActiveWithRot.get() );
    OsgTools::Group::removeAllChildren( _cursorActiveNoRot.get() );
    if ( _navigatorH.valid() && id == _navigatorH->id() )
    {
      OsgTools::Axes horizRot;
      horizRot.state( OsgTools::Axes::ROTATE_Y );
      _cursorActiveNoRot->addChild( horizRot() );
    }
  }
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
  typedef CV::Functors::JoystickVertical Analog;
  typedef CV::Functors::IdentityMatrix Mat;
  typedef CV::Functors::Translate Nav;

  // The id for this navigator.
  const unsigned int id ( 1084438318u );
  float speed ( _prefs->translationSpeed() );
  CV::Functors::Direction::Vector dir ( 0, 1, 0 );
  CV::NavCB<Analog,Mat,Nav>::execute ( id, m, item, _navigatorV, speed, dir, this );

  if ( MenuKit::MESSAGE_SELECTED == m )
  {
    OsgTools::Group::removeAllChildren( _cursorActiveWithRot.get() );
    OsgTools::Group::removeAllChildren( _cursorActiveNoRot.get() );
    if ( _navigatorV.valid() && id == _navigatorV->id() )
    {
      OsgTools::Axes vertTrans;
      vertTrans.state( OsgTools::Axes::POSITIVE_Y |
                       OsgTools::Axes::NEGATIVE_Y );
      _cursorActiveNoRot->addChild( vertTrans() );
    }
  }
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
  typedef CV::Functors::Direction Dir;
  typedef Dir::Vector Vec;
  typedef CV::Functors::WandRotation MF;
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
        const VRV::Prefs::Settings::Color &c = _prefs->selectionColor();
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
      item->checked ( OsgTools::State::StateSet::getPolygonsFilled ( _models.get(), false ) &&
                      OsgTools::State::StateSet::getPolygonsSmooth ( _models.get() ) );
      break;

    case MenuKit::MESSAGE_SELECTED:
      OsgTools::State::StateSet::setPolygonsFilled ( _models.get(), false );
      OsgTools::State::StateSet::setPolygonsSmooth ( _models.get() );
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
      item->checked ( OsgTools::State::StateSet::getPolygonsFilled ( _models.get(), false ) &&
                      OsgTools::State::StateSet::getPolygonsFlat   ( _models.get() ) );
      break;

    case MenuKit::MESSAGE_SELECTED:
      OsgTools::State::StateSet::setPolygonsFilled ( _models.get(), false );
      OsgTools::State::StateSet::setPolygonsFlat   ( _models.get() );
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
      item->checked ( OsgTools::State::StateSet::getPolygonsLines ( _models.get(), false ) );
      break;

    case MenuKit::MESSAGE_SELECTED:
      OsgTools::State::StateSet::setPolygonsLines ( _models.get(), true );
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
      item->checked ( OsgTools::State::StateSet::getPolygonsPoints ( _models.get(), false ) );
      break;

    case MenuKit::MESSAGE_SELECTED:
      OsgTools::State::StateSet::setPolygonsPoints ( _models.get(), true );
      break;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
// Draw polygons with scribe effect
//
///////////////////////////////////////////////////////////////////////////////

void Application::_polysScribe ( MenuKit::Message m, MenuKit::Item *item )
{
  ErrorChecker ( 1084120995u, isAppThread(), CV::NOT_APP_THREAD );

  // Process the message.
  switch ( m )
  {
    case MenuKit::MESSAGE_UPDATE:
      item->checked ( _navBranch->containsNode ( _scribeBranch.get() ) );
      break;

    case MenuKit::MESSAGE_SELECTED:
      if ( item->checked() && _navBranch->containsNode ( _scribeBranch.get() ) )
        OsgTools::Group::removeAllOccurances ( _scribeBranch.get(), _navBranch.get() );
      else
        _navBranch->addChild ( _scribeBranch.get() );
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
      //item->checked ( OsgTools::State::StateSet::getPolygonsTextures ( _models->getStateSet() ) );
      item->checked ( _textures );
      break;

    case MenuKit::MESSAGE_SELECTED:
      //bool state = OsgTools::State::StateSet::getPolygonsTextures ( _models->getStateSet() );
      //OsgTools::State::StateSet::setPolygonsTextures (  _models->getStateSet() , state );
      _textures = !_textures;
      OsgTools::State::StateSet::setPolygonsTextures( _models->getOrCreateStateSet(), _textures );
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

  // Process the message.
  switch ( m )
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
  }
}


///////////////////////////////////////////////////////////////////////////////
//
// Set the status-bar visibility.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_statusBarVis ( MenuKit::Message m, MenuKit::Item *item )
{
  ErrorChecker ( 1552310702u, isAppThread(), CV::NOT_APP_THREAD );

  // Process the message.
  switch ( m )
  {
    case MenuKit::MESSAGE_UPDATE:
      item->checked ( _statusBar->menu()->expanded() );
      break;

    case MenuKit::MESSAGE_SELECTED:
      _statusBar->menu()->expanded ( !_statusBar->menu()->expanded() );
      _statusBar->updateScene();
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
      if( dynamic_cast<MenuKit::Button*>( item ) )
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
      _navBranch->setMatrix ( _home );
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
    osg::Matrix original ( _models->getMatrix() );

    // Perform the "view all" on the model's branch.
    this->viewAll ( _models.get(), _prefs->viewAllScaleZ() );
    this->viewAll ( _scribeBranch.get(), _prefs->viewAllScaleZ() );

    // Move the navigation branch.
    _navBranch->setMatrix ( _models->getMatrix() );

    // Restore the model's matrix.
    _models->setMatrix ( original );
    _scribeBranch->setMatrix ( original );

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
      this->viewAll ( _navBranch.get(), _prefs->viewAllScaleZ() );
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
  typedef CV::Functors::JoystickVertical Analog;
  typedef CV::Functors::ScaleTool Tool;
  typedef Tool::Vector Vector;

  // The id for this navigator.
  const unsigned int id ( 2747412269u );

  // The scale "directions" and the speed.
  float speed ( _prefs->scaleSpeed() );
  Tool::Vector scale ( 1, 1, 1 );

  // Put the models-branch in a vector.
  Tool::Transforms vt;
  vt.push_back ( _models.get() );
  vt.push_back ( _scribeBranch.get() );

  // Call the convenience function.
  CV::ScaleCB<Analog,Tool>::execute ( id, m, item, _sceneTool, vt, speed, scale, this );

  if ( MenuKit::MESSAGE_SELECTED == m )
  {
    OsgTools::Group::removeAllChildren( _cursorActiveWithRot.get() );
    OsgTools::Group::removeAllChildren( _cursorActiveNoRot.get() );
    if ( _sceneTool.valid() && id == _sceneTool->id() )
    {
      _cursorActiveNoRot->addChild( _cursor_zoom.get() );
    }
  }
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
  typedef CV::Functors::JoystickVertical Analog;
  typedef CV::Functors::ScaleTool Tool;
  typedef Tool::Vector Vector;

  // The id for this navigator.
  const unsigned int id ( 1612324357u );

  // The scale "directions" and the speed.
  float speed ( _prefs->scaleSpeed() );
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

  typedef CV::Functors::ToolPair ToolPair;
  typedef CV::Functors::MoveTool MoveTool;
  typedef CV::Functors::JoystickHorizontal JH;
  typedef CV::Functors::JoystickVertical JV;
  typedef CV::Functors::MatrixFunctor MF;
  typedef CV::Functors::WandRotation WR;
  typedef CV::Functors::Direction Dir;
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
      float speed ( _prefs->scaleSpeed() );

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
  typedef CV::Functors::JoystickHorizontal JH;
  typedef CV::Functors::JoystickVertical JV;
  typedef CV::Functors::MatrixFunctor MF;
  typedef CV::Functors::WandRotation WR;
  typedef CV::Functors::Direction Dir;
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
      float speed ( _prefs->scaleSpeed() );

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
      vt.push_back ( _models.get() );
      vt.push_back ( _scribeBranch.get() );
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
    this->_initGrid ( _models.get() );
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
	  for(unsigned int i=0; i<_gridFunctors.size(); ++i){
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
    _prefs->selectionColor ( c[0], c[1], c[2], c[3] );

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

void Application::_hvTransWandXZ ( MenuKit::Message m, MenuKit::Item *item )
{
  ErrorChecker ( 1084438123u, isAppThread(), CV::NOT_APP_THREAD );

  // Local id.
  const unsigned int id ( 1084438124u );

  // Process the message.
  switch ( m )
  {
    case MenuKit::MESSAGE_SELECTED:

      // If the navigator matches the given id...
      if ( _navigatorH.valid() && _navigatorV.valid() && 
           id == _navigatorH->id() && id == _navigatorV->id() )
      {
        _navigatorH = 0x0;          // invalidate the _navigators ...
        _navigatorV = 0x0;          // ... in other words, toggle them off
      }

      // Otherwise, set the navigator.
      else
      {
        // Call the other functions.
        this->_hTransWandPosX ( m, item );
        this->_vTransWandPosZ ( m, item );

        // Reset the ids.
        _navigatorH->id ( id );
        _navigatorV->id ( id );
      }

      break;

    case MenuKit::MESSAGE_UPDATE:

      item->checked ( _navigatorH.valid() && id == _navigatorH->id() && 
                      _navigatorV.valid() && id == _navigatorV->id() );
      break;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Translate with respect to the global axes
//
///////////////////////////////////////////////////////////////////////////////

void Application::_hvTransGlobalXZ ( MenuKit::Message m, MenuKit::Item *item )
{
  ErrorChecker (1084438119u, isAppThread(), CV::NOT_APP_THREAD );

  // Local id.
  const unsigned int id ( 1084438120u );

  // Process the message.
  switch ( m )
  {
    case MenuKit::MESSAGE_SELECTED:

      // If the navigator matches the given id...
      if ( _navigatorH.valid() && _navigatorV.valid() &&
           id == _navigatorH->id() && id == _navigatorV->id() )
      {
        _navigatorH = 0x0;
        _navigatorV = 0x0;
      }

      // Otherwise, set the navigator.
      else
      {
        // Call the other functions.
        this->_hTransGlobalPosX ( m, item );
        this->_vTransGlobalPosZ ( m, item );

        // Reset the ids.
        _navigatorH->id ( id );
        _navigatorV->id ( id );
      }

      break;

    case MenuKit::MESSAGE_UPDATE:

      item->checked ( _navigatorH.valid() && id == _navigatorH->id() &&
                      _navigatorV.valid() && id == _navigatorV->id() );
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

  // Local id.
  const unsigned int id ( 1084438122u );

  // Process the message.
  switch ( m )
  {
    case MenuKit::MESSAGE_SELECTED:

      // If the navigator matches the given id...
      if ( _navigatorH.valid() && _navigatorV.valid() &&
           id == _navigatorH->id() && id == _navigatorV->id() )
      {
        _navigatorH = 0x0;
        _navigatorV = 0x0;
      }

      // Otherwise, set the navigator.
      else
      {
        // Call the other functions.
        this->_hRotGlobalPosY ( m, item );
        this->_vTransGlobalPosY ( m, item );

        // Reset the ids.
        _navigatorH->id ( id );
        _navigatorV->id ( id );
      }
      break;

    case MenuKit::MESSAGE_UPDATE:

      item->checked ( _navigatorH.valid() && id == _navigatorH->id() &&
                      _navigatorV.valid() && id == _navigatorV->id() );
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
    const osg::BoundingSphere& bs = _models->getBound();
    if ( !_autoPlacement )
    {
      osg::Matrix trans( osg::Matrix::translate( osg::Vec3(0.0,0.0,-2.0*bs.radius())-bs.center() ) );
      _navBranch->setMatrix( trans );
    }
    else
    {
      _doAutoPlacement( true );
    }
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
    const osg::BoundingSphere& bs = _models->getBound();
    osg::Matrix rot  ( osg::Matrix::rotate   ( osg::PI , osg::Y_AXIS ) );
    if ( !_autoPlacement )
    {
      osg::Matrix zero ( osg::Matrix::translate( -(bs.center()) ) );
      osg::Matrix trans( osg::Matrix::translate( osg::Vec3(0.0,0.0,-2.0*bs.radius()) ) );
      _navBranch->setMatrix( zero*rot*trans );
    }
    else
    {
      _navBranch->setMatrix( rot );
      _doAutoPlacement( false );
    }
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
    const osg::BoundingSphere& bs = _models->getBound();
    osg::Matrix rot  ( osg::Matrix::rotate   ( osg::PI_2 , osg::X_AXIS ) );
    if ( !_autoPlacement )
    {
      osg::Matrix zero ( osg::Matrix::translate( -(bs.center()) ) );
      osg::Matrix trans( osg::Matrix::translate( osg::Vec3(0.0,0.0,-2.0*bs.radius()) ) );
      _navBranch->setMatrix( zero*rot*trans );
    }
    else
    {
      _navBranch->setMatrix( rot );
      _doAutoPlacement( false );
    }
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
    const osg::BoundingSphere& bs = _models->getBound();
    osg::Matrix rot  ( osg::Matrix::rotate   ( -osg::PI_2 , osg::X_AXIS ) );
    if ( !_autoPlacement )
    {
      osg::Matrix zero ( osg::Matrix::translate( -(bs.center()) ) );
      osg::Matrix trans( osg::Matrix::translate( osg::Vec3(0.0,0.0,-2.0*bs.radius()) ) );
      _navBranch->setMatrix( zero*rot*trans );
    }
    else
    {
      _navBranch->setMatrix( rot );
      _doAutoPlacement( false );
    }
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
    const osg::BoundingSphere& bs = _models->getBound();
    osg::Matrix rot  ( osg::Matrix::rotate   ( -osg::PI_2 , osg::Y_AXIS ) );
    if ( !_autoPlacement )
    {
      osg::Matrix zero ( osg::Matrix::translate( -(bs.center()) ) );
      osg::Matrix trans( osg::Matrix::translate( osg::Vec3(0.0,0.0,-2.0*bs.radius()) ) );
      _navBranch->setMatrix( zero*rot*trans );
    }
    else
    {
      _navBranch->setMatrix( rot );
      _doAutoPlacement( false );
    }
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
    const osg::BoundingSphere& bs = _models->getBound();
    osg::Matrix rot  ( osg::Matrix::rotate   ( osg::PI_2 , osg::Y_AXIS ) );
    if ( !_autoPlacement )
    {    
      osg::Matrix zero ( osg::Matrix::translate( -(bs.center()) ) );
      osg::Matrix trans( osg::Matrix::translate( osg::Vec3(0.0,0.0,-2.0*bs.radius()) ) );
      _navBranch->setMatrix( zero*rot*trans );
    }
    else
    {
      _navBranch->setMatrix( rot );
      _doAutoPlacement( false );
    }
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

    const osg::BoundingSphere& bs = _models->getBound();
    osg::Matrix rot ( osg::Matrix::rotate ( radians, vec ) );
    _models->preMult ( rot );
    _scribeBranch->preMult ( rot );
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
//  Turn on or off all animations
//
///////////////////////////////////////////////////////////////////////////////

void Application::_toggleAnimations ( MenuKit::Message m, MenuKit::Item *item )
{
  ErrorChecker ( 3042741394u, isAppThread(), CV::NOT_APP_THREAD );

  bool onOff = true;

  // Process the message.
  switch ( m )
  {
    case MenuKit::MESSAGE_UPDATE:
      item->checked ( _animations );
      break;
    
    case MenuKit::MESSAGE_SELECTED:
      if ( _animations ){
        _animations = false;
      }
      else{
        _animations = true;
      }
      osg::Node *m = dynamic_cast<osg::Node*>( _models.get() );
      if (m){
        _animationsOnOff ( _animations, m );
      }
      break;
  }
}

///////////////////////////////////////////////////////////////////////////////
//
//  Step Animation
//
///////////////////////////////////////////////////////////////////////////////


void Application::_animStepFwd ( MenuKit::Message m, MenuKit::Item *item )
{
  ErrorChecker ( 3042741394u, isAppThread(), CV::NOT_APP_THREAD );

  // Process the message.
  if( m==MenuKit::MESSAGE_SELECTED )
  {
    osg::Node *m = dynamic_cast<osg::Node*>( _models.get() );
    if (m){
      this->_animStep( 5,  m );
    }
  }
}

