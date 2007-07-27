
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "OsgTools/Volume/Texture3DVolume.h"

#include "Usul/Bits/Bits.h"

#include "osg/Texture1D"
#include "osg/Texture3D"
#include "osg/Shader"
#include "osg/BlendFunc"

#include <sstream>

using namespace OsgTools::Volume;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Texture3DVolume::Texture3DVolume() : BaseClass (),
_volume ( 0x0, 0 ),
_geometry ( new Geometry ),
_flags ( 0 ),
_transferFunction ( 0x0, 0 )
{
  // Get the state set.
  osg::ref_ptr < osg::StateSet > ss  ( this->getOrCreateStateSet() );
  
  // Create a blend function.
  osg::ref_ptr< osg::BlendFunc > blendFunc ( new osg::BlendFunc );
	blendFunc->setFunction( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	ss->setAttributeAndModes( blendFunc.get(), osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON );

  // Turn off back face culling
  ss->setMode ( GL_CULL_FACE, osg::StateAttribute::OVERRIDE | osg::StateAttribute::OFF );

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
  return _volume.first.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the image.
//
///////////////////////////////////////////////////////////////////////////////

const osg::Image* Texture3DVolume::image () const
{
  return _volume.first.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the image.
//
///////////////////////////////////////////////////////////////////////////////

void Texture3DVolume::image ( osg::Image* image, TextureUnit unit )
{
  _volume.first = image;
  _volume.second = unit;

  // Create the 3D texture.
  osg::ref_ptr < osg::Texture3D > texture3D ( new osg::Texture3D() );
  texture3D->setImage( image );    

  texture3D->setFilter( osg::Texture3D::MIN_FILTER, osg::Texture3D::LINEAR );
  texture3D->setFilter( osg::Texture3D::MAG_FILTER, osg::Texture3D::LINEAR );
  texture3D->setWrap( osg::Texture3D::WRAP_R, osg::Texture3D::CLAMP );
  texture3D->setWrap( osg::Texture3D::WRAP_S, osg::Texture3D::CLAMP );
  texture3D->setWrap( osg::Texture3D::WRAP_T, osg::Texture3D::CLAMP );

  // Don't resize.
  texture3D->setResizeNonPowerOfTwoHint( false );

  // Get the state set.
  osg::ref_ptr< osg::StateSet > ss ( this->getOrCreateStateSet() );
  ss->setTextureAttributeAndModes ( unit, texture3D.get(), osg::StateAttribute::ON );

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

  std::string buildVertexShader ( double xLength, double yLength, double zLength )
  {
    std::ostringstream os;

    os << "void main(void)\n";
    os << "{\n";
    os << "   gl_TexCoord[0].x =  ( gl_Vertex.x + " << xLength / 2.0f << " ) / " << xLength << ";\n";
    os << "   gl_TexCoord[0].y =  ( gl_Vertex.y + " << yLength / 2.0f << " ) / " << yLength << ";\n";
    os << "   gl_TexCoord[0].z =  ( gl_Vertex.z + " << zLength / 2.0f << " ) / " << zLength << ";\n";
    os << "   gl_TexCoord[0].w =  ( gl_Vertex.w + 1.0 ) / 2.0;\n";
    os << "   gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;\n";
    os << "}\n";

    return os.str();
  }

  static const char* fragSource = 
  {
    "uniform sampler3D Volume;\n"
    "void main(void)\n"
    "{\n"
    "   vec4 color = vec4( texture3D( Volume, gl_TexCoord[0].xyz ) );\n"
    "   gl_FragColor = vec4( color );\n"
    "}\n"
  };

  static const char* fragTransferFunctionSource = 
  {
    "uniform sampler3D Volume;\n"
    "uniform sampler1D TransferFunction;\n"
    "void main(void)\n"
    "{\n"
    "   float index = vec4( texture3D( Volume, gl_TexCoord[0].xyz ) ).x;\n"
    "   vec4 color = vec4( texture1D( TransferFunction, index ) );\n"
    "   gl_FragColor = vec4( color );\n"
    "}\n"
  };

  std::string buildFagmentShader ( bool transferFunction )
  {
    if ( transferFunction )
      return fragTransferFunctionSource;

    return fragSource;
  }
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

  osg::Vec3 min ( _geometry->boundingBox()._min );
  osg::Vec3 max ( _geometry->boundingBox()._max );

  // Get the lengths of the bounding box.
  double xLength ( max.x() - min.x() );
  double yLength ( max.y() - min.y() );
  double zLength ( max.z() - min.z() );

  vertShader->setShaderSource( Detail::buildVertexShader ( xLength, yLength, zLength ) );
  fragShader->setShaderSource( Detail::buildFagmentShader ( this->useTransferFunction() ) );

	program->addShader( vertShader.get() );
	program->addShader( fragShader.get() );
 
  ss->setAttributeAndModes( program.get(), osg::StateAttribute::ON );
  ss->addUniform( new osg::Uniform( "Volume",           static_cast < int > ( _volume.second           ) ) );
  ss->addUniform( new osg::Uniform( "TransferFunction", static_cast < int > ( _transferFunction.second ) ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the bounding box.
//
///////////////////////////////////////////////////////////////////////////////

void Texture3DVolume::boundingBox ( const osg::BoundingBox& bb )
{
  _geometry->boundingBox ( bb );
  this->_createShaders ();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the bounding box.
//
///////////////////////////////////////////////////////////////////////////////

const osg::BoundingBox& Texture3DVolume::boundingBox () const
{
  return _geometry->boundingBox();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the transfer function flag.
//
///////////////////////////////////////////////////////////////////////////////

void Texture3DVolume::useTransferFunction ( bool b )
{
  _flags = Usul::Bits::set ( _flags, _USE_TRANSFER_FUNCTION, b );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the transfer function flag.
//
///////////////////////////////////////////////////////////////////////////////

bool Texture3DVolume::useTransferFunction () const
{
  return Usul::Bits::has ( _flags, _USE_TRANSFER_FUNCTION );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the transfer function image.
//
///////////////////////////////////////////////////////////////////////////////

void Texture3DVolume::transferFunction ( osg::Image* image, TextureUnit unit )
{
  if ( 0x0 != image )
  {
    this->useTransferFunction ( true );
    _transferFunction.first = image;
    _transferFunction.second = unit;

    // Create the 1D texture.
    osg::ref_ptr < osg::Texture1D > texture1D ( new osg::Texture1D() );
    texture1D->setImage( image );

    texture1D->setFilter( osg::Texture::MIN_FILTER, osg::Texture::NEAREST );
    texture1D->setFilter( osg::Texture::MAG_FILTER, osg::Texture::NEAREST );
    texture1D->setWrap  ( osg::Texture::WRAP_S, osg::Texture::CLAMP );
    texture1D->setInternalFormatMode ( osg::Texture::USE_IMAGE_DATA_FORMAT );

    // Get the state set.
    osg::ref_ptr< osg::StateSet > ss ( this->getOrCreateStateSet() );
    ss->setTextureAttributeAndModes ( unit, texture1D.get(), osg::StateAttribute::ON );

    // Create the shaders.
    this->_createShaders ();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the transfer function image.
//
///////////////////////////////////////////////////////////////////////////////

osg::Image* Texture3DVolume::transferFunction () const
{
  return _transferFunction.first.get();
}

