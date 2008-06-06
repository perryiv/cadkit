
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
#include "ToolFactory.h"
#include "ToolCallback.h"
#include "SceneFunctors.h"
#include "ScenePredicates.h"

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
//  The ray-intersector callback.
//
///////////////////////////////////////////////////////////////////////////////
#if 0
void Application::_raySelector ( MenuKit::Message m, MenuKit::Item *item )
{
  ErrorChecker ( 3690175917u, isAppThread(), CV::NOT_APP_THREAD );

  // To shorten up the lines.
  typedef Usul::Functors::Interaction::Navigate::Direction Dir;
  typedef Usul::Functors::Interaction::Wand::WandRotation MF;
  typedef Dir::Vector Vec;
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
#endif



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
#if 0
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
#endif



///////////////////////////////////////////////////////////////////////////////
//
// Scale the world with the vertical joystick.
//
///////////////////////////////////////////////////////////////////////////////
#if 0
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
#endif

///////////////////////////////////////////////////////////////////////////////
//
// Scale the selected geometry with the vertical joystick.
//
///////////////////////////////////////////////////////////////////////////////

#if 0
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
#endif
}
#endif

///////////////////////////////////////////////////////////////////////////////
//
// Move the selected geometry.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_wMoveSelLocal ( MenuKit::Message m, MenuKit::Item *item )
{
  ErrorChecker ( 2457117960u, isAppThread(), CV::NOT_APP_THREAD );
#if 0
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
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
// Move the selected geometry.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_wMoveTopLocal ( MenuKit::Message m, MenuKit::Item *item )
{
  ErrorChecker ( 1084528454u, isAppThread(), CV::NOT_APP_THREAD );
#if 0
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
#endif
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

