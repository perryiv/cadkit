
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Aashish Chaudhary
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "LayerManipulator.h"

void Magrathea::LayerManipulator::opacity( const float& val )
{
  if( _land.valid() )
  {
    _land->setCurrentFragementShaderType( ossimPlanetShaderProgramSetup::OPACITY );    
    osg::ref_ptr< osg::Uniform > param = _land->getCurrentShader()->getUniform( "param" );
    if( param.valid() )
    {
       param->set( val );
    }
  }
}

float Magrathea::LayerManipulator::opacity() const 
{
  float opacity( 1.0 );
  if( _land.valid() )
  {
    _land->setCurrentFragementShaderType( ossimPlanetShaderProgramSetup::OPACITY );    
    osg::ref_ptr< osg::Uniform > param = _land->getCurrentShader()->getUniform( "param" );
    if( param.valid() )
    {
       param->get( opacity );
    }
  }

  return opacity;
}

void Magrathea::LayerManipulator::top()
{
  _land->setCurrentFragementShaderType( ossimPlanetShaderProgramSetup::TOP );
}

void Magrathea::LayerManipulator::reference()
{
  _land->setCurrentFragementShaderType( ossimPlanetShaderProgramSetup::REFERENCE );
}

void Magrathea::LayerManipulator::absoluteDifference()
{
  _land->setCurrentFragementShaderType( ossimPlanetShaderProgramSetup::ABSOLUTE_DIFFERENCE );
}

void Magrathea::LayerManipulator::falseColorReplacement()
{
  _land->setCurrentFragementShaderType( ossimPlanetShaderProgramSetup::FALSE_COLOR_REPLACEMENT );
}

void Magrathea::LayerManipulator::setLand( ossimPlanetLand* land )
{
  _land = const_cast< ossimPlanetLand* >( land );
}
