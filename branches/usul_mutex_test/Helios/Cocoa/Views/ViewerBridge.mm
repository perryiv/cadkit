/*
 *  ViewerBridge.mm
 *  Helios
 *
 *  Created by Michael A Jackson on 1/10/06.
 *  Copyright 2006 __MyCompanyName__. All rights reserved.
 *
 */

#include "Helios/Cocoa/Views/ViewerBridge.h"


using namespace Helios::Cocoa::Views;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor and Destructor Section
//
///////////////////////////////////////////////////////////////////////////////

ViewerBridge::ViewerBridge(OSGView *osgView)
{
  this->_osgView = osgView;
  [_osgView retain];
}

ViewerBridge::~ViewerBridge()
{
  if ( _osgView ) [ _osgView release];
}


///////////////////////////////////////////////////////////////////////////////
//
//      Usul/Interfaces/ITimeoutSpin Interface
//
///////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
//  Stop the model spinning
// -----------------------------------------------------------------------------
void ViewerBridge::stopSpin ( )
{
  [_osgView stopSpin];
}

// -----------------------------------------------------------------------------
//  Start the Model Spinning
// -----------------------------------------------------------------------------
void ViewerBridge::startSpin ( double timeout )
{
  [_osgView startSpin:timeout*0.001];
}


///////////////////////////////////////////////////////////////////////////////
//
//     Usul/Interfaces/ITimeoutAnimate Interface
//
///////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
//  Start the Animation
// -----------------------------------------------------------------------------
void ViewerBridge::startAnimation ( double interval )
{
  [_osgView startAnimation:interval*0.001];
}



///////////////////////////////////////////////////////////////////////////////
//
//  Usul/Interfaces/IUnknown interface Implementation
//
///////////////////////////////////////////////////////////////////////////////


//  This implements the ref/unref methods
USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( ViewerBridge , ViewerBridge::BaseClass );


//    queryInterface method
Usul::Interfaces::IUnknown* ViewerBridge::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
    case Usul::Interfaces::IUnknown::IID:
    case Usul::Interfaces::ITimeoutSpin::IID:
      return static_cast < Usul::Interfaces::ITimeoutSpin* > ( this );
    case Usul::Interfaces::ITimeoutAnimate::IID:
      return static_cast < Usul::Interfaces::ITimeoutAnimate* > ( this );
    default:
      return 0x0;
  };
}
