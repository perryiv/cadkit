
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "FlipNormal.h"

#include "FoxTools/Cursors/Factory.h"
#include "FoxTools/Headers/Cursor.h"
#include "FoxTools/Headers/App.h"

#include "Usul/Math/MinMax.h"
#include "Usul/Interfaces/ISceneIntersect.h"
#include "Usul/Interfaces/IGetDocument.h"
#include "Usul/Interfaces/IDocument.h"
#include "Usul/Interfaces/ISendMessage.h"
#include "Usul/Interfaces/IGetDocument.h"
#include "Usul/Interfaces/IDocument.h"
#include "Usul/Interfaces/IFlipNormal.h"

#include "osgUtil/IntersectVisitor"


USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( FlipNormal , FlipNormal::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

FlipNormal::FlipNormal (  ) : BaseClass(),
  _cursor ( FoxTools::Cursors::Factory::create ( FoxTools::Cursors::Factory::CURSOR_GUN_SIGHT ) )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

FlipNormal::~FlipNormal()
{
  delete _cursor;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *FlipNormal::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::ITool::IID:
    return static_cast< Usul::Interfaces::ITool* >(this);
  case Usul::Interfaces::ISetCursor::IID:
    return static_cast< Usul::Interfaces::ISetCursor*>(this);
  default:
    return 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  If click intersects the scene, remove primitive
//
///////////////////////////////////////////////////////////////////////////////

bool FlipNormal::execute ( Usul::Interfaces::IUnknown *caller, bool left, bool middle, bool right, bool motion, float x, float y, float z )
{
  // If left button press
  if( left )
  {
    // Get needed interfaces.
    Usul::Interfaces::ISceneIntersect::ValidQueryPtr sceneIntersect ( caller );
    Usul::Interfaces::IGetDocument::ValidQueryPtr    getDocument    ( caller );
    Usul::Interfaces::IDocument::ValidQueryPtr       document       ( getDocument->getDocument() );

    osgUtil::Hit hit;

    // See if the mouse click intersects the scene.
    if( sceneIntersect->intersect( x,y, hit ) )
    {
      Usul::Interfaces::IFlipNormal::QueryPtr flipNormal ( document );

      if( flipNormal.valid() )
      {
        flipNormal->flipNormalVector ( hit._primitiveIndex );

        Usul::Interfaces::ISendMessage::ValidQueryPtr sendMessage ( document );

        sendMessage->sendMessage ( Usul::Interfaces::ISendMessage::ID_RENDER_SCENE );
      }

      return true; 

    }
  }

  // Returns false if the event was not a left mouse button click or if the click did not intersect the scene.
  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the cursor
//
///////////////////////////////////////////////////////////////////////////////

FX::FXCursor* FlipNormal::getCursor()
{
  return _cursor;
}

