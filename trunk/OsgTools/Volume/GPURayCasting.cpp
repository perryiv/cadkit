
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

#include "osgUtil/CullVisitor"

#include <sstream>

using namespace OsgTools::Volume;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

GPURayCasting::GPURayCasting() : BaseClass (),
  _program( GPURayCasting::createProgram() ),
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
  this->_construct();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

GPURayCasting::GPURayCasting( osg::Program * program ) : BaseClass (),
_program( program ),
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
  this->_construct();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

void GPURayCasting::_construct()
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
  
  ss->setAttributeAndModes( _program.get(), osg::StateAttribute::ON );
  
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

#if 0
  texture3D->setWrap( osg::Texture3D::WRAP_R, osg::Texture3D::CLAMP );
  texture3D->setWrap( osg::Texture3D::WRAP_S, osg::Texture3D::CLAMP );
  texture3D->setWrap( osg::Texture3D::WRAP_T, osg::Texture3D::CLAMP );
#else
  texture3D->setWrap( osg::Texture3D::WRAP_R, osg::Texture3D::CLAMP_TO_EDGE );
  texture3D->setWrap( osg::Texture3D::WRAP_S, osg::Texture3D::CLAMP_TO_EDGE );
  texture3D->setWrap( osg::Texture3D::WRAP_T, osg::Texture3D::CLAMP_TO_EDGE );
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

