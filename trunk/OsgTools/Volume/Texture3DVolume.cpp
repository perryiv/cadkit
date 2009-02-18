
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
  _flags ( _USE_TRANSFER_FUNCTION ),
  _transferFunction ( 0x0 ),
  _tfTextureUnit ( 0 ),
  _program ( Texture3DVolume::createProgram() ),
  _bbLengths ( new osg::Uniform ( "bb", osg::Vec3 ( 1.0f, 1.0f, 1.0f ) ) ),
  _bbMin ( new osg::Uniform ( "bbMin", osg::Vec3 ( 0.0f, 0.0f, 0.0f ) ) ),
  _volumeSampler ( new osg::Uniform ( "Volume", 0 ) ),
  _tfSampler ( new osg::Uniform ( "TransferFunction", 1 ) )
{
  this->_construct();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Texture3DVolume::Texture3DVolume( osg::Program *program ) : BaseClass (),
  _volume ( 0x0, 0 ),
  _geometry ( new Geometry ),
  _flags ( _USE_TRANSFER_FUNCTION ),
  _transferFunction ( 0x0 ),
  _tfTextureUnit ( 0 ),
  _program ( program ),
  _bbLengths ( new osg::Uniform ( "bb", osg::Vec3 ( 1.0f, 1.0f, 1.0f ) ) ),
  _bbMin ( new osg::Uniform ( "bbMin", osg::Vec3 ( 0.0f, 0.0f, 0.0f ) ) ),
  _volumeSampler ( new osg::Uniform ( "Volume", 0 ) ),
  _tfSampler ( new osg::Uniform ( "TransferFunction", 1 ) )
{
  this->_construct();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Construction.
//
///////////////////////////////////////////////////////////////////////////////

void Texture3DVolume::_construct()
{
  // Get the state set.
  osg::ref_ptr < osg::StateSet > ss  ( this->getOrCreateStateSet() );
  
  // Create a blend function.
  osg::ref_ptr< osg::BlendFunc > blendFunc ( new osg::BlendFunc );
	blendFunc->setFunction( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	ss->setAttributeAndModes( blendFunc.get(), osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON );
  
  // Turn off back face culling
  ss->setMode ( GL_CULL_FACE, osg::StateAttribute::OVERRIDE | osg::StateAttribute::OFF );
  
  ss->setRenderBinDetails ( 1000, "RenderBin" );
  
  // Add the uniforms.
  ss->addUniform ( _bbLengths.get() );
  ss->addUniform ( _bbMin.get() );
  ss->addUniform ( _volumeSampler.get() );
  ss->addUniform ( _tfSampler.get() );
  
  // Add the program
  ss->setAttributeAndModes( _program.get(), osg::StateAttribute::ON | osg::StateAttribute::PROTECTED );
  
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

osg::Image* Texture3DVolume::image()
{
  return _volume.first.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the image.
//
///////////////////////////////////////////////////////////////////////////////

const osg::Image* Texture3DVolume::image() const
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
  
  //texture3D->setUnRefImageDataAfterApply ( true );

  texture3D->setFilter( osg::Texture3D::MIN_FILTER, osg::Texture3D::LINEAR );
  texture3D->setFilter( osg::Texture3D::MAG_FILTER, osg::Texture3D::LINEAR );
  texture3D->setWrap( osg::Texture3D::WRAP_R, osg::Texture3D::CLAMP_TO_EDGE );
  texture3D->setWrap( osg::Texture3D::WRAP_S, osg::Texture3D::CLAMP_TO_EDGE );
  texture3D->setWrap( osg::Texture3D::WRAP_T, osg::Texture3D::CLAMP_TO_EDGE );

  // Resize if we are suppose to.
  texture3D->setResizeNonPowerOfTwoHint( this->resizePowerTwo () );

  // Get the state set.
  osg::ref_ptr< osg::StateSet > ss ( this->getOrCreateStateSet() );
  ss->setTextureAttributeAndModes ( unit, texture3D.get(), osg::StateAttribute::ON );
  
  // Set the uniform value.
  _volumeSampler->set ( static_cast<int> ( unit ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the number of planes.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Texture3DVolume::numPlanes() const
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

// Define this to test work around for clipping planes not working on OS X with NVIDIA drivers.
//#define CLIP_PLANE_WORKAROUND 1

namespace Detail
{
# if 0
  // I don't think this one is needed any more...
  static const char* vertSource = 
  { 
    "void main(void)\n"
    "{\n"
    "   gl_TexCoord[0] =  ( gl_Vertex.xyzw + 1.0 ) / 2.0;\n"
    "   gl_Position = ftransform();\n"
    "   gl_ClipVertex = glModelViewMatrix * gl_Vertex;\n"
    "}\n"
  };
#endif

  std::string buildVertexShader ( )
  {
    std::ostringstream os;

    os << "uniform vec3 bb;\n";
    os << "uniform vec3 bbMin;\n";
#ifdef CLIP_PLANE_WORKAROUND
    os << "varying vec4 vertex;\n";
#endif
    os << "void main(void)\n";
    os << "{\n";
    os << "   gl_TexCoord[0].x =  ( ( gl_Vertex.x - bbMin.x ) ) / bb.x;\n";
    os << "   gl_TexCoord[0].y =  ( ( gl_Vertex.y - bbMin.y ) ) / bb.y;\n";
    os << "   gl_TexCoord[0].z =  ( ( gl_Vertex.z - bbMin.z ) ) / bb.z;\n";
    os << "   gl_TexCoord[0].w =  ( gl_Vertex.w + 1.0 ) / 2.0;\n";
    os << "   gl_Position = ftransform();\n";
    os << "   gl_ClipVertex = gl_ModelViewMatrix * gl_Vertex;\n";
#ifdef CLIP_PLANE_WORKAROUND
    os << "   vertex = gl_ClipVertex;\n";
#endif
    os << "}\n";

    return os.str();
  }

  inline std::string buildShadingFunction ( const osg::Vec3f& ambient, const osg::Vec3f& diffuse, const osg::Vec3f& specular, float shininess )
  {
    std::ostringstream os;

    os << "vec3 shading ( vec3 normal, vec3 viewVector, vec3 lightPosition )\n"
      << "{\n"
      // Material properties.
      << "  vec3 Ka = vec3 ( " << ambient[0] << ", " << ambient[1] << ", " << ambient[2] << ");\n"
      << "  vec3 Kd = vec3 ( " << diffuse[0] << ", " << diffuse[1] << ", " << diffuse[2] << ");\n"
      << "  vec3 Ks = vec3 ( " << specular[0] << ", " << specular[1] << ", " << specular[2] << ");\n"
      << "  float shininess = " << shininess << ";\n"

      // Light properties.
      //<< "  vec3 lightColor = vec3 ( 1.0, 1.0, 1.0 );\n"
      << "  vec3 lightColor = vec3 ( 0.5, 0.5, 0.5 );\n"
      << "  vec3 ambientLight = vec3 ( 0.3, 0.3, 0.3 );\n"

      // Half vector.
      << "  vec3 h = normalize ( lightPosition + viewVector );\n"

      // Compute ambient.
      << "  vec3 ambient = Ka * ambientLight;\n"

      // Compute diffuse.
      << "  float diffuseLight = max ( dot ( lightPosition, viewVector ), 0.0 );\n"
      << "  vec3 diffuse = Kd * lightColor * diffuseLight;\n"

      // Compute specular.
      << "  float specularLight = pow ( max ( dot ( h, normal ), 0.0 ), shininess );\n"
      << "  if ( diffuseLight <= 0.0 ) specularLight = 0.0;\n"
      << "  vec3 specular = Ks * lightColor * specularLight;\n"

      // Return the result.
      //<< "  return vec4 ( ambient + diffuse + specular, 0 );\n"
      << "  return ambient + diffuse + specular;\n"
      << "}\n";

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
#ifdef CLIP_PLANE_WORKAROUND
    "varying vec4 vertex;\n"
    //"uniform int numClipPlanes;\n"
#endif
    "void main(void)\n"
    "{\n"    
    "   float index = vec4( texture3D( Volume, gl_TexCoord[0].xyz ) ).x;\n"
    "   vec4 color = vec4( texture1D( TransferFunction, index ) );\n"
    
#ifdef CLIP_PLANE_WORKAROUND
#if 0
    "   for ( int i = 0; i < numClipPlanes; i++ )\n"
    "   {\n"
    "   if ( dot( vertex, vec3 ( gl_ClipPlane[i].xyz ) ) < gl_ClipPlane[i].w );\n"
    "     discard;\n"
    "   }"
#else
    "   if ( dot( vertex, gl_ClipPlane[0] ) < 0.0 );\n"
    "     discard;\n"
#endif
#endif
    
    #if 0
    "   vec3 camera = vec3 ( gl_ModelViewMatrixInverse[0][3], gl_ModelViewMatrixInverse[1][3], gl_ModelViewMatrixInverse[2][3] );\n"
    "   vec3 lightPosition = vec3 ( 0.0, 0.0, 1.0 );\n"

    "    vec3 texCoord = gl_TexCoord[0].xyz;\n"

       // Compute gradient.
    "   vec3 sample1, sample2;\n"
    "   sample1.x = texture3D( Volume, texCoord - vec3 ( 0.1, 0.0, 0.0 ) ).x;\n"
    "   sample2.x = texture3D( Volume, texCoord + vec3 ( 0.1, 0.0, 0.0 ) ).x;\n"
    "   sample1.y = texture3D( Volume, texCoord - vec3 ( 0.0, 0.1, 0.0 ) ).x;\n"
    "   sample2.y = texture3D( Volume, texCoord + vec3 ( 0.0, 0.1, 0.0 ) ).x;\n"
    "   sample1.z = texture3D( Volume, texCoord - vec3 ( 0.0, 0.0, 0.1 ) ).x;\n"
    "   sample2.z = texture3D( Volume, texCoord + vec3 ( 0.0, 0.0, 0.1 ) ).x;\n"

    // Take difference and normalize.
    "   vec3 normal = normalize ( sample2 - sample1 );\n"

    // Calculate light and viewing direction.
    "   vec3 light = normalize ( lightPosition - texCoord );\n"
    "   vec3 view = normalize ( camera - texCoord );\n"

    // Add to color.
    "   color.rgb += shading ( normal, view, light );\n"
#endif
    "  gl_FragColor = vec4( color );\n"
    "}\n"
  };

  std::string buildFagmentShader ( bool transferFunction, bool shading = false )
  {
    if ( transferFunction )
    {
      if ( shading )
        return buildShadingFunction ( osg::Vec3f ( 0.1f, 0.1f, 0.1f ), osg::Vec3f ( 0.6f, 0.6f, 0.6f ), osg::Vec3f ( 0.2f, 0.2f, 0.2f ), 50 ) + fragTransferFunctionSource;
      
      return fragTransferFunctionSource;
    }

    return fragSource;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create the shaders.
//
///////////////////////////////////////////////////////////////////////////////

osg::Program* Texture3DVolume::createProgram ( bool useTransferFunction )
{
  osg::ref_ptr< osg::Program > program( new osg::Program() ); 
	osg::ref_ptr< osg::Shader > vertShader( new osg::Shader( osg::Shader::VERTEX ) );
	osg::ref_ptr< osg::Shader > fragShader( new osg::Shader( osg::Shader::FRAGMENT ) );

  vertShader->setShaderSource( Detail::buildVertexShader() );
  fragShader->setShaderSource( Detail::buildFagmentShader ( useTransferFunction ) );

	program->addShader( vertShader.get() );
	program->addShader( fragShader.get() );
 
  return program.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the bounding box.
//
///////////////////////////////////////////////////////////////////////////////

void Texture3DVolume::boundingBox ( const osg::BoundingBox& bb )
{
  _geometry->boundingBox ( bb );
  
  const osg::Vec3 min ( bb._min );
  const osg::Vec3 max ( bb._max );
  
  // Get the lengths of the bounding box.
  const double xLength ( max.x() - min.x() );
  const double yLength ( max.y() - min.y() );
  const double zLength ( max.z() - min.z() );
  
  // Set uniform values.
  _bbLengths->set ( osg::Vec3 ( xLength, yLength, zLength ) );
  _bbMin->set ( min );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the bounding box.
//
///////////////////////////////////////////////////////////////////////////////

const osg::BoundingBox& Texture3DVolume::boundingBox() const
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

bool Texture3DVolume::useTransferFunction() const
{
  return Usul::Bits::has ( _flags, _USE_TRANSFER_FUNCTION );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the resize power of two flag.
//
///////////////////////////////////////////////////////////////////////////////

void Texture3DVolume::resizePowerTwo ( bool b )
{
  _flags = Usul::Bits::set ( _flags, _RESIZE_POWER_TWO, b );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the resize power of two flag.
//
///////////////////////////////////////////////////////////////////////////////

bool Texture3DVolume::resizePowerTwo() const
{
  return Usul::Bits::has ( _flags, _RESIZE_POWER_TWO );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the transfer function image.
//
///////////////////////////////////////////////////////////////////////////////

void Texture3DVolume::transferFunction ( TransferFunction* tf, TextureUnit unit )
{
  if ( 0x0 != tf )
  {
    this->useTransferFunction ( true );
    _transferFunction = tf;
    _transferFunction->textureUnit ( unit );

    // Get the state set.
    osg::ref_ptr< osg::StateSet > ss ( this->getOrCreateStateSet() );
    ss->setTextureAttributeAndModes ( unit, tf->texture (), osg::StateAttribute::ON );

    // Set the uniform value.
    _tfSampler->set ( static_cast<int> ( unit ) );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the transfer function image.
//
///////////////////////////////////////////////////////////////////////////////

OsgTools::Volume::TransferFunction* Texture3DVolume::transferFunction() const
{
  return _transferFunction.get();
}
