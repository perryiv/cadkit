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

#include "Usul/Adaptors/Random.h"
#include "Usul/Bits/Bits.h"

#include "osg/Node"
#include "osg/StateSet"
#include "osg/PolygonMode"
#include "osg/PolygonOffset"
#include "osg/ShadeModel"
#include "osg/LightModel"
#include "osg/LineWidth"
#include "osg/Point"
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
  osg::ref_ptr<osg::StateSet> ss ( node->getOrCreateStateSet() );

  // Set the mode.
  ss->setMode ( GL_NORMALIZE, ( ( on ) ? osg::StateAttribute::ON : osg::StateAttribute::OFF ) | osg::StateAttribute::PROTECTED );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the normalization state.
//
///////////////////////////////////////////////////////////////////////////////

void StateSet::setNormalize ( osg::StateSet *ss, bool on )
{
  // Handle bad input.
  if ( 0x0 == ss )
    return;

  // Set the mode.
  ss->setMode ( GL_NORMALIZE, ( ( on ) ? osg::StateAttribute::ON : osg::StateAttribute::OFF ) | osg::StateAttribute::PROTECTED );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the normalization state.
//
///////////////////////////////////////////////////////////////////////////////

bool StateSet::getNormalize ( const osg::Node *node )
{
  // Handle bad input.
  if ( 0x0 == node )
    return false;

  // Get the state set.
  osg::ref_ptr< const osg::StateSet > ss ( node->getStateSet() );
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
    osg::ref_ptr<osg::StateSet> ss ( node->getOrCreateStateSet() );

    // Make a polygon-mode.
    osg::ref_ptr<osg::PolygonMode> pm ( new osg::PolygonMode );

    // Set the face.
    osg::PolygonMode::Face face ( ( twoSided ) ? 
                                  osg::PolygonMode::FRONT_AND_BACK : 
                                  osg::PolygonMode::FRONT );

    // Set the mode.
    pm->setMode ( face, mode );

    // Set the state. Make it override any other similar states.
    ss->setAttributeAndModes ( pm.get(), 
                               osg::StateAttribute::OVERRIDE | 
                               osg::StateAttribute::ON |
                               osg::StateAttribute::PROTECTED );
  }
};


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function for quering the polygon mode.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  bool hasPolygonMode ( const osg::Node *node, bool twoSided, osg::PolygonMode::Mode mode )
  {
    // Handle bad input.
    if ( 0x0 == node )
      return false;

    // Get the state set.
    const osg::StateSet *ss = node->getStateSet();
    if ( 0x0 == ss )
      return false;

    // Get the polygon-mode attribute, if any.
    const osg::StateAttribute *sa = ss->getAttribute ( osg::StateAttribute::POLYGONMODE );
    if ( 0x0 == sa )
      return false;

    // Should be true.
    const osg::PolygonMode *pm = dynamic_cast < const osg::PolygonMode * > ( sa );
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
    osg::ref_ptr<osg::StateSet> ss ( node->getOrCreateStateSet() );

    // Make a shade-model.
    osg::ref_ptr<osg::ShadeModel> sm ( new osg::ShadeModel );

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
  bool hasShadeModel ( const osg::Node *node, osg::ShadeModel::Mode mode )
  {
    // Handle bad input.
    if ( 0x0 == node )
      return false;

    // Get the state set.
    const osg::StateSet *ss = node->getStateSet();
    if ( 0x0 == ss )
      return false;

    // Get the shading attribute, if any.
    const osg::StateAttribute *sa = ss->getAttribute ( osg::StateAttribute::SHADEMODEL );
    if ( 0x0 == sa )
      return false;

    // Should be true.
    const osg::ShadeModel *sm = dynamic_cast < const osg::ShadeModel * > ( sa );
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

bool StateSet::getPolygonsFilled ( const osg::Node *node, bool twoSided )
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

bool StateSet::getPolygonsLines ( const osg::Node *node, bool twoSided )
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

bool StateSet::getPolygonsPoints ( const osg::Node *node, bool twoSided )
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

bool StateSet::getPolygonsSmooth ( const osg::Node *node )
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

bool StateSet::getPolygonsFlat ( const osg::Node *node )
{
  return Helper::hasShadeModel ( node, osg::ShadeModel::FLAT );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the Texture state
//
///////////////////////////////////////////////////////////////////////////////

void StateSet::setPolygonsTextures ( osg::StateSet* ss, bool on )
{
  if ( 0x0 == ss )
    return;

  if ( on ) // Turn on textures.
  {
    ss->setTextureMode ( 0, GL_TEXTURE_2D, osg::StateAttribute::INHERIT | osg::StateAttribute::PROTECTED | osg::StateAttribute::ON );
  }

  else
  {
    ss->setTextureMode( 0, GL_TEXTURE_2D, osg::StateAttribute::OVERRIDE | osg::StateAttribute::PROTECTED | osg::StateAttribute::OFF );
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

  // If 2D textures are turned on...
  if ( texture_value == ( osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE ) )
  {
    return false;
  }

  else
  {
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
  ss->setAttributeAndModes ( lm.get(), Attribute::OVERRIDE | Attribute::ON | Attribute::PROTECTED );
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
    osg::ref_ptr<osg::LineWidth> lw ( new osg::LineWidth );
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

void StateSet::setLineWidth ( osg::Node *node, float width )
{
  StateSet::setLineWidth ( ( ( node ) ? node->getOrCreateStateSet() : 0x0 ), width );
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
//  Helper function to set the point size.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  void setPointSize ( osg::StateSet* ss, float size )
  {
    osg::ref_ptr<osg::Point> pt ( new osg::Point );
    pt->setSize ( size );

    // Set the state. Make it override any other similar states.
    typedef osg::StateAttribute Attribute;
    ss->setAttributeAndModes ( pt.get(), Attribute::OVERRIDE | Attribute::ON );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set point size.
//
///////////////////////////////////////////////////////////////////////////////

void StateSet::setPointSize ( osg::Node *node, float size )
{
  StateSet::setPointSize ( ( ( node ) ? node->getOrCreateStateSet() : 0x0 ), size );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set point size.
//
///////////////////////////////////////////////////////////////////////////////

void StateSet::setPointSize ( osg::StateSet *ss, float size )
{
  Helper::setPointSize ( ss, size );
}


///////////////////////////////////////////////////////////////////////////////
//
//   Get point size.
//
///////////////////////////////////////////////////////////////////////////////

float StateSet::getPointSize ( osg::Node *node )
{
  // Get or create the state set.
  osg::ref_ptr<osg::StateSet> ss ( node->getOrCreateStateSet() );

  // Get the shade-model attribute, if any.
  const osg::StateAttribute *sa = ss->getAttribute ( osg::StateAttribute::POINT );
  if ( 0x0 == sa )
    return 1.0f;

  // Should be true.
  const osg::Point *pt = dynamic_cast < const osg::Point * > ( sa );
  if ( 0x0 == pt )
    return 1.0f;

  // Get the line width
  return ( pt->getSize() );
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
  hidden->setAttributeAndModes ( mode.get(), osg::StateAttribute::OVERRIDE | osg::StateAttribute::PROTECTED | osg::StateAttribute::ON );

  // Set the polygon-offset. See osgscribe example.
  osg::ref_ptr<osg::PolygonOffset> offset ( new osg::PolygonOffset );
  offset->setFactor ( -1.0f );
  offset->setUnits  ( -1.0f );
  hidden->setAttributeAndModes ( offset.get(), osg::StateAttribute::OVERRIDE | osg::StateAttribute::PROTECTED | osg::StateAttribute::ON );

  // Material same as background and no lighting.
  osg::ref_ptr<osg::Material> material ( new osg::Material );
  material->setAmbient ( osg::Material::FRONT_AND_BACK, color );
  material->setDiffuse ( osg::Material::FRONT_AND_BACK, color );
  normal->setAttributeAndModes ( material.get(), osg::StateAttribute::OVERRIDE | osg::StateAttribute::PROTECTED | osg::StateAttribute::ON );
  normal->setMode ( GL_LIGHTING, osg::StateAttribute::OVERRIDE | osg::StateAttribute::PROTECTED | osg::StateAttribute::OFF );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set a random material.
//
///////////////////////////////////////////////////////////////////////////////

void StateSet::setMaterialRandom ( osg::Node *node )
{
  // Handle bad input.
  if ( 0x0 == node )
    return;

  osg::Vec4 emissive ( 0.0f, 0.0f, 0.0f, 1.0f );
  osg::Vec4 specular ( 0.2f, 0.2f, 0.2f, 1.0f );

  Usul::Adaptors::Random < float > rd ( 0.2f, 1.0f );

  osg::Vec4 diffuse ( rd(), rd(), rd(), 1.0f );
  osg::Vec4 ambient ( diffuse );

  osg::ref_ptr<osg::Material> mat ( new osg::Material );
  mat->setAmbient   ( osg::Material::FRONT_AND_BACK, ambient  );
  mat->setDiffuse   ( osg::Material::FRONT_AND_BACK, diffuse  );
  mat->setEmission  ( osg::Material::FRONT_AND_BACK, emissive );
  mat->setSpecular  ( osg::Material::FRONT_AND_BACK, specular );
  mat->setShininess ( osg::Material::FRONT_AND_BACK, 100      );

  StateSet::setMaterial ( node, mat.get() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get a default material.
//
///////////////////////////////////////////////////////////////////////////////

osg::Material *StateSet::getMaterialDefault()
{
  osg::ref_ptr<osg::Material> mat ( new osg::Material );
  mat->setAmbient   ( osg::Material::FRONT_AND_BACK, osg::Vec4 ( 1.0f, 1.0f, 1.0f, 1.0f ) );
  mat->setDiffuse   ( osg::Material::FRONT_AND_BACK, osg::Vec4 ( 1.0f, 1.0f, 1.0f, 1.0f ) );
  mat->setEmission  ( osg::Material::FRONT_AND_BACK, osg::Vec4 ( 0.0f, 0.0f, 0.0f, 1.0f ) );
  mat->setSpecular  ( osg::Material::FRONT_AND_BACK, osg::Vec4 ( 0.2f, 0.2f, 0.2f, 1.0f ) );
  mat->setShininess ( osg::Material::FRONT_AND_BACK, 100 );
  return mat.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set a default material.
//
///////////////////////////////////////////////////////////////////////////////

void StateSet::setMaterialDefault ( osg::Node *node )
{
  // Handle bad input.
  if ( 0x0 == node )
    return;

  // Pass through for backward compatability.
  StateSet::setMaterial ( node, OsgTools::State::StateSet::getMaterialDefault() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the material.
//
///////////////////////////////////////////////////////////////////////////////

void StateSet::setMaterial ( osg::Node *node, osg::Material *mat )
{
  // Handle bad input.
  if ( 0x0 == node || 0x0 == mat )
    return;

  OsgTools::State::StateSet::setMaterial ( node->getOrCreateStateSet(), mat );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the material.
//
///////////////////////////////////////////////////////////////////////////////

void StateSet::setMaterial ( osg::StateSet *ss, osg::Material *mat )
{
  // Handle bad input.
  if ( 0x0 == ss || 0x0 == mat )
    return;

  ss->setAttributeAndModes ( mat, osg::StateAttribute::OVERRIDE | osg::StateAttribute::PROTECTED | osg::StateAttribute::ON );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the material.
//
///////////////////////////////////////////////////////////////////////////////

void StateSet::setMaterial ( osg::Node *node, const osg::Vec4f &ambient, const osg::Vec4f &diffuse, float alpha )
{
  // Handle bad input.
  if ( 0x0 == node )
    return;

  osg::ref_ptr<osg::Material> mat ( OsgTools::State::StateSet::getMaterialDefault() );
  if ( false == mat.valid() )
    return;

  mat->setAmbient ( osg::Material::FRONT_AND_BACK, ambient );
  mat->setDiffuse ( osg::Material::FRONT_AND_BACK, diffuse );
  //mat->setSpecular ( osg::Material::FRONT_AND_BACK, osg::Vec4 ( 0.2, 0.2, 0.2, 1.0 ) );
  //mat->setShininess ( osg::Material::FRONT_AND_BACK, 20 );

  if ( alpha < 1 )
  {
    node->getOrCreateStateSet()->setMode ( GL_BLEND, osg::StateAttribute::OVERRIDE | osg::StateAttribute::PROTECTED | osg::StateAttribute::ON );
    node->getOrCreateStateSet()->setRenderingHint ( osg::StateSet::TRANSPARENT_BIN );
    mat->setAlpha ( osg::Material::FRONT_AND_BACK, alpha );
  }

  OsgTools::State::StateSet::setMaterial ( node, mat.get() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the alpha. Add default material if needed.
//
///////////////////////////////////////////////////////////////////////////////

void StateSet::setAlpha ( osg::Node *node, float alpha )
{
  // Handle bad input.
  if ( 0x0 == node )
    return;

  osg::ref_ptr<osg::StateSet> ss ( node->getOrCreateStateSet() );
  OsgTools::State::StateSet::setAlpha ( ss.get(), alpha );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the alpha. Add default material if needed.
//
///////////////////////////////////////////////////////////////////////////////

void StateSet::setAlpha ( osg::StateSet *ss, float alpha )
{
  // Handle bad input.
  if ( 0x0 == ss )
    return;

  // Get the material attribute.
  osg::ref_ptr<osg::Material> mat ( dynamic_cast < osg::Material * > ( ss->getAttribute ( osg::StateAttribute::MATERIAL ) ) );

  // Set a default material if there isn't one.
  if ( false == mat.valid() )
  {
    OsgTools::State::StateSet::setMaterial ( ss, OsgTools::State::StateSet::getMaterialDefault() );
    mat = dynamic_cast < osg::Material * > ( ss->getAttribute ( osg::StateAttribute::MATERIAL ) );
  }

  // Check.
  if ( false == mat.valid() )
    return;

  // Set these other properties if it's not completely opaque.
  if ( alpha < 1 )
  {
    ss->setMode ( GL_BLEND, osg::StateAttribute::OVERRIDE | osg::StateAttribute::PROTECTED | osg::StateAttribute::ON );
    ss->setRenderingHint ( osg::StateSet::TRANSPARENT_BIN );
  }

  // Set the alpha.
  mat->setAlpha ( osg::Material::FRONT_AND_BACK, alpha );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the back face culling state.
//
///////////////////////////////////////////////////////////////////////////////

bool StateSet::getBackFaceCulling ( const osg::StateSet *ss )
{
  // Handle bad input
  if ( 0x0 == ss )
    return false;
  
  // Query the mode.
  return Usul::Bits::has ( ss->getMode ( GL_CULL_FACE ), osg::StateAttribute::ON );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the back face culling state.
//
///////////////////////////////////////////////////////////////////////////////

void StateSet::setBackFaceCulling ( osg::StateSet *ss, bool state )
{
  // Handle bad input
  if ( 0x0 == ss )
    return;
  
  // Apply the mode settings
  ss->setMode ( GL_CULL_FACE, ( ( state ) ? osg::StateAttribute::ON : osg::StateAttribute::OFF ) | osg::StateAttribute::OVERRIDE | osg::StateAttribute::PROTECTED );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove the material, if there is any.
//
///////////////////////////////////////////////////////////////////////////////

void StateSet::removeMaterial ( osg::Node *node )
{
  if ( 0x0 != node )
  {
    OsgTools::State::StateSet::removeMaterial ( node->getOrCreateStateSet() );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove the material, if there is any.
//
///////////////////////////////////////////////////////////////////////////////

void StateSet::removeMaterial ( osg::StateSet *ss )
{
  // Handle bad input
  if ( 0x0 == ss )
    return;

  // Remove the material.
  ss->removeAttribute ( osg::StateAttribute::MATERIAL );
}
