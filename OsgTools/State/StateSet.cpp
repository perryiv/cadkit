///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV and John K. Grant
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Class for working with the state.
//
///////////////////////////////////////////////////////////////////////////////

#include "OsgTools/State/StateSet.h"

#include "Usul/Pointers/Pointers.h"
#include "Usul/Bits/Bits.h"

#include "osg/Node"
#include "osg/StateSet"
#include "osg/PolygonMode"
#include "osg/PolygonOffset"
#include "osg/ShadeModel"
#include "osg/LightModel"
#include "osg/LineWidth"
#include "osg/Material"

#include <stdexcept>
#include <iostream>

using namespace OsgTools;
using namespace OsgTools::State;


///////////////////////////////////////////////////////////////////////////////
//
//  Get the lighting state.
//
///////////////////////////////////////////////////////////////////////////////

bool StateSet::getLighting ( const osg::StateSet *ss )
{
  // Handle bad input
  if ( 0x0 == ss )
    return false;

  // Query the mode.
  return ( osg::StateAttribute::ON == ss->getMode ( GL_LIGHTING ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the lighting state.
//
///////////////////////////////////////////////////////////////////////////////

void StateSet::setLighting ( osg::StateSet *ss, bool state )
{
  // Handle bad input
  if ( 0x0 == ss )
    return;

  // Apply the mode settings
  ss->setMode ( GL_LIGHTING, ( ( state ) ? osg::StateAttribute::ON : osg::StateAttribute::OFF ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the lighting state.
//
///////////////////////////////////////////////////////////////////////////////

bool StateSet::getLighting ( osg::Node *node )
{
  return StateSet::getLighting ( ( node ) ? node->getOrCreateStateSet() : 0x0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the lighting state.
//
///////////////////////////////////////////////////////////////////////////////

void StateSet::setLighting ( osg::Node *node, bool state )
{
  StateSet::setLighting ( ( node ) ? node->getOrCreateStateSet() : 0x0, state );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the normalization state.
//
///////////////////////////////////////////////////////////////////////////////

void StateSet::setNormalize ( osg::Node *node, bool on )
{
  // Handle bad input.
  if ( 0x0 == node )
    return;

  // Get the state set, or make one.
  USUL_VALID_REF_POINTER(osg::StateSet) ss ( node->getOrCreateStateSet() );

  // Set the mode.
  ss->setMode ( GL_NORMALIZE, ( ( on ) ? osg::StateAttribute::ON : osg::StateAttribute::OFF ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the normalization state.
//
///////////////////////////////////////////////////////////////////////////////

bool StateSet::getNormalize ( osg::Node *node )
{
  // Handle bad input.
  if ( 0x0 == node )
    return false;

  // Get the state set.
  osg::ref_ptr<osg::StateSet> ss ( node->getOrCreateStateSet() );
  if ( false == ss.valid() )
    return false;

  // Query the mode.
  return ( osg::StateAttribute::ON == ss->getMode ( GL_NORMALIZE ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function for polygon-mode.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  void setPolygonMode ( osg::Node *node, bool twoSided, osg::PolygonMode::Mode mode )
  {
    // Handle bad input.
    if ( 0x0 == node )
      return;

    // Get the state set, or make one.
    USUL_VALID_REF_POINTER(osg::StateSet) ss ( node->getOrCreateStateSet() );

    // Make a polygon-mode.
    USUL_VALID_REF_POINTER(osg::PolygonMode) pm ( new osg::PolygonMode );

    // Set the face.
    osg::PolygonMode::Face face ( ( twoSided ) ? 
                                  osg::PolygonMode::FRONT_AND_BACK : 
                                  osg::PolygonMode::FRONT );

    // Set the mode.
    pm->setMode ( face, mode );

    // Set the state. Make it override any other similar states.
    ss->setAttributeAndModes ( pm.get(), 
                               osg::StateAttribute::OVERRIDE | 
                               osg::StateAttribute::ON );
  }
};


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function for quering the polygon mode.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  bool hasPolygonMode ( osg::Node *node, bool twoSided, osg::PolygonMode::Mode mode )
  {
    // Handle bad input.
    if ( 0x0 == node )
      return false;

    // Get the state set.
    osg::StateSet *ss = node->getStateSet();
    if ( 0x0 == ss )
      return false;

    // Get the polygon-mode attribute, if any.
    osg::StateAttribute *sa = ss->getAttribute ( osg::StateAttribute::POLYGONMODE );
    if ( 0x0 == sa )
      return false;

    // Should be true.
    osg::PolygonMode *pm = dynamic_cast < osg::PolygonMode * > ( sa );
    if ( 0x0 == pm )
      return false;

    // Set the face.
    osg::PolygonMode::Face face ( ( twoSided ) ? 
                                  osg::PolygonMode::FRONT_AND_BACK : 
                                  osg::PolygonMode::FRONT );

    // Is the mode the same?
    return ( pm->getMode ( face ) == mode );
  }
};


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function for setting the shading models.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  void setShadeModel ( osg::Node *node, osg::ShadeModel::Mode mode )
  {
    // Handle bad input.
    if ( 0x0 == node )
      return;

    // Get the state set, or make one.
    USUL_VALID_REF_POINTER(osg::StateSet) ss ( node->getOrCreateStateSet() );

    // Make a shade-model.
    USUL_VALID_REF_POINTER(osg::ShadeModel) sm ( new osg::ShadeModel );

    // Set the mode.
    sm->setMode ( mode );

    // Set the state. Make it override any other similar states.
    ss->setAttributeAndModes ( sm.get(), 
                               osg::StateAttribute::OVERRIDE | 
                               osg::StateAttribute::ON );
  }
};


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function for quering the shading models.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  bool hasShadeModel ( osg::Node *node, osg::ShadeModel::Mode mode )
  {
    // Handle bad input.
    if ( 0x0 == node )
      return false;

    // Get the state set.
    osg::StateSet *ss = node->getStateSet();
    if ( 0x0 == ss )
      return false;

    // Get the shading attribute, if any.
    osg::StateAttribute *sa = ss->getAttribute ( osg::StateAttribute::SHADEMODEL );
    if ( 0x0 == sa )
      return false;

    // Should be true.
    osg::ShadeModel *sm = dynamic_cast < osg::ShadeModel * > ( sa );
    if ( 0x0 == sm )
      return false;

    // See if th emode is the same.
    return ( sm->getMode() == mode );
  }
};


///////////////////////////////////////////////////////////////////////////////
//
//  Make polygons draw filled.
//
///////////////////////////////////////////////////////////////////////////////

void StateSet::setPolygonsFilled ( osg::Node *node, bool twoSided )
{
  Helper::setPolygonMode ( node, twoSided, osg::PolygonMode::FILL );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the polygon state.
//
///////////////////////////////////////////////////////////////////////////////

bool StateSet::getPolygonsFilled ( osg::Node *node, bool twoSided )
{
  return Helper::hasPolygonMode ( node, twoSided, osg::PolygonMode::FILL );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make polygons draw with lines.
//
///////////////////////////////////////////////////////////////////////////////

void StateSet::setPolygonsLines ( osg::Node *node, bool twoSided )
{
  Helper::setPolygonMode ( node, twoSided, osg::PolygonMode::LINE );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the polygon state.
//
///////////////////////////////////////////////////////////////////////////////

bool StateSet::getPolygonsLines ( osg::Node *node, bool twoSided )
{
  return Helper::hasPolygonMode ( node, twoSided, osg::PolygonMode::LINE );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make polygons draw with points.
//
///////////////////////////////////////////////////////////////////////////////

void StateSet::setPolygonsPoints ( osg::Node *node, bool twoSided )
{
  Helper::setPolygonMode ( node, twoSided, osg::PolygonMode::POINT );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the polygon state.
//
///////////////////////////////////////////////////////////////////////////////

bool StateSet::getPolygonsPoints ( osg::Node *node, bool twoSided )
{
  return Helper::hasPolygonMode ( node, twoSided, osg::PolygonMode::POINT );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make filled polygons draw with smooth shading.
//
///////////////////////////////////////////////////////////////////////////////

void StateSet::setPolygonsSmooth ( osg::Node *node )
{
  Helper::setShadeModel ( node, osg::ShadeModel::SMOOTH );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the polygon shading model.
//
///////////////////////////////////////////////////////////////////////////////

bool StateSet::getPolygonsSmooth ( osg::Node *node )
{
  return Helper::hasShadeModel ( node, osg::ShadeModel::SMOOTH );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make filled polygons draw with flat shading.
//
///////////////////////////////////////////////////////////////////////////////

void StateSet::setPolygonsFlat ( osg::Node *node )
{
  Helper::setShadeModel ( node, osg::ShadeModel::FLAT );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the polygon shading model.
//
///////////////////////////////////////////////////////////////////////////////

bool StateSet::getPolygonsFlat ( osg::Node *node )
{
  return Helper::hasShadeModel ( node, osg::ShadeModel::FLAT );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the Texture state
//
///////////////////////////////////////////////////////////////////////////////

void StateSet::setPolygonsTextures ( osg::StateSet* ss , bool on )
{
  if ( 0x0 == ss )
    return;

  if ( on ) // Turn on textures.
  {
    std::cout << "turning on textures" << std::endl;
    ss->setTextureMode(0,GL_TEXTURE_2D,osg::StateAttribute::INHERIT|osg::StateAttribute::ON);
  }

  else
  {
    std::cout << "turning off textures" << std::endl;
    ss->setTextureMode(0,GL_TEXTURE_2D,osg::StateAttribute::OVERRIDE|osg::StateAttribute::OFF);
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the Texture state
//
///////////////////////////////////////////////////////////////////////////////

bool StateSet::getPolygonsTextures ( osg::StateSet* ss )
{
  if( 0x0 == ss )
    return false;

  osg::StateAttribute::GLModeValue texture_value = ss->getTextureMode(0,GL_TEXTURE_2D);

  std::cout << "get: value=" << std::endl;

  // If 2D textures are turned on...
  if ( texture_value == ( osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE ) )
  {
    std::cout << "should return false" << std::endl;
    return false;
  }

  else
  {
    std::cout << "should return true" << std::endl;
    return true;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the two sided lighting state.
//
///////////////////////////////////////////////////////////////////////////////

bool StateSet::getTwoSidedLighting ( const osg::StateSet *ss )
{
  if ( 0x0 == ss )
    return false;

  // Get the light-model attribute, if any.
  const osg::StateAttribute *sa = ss->getAttribute ( osg::StateAttribute::LIGHTMODEL );
  if ( 0x0 == sa )
    return false;

  // Should be true.
  const osg::LightModel *lm = dynamic_cast < const osg::LightModel * > ( sa );
  if ( 0x0 == lm )
    return false;

  //Is two sided light on?
  return ( lm->getTwoSided() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the two sided lighting state.
//
///////////////////////////////////////////////////////////////////////////////

void StateSet::setTwoSidedLighting ( osg::StateSet *ss, bool state )
{
  if ( 0x0 == ss )
    return;

  // Initialize.
  osg::ref_ptr<osg::LightModel> lm ( 0x0 );

  // Get the light-model attribute, if any.
  osg::StateAttribute *sa = ss->getAttribute ( osg::StateAttribute::LIGHTMODEL );
  if ( 0x0 == sa )
    lm = new osg::LightModel;
  else
    lm = dynamic_cast < osg::LightModel * > ( sa );

  lm->setTwoSided( state );

  // Set the state. Make it override any other similar states.
  typedef osg::StateAttribute Attribute;
  ss->setAttributeAndModes ( lm.get(), Attribute::OVERRIDE | Attribute::ON );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the two sided lighting state.
//
///////////////////////////////////////////////////////////////////////////////

bool StateSet::getTwoSidedLighting ( osg::Node *node )
{
  return StateSet::getTwoSidedLighting ( ( node ) ? node->getOrCreateStateSet() : 0x0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the two sided lighting state.
//
///////////////////////////////////////////////////////////////////////////////

void StateSet::setTwoSidedLighting ( osg::Node *node, bool state )
{
  StateSet::setTwoSidedLighting ( ( node ) ? node->getOrCreateStateSet() : 0x0, state );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to set the line width.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  void setLineWidth ( osg::StateSet* ss, float width )
  {
    osg::ref_ptr<osg::LineWidth> lw;

    // Get the shade-model attribute, if any.
    osg::StateAttribute *sa = ss->getAttribute ( osg::StateAttribute::LINEWIDTH );
    if ( 0x0 == sa )
      lw = new osg::LineWidth;
    else
      lw = dynamic_cast < osg::LineWidth * > ( sa );

    lw->setWidth( width );

    // Set the state. Make it override any other similar states.
    typedef osg::StateAttribute Attribute;
    ss->setAttributeAndModes ( lw.get(), Attribute::OVERRIDE | Attribute::ON );
  }
}

///////////////////////////////////////////////////////////////////////////////
//
//  Set line width.
//
///////////////////////////////////////////////////////////////////////////////

void StateSet::setLineWidth ( osg::StateSet *ss, float width )
{
  Helper::setLineWidth ( ss, width );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set line width.
//
///////////////////////////////////////////////////////////////////////////////

void StateSet::setLineWidth ( osg::Node *node, float width )
{
  // Get or create the state set.
  osg::ref_ptr<osg::StateSet> ss ( node->getOrCreateStateSet() );
  Helper::setLineWidth ( ss.get(), width );
}


///////////////////////////////////////////////////////////////////////////////
//
//   Get line width.
//
///////////////////////////////////////////////////////////////////////////////

float StateSet::getLineWidth ( osg::Node *node )
{
  // Get or create the state set.
  osg::ref_ptr<osg::StateSet> ss ( node->getOrCreateStateSet() );

  // Get the shade-model attribute, if any.
  const osg::StateAttribute *sa = ss->getAttribute ( osg::StateAttribute::LINEWIDTH );
  if ( 0x0 == sa )
    return 1.0;

  // Should be true.
  const osg::LineWidth *lw = dynamic_cast < const osg::LineWidth * > ( sa );
  if ( 0x0 == lw )
    return 1.0;

  // Get the line width
  return ( lw->getWidth() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make the state for hidden lines.
//
///////////////////////////////////////////////////////////////////////////////

void StateSet::hiddenLines ( const osg::Vec4f &color, osg::StateSet *normal, osg::StateSet *hidden )
{
  // Handle bad input.
  if ( 0x0 == normal && 0x0 == hidden )
    return;

  // Set the polygon-mode.
  osg::ref_ptr<osg::PolygonMode> mode ( new osg::PolygonMode );
  mode->setMode ( osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE );
  hidden->setAttributeAndModes ( mode.get(), osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON );

  // Set the polygon-offset. See osgscribe example.
  osg::ref_ptr<osg::PolygonOffset> offset ( new osg::PolygonOffset );
  offset->setFactor ( -1.0f );
  offset->setUnits  ( -1.0f );
  hidden->setAttributeAndModes ( offset.get(), osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON );

  // Material same as background and no lighting.
  osg::ref_ptr<osg::Material> material ( new osg::Material );
  material->setAmbient ( osg::Material::FRONT_AND_BACK, color );
  material->setDiffuse ( osg::Material::FRONT_AND_BACK, color );
  normal->setAttributeAndModes ( material.get(), osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON );
  normal->setMode ( GL_LIGHTING, osg::StateAttribute::OVERRIDE | osg::StateAttribute::OFF );
}
