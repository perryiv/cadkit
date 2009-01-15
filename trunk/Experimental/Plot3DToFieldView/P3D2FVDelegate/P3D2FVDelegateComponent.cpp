
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Jeff Conner
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The component class.
//
///////////////////////////////////////////////////////////////////////////////

#include "P3D2FVDelegateComponent.h"

#include "Usul/Documents/Document.h"
#include "Usul/Interfaces/IBuildScene.h"
#include "Usul/Interfaces/Qt/IWorkspace.h"

#include "Usul/Shared/Preferences.h"
#include "Usul/Registry/Constants.h"

#include "Helios/Qt/Views/OSG/Viewer.h"
#include "Helios/Qt/Views/OSG/Format.h"

#include "QtGui/QWorkspace"

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( P3D2FVDelegateComponent, P3D2FVDelegateComponent::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

P3D2FVDelegateComponent::P3D2FVDelegateComponent() : BaseClass()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

P3D2FVDelegateComponent::~P3D2FVDelegateComponent()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *P3D2FVDelegateComponent::queryInterface ( unsigned long iid )
{
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

bool P3D2FVDelegateComponent::doesHandle( const std::string& token ) const
{
  return ( token == "Plot3D to FieldView Converter" );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Build the default GUI
//
/////////////////////////////////////////////////////////////////////////////

void P3D2FVDelegateComponent::createDefaultGUI ( Usul::Documents::Document *document, Usul::Interfaces::IUnknown* caller )
{
  typedef CadKit::Helios::Views::OSG::Viewer QtViewer;

  // Non-ref'ing smart-pointers that throw if given null.
  typedef Usul::Pointers::Configs::NoRefCountingNullThrows Policy;
  typedef Usul::Pointers::SmartPointer < QtViewer, Policy > QtViewerPtr;

  Usul::Interfaces::Qt::IWorkspace::QueryPtr workspace ( caller );

  if( workspace.valid() )
  {
    QWorkspace *parent ( workspace->workspace() );
    QtViewerPtr viewer ( new QtViewer ( document, CadKit::Helios::Views::OSG::defaultFormat(), parent, caller ) );
    parent->addWindow ( viewer.get() );

    // Build the scene.
    Usul::Interfaces::IBuildScene::QueryPtr build ( document );
    if ( build.valid () )
    {
      viewer->viewer()->scene ( build->buildScene ( document->options(), caller ) );
    }

    viewer->show();
  }
}
