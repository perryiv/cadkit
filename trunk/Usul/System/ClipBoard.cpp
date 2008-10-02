
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

#include "Usul/Scope/Caller.h"

#ifdef __APPLE__
#include <Carbon/Carbon.h>
#endif

#ifdef WIN32
# define NOMINMAX
# define WIN32_LEAN_AND_MEAN
# include <windows.h>
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
#elif WIN32
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

	// Windows clipboard reference: http://msdn.microsoft.com/en-us/library/ms648709(VS.85).aspx

	// Get the top window.
	HWND hwnd ( GetTopWindow ( GetDesktopWindow() ) );
	if ( FALSE == ::OpenClipboard ( hwnd ) )
		throw std::runtime_error ( "Error 3764112647: Could not open clipboard." );

	// Make sure it's closed.
	Usul::Scope::Caller::RefPtr close ( Usul::Scope::makeCaller ( ::CloseClipboard ) );

	// Empty the clipboard.  This will give us ownership.
	::EmptyClipboard();

	// Allocate data.
	HGLOBAL data ( ::GlobalAlloc ( GMEM_MOVEABLE, text.size() + 1 * sizeof ( std::string::value_type ) ) );
  if ( 0x0 == data )
		throw std::runtime_error ( "Error 3142978366: Could not allocate data. " );

  // Lock the handle.
	LPTSTR copy ( (LPTSTR) ::GlobalLock( data ) );

	// Copy the text to the buffer.
	::memcpy ( copy, text.c_str(), text.size() * sizeof( std::string::value_type ) );

	// Add the NULL terminator.
	copy[text.size()] = 0x0;

	// Unlock the handle.
	::GlobalUnlock ( data );

  // Place the handle on the clipboard. 
	if ( 0x0 == ::SetClipboardData ( CF_TEXT, data ) )
		throw std::runtime_error ( "Error 1119617596: Could not copy data to the clipboard." );

#endif
}
