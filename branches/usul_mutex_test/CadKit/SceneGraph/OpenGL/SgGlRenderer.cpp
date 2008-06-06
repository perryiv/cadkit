
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  SgGlRenderer: The renderer class.
//
///////////////////////////////////////////////////////////////////////////////

#include "SgGlPrecompiled.h"
#include "SgGlRenderer.h"
#include "SgGlConstants.h"
#include "SgGlDefine.h"
#include "SgGlInternalMacros.h"
#include "SgGlErrorCheck.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include "SceneGraph/Core/SgAllNodes.h"
# include "Standard/SlViewport.h"
# include "Standard/SlPrint.h"
# include "Standard/SlTrace.h"
# include "Standard/SlBoundingBox.h"
# include "Standard/SlPreComputedArrays.h"
# include "Standard/SlConstants.h"
# include <GL/glu.h>
#endif

using namespace CadKit;

#define SHININESS_SCALE 128.0f // OpenGL is [0,128].

SG_IMPLEMENT_DYNAMIC_VISITOR(SgGlRenderer,SgScreenRenderer);


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

SgGlRenderer::SgGlRenderer() : SgScreenRenderer(),
  _context ( NULL ),
  _state ( NULL ),
  _flags ( DOUBLE_BUFFER ),
  _quadric ( NULL ),
  _coordinate3 ( NULL ),
  _coordinate4 ( NULL ),
  _coordinate ( NULL ),
  _normal ( NULL )
{
  SL_PRINT2 ( "SgGlRenderer::SgGlRenderer(), this = %X\n", this );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

SgGlRenderer::~SgGlRenderer()
{
  SL_PRINT2 ( "SgGlRenderer::~SgGlRenderer(), this = %X\n", this );

  // Delete the quadric (it may be NULL).
  if ( _quadric ) 
    ::gluDeleteQuadric ( (GLUquadricObj *) _quadric );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize the next rendering.
//
///////////////////////////////////////////////////////////////////////////////

bool SgGlRenderer::preRender ( SgNode &scene )
{
  SL_ASSERT ( this );

  // Make our context the current one.
  if ( false == this->_makeContextCurrent() ) 
    return false;

  SG_GL_ERROR_CHECKER;

  // Reset this visitor.
  if ( false == this->reset() )
    return false;

  // Clear the background.
  ::glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

  // Should be true.
  SL_ASSERT ( 1 == _state->getModelviewMatrixStackDepth() );
  SL_ASSERT ( 1 == _state->getProjectionMatrixStackDepth() );

  // Initialize the stacks.
  _state->initModelviewMatrixStack();
  _state->initProjectionMatrixStack();

  // All is good.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Render the node.
//
///////////////////////////////////////////////////////////////////////////////

bool SgGlRenderer::render ( SgNode &scene )
{
  SL_ASSERT ( this );

  // Reset this visitor.
  if ( false == this->reset() )
    return false;

  // This will draw the scene graph.
  if ( false == scene.accept ( *this ) ) 
  {
    SL_ASSERT ( 0 ); // Why didn't it render?
    return false;
  }

  // All is good.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Finalize the rendering.
//
///////////////////////////////////////////////////////////////////////////////

bool SgGlRenderer::postRender ( SgNode &scene )
{
  SL_ASSERT ( this );

  // Reset this visitor.
  if ( false == this->reset() )
    return false;

  // Draw everything right now.
  ::glFlush();

  // Swap the buffers if we are supposed to.
  if ( this->hasFlags ( DOUBLE_BUFFER ) )
    this->_swapBuffers();

  // Should be true.
  SL_ASSERT ( 1 == _state->getModelviewMatrixStackDepth() );
  SL_ASSERT ( 1 == _state->getProjectionMatrixStackDepth() );

  // All is good.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set our OpenGL defaults.
//
///////////////////////////////////////////////////////////////////////////////

bool SgGlRenderer::init ( const SlViewporti &viewport, SgGlContext *context )
{
  SL_PRINT ( "SgGlRenderer::init()\n" );
  SL_ASSERT ( this );

  // Make sure we have a state. Since "_state" is a reference pointer, if its 
  // internal pointer is not null then it will be unreferenced before the new 
  // pointer is assigned. In other words, repeated calls to this function do 
  // not produce a memory leak (at least not from this line).
  _state = new SgGlState;
  if ( _state.isNull() )
    return false;

  // Set this rendering context.
  this->_setContext ( context );

  // Make our new context the current one.
  if ( false == this->_makeContextCurrent() ) 
    return false;

  SG_GL_ERROR_CHECKER;

  // If we are given good width and height values...
  if ( viewport.getWidth() > 0 && viewport.getHeight() > 0 )
  {
    // Projection matrix defaults.
    _state->makeProjectionMatrixIdentity();
    ::gluPerspective ( SG_DEFAULT_PERSPECTIVE_CAMERA_HEIGHT, 
                       double ( viewport.getWidth() ) / double ( viewport.getHeight() ), 
                       SG_DEFAULT_CAMERA_Z_NEAR, 
                       SG_DEFAULT_CAMERA_Z_FAR );

    // Set the viewport.
    if ( false == this->setViewport ( viewport ) )
      return false;
  }

  // Default materials. Grey on the front.
  SlVec4f ambient  ( SG_GL_DEFAULT_MATERIAL_FRONT_AMBIENT );
  SlVec4f diffuse  ( SG_GL_DEFAULT_MATERIAL_FRONT_DIFFUSE );
  SlVec4f specular ( SG_GL_DEFAULT_MATERIAL_FRONT_SPECULAR );
  SlVec4f emissive ( SG_GL_DEFAULT_MATERIAL_FRONT_EMISSIVE );
  float shininess  = SG_GL_DEFAULT_MATERIAL_FRONT_SHININESS * SHININESS_SCALE;

  ::glMaterialfv ( GL_FRONT, GL_AMBIENT,   ambient );
  ::glMaterialfv ( GL_FRONT, GL_DIFFUSE,   diffuse );
  ::glMaterialfv ( GL_FRONT, GL_SPECULAR,  specular );
  ::glMaterialfv ( GL_FRONT, GL_EMISSION,  emissive );
  ::glMaterialf  ( GL_FRONT, GL_SHININESS, shininess ); 

  // Black on the back.
  ambient.setValue  ( SG_GL_DEFAULT_MATERIAL_BACK_AMBIENT );
  diffuse.setValue  ( SG_GL_DEFAULT_MATERIAL_BACK_DIFFUSE );
  specular.setValue ( SG_GL_DEFAULT_MATERIAL_BACK_SPECULAR );
  emissive.setValue ( SG_GL_DEFAULT_MATERIAL_BACK_EMISSIVE );
  shininess         = SG_GL_DEFAULT_MATERIAL_BACK_SHININESS * SHININESS_SCALE;

  ::glMaterialfv ( GL_BACK, GL_AMBIENT,   ambient );
  ::glMaterialfv ( GL_BACK, GL_DIFFUSE,   diffuse );
  ::glMaterialfv ( GL_BACK, GL_SPECULAR,  specular );
  ::glMaterialfv ( GL_BACK, GL_EMISSION,  emissive );
  ::glMaterialf  ( GL_BACK, GL_SHININESS, shininess );

  // Default light. Have to make sure the lights are rendered with 
  // respect to world space.
  ::glLoadIdentity();              // Needed?

  // All white light.
  ambient.setValue  ( SG_GL_DEFAULT_LIGHT_AMBIENT );
  specular.setValue ( SG_GL_DEFAULT_LIGHT_DIFFUSE );
  diffuse.setValue  ( SG_GL_DEFAULT_LIGHT_SPECULAR );
  SlVec4f position  ( SG_GL_DEFAULT_LIGHT_POSITION );

  _state->enable ( GL_LIGHT0 );
  ::glLightfv ( GL_LIGHT0, GL_AMBIENT, ambient );
  ::glLightfv ( GL_LIGHT0, GL_DIFFUSE, diffuse );
  ::glLightfv ( GL_LIGHT0, GL_SPECULAR, specular );
  ::glLightfv ( GL_LIGHT0, GL_POSITION, position );

  // This will let the material control two-sided lighting.
  ::glLightModelf ( GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE );

  // Misc.
  ::glClearDepth ( 1.0f ); // This came recommended, not sure about what happens if it's changed.
  _state->enable ( GL_AUTO_NORMAL );
  _state->enable ( GL_DEPTH_TEST );
  ::glClearColor ( SG_GL_DEFAULT_BACKGROUND );

  // Scale normals. See: 
  // http://www.opengl.org/developers/code/features/KilgardTechniques/oglpitfall/oglpitfall.html
  // Basically, scaling on the modelview matrix will mess up lighting unless 
  // you tell OpenGL to scale the normal vectors.
  _state->enable ( GL_NORMALIZE );

  // If we already have one then delete it.
  if ( _quadric )
    ::gluDeleteQuadric ( (GLUquadricObj *) _quadric );

  // Create a new quadric.
  _quadric = ::gluNewQuadric();

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the clear debth.
//
///////////////////////////////////////////////////////////////////////////////

bool SgGlRenderer::setClearDepth ( const float &depth )
{
  SL_PRINT ( "SgGlRenderer::setClearDepth()\n" );
  SL_ASSERT ( this );
  SL_ASSERT ( depth >= 0.0f && depth <= 1.0f );

  // Make our context the current one.
  if ( false == this->_makeContextCurrent() ) 
    return false;

  SG_GL_ERROR_CHECKER;

  // Set the clear debth.
  ::glClearDepth ( depth );

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the rendering context.
//
///////////////////////////////////////////////////////////////////////////////

void SgGlRenderer::_setContext ( SgGlContext *context )
{
  SL_PRINT ( "SgGlRenderer::_setBackgroundColor()\n" );
  SL_ASSERT ( this );
  SL_ASSERT ( _state.isValid() );

  // If this is a new context then clear the state.
  if ( _context.getValue() != context )
    _state->clear();

  // Set the new context.
  _context = context;
}


///////////////////////////////////////////////////////////////////////////////
//
//  The background color.
//
///////////////////////////////////////////////////////////////////////////////

bool SgGlRenderer::setBackgroundColor ( const SlColorf &color )
{
  SL_PRINT ( "SgGlRenderer::setBackgroundColor()\n" );
  SL_ASSERT ( this );
  SL_ASSERT ( color.isValid() );

  // Make our context the current one.
  if ( false == this->_makeContextCurrent() ) 
    return false;

  SG_GL_ERROR_CHECKER;

  // Call the base class's function to set the data member.
  if ( !SgScreenRenderer::setBackgroundColor ( color ) ) 
    return false;

  // Set the color.
  ::glClearColor ( color[SlColorf::RED], color[SlColorf::GREEN], color[SlColorf::BLUE], color[SlColorf::ALPHA] );

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Enable these flags.
//
///////////////////////////////////////////////////////////////////////////////

bool SgGlRenderer::enable ( const GLenum &state )
{
  SL_PRINT ( "SgGlRenderer::enable()\n" );
  SL_ASSERT ( this );

  // Make our context the current one.
  if ( false == this->_makeContextCurrent() ) 
    return false;

  SG_GL_ERROR_CHECKER;

  // Enable the flags.
  _state->enable ( state );

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Disable these flags.
//
///////////////////////////////////////////////////////////////////////////////

bool SgGlRenderer::disable ( const GLenum &state )
{
  SL_PRINT ( "SgGlRenderer::disable()\n" );
  SL_ASSERT ( this );

  // Make our context the current one.
  if ( false == this->_makeContextCurrent() ) 
    return false;

  SG_GL_ERROR_CHECKER;

  // Disable the flags.
  _state->disable ( state );

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Check the state.
//
///////////////////////////////////////////////////////////////////////////////

bool SgGlRenderer::isEnabled ( const GLenum &state )
{
  SL_PRINT ( "SgGlRenderer::isEnabled()\n" );
  SL_ASSERT ( this );

  // Make our context the current one.
  if ( false == this->_makeContextCurrent() ) 
    return false;

  SG_GL_ERROR_CHECKER;

  // See if the flag is enabled.
  return _state->isEnabled ( state );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Check the state.
//
///////////////////////////////////////////////////////////////////////////////

bool SgGlRenderer::isDisabled ( const GLenum &state )
{
  SL_PRINT ( "SgGlRenderer::isDisabled()\n" );
  SL_ASSERT ( this );

  // See if the flag is disabled.
  return false == this->isEnabled ( state );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the OpenGL viewport based on the given viewport.
//
///////////////////////////////////////////////////////////////////////////////

bool SgGlRenderer::setViewport ( const SlViewporti &viewport ) 
{
  SL_PRINT ( "SgGlRenderer::setViewport()\n" );
  SL_ASSERT ( this );
  SL_ASSERT ( viewport.getX() >= 0 || viewport.getY() >= 0 || viewport.getWidth() > 0 || viewport.getHeight() > 0 );

  // Make our context the current one.
  if ( false == this->_makeContextCurrent() )
    return false;

  SG_GL_ERROR_CHECKER;

  // Set the viewport.
  ::glViewport ( viewport.getX(), viewport.getY(), viewport.getWidth(), viewport.getHeight() );

  // Success.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the OpenGL viewport.
//
///////////////////////////////////////////////////////////////////////////////

bool SgGlRenderer::getViewport ( SlViewporti &viewport ) const
{
  SL_PRINT ( "SgGlRenderer::getViewport()\n" );
  SL_ASSERT ( this );

  // Make our context the current one.
  if ( false == this->_makeContextCurrent() )
  {
    SL_ASSERT ( 0 );
    return false;
  }

  SG_GL_ERROR_CHECKER;

  // Get the OpenGL viewport.
  int vp[4];
  ::glGetIntegerv ( GL_VIEWPORT, vp );

  // Set the given values.
  viewport.setValue ( vp[0], vp[1], vp[2], vp[3] );

  // Success.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make this context current.
//
///////////////////////////////////////////////////////////////////////////////

bool SgGlRenderer::_makeContextCurrent() const
{
  SL_PRINT ( "SgGlRenderer::_makeContextCurrent()\n" );
  SL_ASSERT ( this );
  return _context->makeCurrent();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Swap the buffers.
//
///////////////////////////////////////////////////////////////////////////////

bool SgGlRenderer::_swapBuffers() const
{
  SL_PRINT ( "SgGlRenderer::_swapBuffers()\n" );
  SL_ASSERT ( this );
  return _context->swapBuffers();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit this kind of node.
//
///////////////////////////////////////////////////////////////////////////////

bool SgGlRenderer::visit ( SgBackground & )
{
  SL_ASSERT ( this );
  SL_ASSERT ( 0 ); return true; // TODO.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit this kind of node.
//
///////////////////////////////////////////////////////////////////////////////

bool SgGlRenderer::visit ( SgBaseColor & )
{
  SL_ASSERT ( this );
  SL_ASSERT ( 0 ); return true; // TODO.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit this kind of node.
//
///////////////////////////////////////////////////////////////////////////////

bool SgGlRenderer::visit ( SgBillboard & )
{
  SL_ASSERT ( this );
  SL_ASSERT ( 0 ); return true; // TODO.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit this kind of node.
//
///////////////////////////////////////////////////////////////////////////////

bool SgGlRenderer::visit ( SgCamera & )
{
  SL_ASSERT ( this );
  SL_ASSERT ( 0 ); return true; // TODO.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit this kind of node.
//
///////////////////////////////////////////////////////////////////////////////

bool SgGlRenderer::visit ( SgCircle & )
{
  SL_ASSERT ( this );
  SL_ASSERT ( 0 ); return true; // TODO.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit this kind of node.
//
///////////////////////////////////////////////////////////////////////////////

bool SgGlRenderer::visit ( SgColorWheel & )
{
  SL_ASSERT ( this );
  SL_ASSERT ( 0 ); return true; // TODO.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit this kind of node.
//
///////////////////////////////////////////////////////////////////////////////

bool SgGlRenderer::visit ( SgCone & )
{
  SL_ASSERT ( this );
  SL_ASSERT ( 0 ); return true; // TODO.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit this kind of node.
//
///////////////////////////////////////////////////////////////////////////////

bool SgGlRenderer::visit ( SgCoordinate3 & )
{
  SL_ASSERT ( this );
  SL_ASSERT ( 0 ); return true; // TODO.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit this kind of node.
//
///////////////////////////////////////////////////////////////////////////////

bool SgGlRenderer::visit ( SgCoordinate4 & )
{
  SL_ASSERT ( this );
  SL_ASSERT ( 0 ); return true; // TODO.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit this kind of node.
//
///////////////////////////////////////////////////////////////////////////////

bool SgGlRenderer::visit ( SgCube &cube )
{
  SL_ASSERT ( this );
  SL_ASSERT ( GL_NO_ERROR == ::glGetError() );

  float halfSize ( cube.getSize() * 0.5f );
  const SlVec3f &center = cube.getCenter();

  float a ( center[0] - halfSize );
  float b ( center[0] + halfSize );
  float c ( center[1] - halfSize );
  float d ( center[1] + halfSize );
  float e ( center[2] + halfSize );
  float f ( center[2] - halfSize );

  // I thought about making this tri-strips or perhaps individual triangles,
  // but this way there are fewer calls to glVertex().
  ::glBegin ( GL_QUADS );
  {
    ::glNormal3f ( 0.0f, 0.0f, 1.0f );
    ::glVertex3f ( a, c, e );
    ::glVertex3f ( b, c, e );
    ::glVertex3f ( b, d, e );
    ::glVertex3f ( a, d, e );

    ::glNormal3f ( 0.0f, 1.0f, 0.0f );
    ::glVertex3f ( a, d, e );
    ::glVertex3f ( b, d, e );
    ::glVertex3f ( b, d, f );
    ::glVertex3f ( a, d, f );

    ::glNormal3f ( 0.0f, 0.0f, -1.0f );
    ::glVertex3f ( a, d, f );
    ::glVertex3f ( b, d, f );
    ::glVertex3f ( b, c, f );
    ::glVertex3f ( a, c, f );

    ::glNormal3f ( 0.0f, -1.0f, 0.0f );
    ::glVertex3f ( a, c, f );
    ::glVertex3f ( b, c, f );
    ::glVertex3f ( b, c, e );
    ::glVertex3f ( a, c, e );

    ::glNormal3f ( -1.0f, 0.0f, 0.0f );
    ::glVertex3f ( a, c, f );
    ::glVertex3f ( a, c, e );
    ::glVertex3f ( a, d, e );
    ::glVertex3f ( a, d, f );

    ::glNormal3f ( 1.0f, 0.0f, 0.0f );
    ::glVertex3f ( b, c, e );
    ::glVertex3f ( b, c, f );
    ::glVertex3f ( b, d, f );
    ::glVertex3f ( b, d, e );
  }
  ::glEnd();
  SL_ASSERT ( GL_NO_ERROR == ::glGetError() );

#ifdef _DEBUG

  // Draw the bounding box in global space.
  float hs = halfSize;
  SlVec3f min ( center[0] - hs, center[1] - hs, center[2] - hs );
  SlVec3f max ( center[0] + hs, center[1] + hs, center[2] + hs );
  SlBoundingBoxf lbbox ( min, max );

  // Put it in global space.
  const SlMatrix44f &M = _state->getModelviewMatrix();
  SlBoundingBoxf gbbox = M * lbbox;
  min = gbbox.getMin();
  max = gbbox.getMax();
  //SL_TRACE ( "      gbbox = %12.7f%12.7f%12.7f%12.7f%12.7f%12.7f\n", min[0], min[1], min[2], max[0], max[1], max[2] );

  _state->pushModelviewMatrix();
  _state->makeModelviewMatrixIdentity();

  ::glBegin ( GL_LINE_LOOP );
  {
    ::glNormal3f ( 0.0f, 0.0f, 1.0f );
    ::glVertex3f ( min[0], min[1], min[2] );
    ::glVertex3f ( max[0], min[1], min[2] );
    ::glVertex3f ( max[0], max[1], min[2] );
    ::glVertex3f ( min[0], max[1], min[2] );
  }
  ::glEnd();

  ::glBegin ( GL_LINE_LOOP );
  {
    ::glVertex3f ( min[0], min[1], max[2] );
    ::glVertex3f ( max[0], min[1], max[2] );
    ::glVertex3f ( max[0], max[1], max[2] );
    ::glVertex3f ( min[0], max[1], max[2] );
  }
  ::glEnd();

  ::glBegin ( GL_LINES );
  {
    ::glVertex3f ( min[0], min[1], min[2] );
    ::glVertex3f ( min[0], min[1], max[2] );

    ::glVertex3f ( max[0], min[1], min[2] );
    ::glVertex3f ( max[0], min[1], max[2] );

    ::glVertex3f ( max[0], max[1], min[2] );
    ::glVertex3f ( max[0], max[1], max[2] );

    ::glVertex3f ( min[0], max[1], min[2] );
    ::glVertex3f ( min[0], max[1], max[2] );
  }
  ::glEnd();

  _state->popModelviewMatrix();

#endif // _DEBUG

  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit this kind of node.
//
///////////////////////////////////////////////////////////////////////////////

bool SgGlRenderer::visit ( SgCylinder & )
{
  SL_ASSERT ( this );
  SL_ASSERT ( 0 ); return true; // TODO.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit this kind of node.
//
///////////////////////////////////////////////////////////////////////////////

bool SgGlRenderer::visit ( SgDirectionalLight & )
{
  SL_ASSERT ( this );
  SL_ASSERT ( 0 ); return true; // TODO.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit this kind of node.
//
///////////////////////////////////////////////////////////////////////////////

bool SgGlRenderer::visit ( SgDrawStyle &drawStyle )
{
  SL_ASSERT ( this );

  // Set the current node.
  this->_setCurrentDrawStyle ( &drawStyle );

  // Set the polygon mode.
  switch ( drawStyle.polygonStyle )
  {
    case SgDrawStyle::FILLED:

      switch ( drawStyle.side )
      {
      case SgDrawStyle::FRONT_AND_BACK: ::glPolygonMode ( GL_FRONT_AND_BACK, GL_FILL ); break;
      case SgDrawStyle::FRONT:          ::glPolygonMode ( GL_FRONT,          GL_FILL ); break;
      case SgDrawStyle::BACK:           ::glPolygonMode ( GL_BACK,           GL_FILL ); break;
      default: SL_ASSERT ( 0 );
      }

      break;

    case SgDrawStyle::LINES:

      switch ( drawStyle.side )
      {
      case SgDrawStyle::FRONT_AND_BACK: ::glPolygonMode ( GL_FRONT_AND_BACK, GL_LINE ); break;
      case SgDrawStyle::FRONT:          ::glPolygonMode ( GL_FRONT,          GL_LINE ); break;
      case SgDrawStyle::BACK:           ::glPolygonMode ( GL_BACK,           GL_LINE ); break;
      default: SL_ASSERT ( 0 );
      }

      break;

    case SgDrawStyle::POINTS:

      switch ( drawStyle.side )
      {
      case SgDrawStyle::FRONT_AND_BACK: ::glPolygonMode ( GL_FRONT_AND_BACK, GL_POINT ); break;
      case SgDrawStyle::FRONT:          ::glPolygonMode ( GL_FRONT,          GL_POINT ); break;
      case SgDrawStyle::BACK:           ::glPolygonMode ( GL_BACK,           GL_POINT ); break;
      default: SL_ASSERT ( 0 );
      }

      break;

    default:

      SL_ASSERT ( 0 );
      break;
  }

  // Set these for all cases.
  _state->enable ( GL_LINE_STIPPLE );
  ::glPointSize ( drawStyle.pointSize );
  ::glLineWidth ( drawStyle.lineWidth );
  ::glLineStipple ( drawStyle.lineStippleFactor, drawStyle.lineStipplePattern );

  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit this kind of node.
//
///////////////////////////////////////////////////////////////////////////////

bool SgGlRenderer::visit ( SgEllipse & )
{
  SL_ASSERT ( this );
  SL_ASSERT ( 0 ); return true; // TODO.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit this kind of node.
//
///////////////////////////////////////////////////////////////////////////////

bool SgGlRenderer::visit ( SgIdentity & )
{
  SL_ASSERT ( this );
  SL_ASSERT ( 0 ); return true; // TODO.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit this kind of node.
//
///////////////////////////////////////////////////////////////////////////////

bool SgGlRenderer::visit ( SgLineSet & )
{
  SL_ASSERT ( this );
  SL_ASSERT ( 0 ); return true; // TODO.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Convenient function used to set the material.
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit
{
void _setMaterial ( const GLenum &which, SlMaterialf &material )
{
  // Set the colors and values that are valid.
  if ( material.isValid ( SlMaterialf::AMBIENT ) )
    ::glMaterialfv ( GL_FRONT, GL_AMBIENT, material.getAmbient() );

  if ( material.isValid ( SlMaterialf::DIFFUSE ) )
    ::glMaterialfv ( GL_FRONT, GL_DIFFUSE, material.getDiffuse() );

  if ( material.isValid ( SlMaterialf::SPECULAR ) )
    ::glMaterialfv ( GL_FRONT, GL_SPECULAR, material.getSpecular() );

  if ( material.isValid ( SlMaterialf::EMISSIVE ) )
    ::glMaterialfv ( GL_FRONT, GL_EMISSION, material.getEmissive() );

  if ( material.isValid ( SlMaterialf::SHININESS ) )
    ::glMaterialf  ( GL_FRONT, GL_SHININESS, material.getShininess() * SHININESS_SCALE );
}
}; // namespace CadKit


///////////////////////////////////////////////////////////////////////////////
//
//  Visit this kind of node.
//
///////////////////////////////////////////////////////////////////////////////

bool SgGlRenderer::visit ( SgMaterial &material )
{
  SL_ASSERT ( this );

  // Set the current node.
  this->_setCurrentMaterial ( &material );

  // Have to have this to do materials.
  _state->disable ( GL_COLOR_MATERIAL );
  _state->enable  ( GL_LIGHTING );

	// For transparency.
	if ( 1.0f != material.material.getDiffuse()[3] )
	{
		_state->enable ( GL_BLEND );
    ::glBlendFunc ( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	}

  // Process the material side.
  switch ( material.side )
  {
  case SgMaterial::FRONT:

    CadKit::_setMaterial ( GL_FRONT, material.material );
    break;

  case SgMaterial::BACK:

    CadKit::_setMaterial ( GL_BACK, material.material );
    break;

  case SgMaterial::FRONT_AND_BACK:

    CadKit::_setMaterial ( GL_FRONT_AND_BACK, material.material );
    break;

  default:

    SL_ASSERT ( 0 ); // What?
    return false;
  }

  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit this kind of node.
//
///////////////////////////////////////////////////////////////////////////////

bool SgGlRenderer::visit ( SgNormal & )
{
  SL_ASSERT ( this );
  SL_ASSERT ( 0 ); return true; // TODO.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit this kind of node.
//
///////////////////////////////////////////////////////////////////////////////

bool SgGlRenderer::visit ( SgOrthographicCamera &camera )
{
  SL_ASSERT ( this );

  GLdouble width = camera.getHeight() * camera.getAspectRatio();

  GLdouble left = -width * 0.5;
  GLdouble right = width * 0.5;

  GLdouble bottom = -camera.getHeight() * 0.5;
  GLdouble top =     camera.getHeight() * 0.5;

  ::glLoadIdentity();
  ::glOrtho ( left, right, bottom, top, camera.getZNear(), camera.getZFar() );
  ::glLoadMatrixf ( camera.getModelviewMatrix() );

  return true;
}

/*
void SgGlRenderer::visit ( SgPerspectiveCamera &camera )
{
  // Set the projection.
  ::glMatrixMode ( GL_PROJECTION );
  ::glLoadIdentity();
  ::gluPerspective ( camera.getFieldOfView(), camera.getAspectRatio(), camera.getZNear(), camera.getZFar() );

  // Now set the modelview matrix.
  ::glMatrixMode ( GL_MODELVIEW );
  ::glLoadMatrixf ( camera.getModelviewMatrix() ); // This is critical.
}
*/

///////////////////////////////////////////////////////////////////////////////
//
//  Visit this kind of node.
//
///////////////////////////////////////////////////////////////////////////////

bool SgGlRenderer::visit ( SgPerspectiveCamera &camera )
{
  SL_ASSERT ( this );

  // Set the projection.
  SlMatrix44f P ( false );
  P.setPerspective ( SL_DEG_TO_RAD_F * camera.getFieldOfView(), camera.getAspectRatio(), camera.getZNear(), camera.getZFar() );
  _state->setProjectionMatrix ( P );

  // Now set the modelview matrix.
  _state->setModelviewMatrix ( camera.getModelviewMatrix() ); // This is critical.

  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit this kind of node.
//
///////////////////////////////////////////////////////////////////////////////

bool SgGlRenderer::visit ( SgPointSet & )
{
  SL_ASSERT ( this );
  SL_ASSERT ( 0 ); return true; // TODO.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit this kind of node.
//
///////////////////////////////////////////////////////////////////////////////

bool SgGlRenderer::visit ( SgQuadMesh & )
{
  SL_ASSERT ( this );
  SL_ASSERT ( 0 ); return true; // TODO.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit this kind of node.
//
///////////////////////////////////////////////////////////////////////////////

bool SgGlRenderer::visit ( SgRectangle & )
{
  SL_ASSERT ( this );
  SL_ASSERT ( 0 ); return true; // TODO.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit this kind of node.
//
///////////////////////////////////////////////////////////////////////////////

bool SgGlRenderer::visit ( SgRotation &rotation )
{
  SL_ASSERT ( this );
  const SlVec3f &axis = rotation.getAxis();
  ::glRotatef ( rotation.getDegrees(), axis[0], axis[1], axis[2] );
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit this kind of node.
//
///////////////////////////////////////////////////////////////////////////////

bool SgGlRenderer::visit ( SgScale &scale )
{
  SL_ASSERT ( this );
  const SlVec3f &s = scale.getScale();
  ::glScalef ( s[0], s[1], s[2] );
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit this kind of node.
//
///////////////////////////////////////////////////////////////////////////////

bool SgGlRenderer::visit ( SgSelection &selection )
{
  SL_ASSERT ( this );

  // Pass this to the separator function.
  if ( false == this->visit ( (SgSeparator &) selection ) )
  {
    SL_ASSERT ( 0 ); // Why didn't it render?
    return false;
  }

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit this kind of node.
//
///////////////////////////////////////////////////////////////////////////////

bool SgGlRenderer::visit ( SgSeparator &separator )
{
  SL_ASSERT ( this );
  SL_ASSERT ( GL_NO_ERROR == ::glGetError() );

  // Save the current state.
  this->_pushStateNodes ( separator );
  this->_pushMatrices ( separator );
  SL_ASSERT ( GL_NO_ERROR == ::glGetError() );

  // Pass this to the group function.
  if ( false == this->visit ( (SgGroup &) separator ) )
  {
    SL_ASSERT ( 0 ); // Why didn't it render?
    return false;
  }

  // Pop everything we pushed.
  this->_popStateNodes ( separator );
  this->_popMatrices ( separator );
  SL_ASSERT ( GL_NO_ERROR == ::glGetError() );

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit this kind of node.
//
///////////////////////////////////////////////////////////////////////////////

bool SgGlRenderer::visit ( SgGroup &group )
{
  SL_ASSERT ( this );
  SL_ASSERT ( GL_NO_ERROR == ::glGetError() );

  // Used below.
  SgNode *child = NULL;

  // Get the number of children.
  unsigned int numChildren = group.getNumChildren();

  // Loop through all the nodes.
  for ( unsigned int i = 0; i < numChildren; ++i )
  {
    // Get the child.
    child = group.getChild ( i );

    // Give the child this visitor.
    if ( false == child->accept ( *this ) ) 
    {
      SL_ASSERT ( 0 ); // Why didn't it render?
      return false;
    }

    // Should be true.
    SL_ASSERT ( GL_NO_ERROR == ::glGetError() );

    // How long did it take?
    //SL_TRACE ( "node = %X, name = %s, type = %s, time = %d\n", child, ( ( NULL != child->getName() ) ? child->getName() : "" ), child->getClassName(), child->getRenderTime() );
  }

  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit this kind of node.
//
///////////////////////////////////////////////////////////////////////////////

bool SgGlRenderer::visit ( SgShading & )
{
  SL_ASSERT ( this );
  SL_ASSERT ( 0 ); return true; // TODO.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Subdivide the sphere.
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit
{
void _subdivideSphere (
  const float &x1, const float &y1, const float &z1, 
  const float &x2, const float &y2, const float &z2, 
  const float &x3, const float &y3, const float &z3, 
  const float &r,  unsigned int depth )
{
  if ( depth == 0 )
  {
    // The vertices are all a unit length from the center, so they are also the normal.
    glBegin ( GL_TRIANGLES );
    {
      if ( 1.0f == r )
      {
        glNormal3f ( x1, y1, z1 ); glVertex3f ( x1, y1, z1 );
        glNormal3f ( x2, y2, z2 ); glVertex3f ( x2, y2, z2 );
        glNormal3f ( x3, y3, z3 ); glVertex3f ( x3, y3, z3 );
        //SL_PRINT ( "\t%18.15ff, %18.15ff, %18.15ff, ", x1, y1, z1 ); // Used this to make the hard coded vertices.
        //SL_PRINT (   "%18.15ff, %18.15ff, %18.15ff, ", x2, y2, z2 );
        //SL_PRINT (   "%18.15ff, %18.15ff, %18.15ff,\n", x3, y3, z3 );
      }

      else
      {
        glNormal3f ( x1, y1, z1 ); glVertex3f ( r * x1, r * y1, r * z1 );
        glNormal3f ( x2, y2, z2 ); glVertex3f ( r * x2, r * y2, r * z2 );
        glNormal3f ( x3, y3, z3 ); glVertex3f ( r * x3, r * y3, r * z3 );
      }
    }
    glEnd();
    return;
  }

  float x12 = x1 + x2;
  float x23 = x2 + x3;
  float x31 = x3 + x1;

  float y12 = y1 + y2;
  float y23 = y2 + y3;
  float y31 = y3 + y1;

  float z12 = z1 + z2;
  float z23 = z2 + z3;
  float z31 = z3 + z1;

  float d = ::sqrtf ( x12 * x12 + y12 * y12 + z12 * z12 ); SL_ASSERT ( d != 0 );
  float invd = 1.0f / d;
  x12 *= invd; y12 *= invd; z12 *= invd;

  d = ::sqrtf ( x23 * x23 + y23 * y23 + z23 * z23 ); SL_ASSERT ( d != 0 );
  invd = 1.0f / d;
  x23 *= invd; y23 *= invd; z23 *= invd;

  d = ::sqrtf ( x31 * x31 + y31 * y31 + z31 * z31 ); SL_ASSERT ( d != 0 );
  invd = 1.0f / d;
  x31 *= invd; y31 *= invd; z31 *= invd;

  --depth;
  _subdivideSphere (  x1,  y1,  z1, x12, y12, z12, x31, y31, z31, r, depth );
  _subdivideSphere (  x2,  y2,  z2, x23, y23, z23, x12, y12, z12, r, depth );
  _subdivideSphere (  x3,  y3,  z3, x31, y31, z31, x23, y23, z23, r, depth );
  _subdivideSphere ( x12, y12, z12, x23, y23, z23, x31, y31, z31, r, depth );
}
}; // namespace CadKit


///////////////////////////////////////////////////////////////////////////////
//
//  Subdivide the sphere.
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit
{
void _renderTriangles ( const float *pts, const int &numPts, const float &cx, const float &cy, const float &cz, const float &r )
{
  SL_ASSERT ( pts );
  SL_ASSERT ( numPts > 0 );
  SL_ASSERT ( numPts % 3 == 0 );

  // Initialize.
  int i = 0;

  // The vertices are all a unit length from the center, so they are also the normal.
  glBegin ( GL_TRIANGLES );
  {
    if ( 1.0 == r )
    {
      while ( i < numPts )
      {
        SL_ASSERT ( i >= 0 && i + 8 < numPts );
        glNormal3f ( pts[i  ], pts[i+1], pts[i+2] ); glVertex3f ( cx + pts[i  ], cy + r * pts[i+1], cz + pts[i+2] );
        glNormal3f ( pts[i+3], pts[i+4], pts[i+5] ); glVertex3f ( cx + pts[i+3], cy + r * pts[i+4], cz + pts[i+5] );
        glNormal3f ( pts[i+6], pts[i+7], pts[i+8] ); glVertex3f ( cx + pts[i+6], cy + r * pts[i+7], cz + pts[i+8] );
        i += 9;
      }
    }

    else
    {
      while ( i < numPts )
      {
        SL_ASSERT ( i >= 0 && i + 8 < numPts );
        glNormal3f ( pts[i  ], pts[i+1], pts[i+2] ); glVertex3f ( cx + r * pts[i  ], cy + r * pts[i+1], cz + r * pts[i+2] );
        glNormal3f ( pts[i+3], pts[i+4], pts[i+5] ); glVertex3f ( cx + r * pts[i+3], cy + r * pts[i+4], cz + r * pts[i+5] );
        glNormal3f ( pts[i+6], pts[i+7], pts[i+8] ); glVertex3f ( cx + r * pts[i+6], cy + r * pts[i+7], cz + r * pts[i+8] );
        i += 9;
      }
    }
  }
  glEnd();

  // Check for walking off the end.
  SL_ASSERT ( i == numPts );
}
}; // namespace CadKit


///////////////////////////////////////////////////////////////////////////////
//
//  Visit this kind of node.
//
///////////////////////////////////////////////////////////////////////////////

bool SgGlRenderer::visit ( SgSphere &sphere )
{
  SL_ASSERT ( this && _quadric );
  SL_ASSERT ( ( sphere.getTessellationType() == SgSphere::POLES && ( sphere.getNumSlices() > 1 && sphere.getNumStacks() > 1 ) ) || ( sphere.getTessellationType() == SgSphere::NO_POLES ) );

  // For convenience.
  const float &r = sphere.getRadius();
  const SlVec3f &c = sphere.getCenter();
  SL_ASSERT ( 0.0f < r );

  // Draw the sphere with no poles.
  if ( SgSphere::NO_POLES == sphere.getTessellationType() )
  {
    const unsigned int &n = sphere.getNumSubdivisions();
    const float X = 0.525731112119133606f;
    const float Z = 0.8506508083528655993f;

    switch ( n )
    {
    case 0: _renderTriangles ( SlPreComputedArrays::getSphereVerts0(), SlPreComputedArrays::getNumSphereVerts0(), c[0], c[1], c[2], r ); break;
    case 1: _renderTriangles ( SlPreComputedArrays::getSphereVerts1(), SlPreComputedArrays::getNumSphereVerts1(), c[0], c[1], c[2], r ); break;
    case 2: _renderTriangles ( SlPreComputedArrays::getSphereVerts2(), SlPreComputedArrays::getNumSphereVerts2(), c[0], c[1], c[2], r ); break;
    case 3: _renderTriangles ( SlPreComputedArrays::getSphereVerts3(), SlPreComputedArrays::getNumSphereVerts3(), c[0], c[1], c[2], r ); break;
    case 4: _renderTriangles ( SlPreComputedArrays::getSphereVerts4(), SlPreComputedArrays::getNumSphereVerts4(), c[0], c[1], c[2], r ); break;
    default:

      _state->pushModelviewMatrix();

      // Translate to where you belong.
      ::glTranslatef ( c[0], c[1], c[2] );

      _subdivideSphere ( -X,  0,  Z,  X,  0,  Z,  0,  Z,  X, r, n );
      _subdivideSphere ( -X,  0,  Z,  0,  Z,  X, -Z,  X,  0, r, n );
      _subdivideSphere ( -Z,  X,  0,  0,  Z,  X,  0,  Z, -X, r, n );
      _subdivideSphere (  0,  Z,  X,  Z,  X,  0,  0,  Z, -X, r, n );
      _subdivideSphere (  0,  Z,  X,  X,  0,  Z,  Z,  X,  0, r, n );
      _subdivideSphere (  Z,  X,  0,  X,  0,  Z,  Z, -X,  0, r, n );
      _subdivideSphere (  Z,  X,  0,  Z, -X,  0,  X,  0, -Z, r, n );
      _subdivideSphere (  0,  Z, -X,  Z,  X,  0,  X,  0, -Z, r, n );
      _subdivideSphere (  0,  Z, -X,  X,  0, -Z, -X,  0, -Z, r, n );
      _subdivideSphere ( -X,  0, -Z,  X,  0, -Z,  0, -Z, -X, r, n );
      _subdivideSphere (  0, -Z, -X,  X,  0, -Z,  Z, -X,  0, r, n );
      _subdivideSphere (  0, -Z, -X,  Z, -X,  0,  0, -Z,  X, r, n );
      _subdivideSphere (  0, -Z, -X,  0, -Z,  X, -Z, -X,  0, r, n );
      _subdivideSphere ( -Z, -X,  0,  0, -Z,  X, -X,  0,  Z, r, n );
      _subdivideSphere ( -X,  0,  Z,  0, -Z,  X,  X,  0,  Z, r, n );
      _subdivideSphere (  0, -Z,  X,  Z, -X,  0,  X,  0,  Z, r, n );
      _subdivideSphere ( -Z,  X,  0, -Z, -X,  0, -X,  0,  Z, r, n );
      _subdivideSphere ( -Z,  X,  0, -X,  0, -Z, -Z, -X,  0, r, n );
      _subdivideSphere ( -Z,  X,  0,  0,  Z, -X, -X,  0, -Z, r, n );
      _subdivideSphere (  0, -Z, -X, -Z, -X,  0, -X,  0, -Z, r, n );

      _state->popModelviewMatrix();

      break;
    }
  }

  // Draw the sphere with poles.
  else
  {
    _state->pushModelviewMatrix();

    // Translate to where you belong.
    ::glTranslatef ( c[0], c[1], c[2] );

    // Draw the sphere with two poles.
    ::gluSphere ( (GLUquadricObj *) _quadric, r, sphere.getNumSlices(), sphere.getNumStacks() );

    _state->popModelviewMatrix();
  }

  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit this kind of node.
//
///////////////////////////////////////////////////////////////////////////////

bool SgGlRenderer::visit ( SgSquare & )
{
  SL_ASSERT ( this );
  SL_ASSERT ( 0 ); return true; // TODO.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit this kind of node.
//
///////////////////////////////////////////////////////////////////////////////

bool SgGlRenderer::visit ( SgSwitch & )
{
  SL_ASSERT ( this );
  SL_ASSERT ( 0 ); return true; // TODO.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit this kind of node.
//
///////////////////////////////////////////////////////////////////////////////

bool SgGlRenderer::visit ( SgTransform & )
{
  SL_ASSERT ( this );
  SL_ASSERT ( 0 ); return true; // TODO.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit this kind of node.
//
///////////////////////////////////////////////////////////////////////////////

bool SgGlRenderer::visit ( SgTranslation &translation )
{
  SL_ASSERT ( this );

  // Set the translation.
  const SlVec3f &t = translation.getTranslation();
  ::glTranslatef ( t[0], t[1], t[2] );

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit this kind of node.
//
///////////////////////////////////////////////////////////////////////////////

bool SgGlRenderer::visit ( SgTriangle & )
{
  SL_ASSERT ( this );
  SL_ASSERT ( 0 ); return true; // TODO.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit this kind of node.
//
///////////////////////////////////////////////////////////////////////////////

bool SgGlRenderer::visit ( SgTriStrip & )
{
  SL_ASSERT ( this );
  SL_ASSERT ( 0 ); return true; // TODO.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit this kind of node.
//
///////////////////////////////////////////////////////////////////////////////

bool SgGlRenderer::visit ( SgUnScale & )
{
  SL_ASSERT ( this );
  SL_ASSERT ( 0 ); return true; // TODO.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit this kind of node.
//
///////////////////////////////////////////////////////////////////////////////

bool SgGlRenderer::visit ( SgText & )
{
  SL_ASSERT ( this );
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  These are all the visit functions that we don't use. They should never 
//  get called. However, the base class requires that we overload them.
//
///////////////////////////////////////////////////////////////////////////////

bool SgGlRenderer::visit ( SgNode & )       { SL_ASSERT ( 0 ); return true; }
bool SgGlRenderer::visit ( SgShape & )      { SL_ASSERT ( 0 ); return true; }
bool SgGlRenderer::visit ( SgCoordinate & ) { SL_ASSERT ( 0 ); return true; }
bool SgGlRenderer::visit ( SgLight & )      { SL_ASSERT ( 0 ); return true; }
bool SgGlRenderer::visit ( SgVertexSet & )  { SL_ASSERT ( 0 ); return true; }
bool SgGlRenderer::visit ( SgPrimitive & )  { SL_ASSERT ( 0 ); return true; }


///////////////////////////////////////////////////////////////////////////////
//
//  Push the appropriate matrices.
//
///////////////////////////////////////////////////////////////////////////////

void SgGlRenderer::_pushMatrices ( const SgSeparator &separator )
{
  SL_ASSERT ( this );

  if ( separator.hasPushPopFlags ( SgSeparator::PROJECTION ) )
    _state->pushProjectionMatrix();

  if ( separator.hasPushPopFlags ( SgSeparator::MODELVIEW ) )
    _state->pushModelviewMatrix();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Pop the appropriate matrices.
//
///////////////////////////////////////////////////////////////////////////////

void SgGlRenderer::_popMatrices ( const SgSeparator &separator )
{
  SL_ASSERT ( this );

  if ( separator.hasPushPopFlags ( SgSeparator::MODELVIEW ) )
    _state->popModelviewMatrix();

  if ( separator.hasPushPopFlags ( SgSeparator::PROJECTION ) )
    _state->popProjectionMatrix();
}
