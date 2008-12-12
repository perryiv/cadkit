
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The component class.
//
///////////////////////////////////////////////////////////////////////////////

#include "Showtime/ShowtimeDelegate/ShowtimeDelegateComponent.h"
#include "Showtime/ShowtimeDelegate/Viewer.h"

#include "Usul/Components/Factory.h"
#include "Usul/Documents/Document.h"
#include "Usul/Interfaces/IBuildScene.h"
#include "Usul/Interfaces/Qt/IWorkspace.h"

#include "QtGui/QWorkspace"

#include "osg/Node"
#include "osg/ref_ptr"

USUL_DECLARE_COMPONENT_FACTORY ( ShowtimeDelegateComponent );
USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( ShowtimeDelegateComponent, ShowtimeDelegateComponent::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

ShowtimeDelegateComponent::ShowtimeDelegateComponent() : BaseClass()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

ShowtimeDelegateComponent::~ShowtimeDelegateComponent()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *ShowtimeDelegateComponent::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::IPlugin::IID:
    return static_cast < Usul::Interfaces::IPlugin*>(this);
  case Usul::Interfaces::IDefaultGUIDelegate::IID:
    return static_cast < Usul::Interfaces::IDefaultGUIDelegate * > ( this );
  case Usul::Interfaces::IGUIDelegate::IID:
    return static_cast < Usul::Interfaces::IGUIDelegate * > ( this );
  default:
    return 0x0;
  }
}


/////////////////////////////////////////////////////////////////////////////
//
//  Does this delegate handle the given token?
//
/////////////////////////////////////////////////////////////////////////////

bool ShowtimeDelegateComponent::doesHandle ( const std::string& token ) const
{
  return ( ( token == "Showtime Document" ) || ( token == "Scene Document" ) );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Build the default GUI
//
/////////////////////////////////////////////////////////////////////////////

void ShowtimeDelegateComponent::createDefaultGUI ( Usul::Documents::Document *document, Usul::Interfaces::IUnknown* caller )
{
  Usul::Interfaces::Qt::IWorkspace::QueryPtr ws ( caller );
  QWorkspace *workspace ( ws.valid() ? ws->workspace() : 0x0 );

  Showtime::Viewer::RefPtr viewer ( new Showtime::Viewer ( Usul::Interfaces::IUnknown::QueryPtr ( document ), QGLFormat::defaultFormat(), workspace, caller ) );
  if ( 0x0 != workspace )
  {
    workspace->addWindow ( viewer.get() );
  }

  // We want mouse-move events even when there are no mouse buttons pressed.
  viewer->setMouseTracking ( true );

  // Build the scene and add it to the internal viewer.
  Usul::Interfaces::IBuildScene::QueryPtr build ( document );
  Usul::Documents::Document::Options options ( ( 0x0 != document ) ? document->options() : Usul::Documents::Document::Options() );
  Showtime::Viewer::ViewerPtr internalViewer ( viewer->viewer() );
  if ( ( true == internalViewer.valid() ) && ( true == build.valid() ) )
  {
    osg::ref_ptr<osg::Node> node ( build->buildScene ( options, caller ) );
    internalViewer->modelAdd ( node );
  }

  // Get the viewer on the screen.
  viewer->stateLoad();
  viewer->show();
  viewer->setFocus();
}
