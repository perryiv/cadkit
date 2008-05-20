
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

#include "ModelPresentationDelegateComponent.h"
#include "NewMpdDialog.h"

#include "Usul/Documents/Document.h"
#include "Usul/Exceptions/Canceled.h"
#include "Usul/Interfaces/IBuildScene.h"
#include "Usul/Interfaces/Qt/IWorkspace.h"
#include "Usul/Interfaces/Qt/IMainWindow.h"
#include "Usul/Trace/Trace.h"

#include "Usul/Shared/Preferences.h"
#include "Usul/Registry/Constants.h"

#include "Helios/Qt/Views/OSG/Viewer.h"
#include "Helios/Qt/Views/OSG/Format.h"

#include "QtGui/QMainWindow"
#include "QtGui/QWorkspace"

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( ModelPresentationDelegateComponent, ModelPresentationDelegateComponent::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

ModelPresentationDelegateComponent::ModelPresentationDelegateComponent() : BaseClass()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

ModelPresentationDelegateComponent::~ModelPresentationDelegateComponent()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *ModelPresentationDelegateComponent::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::IPlugin::IID:
    return static_cast < Usul::Interfaces::IPlugin*>(this);
  case Usul::Interfaces::IGUIDelegate::IID:
    return static_cast < Usul::Interfaces::IGUIDelegate*>(this);
  case Usul::Interfaces::IInitNewDocument::IID:
    return static_cast < Usul::Interfaces::IInitNewDocument*>(this);
    
  default:
    return 0x0;
  }
}


/////////////////////////////////////////////////////////////////////////////
//
//  Does this delegate handle the given token
//
/////////////////////////////////////////////////////////////////////////////

bool ModelPresentationDelegateComponent::doesHandle( const std::string& token ) const
{
  return ( token == "Model Presentation Document" );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Build the default GUI
//
/////////////////////////////////////////////////////////////////////////////

void ModelPresentationDelegateComponent::createDefaultGUI ( Usul::Documents::Document *document, Usul::Interfaces::IUnknown* caller )
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

    // Turn on render loop
    //viewer->renderLoop( true );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  See if the given document type is handled.
//
///////////////////////////////////////////////////////////////////////////////

bool ModelPresentationDelegateComponent::handlesDocumentType ( Usul::Interfaces::IUnknown *document )
{
  USUL_TRACE_SCOPE;
  // Re-entrant.

  Usul::Interfaces::IDocument::QueryPtr doc ( document );
  return ( ( true == doc.valid() ) ? this->doesHandle ( doc->typeName() ) : false );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize the document.
//
///////////////////////////////////////////////////////////////////////////////

void ModelPresentationDelegateComponent::initNewDocument ( Unknown *document, Unknown *caller )
{
  USUL_TRACE_SCOPE;
 
  // Get the parent.
  Usul::Interfaces::Qt::IMainWindow::QueryPtr mainWindow ( caller );
  QWidget *parent ( mainWindow.valid() ? mainWindow->mainWindow() : 0x0 );

  // Make the dialog.
  NewMpdDialog dialog ( parent );

  // Show the dialog.
  if ( QDialog::Accepted != dialog.exec() )
    throw Usul::Exceptions::Canceled();

#if 0
  // Get the xml.
  std::string xml ( dialog.buildXml() );

  // The filename.
  std::string filename;
  
  // Write the temp file.
  {
    Usul::File::Temp temp;
    temp.stream() << xml;
    temp.stream().flush();

    // Capture the filename.
    filename = temp.name();

    // Do not delete file.
    temp.release();
  }

  // Make sure file gets deleted.
  Usul::Scope::RemoveFile remove ( filename );

  // Read the file.
  Usul::Interfaces::IRead::QueryPtr read ( document );
  if ( false == read.valid() )
    return;

  // Create a job to read the file.
  Usul::Jobs::Job::RefPtr job ( Usul::Jobs::create 
    ( Usul::Adaptors::bind3 ( filename, caller, static_cast < Usul::Interfaces::IUnknown * > ( 0x0 ), 
      Usul::Adaptors::memberFunction ( read, &Usul::Interfaces::IRead::read ) ) ) );
  if ( false == job.valid() )
    return;

  // Add job to manager.
  Usul::Jobs::Manager::instance().addJob ( job.get() );

  // Spin until the job is done. This keeps the GUI responsive.
  QEventLoop loop;
  while ( false == job->isDone() )
  {
    loop.processEvents();
  }
#endif
  // Set document state.
  Usul::Interfaces::IDocument::QueryPtr doc ( document );
  if ( doc.valid() )
  {
    doc->fileValid ( false );
    doc->modified ( true );
  }
}
