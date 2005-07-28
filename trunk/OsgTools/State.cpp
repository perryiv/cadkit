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

#include "State.h"

#include "Usul/Pointers/Pointers.h"
#include "Usul/Bits/Bits.h"

#include "osg/Node"
#include "osg/StateSet"
#include "osg/PolygonMode"
#include "osg/ShadeModel"
#include "osg/LightModel"
#include "osg/LineWidth"

#include <stdexcept>
#include <iostream>

using namespace OsgTools;

///////////////////////////////////////////////////////////////////////////////
//
//  Get the lighting state.
//
///////////////////////////////////////////////////////////////////////////////

bool State::getLighting( osg::Node *node )
{
  // Handle bad input
  if( !node )
    return false;

  // Get the state set.
  osg::ref_ptr<osg::StateSet> ss = node->getOrCreateStateSet();
  if ( !ss )
    return false;

  // Query the mode.
  return ( osg::StateAttribute::ON == ss->getMode ( GL_LIGHTING ) );
}

///////////////////////////////////////////////////////////////////////////////
//
//  Set the lighting state.
//
///////////////////////////////////////////////////////////////////////////////

void State::setLighting( osg::Node *node, bool on )
{
  // handle bad input
  if( !node )
    return;

  // get the stateset
  USUL_VALID_REF_POINTER(osg::StateSet) ss( node->getOrCreateStateSet() );

  // apply the mode settings
  ss->setMode( GL_LIGHTING,((on) ? osg::StateAttribute::ON : osg::StateAttribute::OFF) );
}

///////////////////////////////////////////////////////////////////////////////
//
//  Set the normalization state.
//
///////////////////////////////////////////////////////////////////////////////

