
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "OsgTools/Volume/Texture3DVolume.h"

#include "osg/Texture3D"
#include "osg/Shader"
#include "osg/BlendFunc"

using namespace OsgTools::Volume;


///////////////////////////////////////////////////////////////////////////////
//
//  Vertex and Fragment shaders.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  static const char* vertSource = 
  { 
    "void main(void)\n"
    "{\n"
    "   gl_TexCoord[0] =  ( gl_Vertex.xyzw + 1.0 ) / 2.0;\n"
    "   gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;\n"
    "}\n"
  };

  static const char* fragSource = 
  {
    "uniform sampler3D TextureSampler1;\n"
    "void main(void)\n"
    "{\n"
    "   vec4 color = vec4( texture3D( TextureSampler1, gl_TexCoord[0].xyz ) );  \n"
    "   gl_FragColor = vec4( color );\n"
    "}\n"
  };
}

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Texture3DVolume::Texture3DVolume() : BaseClass (),
_image ( 0x0 ),
_geometry ( new Geometry )
{
  // Get the state set.
  osg::ref_ptr < osg::StateSet > ss  ( this->getOrCreateStateSet() );
  
  // Create a blend function.
  osg::ref_ptr< osg::BlendFunc > blendFunc ( new osg::BlendFunc );
	blendFunc->setFunction( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	ss->setAttributeAndModes( blendFunc.get(), osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON );

  // Add the planes.
  this->addDrawable ( _geometry.get() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Texture3DVolume::~Texture3DVolume()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the image.
//
///////////////////////////////////////////////////////////////////////////////

osg::Image* Texture3DVolume::image ()
{
  return _image.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the image.
//
///////////////////////////////////////////////////////////////////////////////

const osg::Image* Texture3DVolume::image () const
{
  return _image.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the image.
//
///////////////////////////////////////////////////////////////////////////////

void Texture3DVolume::image ( osg::Image* image )
{
  _image = image;

  // Create the 3D texture.
  osg::ref_ptr < osg::Texture3D > texture3D ( new osg::Texture3D() );
  texture3D->setImage( image );    

  texture3D->setFilter( osg::Texture3D::MIN_FILTER, osg::Texture3D::LINEAR );
  texture3D->setFilter( osg::Texture3D::MAG_FILTER, osg::Texture3D::LINEAR );
  texture3D->setWrap( osg::Texture3D::WRAP_R, osg::Texture3D::CLAMP );
  texture3D->setWrap( osg::Texture3D::WRAP_S, osg::Texture3D::CLAMP );
  texture3D->setWrap( osg::Texture3D::WRAP_T, osg::Texture3D::CLAMP );

  // Get the state set.
  osg::ref_ptr< osg::StateSet > ss ( this->getOrCreateStateSet() );
  ss->setTextureAttributeAndModes ( 0, texture3D.get(), osg::StateAttribute::ON );

  // Create the shaders.
  this->_createShaders ();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the number of planes.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Texture3DVolume::numPlanes () const
{
  return _geometry->numPlanes();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the number of planes.
//
///////////////////////////////////////////////////////////////////////////////

void Texture3DVolume::numPlanes ( unsigned int num )
{
  _geometry->numPlanes( num );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create the shaders.
//
///////////////////////////////////////////////////////////////////////////////

void Texture3DVolume::_createShaders ()
{
  // Get the state set.
  osg::ref_ptr< osg::StateSet > ss ( this->getOrCreateStateSet() );

  osg::ref_ptr< osg::Program > program( new osg::Program() ); 
	osg::ref_ptr< osg::Shader > vertShader( new osg::Shader( osg::Shader::VERTEX ) );
	osg::ref_ptr< osg::Shader > fragShader( new osg::Shader( osg::Shader::FRAGMENT ) );

  vertShader->setShaderSource( Detail::vertSource );
  fragShader->setShaderSource( Detail::fragSource );

	program->addShader( vertShader.get() );
	program->addShader( fragShader.get() );
 
  ss->setAttributeAndModes( program.get(), osg::StateAttribute::ON );
  ss->addUniform( new osg::Uniform( "TextureSampler1", 0 ) );
}
