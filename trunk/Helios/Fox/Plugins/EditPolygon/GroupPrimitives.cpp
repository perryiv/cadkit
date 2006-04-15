
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "GroupPrimitives.h"

#include "FoxTools/Cursors/Factory.h"
#include "FoxTools/Headers/Cursor.h"
#include "FoxTools/Headers/App.h"


#include "Usul/Resources/ProgressBar.h"
#include "Usul/Resources/CancelButton.h"

#include "Usul/Interfaces/IActiveView.h"
#include "Usul/Interfaces/ISceneIntersect.h"
#include "Usul/Interfaces/IGetDocument.h"
#include "Usul/Interfaces/IDocument.h"
#include "Usul/Interfaces/ISendMessage.h"
#include "Usul/Interfaces/IGroupPrimitives.h"
#include "Usul/Interfaces/ISetTool.h"
#include "Usul/Interfaces/ICancelButton.h"
#include "Usul/Interfaces/IProgressBar.h"
#include "Usul/Interfaces/ISendMessage.h"

#include "Usul/State/Busy.h"

using namespace Helios::Plugins::EditPolygons;

GroupPrimitives::GroupPrimitives( Usul::Interfaces::IUnknown *mainWindow) : BaseClass(),
_mainWindow( mainWindow )
{
}

GroupPrimitives::~GroupPrimitives(void)
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  If the menu button was clicked
//
///////////////////////////////////////////////////////////////////////////////

void GroupPrimitives::execute ( Usul::Interfaces::IUnknown *caller )
{
  if( !Usul::State::busy() )
  {
    //Set the busy flag
    Usul::State::Busy busy;

    //Get needed interfaces
    Usul::Interfaces::IActiveView::ValidQueryPtr  activeView  ( _mainWindow.get() );
    Usul::Interfaces::IGetDocument::ValidQueryPtr getDocument ( activeView->getActiveView() );
    Usul::Interfaces::IDocument::ValidQueryPtr    document    ( getDocument->getDocument() );
    Usul::Interfaces::IGroupPrimitives::QueryPtr  groupPrimitives ( document );
    Usul::Interfaces::ISendMessage::ValidQueryPtr sendMessage ( document );

    if( groupPrimitives.valid() )
    {
      //Use Document
      groupPrimitives->groupPrimitives( _mainWindow );

      document->updateGUI();

      sendMessage->sendMessage( Usul::Interfaces::ISendMessage::ID_BUILD_SCENE );
    }
    else
    {
      //TODO, need to tell user what document to use
      throw std::runtime_error ("This document does not suppport grouping of primitives by connectivity.");
    }
  }
}


