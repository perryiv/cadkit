/*
 *  CocoaProgressBar.mm
 *  Helios
 *
 *  Created by Michael A Jackson on 12/27/05.
 *  Copyright 2005 __MyCompanyName__. All rights reserved.
 *
 */

#include "CocoaProgressBar.h"

 NSLock *dnalock;

CocoaProgressBar::CocoaProgressBar( id inObj, id progressBar, id statusText  )
{
  this->_theObj = inObj;
  this->_progressIndicator = progressBar;
  this->_textField = statusText;
}

CocoaProgressBar::~CocoaProgressBar()
{
  
}

///////////////////////////////////////////////////////////////////////////////
//
//        IProgressBar Interface
//
///////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
//    Show the progress bar
// -----------------------------------------------------------------------------
void CocoaProgressBar::showProgressBar()
{
  //NSLog(@"showProgressBar");
  if ( _progressIndicator ) {
    [_progressIndicator setHidden:NO];
  }
}

// -----------------------------------------------------------------------------
//    Set the total of progress bar
// -----------------------------------------------------------------------------
void CocoaProgressBar::setTotalProgressBar ( unsigned int value )
{
 // NSLog(@"setTotalProgressBar %d", value);
  if ( _progressIndicator ) {
    [_progressIndicator setMaxValue:(double)value];
  }
}

// -----------------------------------------------------------------------------
//    Update the progress bar
// -----------------------------------------------------------------------------
void CocoaProgressBar::updateProgressBar ( unsigned int value )
{
  //NSLog(@"updateProgressBar %d", value);
  if ( _progressIndicator ) {
    [dnalock lock]; 
    [_progressIndicator setDoubleValue:(double)value];
    [_textField setNeedsDisplay:YES];
    [dnalock unlock];
  }
}

// -----------------------------------------------------------------------------
//    Hide the progress bar
// -----------------------------------------------------------------------------
void CocoaProgressBar::hideProgressBar()
{
 // NSLog(@"hideProgressBar");
  if ( _progressIndicator ) {
    [dnalock lock]; 
    [_progressIndicator setHidden:YES];
    [_progressIndicator setNeedsDisplay:YES];
    [dnalock unlock];
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//    IStatusBar Interface
//
///////////////////////////////////////////////////////////////////////////////

// Set the status bar text.
void CocoaProgressBar::setStatusBarText ( const std::string &text, bool force )
{
  if ( _textField ) {
    NSString *message = [[NSString alloc] initWithCString:text.c_str() length:text.length() ];
    [dnalock lock]; 
    [_textField setStringValue:message];
    [_textField setNeedsDisplay:YES];
    [message release];
    [dnalock unlock];
  }
  
}


///////////////////////////////////////////////////////////////////////////////
//
//    Usul Base Class Methods
//
///////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
//    Increment Reference Count
// -----------------------------------------------------------------------------
void CocoaProgressBar::ref ()
{
  BaseClass::ref();
}

// -----------------------------------------------------------------------------
//    Decrement Reference Count
// -----------------------------------------------------------------------------
void CocoaProgressBar::unref ( bool allowDeletion )
{
  BaseClass::unref( allowDeletion );
}

///////////////////////////////////////////////////////////////////////////////
//
//    Usul IUnknown Interface
//
///////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
//    queryInterface method
// -----------------------------------------------------------------------------
Usul::Interfaces::IUnknown* CocoaProgressBar::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
    case Usul::Interfaces::IUnknown::IID:
    case Usul::Interfaces::IStatusBar::IID:
      return static_cast < Usul::Interfaces::IStatusBar* > ( this );
    case Usul::Interfaces::IProgressBar::IID:
      return static_cast < Usul::Interfaces::IProgressBar* > ( this );
    default:
      return 0x0;
  };
}
