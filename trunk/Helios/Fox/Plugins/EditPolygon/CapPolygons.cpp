
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "CapPolygons.h"


#include "Usul/Interfaces/IGetDocument.h"
#include "Usul/Interfaces/IDocument.h"
#include "Usul/Interfaces/IActiveView.h"
#include "Usul/Interfaces/IAddTriangle.h"
#include "Usul/Interfaces/IFindLoops.h"
#include "Usul/Interfaces/IGetLoops.h"

#include "Usul/State/Busy.h"


using namespace OsgFox::Plugins::EditPolygons;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

CapPolygons::CapPolygons( Usul::Interfaces::IUnknown *mainWindow) : BaseClass(),
_mainWindow( mainWindow )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

CapPolygons::~CapPolygons()
{

}


///////////////////////////////////////////////////////////////////////////////
//
//  Cap any loops in scene with triangles
//
///////////////////////////////////////////////////////////////////////////////

void CapPolygons::execute ( Usul::Interfaces::IUnknown *caller )
{
  if( !Usul::State::busy() )
  {
    //Set the busy flag
    Usul::State::Busy busy;

    //Get needed interfaces
    Usul::Interfaces::IActiveView::ValidQueryPtr  activeView  ( _mainWindow );
    Usul::Interfaces::IGetDocument::ValidQueryPtr getDocument ( activeView->getActiveView() );
    Usul::Interfaces::IDocument::ValidQueryPtr    document    ( getDocument->getDocument() );

    Usul::Interfaces::IAddTriangleWithSharedVertex::QueryPtr  addTriangle ( document );
    Usul::Interfaces::IFindLoops::QueryPtr                    findLoops   ( document );
    Usul::Interfaces::IGetLoops::QueryPtr                     getLoops    ( document );

    if( addTriangle.valid() && findLoops.valid() && getLoops.valid() )
    {
      //Use Document
      Usul::Interfaces::IFindLoops::ValidQueryPtr findLoops ( document );
      findLoops->findLoops( _mainWindow );

      document->updateGUI();
    }
    else
    {
      //TODO, need to tell user what document to use
      throw std::runtime_error ("This document does not support needed operations.");
    }
  }
}


