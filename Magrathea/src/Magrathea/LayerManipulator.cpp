
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Aashish Chaudhary
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "Magrathea/LayerManipulator.h"

void Magrathea::LayerManipulator::opacity( const float& val )
{
  if( _land.valid() )
  {
    _land->setCurrentFragementShaderType( ossimPlanetShaderProgramSetup::OPACITY );    
    shaderVal( val );
  }
  else
  {
    throw "Invalid ossimPlanetLand pointer";
  }
}

float Magrathea::LayerManipulator::opacity() const 
{  
  if( _land.valid() )
  {
    _land->setCurrentFragementShaderType( ossimPlanetShaderProgramSetup::OPACITY );        
    //return shaderVal();
  }  
  else
  {
    throw "Invalid ossimPlanetLand pointer";
  }
}

float Magrathea::LayerManipulator::horizontalSwipe() const
{
  if( _land.valid() )
  {
    _land->setCurrentFragementShaderType( ossimPlanetShaderProgramSetup::HORIZONTAL_SWIPE );
    return shaderVal();
  }
  else
  {
    throw "Invalid ossimPlanetLand pointer";
  }
}

void Magrathea::LayerManipulator::horizontalSwipe( const float& val )
{
  if( _land.valid() )
  {
    _land->setCurrentFragementShaderType( ossimPlanetShaderProgramSetup::HORIZONTAL_SWIPE );
    return shaderVal( val );
  }
  else
  {
    throw "Invalid ossimPlanetLand pointer";
  }
}

float Magrathea::LayerManipulator::verticalSwipe() const
{
  if( _land.valid() )
  {
    _land->setCurrentFragementShaderType( ossimPlanetShaderProgramSetup::VERTICAL_SWIPE );
    return shaderVal();
  }
  else
  {
    throw "Invalid ossimPlanetLand pointer";
  }
}

void Magrathea::LayerManipulator::verticalSwipe( const float& val )
{
  if( _land.valid() )
  {
    _land->setCurrentFragementShaderType( ossimPlanetShaderProgramSetup::VERTICAL_SWIPE );
    return shaderVal( val );
  }
  else
  {
    throw "Invalid ossimPlanetLand pointer";
  }
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

float Magrathea::LayerManipulator::shaderVal() const
{
  float val( 0.0 );
  osg::ref_ptr< osg::Uniform > param = _land->getCurrentShader()->getUniform( "param" );
  if( param.valid() )
  {
     param->get( val );
  }

  return val;
}

void Magrathea::LayerManipulator::shaderVal( const float& val )
{
  osg::ref_ptr< osg::Uniform > param = _land->getCurrentShader()->getUniform( "param" );
  if( param.valid() )
  {
     param->set( val );
  }
}