void State::setNormalize ( osg::Node *node, bool on )
{
  // Handle bad input.
  if ( !node )
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

bool State::getNormalize ( osg::Node *node )
{
  // Handle bad input.
  if ( !node )
    return false;

  // Get the state set.
  osg::ref_ptr<osg::StateSet> ss = node->getOrCreateStateSet();
  if ( !ss )
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
    if ( !node )
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
    if ( !node )
      return false;

    // Get the state set.
    osg::StateSet *ss = node->getStateSet();
    if ( !ss )
      return false;

    // Get the polygon-mode attribute, if any.
    osg::StateAttribute *sa = ss->getAttribute ( osg::StateAttribute::POLYGONMODE );
    if ( !sa )
      return false;

    // Should be true.
    osg::PolygonMode *pm = dynamic_cast < osg::PolygonMode * > ( sa );
    if ( !pm )
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
    if ( !node )
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
    if ( !node )
      return false;

    // Get the state set.
    osg::StateSet *ss = node->getStateSet();
    if ( !ss )
      return false;

    // Get the shading attribute, if any.
    osg::StateAttribute *sa = ss->getAttribute ( osg::StateAttribute::SHADEMODEL );
    if ( !sa )
      return false;

    // Should be true.
    osg::ShadeModel *sm = dynamic_cast < osg::ShadeModel * > ( sa );
    if ( !sm )
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

void State::setPolygonsFilled ( osg::Node *node, bool twoSided )
{
  Helper::setPolygonMode ( node, twoSided, osg::PolygonMode::FILL );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the polygon state.
//
///////////////////////////////////////////////////////////////////////////////

bool State::getPolygonsFilled ( osg::Node *node, bool twoSided )
{
  return Helper::hasPolygonMode ( node, twoSided, osg::PolygonMode::FILL );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make polygons draw with lines.
//
///////////////////////////////////////////////////////////////////////////////

void State::setPolygonsLines ( osg::Node *node, bool twoSided )
{
  Helper::setPolygonMode ( node, twoSided, osg::PolygonMode::LINE );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the polygon state.
//
///////////////////////////////////////////////////////////////////////////////

bool State::getPolygonsLines ( osg::Node *node, bool twoSided )
{
  return Helper::hasPolygonMode ( node, twoSided, osg::PolygonMode::LINE );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make polygons draw with points.
//
///////////////////////////////////////////////////////////////////////////////

void State::setPolygonsPoints ( osg::Node *node, bool twoSided )
{
  Helper::setPolygonMode ( node, twoSided, osg::PolygonMode::POINT );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the polygon state.
//
///////////////////////////////////////////////////////////////////////////////

bool State::getPolygonsPoints ( osg::Node *node, bool twoSided )
{
  return Helper::hasPolygonMode ( node, twoSided, osg::PolygonMode::POINT );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make filled polygons draw with smooth shading.
//
///////////////////////////////////////////////////////////////////////////////

void State::setPolygonsSmooth ( osg::Node *node )
{
  Helper::setShadeModel ( node, osg::ShadeModel::SMOOTH );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the polygon shading model.
//
///////////////////////////////////////////////////////////////////////////////

bool State::getPolygonsSmooth ( osg::Node *node )
{
  return Helper::hasShadeModel ( node, osg::ShadeModel::SMOOTH );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make filled polygons draw with flat shading.
//
///////////////////////////////////////////////////////////////////////////////

void State::setPolygonsFlat ( osg::Node *node )
{
  Helper::setShadeModel ( node, osg::ShadeModel::FLAT );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the polygon shading model.
//
///////////////////////////////////////////////////////////////////////////////

bool State::getPolygonsFlat ( osg::Node *node )
{
  return Helper::hasShadeModel ( node, osg::ShadeModel::FLAT );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the Texture state
//
///////////////////////////////////////////////////////////////////////////////

void State::setPolygonsTextures ( osg::StateSet* ss , bool on )
{
  if ( !ss )
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

bool State::getPolygonsTextures ( osg::StateSet* ss )
{
  if( !ss )
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

bool State::getTwoSidedLighting ( osg::Node *node )
{
  // Get or create the state set.
  osg::ref_ptr<osg::StateSet> ss ( node->getOrCreateStateSet() );

  // Get the shade-model attribute, if any.
  const osg::StateAttribute *sa = ss->getAttribute ( osg::StateAttribute::LIGHTMODEL );
  if ( !sa )
    return false;

  // Should be true.
  const osg::LightModel *lm = dynamic_cast < const osg::LightModel * > ( sa );
  if ( !lm )
    return false;

  //Is two sided light on?
  return ( lm->getTwoSided() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the two sided lighting state.
//
///////////////////////////////////////////////////////////////////////////////

void State::setTwoSidedLighting ( osg::Node *node, bool state )
{
  // Get or create the state set.
  osg::ref_ptr<osg::StateSet> ss ( node->getOrCreateStateSet() );

  osg::ref_ptr<osg::LightModel> lm;

  // Get the shade-model attribute, if any.
  osg::StateAttribute *sa = ss->getAttribute ( osg::StateAttribute::LIGHTMODEL );
  if ( !sa )
    lm = new osg::LightModel;
  else
    lm = dynamic_cast < osg::LightModel * > ( sa );

  lm->setTwoSided( state );

  // Set the state. Make it override any other similar states.
  typedef osg::StateAttribute Attribute;
  ss->setAttributeAndModes ( lm.get(), Attribute::OVERRIDE | Attribute::ON );
}


// 
///////////////////////////////////////////////////////////////////////////////
//
//  Set line width.
//
///////////////////////////////////////////////////////////////////////////////

void State::setLineWidth ( osg::Node *node, float width )
{
  // Get or create the state set.
  osg::ref_ptr<osg::StateSet> ss ( node->getOrCreateStateSet() );

  osg::ref_ptr<osg::LineWidth> lw;

  // Get the shade-model attribute, if any.
  osg::StateAttribute *sa = ss->getAttribute ( osg::StateAttribute::LINEWIDTH );
  if ( !sa )
    lw = new osg::LineWidth;
  else
    lw = dynamic_cast < osg::LineWidth * > ( sa );

  lw->setWidth( width );

  // Set the state. Make it override any other similar states.
  typedef osg::StateAttribute Attribute;
  ss->setAttributeAndModes ( lw.get(), Attribute::OVERRIDE | Attribute::ON );
}


///////////////////////////////////////////////////////////////////////////////
//
//   Get line width.
//
///////////////////////////////////////////////////////////////////////////////

float State::getLineWidth ( osg::Node *node )
{
  // Get or create the state set.
  osg::ref_ptr<osg::StateSet> ss ( node->getOrCreateStateSet() );

  // Get the shade-model attribute, if any.
  const osg::StateAttribute *sa = ss->getAttribute ( osg::StateAttribute::LINEWIDTH );
  if ( !sa )
    return 1.0;

  // Should be true.
  const osg::LineWidth *lw = dynamic_cast < const osg::LineWidth * > ( sa );
  if ( !lw )
    return 1.0;

  // Get the line width
  return ( lw->getWidth() );
}
