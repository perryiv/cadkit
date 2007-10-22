
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

#include "TriangleComponent.h"
#include "TriangleDocument.h"

#include "Usul/Documents/Manager.h"

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( TriangleComponent, TriangleComponent::BaseClass );
USUL_IMPLEMENT_TYPE_ID ( TriangleComponent );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

TriangleComponent::TriangleComponent() : BaseClass()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

TriangleComponent::~TriangleComponent()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *TriangleComponent::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::IDocumentCreate::IID:
    return static_cast < Usul::Interfaces::IDocumentCreate*>(this);
  case Usul::Interfaces::IPlugin::IID:
    return static_cast < Usul::Interfaces::IPlugin*>(this);
  case Usul::Interfaces::ICreateTriangleDocument::IID:
    return static_cast < Usul::Interfaces::ICreateTriangleDocument* > ( this );
  default:
    return 0x0;
  }
}


/////////////////////////////////////////////////////////////////////////////
//
//  Create document.
//
/////////////////////////////////////////////////////////////////////////////

Usul::Documents::Document *TriangleComponent::createDocument ( Unknown *caller )
{
  TriangleDocument::ValidRefPtr document ( new TriangleDocument );
  return document.release();
}


/////////////////////////////////////////////////////////////////////////////
//
//  Create triangle document.
//
/////////////////////////////////////////////////////////////////////////////

void TriangleComponent::createTriangleDocument ( OsgTools::Triangles::TriangleSet* triangleSet, IUnknown *caller )
{
  TriangleDocument::ValidRefPtr document ( new TriangleDocument );

  // Add the triangle set.
  document->addTriangleSet( triangleSet );

  // Mark the document as modified.
  document->modified( true );

  // Find a delegate for this document
  Usul::Documents::Manager::instance().delegate ( document.get() );

  // Open new window with the document.
  document->createDefaultGUI ( caller );

  // Add the document to the document managers list
  // Need to do this here instead of in the document manager,
  // because a cancel exception can be thrown and the document
  // will not be deleted because it's still in the manager.
  Usul::Documents::Manager::instance().add ( document.get() );

  // Bring this document's windows to the front.
  document->windowsForward();
}

