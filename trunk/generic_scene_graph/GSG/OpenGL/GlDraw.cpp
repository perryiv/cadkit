
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  An OpenGL drawing class.
//
///////////////////////////////////////////////////////////////////////////////

#include "GSG/OpenGL/Precompiled.h"
#include "GSG/OpenGL/GlDraw.h"
#include "GSG/OpenGL/GlContext.h"
#include "GSG/OpenGL/GlApi.h"
#include "GSG/OpenGL/ErrorCheck.h"

#include "GSG/Core/Primitive.h"
#include "GSG/Core/Matrix.h"
#include "GSG/Core/Material.h"
#include "GSG/Core/Defaults.h"
#include "GSG/Core/Bits.h"
#include "GSG/Core/Container.h"


#include <GL/glu.h>

using namespace GSG;

GSG_IMPLEMENT_CLONE ( GlDraw );

const GSG::Real SHININESS_SCALE = 128.0f; // OpenGL is [0,128].


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

GlDraw::GlDraw ( RenderBin *bin, const Viewport &vp, GlContext *context ) : 
  Draw ( bin ),
  _context ( context ),
  _stateBits ( 0 )
{
  // Initialize if we have a valid context and viewport.
  if ( context, vp.valid() )
    this->init ( vp, context );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Copy constructor. 
//  Note, we just reference the context. The client can change this later.
//
///////////////////////////////////////////////////////////////////////////////

GlDraw::GlDraw ( const GlDraw &d ) : Draw ( d ),
  _context ( d._context ),
  _stateBits ( d._stateBits )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

GlDraw::~GlDraw()
{
  // Empty.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set our OpenGL defaults.
//
///////////////////////////////////////////////////////////////////////////////

void GlDraw::init ( const Viewport &vp, GlContext *context )
{
  Lock lock ( this );
  ErrorChecker ( vp.width() > 0 );
  ErrorChecker ( vp.height() > 0 );
  ErrorChecker ( 0x0 != context );

  // Set the context. Have to do this first.
  this->context ( context );

  // Make our new context the current one.
  this->_makeContextCurrent();

  // Set the viewport.
  this->viewport ( vp );

  // Projection matrix defaults.
  Real aspect = static_cast < Real > ( vp.width() ) 
              / static_cast < Real > ( vp.height() );
  Matrix P;
  P.perspective ( GSG_FIELD_OF_VIEW_Y, aspect, GSG_CAMERA_Z_NEAR, GSG_CAMERA_Z_FAR );
  GL::matrixMode ( GL_PROJECTION );
  GL::loadMatrix ( P );
  GL::matrixMode ( GL_MODELVIEW );

  // Default materials. Grey on the front.
  GL::material ( GL_FRONT, GL_AMBIENT,   GSG_MATERIAL_FRONT_AMBIENT );
  GL::material ( GL_FRONT, GL_DIFFUSE,   GSG_MATERIAL_FRONT_DIFFUSE );
  GL::material ( GL_FRONT, GL_SPECULAR,  GSG_MATERIAL_FRONT_SPECULAR );
  GL::material ( GL_FRONT, GL_EMISSION,  GSG_MATERIAL_FRONT_EMISSIVE );
  GL::material ( GL_FRONT, GL_SHININESS, GSG_MATERIAL_FRONT_SHININESS * SHININESS_SCALE ); 

  // Black on the back.
  GL::material ( GL_BACK, GL_AMBIENT,   GSG_MATERIAL_BACK_AMBIENT );
  GL::material ( GL_BACK, GL_DIFFUSE,   GSG_MATERIAL_BACK_DIFFUSE );
  GL::material ( GL_BACK, GL_SPECULAR,  GSG_MATERIAL_BACK_SPECULAR );
  GL::material ( GL_BACK, GL_EMISSION,  GSG_MATERIAL_BACK_EMISSIVE );
  GL::material ( GL_BACK, GL_SHININESS, GSG_MATERIAL_BACK_SHININESS * SHININESS_SCALE );

  // Default light. Have to make sure the lights are drawn with 
  // respect to world space.
  GL::loadIdentity(); // TODO, Needed?

  // All white light.
  GL::enable ( GL_LIGHT0 );
  GL::light ( GL_LIGHT0, GL_AMBIENT,  GSG_LIGHT_AMBIENT );
  GL::light ( GL_LIGHT0, GL_DIFFUSE,  GSG_LIGHT_DIFFUSE );
  GL::light ( GL_LIGHT0, GL_SPECULAR, GSG_LIGHT_SPECULAR );
  GL::light ( GL_LIGHT0, GL_POSITION, GSG_LIGHT_POSITION );

  // This will let the material control two-sided lighting.
  GL::lightModel ( GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE );

  // Misc.
  GL::clearDepth ( 1.0f );
  GL::enable ( GL_AUTO_NORMAL );
  GL::enable ( GL_DEPTH_TEST );
  GL::clearColor ( GSG_BACKGROUND_COLOR );

  // Scale normals. See: 
  // http://www.opengl.org/developers/code/features/KilgardTechniques/oglpitfall/oglpitfall.html
  // Basically, scaling on the modelview matrix will mess up lighting unless 
  // you tell OpenGL to scale the normal vectors.
  GL::enable ( GL_NORMALIZE );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the OpenGL drawing context.
//
///////////////////////////////////////////////////////////////////////////////

void GlDraw::context ( GlContext *context )
{
  Lock lock ( this );
  _context = context;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the OpenGL drawing context.
//
///////////////////////////////////////////////////////////////////////////////

const GlContext *GlDraw::context() const
{
  Lock lock ( this );
  return _context.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  The background color.
//
///////////////////////////////////////////////////////////////////////////////

void GlDraw::backgroundColor ( const Color &c )
{
  Lock lock ( this );
  this->_makeContextCurrent();

  // Set the color.
  GL::clearColor ( c );

  // Call the base class's function.
  BaseClass::backgroundColor ( c );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the OpenGL viewport.
//
///////////////////////////////////////////////////////////////////////////////

void GlDraw::viewport ( const Viewport &vp ) 
{
  Lock lock ( this );
  this->_makeContextCurrent();

  // Set the viewport.
  GL::viewport ( vp );

  // Call the base class's function.
  BaseClass::viewport ( vp );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Swap the buffers.
//
///////////////////////////////////////////////////////////////////////////////

void GlDraw::_swapBuffers() const
{
  Lock lock ( this );
  ErrorChecker ( _context.valid() );
  _context->swapBuffers();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make this context current.
//
///////////////////////////////////////////////////////////////////////////////

void GlDraw::_makeContextCurrent() const
{
  Lock lock ( this );
  ErrorChecker ( _context.valid() );
  _context->makeCurrent();
}


/////////////////////////////////////////////////////////////////////////////
//
//  Called before the scene is traversed.
//
/////////////////////////////////////////////////////////////////////////////

void GlDraw::_preTraverse ( Node & )
{
  Lock lock ( this );
  GSG_GL_ERROR_CHECKER;
  this->_makeContextCurrent();

  // Clear the background.
  GL::clear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Draw the shapes in the render-bin.
//
/////////////////////////////////////////////////////////////////////////////

void GlDraw::_traverse ( Node & )
{
  Lock lock ( this );
  GSG_GL_ERROR_CHECKER;
  this->_makeContextCurrent();

  // See if we have a valid render-bin.
  RenderBin *bin = this->renderBin();
  if ( 0x0 == bin )
    return;

  // Loop through the render-bin.
  for ( RenderBin::iterator i = bin->begin(); i != bin->end(); ++i )
  {
    // Get the element.
    RenderBin::reference element = *i;

    // Have the element accept this visitor.
    element->_accept ( *this );
  }
}


/////////////////////////////////////////////////////////////////////////////
//
//  Called after the scene is traversed.
//
/////////////////////////////////////////////////////////////////////////////

void GlDraw::_postTraverse ( Node & )
{
  Lock lock ( this );
  GSG_GL_ERROR_CHECKER;
  this->_makeContextCurrent();

  // Draw everything right now.
  GL::flush();

  // Swap the buffers.
  this->_swapBuffers();
}


/////////////////////////////////////////////////////////////////////////////
//
//  Visit the element.
//
/////////////////////////////////////////////////////////////////////////////

void GlDraw::_visit ( PrimitiveElement &e )
{
  Lock lock ( this );
  GSG_GL_ERROR_CHECKER;

  // Get the primitive.
  const Primitive *p = e.primitive();
  ErrorChecker ( 0x0 != p );

  // Loop through the vertices.
  for ( Primitive::ConstVertexIterator i = p->beginVertices(); i != p->endVertices(); ++i )
  {
    Primitive::ConstVertexIterator::reference vertex = *i;

    HERE
    Consider making Primitive not use an indexed pool, but require all the vertices.
    This will require more memory but you can use vertex-arrays (just pass the array to OpenGL).
    Then you won't need all the fancy iterator business, and can go back to using VC6.
    Besides, VC7 doesn't have OpenGL help.
  }
}


/////////////////////////////////////////////////////////////////////////////
//
//  Visit the element.
//
/////////////////////////////////////////////////////////////////////////////

void GlDraw::_visit ( MatrixLoadElement &e )
{
  Lock lock ( this );
  GSG_GL_ERROR_CHECKER;
  GL::loadMatrix ( e.matrix() );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Visit the element.
//
/////////////////////////////////////////////////////////////////////////////

void GlDraw::_visit ( MatrixModeElement &e )
{
  Lock lock ( this );
  GSG_GL_ERROR_CHECKER;

  switch ( e.mode() )
  {
    case MatrixModeElement::MODELVIEW:
      GL::matrixMode ( GL_MODELVIEW );
      break;
    case MatrixModeElement::PROJECTION:
      GL::matrixMode ( GL_PROJECTION );
      break;
    case MatrixModeElement::TEXTURE:
      GL::matrixMode ( GL_TEXTURE );
      break;
    default:
      ErrorChecker ( false );
      break;
  }
}


/////////////////////////////////////////////////////////////////////////////
//
//  Visit the element.
//
/////////////////////////////////////////////////////////////////////////////

void GlDraw::_visit ( MaterialElement &e )
{
  Lock lock ( this );
  GSG_GL_ERROR_CHECKER;

  GLenum face;

  switch ( e.mode() )
  {
    case MaterialElement::FRONT:
      face = GL_FRONT;
      break;
    case MaterialElement::BACK:
      face = GL_BACK;
      break;
    case MaterialElement::BOTH:
      face = GL_FRONT_AND_BACK;
      break;
    default:
      ErrorChecker ( false );
      break;
  }

  const Material *m = e.material();
  ErrorChecker ( 0x0 != m );

  this->_disable ( COLOR_MATERIAL, GL_COLOR_MATERIAL );
  this->_enable  ( LIGHTING, GL_LIGHTING );

  GL::material ( face, GL_AMBIENT,   m->ambient() );
  GL::material ( face, GL_DIFFUSE,   m->diffuse() );
  GL::material ( face, GL_SPECULAR,  m->specular() );
  GL::material ( face, GL_EMISSION,  m->emissive() );
  GL::material ( face, GL_SHININESS, m->shininess() * SHININESS_SCALE ); 
}


/////////////////////////////////////////////////////////////////////////////
//
//  Visit the element.
//
/////////////////////////////////////////////////////////////////////////////

void GlDraw::_visit ( ColorElement &e )
{
  Lock lock ( this );
  GSG_GL_ERROR_CHECKER;

  this->_enable  ( COLOR_MATERIAL, GL_COLOR_MATERIAL );
  this->_disable ( LIGHTING, GL_LIGHTING );

  GL::color ( e.color() );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Enable the state.
//
/////////////////////////////////////////////////////////////////////////////

void GlDraw::_enable  ( UnsignedInteger f, gl_enum g )
{
  BOOST_MPL_ASSERT_IS_SAME ( gl_enum, GLenum );

  if ( false == GSG::hasBits ( _stateBits, f ) )
  {
    GL::enable ( g );
    _stateBits = GSG::addBits ( _stateBits, f );
  }
}


/////////////////////////////////////////////////////////////////////////////
//
//  Disable the state.
//
/////////////////////////////////////////////////////////////////////////////

void GlDraw::_disable ( UnsignedInteger f, gl_enum g )
{
  BOOST_MPL_ASSERT_IS_SAME ( gl_enum, GLenum );

  if ( GSG::hasBits ( _stateBits, f ) )
  {
    GL::disable ( g );
    _stateBits = GSG::removeBits ( _stateBits, f );
  }
}