osg::Shader* GPURayCasting::_buildVertexShader ( )
{
  std::ostringstream os;

  os << "uniform vec3 bbMax;\n";
  os << "uniform vec3 bbMin;\n";
  os << "varying vec3 v;\n";
  os << "void main(void)\n";
  os << "{\n";
  os << "   gl_TexCoord[0].x =  ( ( gl_Vertex.x - bbMin.x ) ) / ( bbMax.x - bbMin.x );\n";
  os << "   gl_TexCoord[0].y =  ( ( gl_Vertex.y - bbMin.y ) ) / ( bbMax.y - bbMin.y );\n";
  os << "   gl_TexCoord[0].z =  ( ( gl_Vertex.z - bbMin.z ) ) / ( bbMax.z - bbMin.z );\n";
  os << "   gl_TexCoord[0].w =  ( gl_Vertex.w + 1.0 ) / 2.0;\n";
  os << "   v = gl_Vertex.xyz;\n";
  os << "   gl_Position = ftransform();\n";
  os << "   v = ftransform().xyz;\n";
  os << "   gl_ClipVertex = gl_ModelViewMatrix * gl_Vertex;\n";
  os << "}\n";

  osg::ref_ptr< osg::Shader > vertexShader ( new osg::Shader( osg::Shader::VERTEX ) );
  vertexShader->setShaderSource ( os.str() );
  return vertexShader.release();
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
      << "  if ( diffuseLight <= 0.0 ) specularLight = 0.0;\n"
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
       << " varying vec3 v;\n"
       <<  "void main(void)\n"
       <<  "{\n"
#if 1
       <<  "   float scalar;\n"
       <<  "   vec3 lightPosition = vec3 ( 0.0, 0.0, 1.0 );\n"

    // Initialize answer fragment.
       <<  "   vec4 dst = vec4 ( 0, 0, 0, 0 );\n"

    // Find the entry position in the volume.
       <<  "   vec3 position = gl_TexCoord[0].xyz;\n"

    // Compute the ray direction.
       //<<  "   vec3 direction = normalize ( vec3 ( camera - v ) );\n"
    <<  "   vec3 direction = normalize ( vec3 ( - v ) );\n"

    // Ray traversal loop.  Should probably make the 200 an input.
       <<  "   for ( int i = 0; i < 100; i++ )\n"
       <<  "   {\n"

    //  Look up the scalar value.
       <<  "   scalar = vec4( texture3D( Volume, position ) ).x;\n"
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

       // Advance the ray position
       <<  "   position = position + direction * SampleRate;\n"

       // Ray termination.
       //<<  "   vec3 temp1 = sign ( position - bbMin );\n"
       //<<  "   vec3 temp2 = sign ( bbMax - position );\n"
#if 0
    <<  "   vec3 temp1 = sign ( position - vec3 ( 0.0, 0.0, 0.0 ) );\n"
    <<  "   vec3 temp2 = sign ( vec3 ( 1.0, 1.0, 1.0 ) - position );\n"
    
       <<  "   float inside = dot ( temp1, temp2 );\n"
    
    // If inside, break.
       <<  "   if ( inside < 3.0 )\n"
    <<  "     break;\n"

#else
       
       <<  "   if ( position.x < 0.0 || position.y < 0.0 || position.z < 0.0 || position.x > 1.0 || position.y > 1.0 || position.z > 1.0 )\n"
       <<  "     break;\n"
#endif
    // End of the for loop.
       <<  "   }\n"

    // Return the result.
       <<  "   gl_FragColor = vec4( dst );\n"
#else
       <<  "   gl_FragColor = vec4 ( gl_TexCoord[0].xyz, 1 );\n"
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

osg::Shader* GPURayCasting::_buildFragmentShader ( )
{
  osg::ref_ptr< osg::Shader > fragmentShader ( new osg::Shader( osg::Shader::FRAGMENT ) );
  
  std::ostringstream os;
  os //<< Detail::buildShadingFunction ( osg::Vec3 ( 0.1, 0.1, 0.1 ), osg::Vec3 ( 0.6, 0.6, 0.6 ), osg::Vec3 ( 0.2, 0.2, 0.2 ), 50 )
     //<< "\n"
     << Detail::buildFragmentShader( 0.01 );

  fragmentShader->setShaderSource( os.str() );
  return fragmentShader.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create the shaders.
//
///////////////////////////////////////////////////////////////////////////////

osg::Program* GPURayCasting::createProgram ( bool useTransferFunction )
{
  // Get the state set.

  osg::ref_ptr< osg::Program > program ( new osg::Program ); 

	program->addShader( GPURayCasting::_buildVertexShader() );
	program->addShader( GPURayCasting::_buildFragmentShader() );
 
  return program.release();
}

namespace Detail {
  
// Callback to get the eye position.  This is a bit of a hack and needs to be improved.
  class Callback : public osg::Drawable::CullCallback
{
public:
  typedef osg::Drawable::CullCallback BaseClass;
  
  Callback ( osg::Uniform *uniform ) : BaseClass(), _uniform( uniform )
  {
  }
  
  virtual void operator()( osg::Node* node, osg::NodeVisitor* nv )
  {
    if ( osg::NodeVisitor::CULL_VISITOR ==  nv->getVisitorType() )
    {
      osgUtil::CullVisitor* cullVisitor = dynamic_cast<osgUtil::CullVisitor*>( nv );
      if( cullVisitor && _uniform.valid() )
      {
        // Set the eye position.
        _uniform->set ( cullVisitor->getEyePoint() );
      }
    }
  }
  
private:
  osg::ref_ptr<osg::Uniform> _uniform;
};
  
}

///////////////////////////////////////////////////////////////////////////////
//
//  Set the bounding box.
//
///////////////////////////////////////////////////////////////////////////////

void GPURayCasting::boundingBox ( const osg::BoundingBox& bb )
{
  _bb = bb;
  
  const osg::Vec3 min ( bb._min );
  const osg::Vec3 max ( bb._max );

  // Half lengths
  const float hw ( 0.5f * ( max.x() - min.x() ) );
  const float hh ( 0.5f * ( max.y() - min.y() ) );
  const float hd ( 0.5f * ( max.z() - min.z() ) );
  
  // Vertices and normals.
  osg::ref_ptr<osg::Vec3Array> vertices ( new osg::Vec3Array );
  osg::ref_ptr<osg::Vec3Array> normals  ( new osg::Vec3Array );
  
  // Front face
  vertices->push_back ( bb.center() + osg::Vec3 ( -hw, -hh,  hd ) );
  vertices->push_back ( bb.center() + osg::Vec3 (  hw, -hh,  hd ) );
  vertices->push_back ( bb.center() + osg::Vec3 (  hw,  hh,  hd ) );
  vertices->push_back ( bb.center() + osg::Vec3 ( -hw,  hh,  hd ) );
  normals->push_back  ( osg::Vec3 ( 0, 0, 1 ) );
  
  // Back face
  vertices->push_back ( bb.center() + osg::Vec3 (  hw, -hh, -hd ) );
  vertices->push_back ( bb.center() + osg::Vec3 ( -hw, -hh, -hd ) );
  vertices->push_back ( bb.center() + osg::Vec3 ( -hw,  hh, -hd ) );
  vertices->push_back ( bb.center() + osg::Vec3 (  hw,  hh, -hd ) );
  normals->push_back  ( osg::Vec3 ( 0, 0, -1 ) );
  
  // Top face
  vertices->push_back ( bb.center() + osg::Vec3 (  hw,  hh,  hd ) );
  vertices->push_back ( bb.center() + osg::Vec3 (  hw,  hh, -hd ) );
  vertices->push_back ( bb.center() + osg::Vec3 ( -hw,  hh, -hd ) );
  vertices->push_back ( bb.center() + osg::Vec3 ( -hw,  hh,  hd ) );
  normals->push_back  ( osg::Vec3 ( 0, 1, 0 ) );
  
  // Bottom face
  vertices->push_back ( bb.center() + osg::Vec3 (  hw, -hh,  hd ) );
  vertices->push_back ( bb.center() + osg::Vec3 ( -hw, -hh,  hd ) );
  vertices->push_back ( bb.center() + osg::Vec3 ( -hw, -hh, -hd ) );
  vertices->push_back ( bb.center() + osg::Vec3 (  hw, -hh, -hd ) );
  normals->push_back  ( osg::Vec3 ( 0, -1, 0 ) );
  
  // Left face
  vertices->push_back ( bb.center() + osg::Vec3 ( -hw,  hh,  hd ) );
  vertices->push_back ( bb.center() + osg::Vec3 ( -hw,  hh, -hd ) );
  vertices->push_back ( bb.center() + osg::Vec3 ( -hw, -hh, -hd ) );
  vertices->push_back ( bb.center() + osg::Vec3 ( -hw, -hh,  hd ) );
  normals->push_back  ( osg::Vec3 ( -1, 0, 0 ) );
  
  // Right face
  vertices->push_back ( bb.center() + osg::Vec3 (  hw,  hh, -hd ) );
  vertices->push_back ( bb.center() + osg::Vec3 (  hw,  hh,  hd ) );
  vertices->push_back ( bb.center() + osg::Vec3 (  hw, -hh,  hd ) );
  vertices->push_back ( bb.center() + osg::Vec3 (  hw, -hh, -hd ) );
  normals->push_back  ( osg::Vec3 ( 1, 0, 0 ) );
  
  // Make a new geometry and add the vertices and normals.
  osg::ref_ptr<osg::Geometry> geometry ( new osg::Geometry );
  geometry->setVertexArray ( vertices.get() );
  geometry->setNormalArray ( normals.get() );
  geometry->setNormalBinding ( osg::Geometry::BIND_PER_PRIMITIVE );
  geometry->addPrimitiveSet ( new osg::DrawArrays ( osg::PrimitiveSet::QUADS, 0, vertices->size() ) );

  _geometry = geometry;
  //_geometry->setCullCallback ( new Detail::Callback ( _cameraUniform.get() ) );
  
  this->removeDrawables ( 0 );
  this->addDrawable ( _geometry.get() );

  // Set the min and max values of the bounding box.
  _minUniform->set ( min );
  _maxUniform->set ( max );
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
