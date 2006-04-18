
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "Islands.h"

#include "FoxTools/Cursors/Factory.h"
#include "FoxTools/Headers/Cursor.h"
#include "FoxTools/Headers/App.h"

#include "Usul/Resources/ProgressBar.h"
#include "Usul/Resources/CancelButton.h"

#include "Usul/Interfaces/ISceneIntersect.h"
#include "Usul/Interfaces/IGetDocument.h"
#include "Usul/Interfaces/IDocument.h"
#include "Usul/Interfaces/ISendMessage.h"
#include "Usul/Interfaces/IKeepAllConnected.h"
#include "Usul/Interfaces/ISetTool.h"
#include "Usul/Interfaces/ICancelButton.h"
#include "Usul/Interfaces/IProgressBar.h"

#include "Usul/State/Busy.h"

#include "osgUtil/IntersectVisitor"

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( Islands , Islands::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Islands::Islands( ) : BaseClass(),
  _cursor ( FoxTools::Cursors::Factory::create ( FoxTools::Cursors::Factory::CURSOR_GUN_SIGHT ) )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Islands::~Islands()
{
  delete _cursor;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *Islands::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::ITool::IID:
    return static_cast<ITool*>(this);
  case Usul::Interfaces::ISetCursor::IID:
    return static_cast<Usul::Interfaces::ISetCursor*>(this);
  default:
    return 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  If click intersects the scene, keep all connected polygons
//
///////////////////////////////////////////////////////////////////////////////

bool Islands::execute ( Usul::Interfaces::IUnknown *caller, bool left, bool middle, bool right, bool motion, float x, float y, float z )
{
  if( left && !Usul::State::busy() )
  {
    //Get needed interfaces
    Usul::Interfaces::ISceneIntersect::ValidQueryPtr sceneIntersect  ( caller );
    Usul::Interfaces::IGetDocument::ValidQueryPtr    getDocument     ( caller );
    Usul::Interfaces::IDocument::ValidQueryPtr       document        ( getDocument->getDocument() );

    Usul::Interfaces::IKeepAllConnected::QueryPtr  keepAllConnected ( document );

    if( !keepAllConnected.valid() )
    {
      //TODO, need to tell user what document to use
      throw std::runtime_error ("This document does not support needed operations.");
    }

    osgUtil::Hit hit;

    //Check to see if mouse click has intersected the scene
    if( sceneIntersect->intersect( x, y, hit ) )
    {
      Usul::Interfaces::ISetTool::ValidQueryPtr setTool ( caller );
      setTool->doneTool();
      Usul::State::Busy busy;
      this->_keepAllConnected ( caller, hit );
      return true;
    }
  }
  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Keep all polygons connected to the given polygon
//
///////////////////////////////////////////////////////////////////////////////

void Islands::_keepAllConnected ( Usul::Interfaces::IUnknown* caller, const osgUtil::Hit &hit )
{
  Usul::Interfaces::IGetDocument::ValidQueryPtr       getDocument      ( caller );
  Usul::Interfaces::IDocument::ValidQueryPtr          document         ( getDocument->getDocument() );
  Usul::Interfaces::IKeepAllConnected::ValidQueryPtr  keep             ( document );
  Usul::Interfaces::ISendMessage::ValidQueryPtr       sendMessage      ( document );

  // Show the cancel button and progress bar.
  Usul::Interfaces::ICancelButton::ShowHide cancel ( Usul::Resources::cancelButton() );
  Usul::Interfaces::IProgressBar::ShowHide progress ( Usul::Resources::progressBar() );

  // Run the algorithm.
  keep->keepAllConnected ( caller, hit );

  // Redraw the scene
  sendMessage->sendMessage ( Usul::Interfaces::ISendMessage::ID_RENDER_SCENE );

  // Document is now modifed
  document->modified( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the cursor
//
///////////////////////////////////////////////////////////////////////////////

FX::FXCursor* Islands::getCursor()
{
  return _cursor;
}

