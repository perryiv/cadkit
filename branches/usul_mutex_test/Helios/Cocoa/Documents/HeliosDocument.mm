//
//  HeliosDocument.m
//  Helios
//
//  Created by Michael A Jackson on 12/22/05.
//  Copyright 2005 __MyCompanyName__. All rights reserved.
//

#import "HeliosDocument.h"

#include "Usul/App/Controller.h"
#include "Usul/Resources/ProgressBar.h"
#include "Usul/Resources/StatusBar.h"


#include "Helios/Cocoa/Views/CocoaProgressBar.h"



@implementation HeliosDocument

- (id)init
{
  self = [super init];
  if (self) {
   /// Your Init Code Here
    _tDoc = NULL;
    _progressStatus = NULL;
    _deferFileLoad = NO;
  }
  return self;
}

- (id)initWithContentsOfFile:(NSString *)fileName ofType:(NSString *)docType
{
  self = [super init];
  if (self) {
    /// Your init from a File Code here
    [self setFileName:fileName];
    [self setFileType:docType];
    _progressStatus = NULL;
    _deferFileLoad = YES;
  }
  return self; 
}


-(void) dealloc
{
  if (_tDoc.valid() ) { _tDoc = 0x0; }
  if ( _progressStatus.valid() ) { _progressStatus = 0x0; }
  [super dealloc];
}



// -----------------------------------------------------------------------------
//    Puts our Progress/Status Text Area into the Usul Resources
// -----------------------------------------------------------------------------
- (void) setCocoaProgressBar:(CocoaProgressBar *)progressBar
{
  _progressStatus = progressBar;
  [self updateProgressBar]; // Set as the global progress bar
}

- (void) updateProgressBar
{
  if (  _progressStatus.valid() ) {
    Usul::Resources::progressBar ( _progressStatus.get() );
    Usul::Resources::statusBar( _progressStatus.get() );
  } 
}

// -----------------------------------------------------------------------------
//  This is called from another thread to read our file and display it in a Window
// -----------------------------------------------------------------------------
-(void) threadedFileRead:(id)sender
{
  NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];			// Threads need one of these.
  NSWindow *window = [(NSWindowController *)sender window];

  //Get the original State
  NSRect origFrame = [window frame];
  NSToolbar *toolbar = [window toolbar];
  BOOL showToolBar = [toolbar isVisible];
  [toolbar setVisible:NO];
  [_osgView setHidden:YES];
  [window setShowsToolbarButton:NO];

  
  NSRect frame = [window frame];
  frame.size.width = 600.0;
  frame.size.height = 600.0;
  //[window setFrame:frame display:YES animate:YES];
  if ( YES == _deferFileLoad && [self fileName] ) {
    BOOL success = [self readFromFile:[self fileName] ofType:[self fileType] ];
    if ( success == NO ) {
      NSRunAlertPanel(@"STL File Error", @"There was an error reading the selected STL File", @"OK", nil, nil);
    } else {
      [_progressBar setHidden:YES];
      [_statusText setHidden:YES];
     
      [toolbar setVisible:showToolBar];
      [window setShowsToolbarButton:YES];
      [[_osgView renderLock] lock]; // Lock out the other thread from rendering
      [_osgView setHidden:NO];  
      
      [window setFrame:frame display:YES animate:YES];
      [window makeFirstResponder:_osgView];
      
      [[_osgView renderLock] unlock]; //Unlock so the RunLoop can use drawRect
      [_osgView setNeedsDisplay:YES];
    }
    
  }
  [pool release];

}

// -----------------------------------------------------------------------------
//  OverRide from NSDocument to read our file
// -----------------------------------------------------------------------------
- (BOOL)readFromFile:(NSString *)fileName ofType:(NSString *)type
{
  [self setFileName:fileName];
  [self setFileType:type];
  
  CocoaProgressBar::RefPtr progPtr ( new CocoaProgressBar (self, _progressBar, _statusText) );
  [self setCocoaProgressBar:progPtr.get()];
  
  const std::string fn ( [[self fileName] cString] );
  _tDoc = new TriangleDocument();
  _tDoc.get()->read(fn,0x0);
  if ( _tDoc ) {
    [_osgView setDocument: (Usul::Documents::Document*)_tDoc  ];
    BOOL success = [_osgView initializeOSG];
    if ( NO == success ) {
      _tDoc = 0x0;
      return NO;
    }
  } else {
    return NO;
  }
  return YES;

}



///////////////////////////////////////////////////////////////////////////////
//
//    NSDocument Implementation Methods
//
///////////////////////////////////////////////////////////////////////////////

- (NSString *)windowNibName {
  // Implement this to return a nib to load OR implement -makeWindowControllers
  //  to manually create your controllers.
  return @"HeliosDocument";
}

- (void)windowControllerDidLoadNib:(NSWindowController *) aController
{
  [super windowControllerDidLoadNib:aController];
  [_osgView setHidden:YES];
  [NSThread detachNewThreadSelector:@selector(threadedFileRead:) toTarget:self withObject:aController];
}

- (NSData *)dataRepresentationOfType:(NSString *)type {
  return nil;
}

- (BOOL)loadDataRepresentation:(NSData *)data ofType:(NSString *)type {
  return YES;
}

// -----------------------------------------------------------------------------
//    Over Ride from NSDocument used to set the Name on the Window Title
// -----------------------------------------------------------------------------
-(NSString *) displayName
{
  return [[NSFileManager defaultManager] displayNameAtPath:[self fileName] ];
}




///////////////////////////////////////////////////////////////////////////////
//
//    NSWindow Delegate Methods
//
///////////////////////////////////////////////////////////////////////////////

- (void)windowDidBecomeMain:(NSNotification *)aNotification
{
  // NSLog(@"  windowDidBecomeMain - %@", [self displayName]);
  [self updateProgressBar];
}

- (void)windowWillClose:(NSNotification *)aNotification
{
  //NSLog(@"  windowWillClose %@", [self displayName]);
  [_osgView cleanUpOSG];
  [[NSNotificationCenter defaultCenter] removeObserver:self];
}

@end