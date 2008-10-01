
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Class that abstracts the system clipboard.
//
///////////////////////////////////////////////////////////////////////////////

#include "Usul/System/ClipBoard.h"

#ifdef __APPLE__
#include <Carbon/Carbon.h>
#endif

#include <stdexcept>

using namespace Usul::System;


///////////////////////////////////////////////////////////////////////////////
//
//  Does the current platform have a clip board?
//
///////////////////////////////////////////////////////////////////////////////

bool ClipBoard::isClipBoardSupported()
{
#ifdef __APPLE__
  return true;
#else
  return false;
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add the given text to the clip board.
//
///////////////////////////////////////////////////////////////////////////////

void ClipBoard::addToClipboard ( const std::string& text )
{
#ifdef __APPLE__

  // Get the reference to the global clipboard;
  PasteboardRef gClipboard;
  if ( noErr != ::PasteboardCreate ( kPasteboardClipboard, &gClipboard ) )
  {
    throw std::runtime_error ( "Error 2724349028: Could not create handle to clipboard." );
  }
  
  // We need to clear the pasteboard of it's current contents so that this application can
	// own it and add it's own gClipboard.
	if ( noErr != ::PasteboardClear ( gClipboard ) )
  {
    throw std::runtime_error ( "Error 1404747496: Could not clear clipboard." );
  }
  
  // Allocate data for the given text.
	CFDataRef data ( ::CFDataCreate ( kCFAllocatorSystemDefault, (UInt8*) text.c_str(), text.size() ) );
  if ( 0x0 == data )
  {
    throw std::runtime_error ( "Error 1989938963: Could not create data to paste to the clipboard." );
  }

	// Paste to the clipboard.
	if ( noErr != ::PasteboardPutItemFlavor ( gClipboard, (PasteboardItemID)1,
                                            CFSTR("com.apple.traditional-mac-plain-text"), data, 0 ) )
  {
    throw std::runtime_error ( "Error 1104512946: Could not paste to this clipboard." );
  }
  
#elif WIN32
  
  // TODO.
  
#endif
}
