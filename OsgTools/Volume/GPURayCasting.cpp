
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "OsgTools/Volume/GPURayCasting.h"

#include "Usul/Bits/Bits.h"

#include "osg/Texture1D"
#include "osg/Texture3D"

#include <sstream>

using namespace OsgTools::Volume;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

GPURayCasting::GPURayCasting() : BaseClass (),
_vertexShader  ( new osg::Shader( osg::Shader::VERTEX ) ),
_fragmentShader ( new osg::Shader( osg::Shader::FRAGMENT ) ),
_volume ( 0x0, 0 ),
_geometry ( new Geometry ),
_transferFunction ( 0x0 ),
_bb ( ),
_samplingRate ( 0.1f ),
_camera (),
_cameraUniform ( new osg::Uniform ( osg::Uniform::FLOAT_VEC3, "camera" ) ),
_minUniform ( new osg::Uniform ( osg::Uniform::FLOAT_VEC3, "bbMin" ) ),
_maxUniform ( new osg::Uniform ( osg::Uniform::FLOAT_VEC3, "bbMax" ) ),
_volumeUniform ( new osg::Uniform ( osg::Uniform::INT, "Volume" ) ),
_tfUniform ( new osg::Uniform ( osg::Uniform::INT, "TransferFunction" ) ),
_rateUniform ( new osg::Uniform ( osg::Uniform::FLOAT, "SampleRate" ) )
{
  // Get the state set.
  osg::ref_ptr < osg::StateSet > ss  ( this->getOrCreateStateSet() );
  
  // Turn off back face culling
  ss->setMode ( GL_CULL_FACE, osg::StateAttribute::OVERRIDE | osg::StateAttribute::OFF );
  ss->setMode ( GL_BLEND, osg::StateAttribute::ON | osg::StateAttribute::PROTECTED );

  ss->setRenderBinDetails ( 1000, "RenderBin" );

  // Add the planes.
  this->addDrawable ( _geometry.get() );

  // Set the internal bounding box.
  this->boundingBox ( osg::BoundingBox ( -1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f ) );

  // Create the shaders.
  this->_createShaders ();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

GPURayCasting::~GPURayCasting()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the image.
//
///////////////////////////////////////////////////////////////////////////////

osg::Image* GPURayCasting::image ()
{
  return _volume.first.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the image.
//
///////////////////////////////////////////////////////////////////////////////

const osg::Image* GPURayCasting::image () const
{
  return _volume.first.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the image.
//
///////////////////////////////////////////////////////////////////////////////

void GPURayCasting::image ( osg::Image* image, TextureUnit unit )
{
  _volume.first = image;
  _volume.second = unit;

  // Set the uniform for the volume.
  _volumeUniform->set ( static_cast < int > ( unit ) );

  // Create the 3D texture.
  osg::ref_ptr < osg::Texture3D > texture3D ( new osg::Texture3D() );
  texture3D->setImage( image );    

  texture3D->setFilter( osg::Texture3D::MIN_FILTER, osg::Texture3D::LINEAR );
  texture3D->setFilter( osg::Texture3D::MAG_FILTER, osg::Texture3D::LINEAR );

#if 1
  texture3D->setWrap( osg::Texture3D::WRAP_R, osg::Texture3D::CLAMP );
  texture3D->setWrap( osg::Texture3D::WRAP_S, osg::Texture3D::CLAMP );
  texture3D->setWrap( osg::Texture3D::WRAP_T, osg::Texture3D::CLAMP );
#else
  texture3D->setWrap( osg::Texture3D::WRAP_R, osg::Texture3D::CLAMP_TO_BORDER );
  texture3D->setWrap( osg::Texture3D::WRAP_S, osg::Texture3D::CLAMP_TO_BORDER );
  texture3D->setWrap( osg::Texture3D::WRAP_T, osg::Texture3D::CLAMP_TO_BORDER );

  texture3D->setBorderColor ( osg::Vec4 ( 0.0, 0.0, 0.0, 0.0 ) );
  texture3D->setBorderWidth ( 1.0f );
#endif

  // Don't resize.
  texture3D->setResizeNonPowerOfTwoHint( false );

  // Get the state set.
  osg::ref_ptr< osg::StateSet > ss ( this->getOrCreateStateSet() );
  ss->setTextureAttributeAndModes ( unit, texture3D.get(), osg::StateAttribute::ON );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the number of planes.
//
///////////////////////////////////////////////////////////////////////////////

float GPURayCasting::samplingRate () const
{
  return _samplingRate;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the number of planes.
//
///////////////////////////////////////////////////////////////////////////////

void GPURayCasting::samplingRate ( float rate )
{
  _rateUniform->set ( rate );
  _samplingRate = rate;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Vertex shaders.
//
///////////////////////////////////////////////////////////////////////////////

void GPURayCasting::_buildVertexShader ( )
{
  std::ostringstream os;

  os << "void main(void)\n";
  os << "{\n";
  os << "   gl_TexCoord[0] = vec4 ( gl_Vertex.x / 512, gl_Vertex.y / 512, gl_Vertex.z, gl_Vertex.w );\n";
  //os << "   gl_TexCoord[0] =  vec4 ( ( gl_Vertex.xy + 1.0 ) / 2.0, 0.0, 1.0 );\n";
  //os << "   gl_TexCoord[0] =  ( gl_Vertex.xyzw + 1.0 ) / 2.0;\n";
  os << "   gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;\n";
  os << "}\n";

  _vertexShader->setShaderSource ( os.str() );
}

namespace Detail
{
  inline std::string buildShadingFunction ( const osg::Vec3& ambient, const osg::Vec3& diffuse, const osg::Vec3& specular, float shininess )
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
      << "  if ( diffuseLight <= 0 ) specularLight = 0;\n"
      << "  vec3 specular = Ks * lightColor * specularLight;\n"

      // Return the result.
      //<< "  return vec4 ( ambient + diffuse + specular, 0 );\n"
      << "  return ambient + diffuse + specular;\n"
      << "}\n";

    return os.str();
  }
}

namespace Detail
{
  inline std::string buildFragmentShader ( float delta ) 
  {
    std::ostringstream os;

    os << "uniform sampler3D Volume;\n"
       <<  "uniform sampler1D TransferFunction;\n"
       <<  "uniform vec3 camera;\n"
       <<  "uniform vec3 bbMin;\n"
       <<  "uniform vec3 bbMax;\n"
       <<  "uniform float SampleRate;\n"
       <<  "void main(void)\n"
       <<  "{\n"

       //<<"   gl_FragColor = vec4 ( gl_TexCoord[0].xyz, 1 );\n"
#if 1
       <<  "   float scalar;\n"
       <<  "   vec3 lightPosition = vec3 ( 0.0, 0.0, 1.0 );\n"

       //<<  "   vec3 camera = vec3 ( gl_ModelViewMatrixInverse[0][3], gl_ModelViewMatrixInverse[1][3], gl_ModelViewMatrixInverse[2][3] );\n"

    // Initialize answer fragment.
       <<  "   vec4 dst = vec4 ( 0, 0, 0, 0 );\n"

       << "    vec3 texCoord = gl_TexCoord[0].xyz;\n"

    // Find the entry position in the volume.
       <<  "   vec3 position = gl_TexCoord[0].xyz;\n"

    // Compute the ray direction.
       <<  "   vec3 direction = normalize ( vec3 ( position - camera ) );\n"
       //<<  "   vec3 direction = vec3 ( 0, 0, 1 );\n"

       //<<  "   gl_FragColor = vec4 ( direction.xyz, 1 );\n"
       //<<  "   gl_FragColor = vec4 ( 0, 0, direction.z, 1 );\n"

    // Ray traversal loop.  Should probably make the 200 an input.
       <<  "   for ( int i = 0; i < 100; i++ )\n"
       <<  "   {\n"

    //  Look up the scalar value.
       <<  "   scalar = vec4( texture3D( Volume, position ) ).a;\n"
       <<  "   vec4 src = vec4( texture1D( TransferFunction, scalar ) );\n"

#if 0
       // Compute gradient.
       <<  "   vec3 sample1, sample2;\n"
       <<  "   sample1.x = texture3D( Volume, texCoord - vec3 ( " << delta << ", 0.0, 0.0 ) ).x;\n"
       <<  "   sample2.x = texture3D( Volume, texCoord + vec3 ( " << delta << ", 0.0, 0.0 ) ).x;\n"
       <<  "   sample1.y = texture3D( Volume, texCoord - vec3 ( 0.0, " << delta << ", 0.0 ) ).x;\n"
       <<  "   sample2.y = texture3D( Volume, texCoord + vec3 ( 0.0, " << delta << ", 0.0 ) ).x;\n"
       <<  "   sample1.z = texture3D( Volume, texCoord - vec3 ( 0.0, 0.0, " << delta << " ) ).x;\n"
       <<  "   sample2.z = texture3D( Volume, texCoord + vec3 ( 0.0, 0.0, " << delta << " ) ).x;\n"

       // Take difference and normalize.
       <<  "   vec3 normal = normalize ( sample2 - sample1 );\n"

       // Calculate light and viewing direction.
       <<  "   vec3 light = normalize ( lightPosition - position );\n"
       <<  "   vec3 view = normalize ( camera - position );\n"

       // Add to color.
       <<  "   src.rgb += shading ( normal, view, light );\n"
#endif
       // Front to back compositing.
       <<  "   dst = ( 1.0 - dst.a ) * src + dst;\n"

       /*<<  "   if ( dst.r > 1.0 && dst.g > 1.0 && dst.b > 1.0 )\n"
       <<  "   {\n"
       <<  "     dst = vec4 ( 0, 1, 0, 1 );\n"
       <<  "     break;\n"
       <<  "   }\n"*/

       //<<  "   if ( dst.a > 0 ) break;\n"

       // Advance the ray position
       <<  "   position = position + direction * SampleRate;\n"

       // Ray termination.
       <<  "   vec3 temp1 = sign ( position - bbMin );\n"
       <<  "   vec3 temp2 = sign ( bbMax - position );\n"
       <<  "   float inside = dot ( temp1, temp2 );\n"

    // If inside, break.
       <<  "   if ( inside < 3.0 )\n"
       <<  "     break;\n"

    // End of the for loop.
       <<  "   }\n"

    // Return the result.
       <<  "   gl_FragColor = vec4( dst );\n"
#else
       //<<  "   gl_FragColor = vec4( 1.0, 0.0, 0.0, 1.0 );\n"
#endif
       <<  "}\n";

    return os.str();
  }
}

///////////////////////////////////////////////////////////////////////////////
//
//  Fragment shader.
//
///////////////////////////////////////////////////////////////////////////////

void GPURayCasting::_buildFragmentShader ( )
{
  std::ostringstream os;
  os << Detail::buildShadingFunction ( osg::Vec3 ( 0.1, 0.1, 0.1 ), osg::Vec3 ( 0.6, 0.6, 0.6 ), osg::Vec3 ( 0.2, 0.2, 0.2 ), 50 )
     << "\n"
     << Detail::buildFragmentShader( 0.01 );

  _fragmentShader->setShaderSource( os.str() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create the shaders.
//
///////////////////////////////////////////////////////////////////////////////

void GPURayCasting::_createShaders ()
{
  // Get the state set.
  osg::ref_ptr< osg::StateSet > ss ( this->getOrCreateStateSet() );

  osg::ref_ptr< osg::Program > program ( new osg::Program ); 

  this->_buildVertexShader ( );
  this->_buildFragmentShader ( );

	program->addShader( _vertexShader.get() );
	program->addShader( _fragmentShader.get() );
 
  ss->setAttributeAndModes( program.get(), osg::StateAttribute::ON );
  
  // Add the uniforms.
  ss->addUniform ( _cameraUniform.get() );
  ss->addUniform ( _minUniform.get() );
  ss->addUniform ( _maxUniform.get() );
  ss->addUniform ( _volumeUniform.get() );
  ss->addUniform ( _tfUniform.get() );
  ss->addUniform ( _rateUniform.get() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the bounding box.
//
///////////////////////////////////////////////////////////////////////////////

void GPURayCasting::boundingBox ( const osg::BoundingBox& bb )
{
  _bb = bb;

  // Set the rotation mode.
  //this->setMode( osg::Billboard::POINT_ROT_EYE );

  // Set the center of the billboard
  //this->setPosition( 0, _bb.center() );

  // Make the half sizes.
  //double halfSize ( _bb.radius() / 2.0 );
  double halfSize ( 0.5f );

  // New geometry.
  _geometry = new osg::Geometry;

  // Create a quad.
  osg::ref_ptr < osg::Vec3Array > vertices  ( new osg::Vec3Array ( 4 ) );
  osg::ref_ptr < osg::Vec3Array > texcoords ( new osg::Vec3Array ( 4 ) );

#if 0
  vertices->at( 0 ).set( -halfSize, halfSize, 0.0 );
  vertices->at( 1 ).set( -halfSize, -halfSize, 0.0 ); 	 
	vertices->at( 2 ).set( halfSize,  -halfSize, 0.0 ); 	 
	vertices->at( 3 ).set( halfSize,  halfSize, 0.0 );
#else
  vertices->at( 0 ).set( 0.0, 512, 0.0 );
  vertices->at( 1 ).set( 0.0, 0.0, 0.0 ); 	 
	vertices->at( 2 ).set( 512, 0.0, 0.0 ); 	 
	vertices->at( 3 ).set( 512, 512, 0.0 );
#endif
  /*texcoords->at( 0 ).set( 0.0f, 1.0f, 0.0f );
	texcoords->at( 1 ).set( 0.0f, 0.0f, 0.0f );
	texcoords->at( 2 ).set( 1.0f, 0.0f, 0.0f );
	texcoords->at( 3 ).set( 1.0f, 1.0f, 0.0f );*/

  // Make the normals
  //osg::ref_ptr < osg::Vec3Array > normals ( new osg::Vec3Array( 4 ) );
  // Set the value
  //std::fill( normals->begin(), normals->end(), osg::Vec3( 0.0f, 0.0f, 1.0f ) );

  // Set the arrays
  _geometry->setVertexArray   ( vertices.get() );
  /*_geometry->setNormalArray   ( normals.get() );
  _geometry->setTexCoordArray ( _volume.second, texcoords.get() );*/

  // Set the bindings
  //_geometry->setNormalBinding( osg::Geometry::BIND_PER_VERTEX );

  // Add the primitive set
  _geometry->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::QUADS, 0, vertices->size() ) );

  this->removeDrawables ( 0 );
  this->addDrawable ( _geometry.get() );

  // Set the min and max values of the bounding box.
  _minUniform->set ( _bb._min );
  _maxUniform->set ( _bb._max );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the bounding box.
//
///////////////////////////////////////////////////////////////////////////////

const osg::BoundingBox& GPURayCasting::boundingBox () const
{
  return _bb;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the transfer function image.
//
///////////////////////////////////////////////////////////////////////////////

void GPURayCasting::transferFunction ( TransferFunction* tf, TextureUnit unit )
{
  if ( 0x0 != tf )
  {
    _transferFunction = tf;
    _transferFunction->textureUnit ( unit );

    // Get the state set.
    osg::ref_ptr< osg::StateSet > ss ( this->getOrCreateStateSet() );
    ss->setTextureAttributeAndModes ( unit, _transferFunction->texture (), osg::StateAttribute::ON );

    // Set the uniform value.
    _tfUniform->set ( static_cast < int > ( unit ) );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the transfer function image.
//
///////////////////////////////////////////////////////////////////////////////

OsgTools::Volume::TransferFunction* GPURayCasting::transferFunction () const
{
  return _transferFunction.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Traverse this node.
//
///////////////////////////////////////////////////////////////////////////////

void GPURayCasting::traverse ( osg::NodeVisitor &nv )
{
  if ( osg::NodeVisitor::CULL_VISITOR == nv.getVisitorType () )
  {
    _camera = nv.getEyePoint ();
    _cameraUniform->set ( _camera );
  }

  // Call the base class' one.
  BaseClass::traverse ( nv );
}


/// Set the camera.
void GPURayCasting::camera ( const osg::Vec3& camera )
{
  _camera = camera;
  _cameraUniform->set ( _camera );
}
