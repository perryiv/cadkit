
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "OsgTools/Images/DownSample.h"
#include "OsgTools/Render/FBOScreenCapture.h"

#include "osg/Image"
#include "osg/Shader"
#include "osg/Geode"
#include "osg/Geometry"
#include "osg/Texture2D"

#include "osgUtil/SceneView"

#include <sstream>

using namespace OsgTools::Images;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

DownSample::DownSample () :
_size (),
_numSamples ( 4 ),
_filterSize (),
_dirty ( true ),
_xProgram ( 0x0 ),
_yProgram ( 0x0 )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the size.
//
///////////////////////////////////////////////////////////////////////////////

void DownSample::size ( unsigned int width, unsigned int height )
{
  _size.set ( width, height );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the size.
//
///////////////////////////////////////////////////////////////////////////////

void DownSample::size ( const Usul::Math::Vec2ui& size )
{
  _size = size;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the size.
//
///////////////////////////////////////////////////////////////////////////////

const Usul::Math::Vec2ui& DownSample::size () const
{
  return _size;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the number of samples.
//
///////////////////////////////////////////////////////////////////////////////

void DownSample::numSamples( unsigned int samples )
{
  _numSamples = samples;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the number of samples.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int DownSample::numSamples() const
{
  return _numSamples;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the filter size.
//
///////////////////////////////////////////////////////////////////////////////

void DownSample::filterSize ( float x, float y )
{
  _filterSize.set ( x, y );
  _dirty = true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build a Quad.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail 
{
  osg::Node* buildQuad ( osg::Image *image, unsigned int width, unsigned int height )
  {
    osg::ref_ptr < osg::Geode > geode ( new osg::Geode );
    osg::ref_ptr < osg::Geometry > geometry ( new osg::Geometry );

    osg::ref_ptr< osg::StateSet > stateset ( geometry->getOrCreateStateSet() );
      
    osg::ref_ptr< osg::Texture2D > texture ( new osg::Texture2D() ); 
    texture->setImage ( image );
    texture->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
    texture->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE);

    // Do not resize.
    texture->setResizeNonPowerOfTwoHint ( false );

    stateset->setTextureAttributeAndModes ( 0, texture.get(), osg::StateAttribute::ON | osg::StateAttribute::PROTECTED );

    osg::ref_ptr< osg::Vec3Array > normal ( new osg::Vec3Array() );
    normal->push_back ( osg::Vec3 ( 0.0f, 0.0f, 1.0f ) );

    osg::ref_ptr< osg::Vec3Array > vertices ( new osg::Vec3Array() );
    vertices->reserve( 4 );
    vertices->push_back ( osg::Vec3f ( 0.0,   0.0, 0.0 ) );
    vertices->push_back ( osg::Vec3f ( 0.0,   height, 0.0 ) );
    vertices->push_back ( osg::Vec3f ( width, height, 0.0 ) );
    vertices->push_back ( osg::Vec3f ( width, 0.0, 0.0 ) );

    osg::Vec2 texCoords[] =
    {
      osg::Vec2(0.0f, 0.0f),
      osg::Vec2(1.0f, 0.0f),
      osg::Vec2(1.0f, 1.0f),
      osg::Vec2(0.0f, 1.0f),            
    };

    osg::ref_ptr< osg::Vec2Array > uvcoords ( new osg::Vec2Array ( 4, texCoords ) );

    geometry->setTexCoordArray ( 0, uvcoords.get() );

    geometry->setVertexArray ( vertices.get() );

    geometry->setNormalArray ( normal.get() );
    geometry->setNormalBinding ( osg::Geometry::BIND_OVERALL );

    geometry->addPrimitiveSet ( new osg::DrawArrays( osg::PrimitiveSet::QUADS, 0, 4 ) );

    geode->addDrawable ( geometry.get() );

    return geode.release();
  }

}


namespace Detail
{
  osg::Image* renderToTexture ( osg::Image *input, osgUtil::SceneView& sceneView, osg::Program *program, unsigned int width, unsigned int height )
  {
    osg::ref_ptr < osg::Image > answer ( new osg::Image );

    // Make enough space
    answer->allocateImage ( width, height, 1, GL_RGBA, GL_UNSIGNED_BYTE );

    osg::ref_ptr < osg::Node > node ( Detail::buildQuad ( input, width, height ) );

    // Get the state set.
    osg::ref_ptr < osg::StateSet > ss ( node->getOrCreateStateSet() );

    // Attach the program.
    ss->setAttributeAndModes( program, osg::StateAttribute::ON );
    ss->addUniform( new osg::Uniform( "TextureSampler1", 0 ) );

    // Set the scene view data.
    sceneView.setSceneData ( node.get() );
    sceneView.setProjectionMatrixAsOrtho2D ( 0, width, 0, height );

    OsgTools::Render::FBOScreenCapture fbo;
    fbo.size ( width, height );
    fbo.clearColor ( sceneView.getClearColor () );
    fbo ( *answer, sceneView, sceneView.getProjectionMatrix() );
    
    // Delete anything that may have a reference to the answer image.
    sceneView.setSceneData ( 0x0 );
    node = 0x0;

    return answer.release();
  }
}



///////////////////////////////////////////////////////////////////////////////
//
//  Down sample the image.
//
///////////////////////////////////////////////////////////////////////////////

osg::Image* DownSample::operator () ( osg::Image* input )
{
  if ( true == _dirty )
    this->_initialize();

  //unsigned int sourceWidth  ( input->s() );
  unsigned int sourceHeight ( input->t() );

  unsigned int answerWidth ( _size[0] );
  unsigned int answerHeight ( _size[0] );

  // Create a scene view.
  osg::ref_ptr < osgUtil::SceneView > sceneView ( new osgUtil::SceneView );
  
  // The x - pass
  osg::ref_ptr < osg::Image > xImage ( Detail::renderToTexture ( input, *sceneView, _xProgram.get(), answerWidth, sourceHeight ) );

  // The y - pass.
  osg::ref_ptr < osg::Image > answer ( Detail::renderToTexture ( xImage.get(), *sceneView, _yProgram.get(), answerWidth, answerHeight ) );

  sceneView = 0x0;

  return answer.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Pass through vertex shader.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  static const char* vertSource = 
  { 
    "void main(void)\n"
    "{\n"
    "   gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;\n"
    "}\n"
  };
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize shaders.
//
///////////////////////////////////////////////////////////////////////////////

void DownSample::_initialize () 
{
  osg::ref_ptr < osg::Shader > vertexShader ( new osg::Shader ( osg::Shader::VERTEX ) );
  osg::ref_ptr < osg::Shader > xFragmentShader ( new osg::Shader ( osg::Shader::FRAGMENT ) );
  osg::ref_ptr < osg::Shader > yFragmentShader ( new osg::Shader ( osg::Shader::FRAGMENT ) );

  // Set the source for the x pass.
  xFragmentShader->setShaderSource ( this->_buildFragmentProgram ( true ) );

  // Set the source for the y pass.
  yFragmentShader->setShaderSource ( this->_buildFragmentProgram ( false ) );

  // Create a pass though vertex shader.
  vertexShader->setShaderSource ( Detail::vertSource );

  // Create a program for the x-pass.
  _xProgram = new osg::Program;
  _xProgram->addShader( vertexShader.get() );
	_xProgram->addShader( xFragmentShader.get() );

  // Create a program for the y-pass.
  _yProgram = new osg::Program;
  _yProgram->addShader( vertexShader.get() );
	_yProgram->addShader( yFragmentShader.get() );

  _dirty = false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build a fragment program to downsample.
//
///////////////////////////////////////////////////////////////////////////////

std::string DownSample::_buildFragmentProgram ( bool isXPass )
{
  std::ostringstream program;

  // Start the program.
  program << "uniform sampler2D TextureSampler1;\n";
  program << "void main(void)\n";
  program << "{\n";

  float filterSize ( isXPass ? _numSamples * _filterSize[0] : _numSamples * _filterSize[1] );
  int r ( static_cast < int > ( ::ceil ( filterSize / 2.0 ) ) );

  // Compute an offset for odd sampling rate
  float evenOffset ( _numSamples % 2 == 0 ? 0.5f : 0.0f );

  // Get the total weight of the filter.
  float totalWeight ( this->_sumFilter ( r, evenOffset, isXPass ) );

  // Account for the padding on the tiles.
  filterSize = isXPass ? _filterSize[0] : _filterSize[1];
  int filterPad ( static_cast < int > ( ::ceil ( filterSize / 2 - 0.5f ) ) );

  // Our final color value.
  program << "vec4 answer = vec4 ( 0.0, 0.0, 0.0, 0.0 );\n";
  program << "vec2 texCoord;\n";
  program << "vec4 color; \n";

  // Loop through our samples.
  for ( int x = -r; x <= r; ++x )
  {
    float weight ( this->_sampleFilter ( x + evenOffset, isXPass ) );

    // Normalize.
    weight /= totalWeight;

    // Skip zero values.
    if ( 0 == weight )
      continue;

    float fraction ( x - filterPad * _numSamples + evenOffset );  

    // Build the texture coordinate.
    if( isXPass )
      program << "texCoord = vec2 ( gl_FragCoord ) * vec2 ( " << _numSamples << ", 1 ) + vec2 ( " << fraction << ", 1 );\n";
    else
      program << "texCoord = vec2 ( gl_FragCoord ) * vec2 ( 1, " << _numSamples << " ) + vec2 ( 1, " << fraction << " );\n";

    program << "color = vec4( texture2D( TextureSampler1, texCoord ) );\n";

    program << "answer += color * " << weight << ";\n";
  }

  // Write the color to the fragment.
  program << "gl_FragColor = vec4( answer );\n";

  // End the program.
  program << "}\n";

  return program.str();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Sum the filter.
//
///////////////////////////////////////////////////////////////////////////////

float DownSample::_sumFilter ( int r, float evenOffset, bool isXPass )
{
  float total ( 0.0 );
  for ( int x = -r; x <= r; ++ x )
    total += this->_sampleFilter ( x + evenOffset, isXPass );

  return total;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get value at u.
//
///////////////////////////////////////////////////////////////////////////////

float DownSample::_sampleFilter ( float u, bool isXPass )
{
  float x ( 0.0 ), y ( 0.0 );

  if( isXPass )
    x = u / _numSamples;
  else
    y = u / _numSamples;

  return this->_filterValue ( x, y );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Gaussian filter.
//
///////////////////////////////////////////////////////////////////////////////

float DownSample::_filterValue ( float x, float y )
{
  x = 2.0f * ::fabsf ( x ) / _filterSize[ 0 ];
  y = 2.0f * ::fabsf ( y ) / _filterSize[ 1 ];

  return ( x < 1.0f && y < 1.0f ) ? ::expf ( -2.0 * ( x * x + y * y ) ) : 0.0f;
}
