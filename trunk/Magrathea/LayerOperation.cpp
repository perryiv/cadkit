
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Aashish Chaudhary
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "LayerOperation.h"

void Magrathea::LayerOperation::opacity( const float& val )
{
  if( mLand.valid() )
  {
    mLand->setCurrentFragementShaderType( ossimPlanetShaderProgramSetup::OPACITY );    
    osg::ref_ptr< osg::Uniform > param = mLand->getCurrentShader()->getUniform( "param" );
    if( param.valid() )
    {
       param->set( val );
    }
  }
}

float Magrathea::LayerOperation::opacity() const 
{
  float opacity( 1.0 );
  if( mLand.valid() )
  {
    mLand->setCurrentFragementShaderType( ossimPlanetShaderProgramSetup::OPACITY );    
    osg::ref_ptr< osg::Uniform > param = mLand->getCurrentShader()->getUniform( "param" );
    if( param.valid() )
    {
       param->get( opacity );
    }
  }

  return opacity;
}

void Magrathea::LayerOperation::setLand( ossimPlanetLand* land )
{
  mLand = const_cast< ossimPlanetLand* >( land );
}
