
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

#include "SAL/Interfaces/IPerformer.h"
#include "SAL/Interfaces/INode.h"

#include "vrj/Kernel/Kernel.h"

#include "Usul/Errors/Assert.h"
#include "Usul/Cast/Cast.h"

#include "Performer/pf/pfGroup.h"
#include "Performer/pf/pfLightSource.h"
#include "Performer/pfdu.h"

using namespace JSAL;
using namespace JSAL::Performer;

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( Application, Usul::Base::Referenced );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Application::Application() : 
  BaseClass ( vrj::Kernel::instance() ),
  _parent ( 0x0 ),
  _root   ( 0x0 ),
  _scene  ( 0x0 ),
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

void Application::initScene()
{
  USUL_ASSERT ( !_root.valid() );
  USUL_ASSERT ( !_scene.valid() );
  USUL_ASSERT ( !_light.valid() );

  // Allocate the nodes.
  ValidPfGroupPtr root  ( new ::pfGroup );
  ValidPfGroupPtr light ( new ::pfGroup );
  ValidPfGroupPtr scene ( new ::pfGroup );

  // Assign members.
  _root  = root.get();
  _light = light.get();
  _scene = scene.get();

  // Add default light.
  USUL_VALID_REF_POINTER(::pfLightSource) sun ( new pfLightSource );
  sun->setPos ( 0.0f, 1.0f, 0.0f, 0.0f );
  sun->setColor ( PFLT_AMBIENT,  0.3f, 0.3f, 0.3f );
  sun->setColor ( PFLT_DIFFUSE,  1.0f, 1.0f, 1.0f );
  sun->setColor ( PFLT_SPECULAR, 1.0f, 1.0f, 1.0f );
  sun->on();
  _light->addChild ( sun );

  // Hook things up. Note that the light is location independent.
  _root->addChild ( _light );
  _root->addChild ( _scene );

  // Try to notify the parent.
  JSAL::Interfaces::IInit::QueryPtr notify ( _parent );
  if ( notify.valid() )
    notify->init();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called by the Performer Draw Manager as soon as the window is opened.
//
///////////////////////////////////////////////////////////////////////////////

void Application::configPWin ( pfPipeWindow *window )
{
  // Call the base class's function first.
  BaseClass::configPWin ( window );

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

void Application::appChanFunc ( pfChannel *channel )
{
  // Call the base class's function first.
  BaseClass::appChanFunc ( channel );

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
  case Usul::Interfaces::IUnknown::IID:
    return static_cast<Usul::Interfaces::IUnknown*>(static_cast<JSAL::Interfaces::IApplication*>(this));
  default:
    return 0x0;
  }
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
  USUL_VALID_REF_POINTER(::pfNode) node ( this->_getNode ( ptr ) );

  // Set the scene
  _scene = node;
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
  USUL_ASSERT ( 0 ); // TODO, use SAL::Performer::Factory::create().
  return 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the pfNode from the IUnknown.
//
///////////////////////////////////////////////////////////////////////////////

::pfNode *Application::_getNode ( Usul::Interfaces::IUnknown *unknown )
{
  SAL::Interfaces::IPerformer::ValidQueryPtr iperf ( unknown );
  USUL_VALID_REF_POINTER(::pfMemory) mp ( iperf->getPfMemory() );
  ValidPfNodePtr node ( ( mp->isOfType ( ::pfNode::getClassType() ) ) ? USUL_UNSAFE_CAST ( ::pfNode *, mp.get() ) : 0x0 );
  return node.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the scene.
//
///////////////////////////////////////////////////////////////////////////////

::pfGroup *Application::getScene()
{
  return _root;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called between pfInit and pfConfig.  This function allows the user
//  application to do any processing that needs to happen before Performer
//  forks its processes off but after pfInit().
//
///////////////////////////////////////////////////////////////////////////////

void Application::preForkInit()
{
  // Dynamically link the converter corresponding to the extension. This 
  // makes sure the loaders run on in any Performer process.
  ::pfdInitConverter ( "pfb" );
  ::pfdInitConverter ( "pfa" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when it's time to clean up.
//
///////////////////////////////////////////////////////////////////////////////

void Application::exit()
{

}
