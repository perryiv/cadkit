
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
#include "GSG/OpenGL/Utility.h"
#include "GSG/OpenGL/GlContext.h"
#include "GSG/OpenGL/GlApi.h"
#include "GSG/OpenGL/ErrorCheck.h"

#include "GSG/Core/PrimitiveSet.h"
#include "GSG/Core/Matrix.h"
#include "GSG/Core/Material.h"
#include "GSG/Core/Defaults.h"
#include "GSG/Core/Bits.h"


using namespace GSG;

GSG_IMPLEMENT_REFERENCED ( GlDraw );

const GSG::Real SHININESS_SCALE = 128.0f; // OpenGL is [0,128].


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

GlDraw::GlDraw ( RenderBin *bin, const Viewport &vp, GlContext *context ) : 
  Draw ( bin ),
  _context ( context ),
  _serverState ( 0 ),
  _clientState ( 0 )
{
  // Initialize if we have a valid context and viewport.
  if ( context && vp.valid() )
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
  _serverState ( d._serverState ),
  _clientState ( d._clientState )
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
//  Set from the given object.
//
///////////////////////////////////////////////////////////////////////////////

void GlDraw::setFrom ( const GlDraw &d )
{
  Lock lock ( this );

  // Set the members.
  _context     = d._context;
  _serverState = d._serverState;
  _clientState = d._clientState;

  // Call the base class's function.
  BaseClass::setFrom ( d );
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

  GSG_GL_ERROR_CHECKER;

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

  // Do not scale the normals. See: 
  // http://www.opengl.org/developers/code/features/KilgardTechniques/oglpitfall/oglpitfall.html
  // Basically, scaling on the modelview matrix will mess up lighting unless 
  // you tell OpenGL to scale the normal vectors.
  GL::disable ( GL_NORMALIZE );
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
  GSG_GL_ERROR_CHECKER;
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
  GSG_GL_ERROR_CHECKER;
  this->_makeContextCurrent();

  // Set the viewport.
  GL::viewport ( vp );

#if 0
  // Projection matrix defaults.
  Real aspect = static_cast < Real > ( vp.width() ) / static_cast < Real > ( vp.height() );
  Matrix P;
  P.perspective ( GSG_FIELD_OF_VIEW_Y, aspect, GSG_CAMERA_Z_NEAR, GSG_CAMERA_Z_FAR );
  GL::matrixMode ( GL_PROJECTION );
  //GL::loadMatrix ( P );
  ::gluPerspective ( GSG_FIELD_OF_VIEW_Y, aspect, GSG_CAMERA_Z_NEAR, GSG_CAMERA_Z_FAR );
  GL::matrixMode ( GL_MODELVIEW );
#endif

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


///////////////////////////////////////////////////////////////////////////////
//
//  Called before the scene is traversed.
//
///////////////////////////////////////////////////////////////////////////////

void GlDraw::_preTraverse ( Node & )
{
  Lock lock ( this );
  GSG_GL_ERROR_CHECKER;
  this->_makeContextCurrent();

  // Clear the background.
  GL::clear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Draw the shapes in the render-bin.
//
///////////////////////////////////////////////////////////////////////////////

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


///////////////////////////////////////////////////////////////////////////////
//
//  Called after the scene is traversed.
//
///////////////////////////////////////////////////////////////////////////////

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


///////////////////////////////////////////////////////////////////////////////
//
//  Visit the primitive-element.
//
//  Good page for vertex arrays:
//  http://www.parallab.uib.no/SGI_bookshelves/SGI_Developer/books/OpenGL_PG/sgi_html/ch03.html#id56018
//
///////////////////////////////////////////////////////////////////////////////

void GlDraw::_visit ( PrimitiveSetElement &e )
{
  // Vertex arrays only support single or double precision.
  BOOST_STATIC_ASSERT ( ( boost::is_same<Real,float>::value || boost::is_same<Real,double>::value ) );

  Lock lock ( this );
  this->_makeContextCurrent();
  GSG_GL_ERROR_CHECKER;

  typedef PrimitiveSet::ConstValidNoRefPtr PrimitiveSetPtr;
  typedef PrimitiveSet::ValuePool  ValuePool;
  typedef PrimitiveSet::NormalPool NormalPool;
  typedef PrimitiveSet::ColorPool  ColorPool;

  // Get the primitive-set.
  PrimitiveSetPtr ps ( e.set() );

  // Get the pools.
  const ValuePool  *interleaved = ps->values();
  const NormalPool *normals     = ps->normals();
  const ColorPool  *colors      = ps->colors();

  // If we don't have vertices or primitives then all bets are off.
  if ( 0x0 == interleaved || 
       0x0 == interleaved->ptr() || 
       0   == ps->numPrimitives() )
    return;

  // Determine the primitive type for the set.
  GLenum type ( Detail::primitiveType ( ps->type() ) );

  // Determine the interleaved format.
  GLenum format ( Detail::interleavedFormat ( interleaved ) );

  // Turn on interleaved arrays.
  GL::interleavedArrays ( format, 0, interleaved->ptr() );

  // See if we have per-primitive bindings.
  bool perPrimNormals ( Detail::isPerVertexBinding ( ps->normalBinding() ) );
  bool perPrimColors  ( Detail::isPerVertexBinding ( ps->colorBinding()  ) );

  // Should be true.
  ErrorChecker ( !perPrimNormals || ( perPrimNormals && ( normals->values().size() == ps->numPrimitives() ) ) );
  ErrorChecker ( !perPrimColors  || ( perPrimColors  && (  colors->values().size() == ps->numPrimitives() ) ) );

  // Loop through all the primitives.
  UnsignedInteger count ( 0 );
  for ( PrimitiveSet::const_iterator i = ps->begin(); i != ps->end(); ++i )
  {
    GSG_GL_ERROR_CHECK_NOW;

    // Get the primitive.
    const Primitive *prim = (*i).get();
    ErrorChecker ( 0x0 != prim );

    // Set per-primitive bindings.
    if ( perPrimNormals )
      GL::normal ( normals->value ( count ) );
    if ( perPrimColors )
      GL::color ( colors->value ( count ) );

    // Get the start and size.
    Index start ( prim->start() );
    Index size  ( prim->size()  );

    // Make sure they are in range.
    ErrorChecker ( start >= 0 && interleaved->values().size() >  start );
    ErrorChecker ( size  >  0 && interleaved->values().size() >= start + size );

    // Draw the primitive.
    GL::drawArrays ( type, start, size );

    // Increment.
    ++count;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit the element.
//
///////////////////////////////////////////////////////////////////////////////

void GlDraw::_visit ( MatrixLoadElement &e )
{
  Lock lock ( this );
  GSG_GL_ERROR_CHECKER;
  GL::loadMatrix ( e.matrix() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit the element.
//
///////////////////////////////////////////////////////////////////////////////

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


///////////////////////////////////////////////////////////////////////////////
//
//  Visit the element.
//
///////////////////////////////////////////////////////////////////////////////

void GlDraw::_visit ( MaterialElement &e )
{
  Lock lock ( this );
  ErrorChecker ( 0x0 != e.material() );
  GSG_GL_ERROR_CHECKER;

  GLenum face;

  switch ( e.material()->side() )
  {
    case Material::FRONT:
      face = GL_FRONT;
      break;
    case Material::BACK:
      face = GL_BACK;
      break;
    case Material::FRONT_AND_BACK:
      face = GL_FRONT_AND_BACK;
      break;
    default:
      ErrorChecker ( false );
      break;
  }

  const Material *m = e.material();
  ErrorChecker ( 0x0 != m );

  this->_disableServerState ( COLOR_MATERIAL, GL_COLOR_MATERIAL );
  this->_enableServerState  ( LIGHTING,       GL_LIGHTING );

  GL::material ( face, GL_AMBIENT,   m->ambient() );
  GL::material ( face, GL_DIFFUSE,   m->diffuse() );
  GL::material ( face, GL_SPECULAR,  m->specular() );
  GL::material ( face, GL_EMISSION,  m->emissive() );
  GL::material ( face, GL_SHININESS, m->shininess() * SHININESS_SCALE ); 
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit the element.
//
///////////////////////////////////////////////////////////////////////////////

void GlDraw::_visit ( ColorElement &e )
{
  Lock lock ( this );
  GSG_GL_ERROR_CHECKER;

  this->_enableServerState  ( COLOR_MATERIAL, GL_COLOR_MATERIAL );
  this->_disableServerState ( LIGHTING,       GL_LIGHTING );

  GL::color ( e.color() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Enable the state on the server (card).
//
///////////////////////////////////////////////////////////////////////////////

void GlDraw::_enableServerState ( UnsignedInteger f, gl_enum g )
{
  BOOST_MPL_ASSERT_IS_SAME ( gl_enum, GLenum );
  GSG_GL_ERROR_CHECKER;

  if ( false == GSG::hasBits ( _serverState, f ) )
  {
    GL::enable ( g );
    _serverState = GSG::addBits ( _serverState, f );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Disable the state on the server (card).
//
///////////////////////////////////////////////////////////////////////////////

void GlDraw::_disableServerState ( UnsignedInteger f, gl_enum g )
{
  BOOST_MPL_ASSERT_IS_SAME ( gl_enum, GLenum );
  GSG_GL_ERROR_CHECKER;

  if ( GSG::hasBits ( _serverState, f ) )
  {
    GL::disable ( g );
    _serverState = GSG::removeBits ( _serverState, f );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Enable the state on the client side.
//
///////////////////////////////////////////////////////////////////////////////

void GlDraw::_enableClientState ( UnsignedInteger f, gl_enum g )
{
  BOOST_MPL_ASSERT_IS_SAME ( gl_enum, GLenum );
  GSG_GL_ERROR_CHECKER;

  if ( false == GSG::hasBits ( _clientState, f ) )
  {
    GL::enableClientState ( g );
    _clientState = GSG::addBits ( _clientState, f );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Disable the state on the client side.
//
///////////////////////////////////////////////////////////////////////////////

void GlDraw::_disableClientState ( UnsignedInteger f, gl_enum g )
{
  BOOST_MPL_ASSERT_IS_SAME ( gl_enum, GLenum );
  GSG_GL_ERROR_CHECKER;

  if ( GSG::hasBits ( _clientState, f ) )
  {
    GL::disableClientState ( g );
    _clientState = GSG::removeBits ( _clientState, f );
  }
}


#if 0

  ::glDisable ( GL_COLOR_MATERIAL );
  ::glDisable ( GL_LIGHTING );

  ::glPushAttrib ( GL_ALL_ATTRIB_BITS );

  ::glMatrixMode ( GL_MODELVIEW );
  ::glPushMatrix();
  ::glLoadIdentity();

  ::glBegin ( GL_TRIANGLES );
    ::glColor3f ( 1, 0, 0 );
    ::glVertex3f ( 0, 0, -10 );
    ::glVertex3f ( 1, 0, -10 );
    ::glVertex3f ( 1, 1, -10 );
  ::glEnd();

  ::glPopMatrix();

  ::glPopAttrib();

#endif
