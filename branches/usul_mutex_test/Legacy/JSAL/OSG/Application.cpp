
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Application class.
//
///////////////////////////////////////////////////////////////////////////////

#include "Precompiled.h"
#include "Application.h"

#include "JSAL/Interfaces/IInit.h"
#include "JSAL/Interfaces/IQuit.h"
#include "JSAL/Interfaces/IContextInit.h"
#include "JSAL/Interfaces/IContextPreDraw.h"
#include "JSAL/Interfaces/IPreFrame.h"
#include "JSAL/Interfaces/IPostFrame.h"

#include "SAL/Interfaces/IOSG.h"
#include "SAL/Interfaces/INode.h"

#include "vrj/Kernel/Kernel.h"

#include "Usul/Errors/Assert.h"

#include "osg/MatrixTransform"
#include "osg/LightSource"

using namespace JSAL;
using namespace JSAL::OSG;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Application::Application() : 
  BaseClass ( vrj::Kernel::instance() ),
  _parent ( 0x0 ),
  _source ( 0x0 ),
  _light  ( 0x0 )
{
  // Tell the kernel that we are the app.
  vrj::Kernel::instance()->setApplication ( this );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Application::~Application()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize this instance.
//
///////////////////////////////////////////////////////////////////////////////

void Application::init()
{
  // Call the base class's function first.
  BaseClass::init();

  // TODO. Do something faster. Also, make this an option.
  const USUL_VALID_REF_POINTER(const osg::StateSet) temp ( this->getGlobalStateSet() );
  USUL_VALID_REF_POINTER(osg::StateSet) state ( new osg::StateSet ( *temp ) );
  state->setMode ( GL_NORMALIZE, osg::StateAttribute::ON );
  this->setGlobalStateSet ( state );

  // Make our light but don't hook it up.
  _source = new osg::LightSource;
  _light = new osg::Light;
  _source->setLight ( _light );
  _source->setReferenceFrame ( osg::LightSource::RELATIVE_RF );
  _source->setLocalStateSetModes ( osg::StateAttribute::ON );

  // Try to notify the parent.
  JSAL::Interfaces::IInit::QueryPtr notify ( _parent );
  if ( notify.valid() )
    notify->init();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called once for each display to initialize the OpenGL context.
//
///////////////////////////////////////////////////////////////////////////////

void Application::contextInit()
{
  // Call the base class's function first.
  BaseClass::contextInit();

  // Try to notify the parent.
  JSAL::Interfaces::IContextInit::QueryPtr notify ( _parent );
  if ( notify.valid() )
    notify->contextInit();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called once for each display every frame.
//
///////////////////////////////////////////////////////////////////////////////

void Application::contextPreDraw()
{
  // Call the base class's function first.
  BaseClass::contextPreDraw();

  // Try to notify the parent.
  JSAL::Interfaces::IContextPreDraw::QueryPtr notify ( _parent );
  if ( notify.valid() )
    notify->contextPreDraw();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called before the frame is drawn.
//
///////////////////////////////////////////////////////////////////////////////

void Application::preFrame()
{
  // Call the base class's function first.
  BaseClass::preFrame();

  // Try to notify the parent.
  JSAL::Interfaces::IPreFrame::QueryPtr notify ( _parent );
  if ( notify.valid() )
    notify->preFrame();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called after the frame is drawn.
//
///////////////////////////////////////////////////////////////////////////////

void Application::postFrame()
{
  // Call the base class's function first.
  BaseClass::postFrame();

  // Try to notify the parent.
  JSAL::Interfaces::IPostFrame::QueryPtr notify ( _parent );
  if ( notify.valid() )
    notify->postFrame();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *Application::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case JSAL::Interfaces::IApplication::IID:
    return static_cast<JSAL::Interfaces::IApplication*>(this);
  case JSAL::Interfaces::IChild::IID:
    return static_cast<JSAL::Interfaces::IChild*>(this);
  case JSAL::Interfaces::IQuit::IID:
    return static_cast<JSAL::Interfaces::IQuit*>(this);
  case JSAL::Interfaces::IViewer::IID:
    return static_cast<JSAL::Interfaces::IViewer*>(this);
  case JSAL::Interfaces::ILight::IID:
    return static_cast<JSAL::Interfaces::ILight*>(this);
  case Usul::Interfaces::IUnknown::IID:
    return static_cast<Usul::Interfaces::IUnknown*>(static_cast<JSAL::Interfaces::IApplication*>(this));
  default:
    return 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Reference the class. Can't use USUL_IMPLEMENT_IUNKNOWN_MEMBERS because
//  osg::Referenced::unref() does not take an argument.
//
///////////////////////////////////////////////////////////////////////////////

void Application::ref()
{
  BaseClass::ref();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Reference the class. Can't use USUL_IMPLEMENT_IUNKNOWN_MEMBERS because
//  osg::Referenced::unref() does not take an argument.
//
///////////////////////////////////////////////////////////////////////////////

void Application::unref ( bool allowDeletion )
{
  if ( allowDeletion )
    BaseClass::unref();
  else
    BaseClass::unref_nodelete();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the parent application.
//
///////////////////////////////////////////////////////////////////////////////

void Application::parent ( Usul::Interfaces::IUnknown *parent )
{
  _parent = parent;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the parent application.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *Application::parent()
{
  return _parent;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the program is about to quit.
//
///////////////////////////////////////////////////////////////////////////////

void Application::quit()
{
  // Do nothing.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the scene.
//
///////////////////////////////////////////////////////////////////////////////

void Application::scene ( SAL::Interfaces::INode *ptr )
{
  // Get the node.
  SAL::Interfaces::IOSG::ValidQueryPtr iosg ( ptr );
  USUL_VALID_REF_POINTER(osg::Node) node ( dynamic_cast<osg::Node*> ( iosg->osgReferenced() ) );

  // Call the base class's function.
  BaseClass::setSceneData ( node );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the scene.
//
///////////////////////////////////////////////////////////////////////////////

SAL::Interfaces::INode *Application::scene()
{
  // Need a way to construct the proper SAL component depending on the 
  // type of osg::Node.
  USUL_ASSERT ( 0 ); // TODO, use SAL::OSG::Factory::create().
  return 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Lighting property.
//
///////////////////////////////////////////////////////////////////////////////

void Application::getAmbientLightColor ( Vec4f &c ) const
{
  const osg::Vec4 &v = _light->getAmbient();
  c.set ( v[0], v[1], v[2], v[3] );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Lighting property.
//
///////////////////////////////////////////////////////////////////////////////

void Application::setAmbientLightColor ( const Vec4f &c )
{
  const osg::Vec4 v ( c[0], c[1], c[2], c[3] );
  _light->setAmbient ( v );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Lighting property.
//
///////////////////////////////////////////////////////////////////////////////

void Application::getDiffuseLightColor ( Vec4f &c ) const
{
  const osg::Vec4 &v = _light->getDiffuse();
  c.set ( v[0], v[1], v[2], v[3] );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Lighting property.
//
///////////////////////////////////////////////////////////////////////////////

void Application::setDiffuseLightColor ( const Vec4f &c )
{
  const osg::Vec4 v ( c[0], c[1], c[2], c[3] );
  _light->setDiffuse ( v );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Lighting property.
//
///////////////////////////////////////////////////////////////////////////////

void Application::getSpecularLightColor ( Vec4f &c ) const
{
  const osg::Vec4 &v = _light->getSpecular();
  c.set ( v[0], v[1], v[2], v[3] );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Lighting property.
//
///////////////////////////////////////////////////////////////////////////////

void Application::setSpecularLightColor ( const Vec4f &c )
{
  const osg::Vec4 v ( c[0], c[1], c[2], c[3] );
  _light->setSpecular ( v );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Lighting property.
//
///////////////////////////////////////////////////////////////////////////////

void Application::getLightDirection ( Vec3f &d ) const
{
  const osg::Vec3 &v = _light->getDirection();
  d.set ( v[0], v[1], v[2] );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Lighting property.
//
///////////////////////////////////////////////////////////////////////////////

void Application::setLightDirection ( const Vec3f &d )
{
  const osg::Vec3 v ( d[0], d[1], d[2] );
  _light->setDirection ( v );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Lighting property.
//
///////////////////////////////////////////////////////////////////////////////

JSAL::Interfaces::ILight::State Application::getLightState() const
{
  typedef JSAL::Interfaces::ILight ILight;
  return ( ( this->getSceneDecorator() == _source.get() ) ? ILight::ON : ILight::OFF );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Lighting property.
//
///////////////////////////////////////////////////////////////////////////////

void Application::setLightState ( JSAL::Interfaces::ILight::State state )
{
  typedef JSAL::Interfaces::ILight ILight;

  // Irix CC doesn't like '?' here.
  if ( ILight::ON == state )
    this->setSceneDecorator ( _source );
  else
    this->setSceneDecorator ( NULL );
}
