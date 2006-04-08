
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "MotionListener.h"
#include "Canvas.h"

#include "Usul/Devices/State.h"

#include "OsgTools/Render/EventAdapter.h"

#include "OsgTools/Draggers/Dragger.h"

using namespace OsgFox::Views;
using OsgTools::Render::EventAdapter;
using namespace OsgTools::Draggers;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

MotionListener::MotionListener ( Canvas &canvas ) :
_canvas ( canvas )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

MotionListener::~MotionListener()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Motion occured.
//
///////////////////////////////////////////////////////////////////////////////

void MotionListener::onMotion( const Usul::Devices::State& state )
{
  bool left   ( state.buttonPressed( 0 ) );
  bool middle ( state.buttonPressed( 1 ) );
  bool right  ( state.buttonPressed( 2 ) );

  float x ( state.x() );
  float y ( state.y() );

  // See if any mouses button are down.
  unsigned int mouse ( left || middle || right );

  // Set the event type.
  EventAdapter::EventType type ( ( mouse ) ? EventAdapter::DRAG : EventAdapter::MOVE );

  // Handle the events. Make sure you pick before you drag.
  _canvas.viewer()->handleNavigation ( x, y, left, middle, right, type );
  _canvas.viewer()->handlePicking    ( x, y, false, 0 );
  _canvas.viewer()->handleDragging   ( x, y, Dragger::MOVE );

  // Handle tool.
  _canvas.viewer()->handleTool ( left, middle, right, true, x, y, 0.0 );
}

