
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

#include "ModflowDelegateComponent.h"

#include "Helios/Qt/Views/OSG/Viewer.h"
#include "Helios/Qt/Views/OSG/Format.h"

#include "Usul/Documents/Document.h"
#include "Usul/Interfaces/IBuildScene.h"
#include "Usul/Interfaces/Qt/IWorkspace.h"
#include "Usul/Trace/Trace.h"

#include "QtGui/QWorkspace"

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( ModflowDelegateComponent , ModflowDelegateComponent::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

ModflowDelegateComponent::ModflowDelegateComponent() : BaseClass()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

ModflowDelegateComponent::~ModflowDelegateComponent()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *ModflowDelegateComponent::queryInterface ( unsigned long iid )
{
  USUL_TRACE_SCOPE;

  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::IPlugin::IID:
    return static_cast < Usul::Interfaces::IPlugin*>(this);
  case Usul::Interfaces::IGUIDelegate::IID:
    return static_cast < Usul::Interfaces::IGUIDelegate*>(this);
  default:
    return 0x0;
  }
}


/////////////////////////////////////////////////////////////////////////////
//
//  Does this delegate handle the given token
//
/////////////////////////////////////////////////////////////////////////////

bool ModflowDelegateComponent::doesHandle ( const std::string& token ) const
{
  USUL_TRACE_SCOPE;
  return ( token == "Modflow Document" );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Build the default GUI
//
/////////////////////////////////////////////////////////////////////////////

void ModflowDelegateComponent::createDefaultGUI ( Usul::Documents::Document *document, Usul::Interfaces::IUnknown* caller )
{
  USUL_TRACE_SCOPE;
  typedef CadKit::Helios::Views::OSG::Viewer QtViewer;

  // Non-ref'ing smart-pointers that throw if given null.
  typedef Usul::Pointers::Configs::NoRefCountingNullThrows Policy;
  typedef Usul::Pointers::SmartPointer < QtViewer, Policy > QtViewerPtr;

  Usul::Interfaces::Qt::IWorkspace::QueryPtr workspace ( caller );

  if ( workspace.valid() )
  {
    QWorkspace *parent ( workspace->workspace() );
    QtViewerPtr viewer ( new QtViewer ( document, CadKit::Helios::Views::OSG::defaultFormat(), parent ) );
    parent->addWindow ( viewer.get() );

    // Build the scene.
    Usul::Interfaces::IBuildScene::QueryPtr build ( document );
    if ( build.valid () )
    {
      viewer->viewer()->scene ( build->buildScene ( document->options(), caller ) );
    }

    // Show the window.
    viewer->show();

    // Make sure the geometry is visible.
    viewer->viewer()->camera ( OsgTools::Render::Viewer::FIT );
  }
}
