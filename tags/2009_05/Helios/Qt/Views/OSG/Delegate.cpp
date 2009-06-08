
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The delegate class.
//
///////////////////////////////////////////////////////////////////////////////

#include "Helios/Qt/Views/OSG/Delegate.h"
#include "Helios/Qt/Views/OSG/Viewer.h"
#include "Helios/Qt/Views/OSG/Format.h"

#include "Usul/Documents/Document.h"
#include "Usul/Interfaces/IBuildScene.h"
#include "Usul/Interfaces/Qt/IWorkspace.h"

#include "QtTools/Color.h"

#include "QtGui/QWorkspace"
#include "QtGui/QColorDialog"

using namespace CadKit::Helios::Views::OSG;

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( Delegate, Delegate::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Delegate::Delegate() : BaseClass()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Delegate::~Delegate()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *Delegate::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::IGUIDelegate::IID:
    return static_cast < Usul::Interfaces::IGUIDelegate*>(this);
  case Usul::Interfaces::IColorEditor::IID:
    return static_cast < Usul::Interfaces::IColorEditor * > ( this );
  default:
    return 0x0;
  }
}


/////////////////////////////////////////////////////////////////////////////
//
//  Build the default GUI
//
/////////////////////////////////////////////////////////////////////////////

void Delegate::createDefaultGUI ( Usul::Documents::Document *document, Usul::Interfaces::IUnknown* caller )
{
  // Make the viewer.
  QtViewerPtr viewer ( this->_makeViewer ( document, caller ) );
  
  // Make sure we got a viewer.
  if ( false == viewer.valid() )
    return;
  
  // We want mouse-move events even when there are no mouse buttons pressed.
  viewer->setMouseTracking ( true );

  // Build the window.
  this->_buildScene ( *viewer, document, caller );
  
  // Show the window.
  this->_restoreStateAndShow ( *viewer );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Make the viewer.
//
/////////////////////////////////////////////////////////////////////////////

Delegate::QtViewer* Delegate::_makeViewer ( Usul::Documents::Document *document, Usul::Interfaces::IUnknown* caller )
{
  Usul::Interfaces::Qt::IWorkspace::QueryPtr ws ( caller );
  
  QWorkspace *workspace ( ws.valid() ? ws->workspace() : 0x0 );
  
  QtViewerPtr viewer ( new QtViewer ( document, CadKit::Helios::Views::OSG::defaultFormat(), workspace, caller ) );
  
  if ( 0x0 != workspace )
  {
    workspace->addWindow ( viewer.get() );
  }
  
  return viewer.release();
}


/////////////////////////////////////////////////////////////////////////////
//
//  Build the scene.
//
/////////////////////////////////////////////////////////////////////////////

void Delegate::_buildScene ( QtViewer &viewer, Usul::Documents::Document *document, Usul::Interfaces::IUnknown* caller )
{
  // Build the scene.
  Usul::Interfaces::IBuildScene::QueryPtr build ( document );
  if ( 0x0 != viewer.viewer() && build.valid () && 0x0 != document )
  {
    viewer.viewer()->scene ( build->buildScene ( document->options(), caller ) );
  }
}


/////////////////////////////////////////////////////////////////////////////
//
//  Show the viewer.
//
/////////////////////////////////////////////////////////////////////////////

void Delegate::_restoreStateAndShow ( QtViewer &viewer )
{
  // Load the saved state.
  viewer.stateLoad();
  
  // Show the window.
  viewer.show();
  
  // Set the focus.
  viewer.setFocus();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Change a color.
//
///////////////////////////////////////////////////////////////////////////////

bool Delegate::editColor ( Usul::Math::Vec4f& c )
{
  // Create and set the color to the value of c
  QColor color;
  QtTools::Color< Usul::Math::Vec4f >::convert( c, color );
  color = QColorDialog::getColor ( color );

  // If the user didn't press cancel
  if ( color.isValid() )
  {
    // Set the return color to the value of "color" selected 
    // in the color dialog.
    QtTools::Color< Usul::Math::Vec4f >::convert( color, c );

    // User pressed the OK button
    return true;
  }

  // User pressed the cancel button
  return false;
}
