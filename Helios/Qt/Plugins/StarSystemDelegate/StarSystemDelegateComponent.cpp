
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

#include "StarSystemDelegateComponent.h"

#include "Helios/Qt/Views/OSG/Viewer.h"
#include "Helios/Qt/Views/OSG/Format.h"

#include "Usul/Documents/Document.h"
#include "Usul/Interfaces/IBuildScene.h"
#include "Usul/Interfaces/Qt/IWorkspace.h"
#include "Usul/Trace/Trace.h"

#include "QtGui/QWorkspace"

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( StarSystemDelegateComponent , StarSystemDelegateComponent::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

StarSystemDelegateComponent::StarSystemDelegateComponent() : BaseClass()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

StarSystemDelegateComponent::~StarSystemDelegateComponent()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *StarSystemDelegateComponent::queryInterface ( unsigned long iid )
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

bool StarSystemDelegateComponent::doesHandle ( const std::string& token ) const
{
  USUL_TRACE_SCOPE;
  return ( token == "StarSystem Document" );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Build the default GUI
//
/////////////////////////////////////////////////////////////////////////////

void StarSystemDelegateComponent::createDefaultGUI ( Usul::Documents::Document *document, Usul::Interfaces::IUnknown* caller )
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

    this->refreshView ( document, viewer->viewer() );

    viewer->show();
  }
}


/////////////////////////////////////////////////////////////////////////////
//
//  Refresh the view
//
/////////////////////////////////////////////////////////////////////////////

void StarSystemDelegateComponent::refreshView ( Usul::Documents::Document *document, Usul::Interfaces::IViewer *viewer )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  Usul::Interfaces::IHeliosView::QueryPtr hv ( viewer );

  if ( true == hv.valid() && 0x0 != hv->heliosView() )
  {
    OsgTools::Render::Viewer* canvas ( hv->heliosView() );

    Usul::Interfaces::IBuildScene::QueryPtr build ( document );

    if ( build.valid () )
    {
      canvas->scene ( build->buildScene ( document->options(), viewer ) );
    }

#ifdef _DEBUG
    Usul::Interfaces::IPolygonMode::QueryPtr polygonMode ( canvas );
    if ( true == polygonMode.valid() )
    {
      polygonMode->polygonMode ( Usul::Interfaces::IPolygonMode::WIRE_FRAME );
    }
#endif
  }
}
