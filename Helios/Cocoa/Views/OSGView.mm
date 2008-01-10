
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Mike Jackson
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#import "OSGView.h"

#include "osgUtil/UpdateVisitor"
#include "osg/ShapeDrawable"
#include "osg/Geode"
#include "osg/Shape"

#include "osg/ref_ptr"
#include "osgGA/GUIEventAdapter"

#include "OsgTools/Render/EventAdapter.h"
#include "OsgTools/Draggers/Dragger.h"

#include "Usul/Interfaces/IBuildScene.h"
#include "Usul/Interfaces/ICleanUp.h"
#include "Usul/Interfaces/IShadeModel.h"
#include "Usul/Documents/Document.h"
#include "Usul/Devices/Keys.h"
#include "Usul/Shared/Preferences.h"
#include "Usul/Registry/Constants.h"


#include "Helios/Cocoa/Views/CocoaGLContext.h"
#include "Helios/Cocoa/Views/ViewerBridge.h"

#include <string>
#include <list>
#include <vector>
#include <map>
#include <iosfwd>


@implementation OSGView


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize.
//
///////////////////////////////////////////////////////////////////////////////

- (id)initWithFrame:(NSRect)frame
{
  // Pixel format attributes...
  NSOpenGLPixelFormatAttribute attribs[] =
	{
		NSOpenGLPFAAccelerated,
		NSOpenGLPFADoubleBuffer,
		NSOpenGLPFAColorSize, (NSOpenGLPixelFormatAttribute) 24,
		NSOpenGLPFAAlphaSize, (NSOpenGLPixelFormatAttribute) 8,
		NSOpenGLPFADepthSize, (NSOpenGLPixelFormatAttribute) 24,
		NSOpenGLPFAStencilSize, (NSOpenGLPixelFormatAttribute) 0,
		NSOpenGLPFAWindow, (NSOpenGLPixelFormatAttribute) 0,
    (NSOpenGLPixelFormatAttribute)nil
	};
  
  // Create the pixel format.
  NSOpenGLPixelFormat *pixFormat = [[NSOpenGLPixelFormat alloc] initWithAttributes:attribs];

  // Initalize our base class with the frame and pixel format.
  if ( self = [super initWithFrame:frame pixelFormat:pixFormat] )
  {
    _osgInitialized = NO;
    _viewer = 0x0;
    _animationTimer = 0x0;
    _spinTimer = 0x0;
    _runModelFlyin = YES;
    renderLock = [[NSLock alloc] init];
    
    const GLint swapInterval = 1;
    [[self openGLContext]
        setValues: &swapInterval
     forParameter: NSOpenGLCPSwapInterval ] ;
    [pixFormat release]; //We are done with it.
  }
  return self; 
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clean up Memory.
//
///////////////////////////////////////////////////////////////////////////////

-(void)dealloc
{  
  [self cleanUpOSG];
  [renderLock release];
  [super dealloc];
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear the viewer and unreference objects.
//
///////////////////////////////////////////////////////////////////////////////

- (void) cleanUpOSG
{
  // Clear the viewer.
  if (_viewer.valid() )
    _viewer->clear();
  
  // Unreference.
  _viewer = 0x0;
  _iDocument = 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the render lock.
//
///////////////////////////////////////////////////////////////////////////////

- (NSLock *) renderLock
{
  return renderLock;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialze the viewer.
//
///////////////////////////////////////////////////////////////////////////////

- (BOOL) initializeOSG
{
  // Create the interface to the context.
  Usul::Interfaces::IOpenGLContext::ValidRefPtr context ( new Helios::Cocoa::Views::CocoaGLContext( [self openGLContext] ) );
  
  // Instantiate new bridge class
  Helios::Cocoa::Views::ViewerBridge::ValidRefPtr _bridge ( new Helios::Cocoa::Views::ViewerBridge(self) );
  
  // Create the viewer.
  _viewer = new OsgTools::Render::Viewer( _iDocument.get(), context , _bridge->queryInterface(Usul::Interfaces::IUnknown::IID) );
  _viewer->create();
  Usul::Shared::Preferences::instance().setBool ( Usul::Registry::Keys::DISPLAY_LISTS, true );
  _viewer->useDisplayLists( true );

  // Build the scene.
  Usul::Interfaces::IBuildScene::ValidQueryPtr buildScene ( _iDocument );
  _viewer->scene ( buildScene->buildScene ( Usul::Documents::Document::Options(), 0x0 ) );
    
  if ( _viewer.valid() )
  {
    _osgInitialized = YES;
   // glEnable (GL_MULTISAMPLE_ARB);
   // glHint (GL_MULTISAMPLE_FILTER_HINT_NV, GL_NICEST);
    _viewer->axesShown( false );
    return YES;
  }
  else
  {
    _osgInitialized = NO;
    return NO;
  }
}


// -----------------------------------------------------------------------------
//    Set the Usul::Documents::Document for this view
// -----------------------------------------------------------------------------
- (void) setDocument:(Usul::Documents::Document *)document
{
  _iDocument = document;
}

- (void) runModelFlyin:(BOOL)value
{
  _runModelFlyin = value;
}

//----------------------------------------------------------------------------
//  Accept First Responder Status to get the key events
//----------------------------------------------------------------------------
- (BOOL)acceptsFirstResponder
{
  return YES;
}

// -----------------------------------------------------------------------------
//    This makes the drawing faster
// -----------------------------------------------------------------------------
- (BOOL) isOpaque
{
  return YES;
}

// -----------------------------------------------------------------------------
//    Over Ride the drawRect to implement our rendering
// -----------------------------------------------------------------------------
- (void) drawRect: (NSRect) bounds
{
  if ( [renderLock tryLock] )
  {
    /// Make sure we have a view to draw into.
    if ( [[self openGLContext] view] == NULL )
    {  
      [[self openGLContext] setView:self];
    }
    
    if ( _osgInitialized && _viewer.valid() )
    {
        if ( _runModelFlyin )
        {
          [self fitModel:NULL];
          _runModelFlyin = NO;
        }
        
        // Draw.
        _viewer->render();
        [[self openGLContext] flushBuffer];
    }
    else
    {
      // Feedback...
      NSLog (@"OSG was NOT initialized.. NO rendering will take place." );
    }
    
    [renderLock unlock];
  }
}

// -----------------------------------------------------------------------------
//    Handle Resize of the window, which we are tied to.
// -----------------------------------------------------------------------------
- (void)reshape
{
  if ( _viewer.get() )
  {
    _viewer.get()->resize (  (int) [self bounds].size.width, (int) [self bounds].size.height );
    [self setNeedsDisplay:YES];
  }
}


// -----------------------------------------------------------------------------
//    Accessor Methods
// -----------------------------------------------------------------------------


- (OsgTools::Render::Viewer *) controller
{
  return _viewer.get();
}

///////////////////////////////////////////////////////////////////////////////
//
//  Our Animation and Spinning Methods
//
///////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
//  Spin the Model Methods
// -----------------------------------------------------------------------------
- (void) startSpin:(double)interval
{
//  NSLog(@"startSpin");
  if (_spinTimer) {
    [_spinTimer release];
    _spinTimer = 0x0;
  }
  _spinTimer = [NSTimer scheduledTimerWithTimeInterval:interval target:self selector:@selector(onTimeoutSpin:) userInfo:NULL repeats:YES];
  [_spinTimer retain];
}

- (void) stopSpin
{
 // NSLog(@"stopSpin");
  if (_spinTimer ) {
    [_spinTimer invalidate];
  }
}

- (void) onTimeoutSpin:(NSTimer *)timer
{
 // NSLog(@"onTimeoutSpin");
  if( _viewer.valid() )
  {
    _viewer->timeoutSpin();
    
  }
}

// -----------------------------------------------------------------------------
//  Animate the Model Methods
// -----------------------------------------------------------------------------
- (void) startAnimation:(double)interval
{
 // NSLog(@"Start Animation");
  _animationTimer = [NSTimer scheduledTimerWithTimeInterval:interval 
                                                     target:self 
                                                   selector:@selector(onTimeoutAnimation:) 
                                                   userInfo:NULL repeats:YES];
  [_animationTimer retain] ;
}

- (void) onTimeoutAnimation:(NSTimer *)timer
{
//  NSLog(@"onTimeoutAnimation");
  if( _viewer.valid() )
  {
    // Set the matrix and see if we should continue.
    if ( _viewer->timeoutAnimate () == false ) 
    {
      [_animationTimer invalidate];
      [_animationTimer release];
    }     
  }
      
}

///////////////////////////////////////////////////////////////////////////////
//
//    Mouse Related Events
//
///////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
//    ScrollWheel Events. The 240 is a scaling factor needed by the _Controller 
// -----------------------------------------------------------------------------
- (void) scrollWheel:(NSEvent *)theEvent
{
   _viewer->zoom ( [theEvent deltaY] * 240.0);
}

///////////////////////////////////////////////////////////////////////////////
//
//  MouseDown Events
//    Default capturing of Left Mouse Down, but we test for which button since 
//    we are called from other mouse down event handlers.
//
///////////////////////////////////////////////////////////////////////////////

- (void) mouseDown:(NSEvent *)theEvent
{
  NSPoint location = [self convertPoint:[theEvent locationInWindow] fromView:nil];
  bool left   ( [theEvent buttonNumber] == 0 );
  bool middle ( [theEvent buttonNumber] == 2 );
  bool right  ( [theEvent buttonNumber] == 1 );
  
  float x ( location.x );
  float y ( location.y );
 
  if ( _viewer.valid() )
  {
    typedef OsgTools::Render::EventAdapter EventAdapter;
    EventAdapter::Ptr ea ( _viewer->eventAdaptor ( x, y, left, middle, right, EventAdapter::PUSH ) );
  
    _viewer->buttonPress ( ea.get() );
  }
}
// -----------------------------------------------------------------------------
- (void) rightMouseDown:(NSEvent *)theEvent
{ 
  [self mouseDown:theEvent];
}
// -----------------------------------------------------------------------------
- (void) otherMouseDown:(NSEvent *)theEvent
{
   [self mouseDown:theEvent];
}

///////////////////////////////////////////////////////////////////////////////
//
//    Mouse Up Events - 
//      All other mouseUp* events call into this event handler
//
///////////////////////////////////////////////////////////////////////////////

- (void) mouseUp:(NSEvent *)theEvent
{
  NSPoint location = [self convertPoint:[theEvent locationInWindow] fromView:nil];
  bool left   ( [theEvent buttonNumber] == 0 );
  bool middle ( [theEvent buttonNumber] == 2 );
  bool right  ( [theEvent buttonNumber] == 1 );
  
  float x ( location.x );
  float y ( location.y );
  
  if ( _viewer.valid() )
  {
    typedef OsgTools::Render::EventAdapter EventAdapter;
    EventAdapter::Ptr ea ( _viewer->eventAdaptor ( x, y, left, middle, right, EventAdapter::RELEASE ) );
  
    _viewer->buttonRelease ( ea.get() );
  }
}

// -----------------------------------------------------------------------------
- (void) rightMouseUp:(NSEvent *)theEvent
{
  [self mouseUp:theEvent];
}

// -----------------------------------------------------------------------------
- (void) otherMouseUp:(NSEvent *)theEvent
{
  [self mouseUp:theEvent];
}

///////////////////////////////////////////////////////////////////////////////
//
//    Mouse Dragged Event Handler
//
///////////////////////////////////////////////////////////////////////////////

- (void) mouseDragged: (NSEvent *) theEvent
{
  NSPoint location = [self convertPoint:[theEvent locationInWindow] fromView:nil];
  bool left   ( [theEvent buttonNumber] == 0 );
  bool middle ( [theEvent buttonNumber] == 2 );
  bool right  ( [theEvent buttonNumber] == 1 );
  
  float x ( location.x );
  float y ( location.y );
  
  // See if any mouses button are down.
  unsigned int mouse ( left || middle || right );
  
  if ( _viewer.valid() )
  {
    typedef OsgTools::Render::EventAdapter EventAdapter;
    EventAdapter::EventType type ( ( mouse ) ? EventAdapter::DRAG : EventAdapter::MOVE );
    EventAdapter::Ptr ea ( _viewer->eventAdaptor ( x, y, left, middle, right, type ) );
  
    _viewer->mouseMove ( ea.get() );
  }
  
}

// -----------------------------------------------------------------------------
- (void) rightMouseDragged:(NSEvent *)theEvent
{
  [self mouseDragged:theEvent];
}

// -----------------------------------------------------------------------------
- (void) otherMouseDragged:(NSEvent *)theEvent
{
  [self mouseDragged:theEvent];
}


///////////////////////////////////////////////////////////////////////////////
//
//  Key has been pressed.
//
///////////////////////////////////////////////////////////////////////////////

- (void)keyDown:(NSEvent *)theEvent
{
  const char* theCode = [[theEvent characters] cString];
  // Stop any spin.
  
  unsigned int code = theCode[0];
  _viewer->spin ( false );
  // Process the key. Get the first key..[0]
  switch ( code )
  {
    // See if it was the space-bar or the r-key...
    case Usul::Devices::KEY_space:
    case Usul::Devices::KEY_r:
      // Move the camera.
      _viewer->camera ( OsgTools::Render::Viewer::RESET );
      break;
      
      // See if it was the f-key...
    case Usul::Devices::KEY_f:
      // Move the camera.
      _viewer->camera ( OsgTools::Render::Viewer::FIT );
      break;
      
      // See if it was the right-arrow key...
    case Usul::Devices::KEY_Right:
      // Move the camera.
      _viewer->rotate ( osg::Vec3 ( 0, 1, 0 ), -osg::PI_2 );
      break;
      
      // See if it was the left-arrow key...
    case Usul::Devices::KEY_Left:
      // Move the camera.
      _viewer->rotate ( osg::Vec3 ( 0, 1, 0 ), osg::PI_2 );
      break;
      
      // See if it was the up-arrow key...
    case Usul::Devices::KEY_Up:
      // Move the camera.
      _viewer->rotate ( osg::Vec3 ( 1, 0, 0 ), osg::PI_2 );
      break;
      
    // See if it was the down-arrow key...
    case Usul::Devices::KEY_Down:
      // Move the camera.
      _viewer->rotate ( osg::Vec3 ( 1, 0, 0 ), -osg::PI_2 );
      break;
      
      // See if it was the p-key...
    case Usul::Devices::KEY_p:
      if( !_viewer->picking())
        _viewer->setMode ( OsgTools::Render::Viewer::PICK );
      break;
      
      // See if it was the n-key...
    case Usul::Devices::KEY_n:
      if( !_viewer->navigating() )
        _viewer->setMode ( OsgTools::Render::Viewer::NAVIGATION );
      break;
      
      // See if it was the s key...
    case Usul::Devices::KEY_s:
      _viewer->stereo ( !_viewer->stereo() );
          [self setNeedsDisplay:YES];
      break;
      
      // See if it was the h key...
    case Usul::Devices::KEY_h:
      if( _viewer->polygonMode() == Usul::Interfaces::IPolygonMode::HIDDEN_LINES )
        _viewer->polygonMode( Usul::Interfaces::IPolygonMode::NONE );
      else
        _viewer->polygonMode( Usul::Interfaces::IPolygonMode::HIDDEN_LINES );
          [self setNeedsDisplay:YES];
      break;
      
      // See if it was the w key...
    case Usul::Devices::KEY_w:
      if( _viewer->polygonMode() == Usul::Interfaces::IPolygonMode::WIRE_FRAME )
        _viewer->polygonMode( Usul::Interfaces::IPolygonMode::NONE );
      else
        _viewer->polygonMode( Usul::Interfaces::IPolygonMode::WIRE_FRAME );
          [self setNeedsDisplay:YES];
      break;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Key has been released.
//
///////////////////////////////////////////////////////////////////////////////

- (void)keyUp:(NSEvent *)theEvent
{
  const char* theCode = [[theEvent characters] cString];
  // Stop any spin.
  unsigned int code = theCode[0];
  // Process the key. Get the first key..[0]
  switch ( code )
  {
    case Usul::Devices::KEY_Escape: 
    case Usul::Devices::KEY_v:
      _viewer->cycleMode();
      break;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Not sure what this is doing...
//
///////////////////////////////////////////////////////////////////////////////

- (BOOL) validateMenuItem:(NSMenuItem *)menuItem
{
  if ([menuItem action] == @selector(toggleLights:)) {
    return YES;
  } else if ([menuItem action] == @selector(enableSurfaceRender:)) {
    return YES;
  } else  if ([menuItem action] == @selector(enableWireframeRender:)) {
    return YES;
  } else if ([menuItem action] == @selector(enableHiddenLines:)) {
    return YES;
  } else if ([menuItem action] == @selector(toggleHiddenLines:)) {
    return YES;
  } else if ([menuItem action] == @selector(toggleShadeModel:)) {
    return YES;
  } else if ([menuItem action] == @selector(enablePerFacetLighting:)) {
    return YES;
  } else if ([menuItem action] == @selector(enablePerVertexLighting:)) {
    return YES;
  } else if ([menuItem action] == @selector(toggleAxis:)) {
    return YES;
  } else if ([menuItem action] == @selector(toggleLOD:)) {
    return YES;
  } else if ([menuItem action] == @selector(toggleBoundingBox:)) {
    return YES;
  } else if ([menuItem action] == @selector(toggleBoundingSphere:)) {
    return YES;
  } else if ([menuItem action] == @selector(toggleTwoSidedLighting:)) {
    return YES;
  } else if ([menuItem action] == @selector(toggleDisplayLists:)) {
    return YES;
  } else if ([menuItem action] == @selector(updateBackgroundColor:)) {
    return YES;
  } else if ([menuItem action] == @selector(toggleModelSpin:)) {
    return YES;
  } else if ([menuItem action] == @selector(fitModel:)) {
    return YES;
  } else if ([menuItem action] == @selector(resetView:)) {
    return YES;
  } 
  return NO;
}


///////////////////////////////////////////////////////////////////////////////
//
//  OsgTools/Render/Viewer Wrapper Methods. 
//     These IBAction methods simply pass through to the underlying 
//      OsgTools/Render/Viewer object. Having these implemented here makes things  
//      easy to hook upwidgets to the methods
//
///////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
//  Turn on/off per-vertex lighting
// -----------------------------------------------------------------------------
- (IBAction) toggleShadeModel:(id)sender
{
  Usul::Documents::Document::RefPtr document ( _iDocument.get() );
  
  // Return now if we didn't get a valid docuument.
  if( false == document.valid() )
    return;
  
  Usul::Documents::Document::Options &options = _iDocument->options();
  
  if ( NSOnState == [sender state] )  {
    options["normals"] = "per-vertex";
    options["colors"] = "per-vertex";
    _viewer->setShadeModel ( osg::ShadeModel::SMOOTH );
  } else {
    options["normals"] = "per-facet";
    options["colors"] = "per-facet";
    _viewer->setShadeModel ( osg::ShadeModel::FLAT );
  }
  Usul::Interfaces::IBuildScene::ValidQueryPtr buildScene ( _iDocument );
  
  _viewer->scene ( buildScene->buildScene ( options, 0x0 ) );
  [self setNeedsDisplay:YES];
}

// -----------------------------------------------------------------------------
//  Enable Smooth Shading (Per Vertex Lighting
// -----------------------------------------------------------------------------
- (IBAction) enablePerVertexLighting:(id)sender
{
  Usul::Documents::Document::RefPtr document ( _iDocument.get() );
  // Return now if we didn't get a valid docuument.
  if( false == document.valid() )
    return;
  
  Usul::Documents::Document::Options &options = _iDocument->options();
  options["normals"] = "per-vertex";
  options["colors"] = "per-vertex";
  _viewer->setShadeModel ( osg::ShadeModel::SMOOTH );
  Usul::Interfaces::IBuildScene::ValidQueryPtr buildScene ( _iDocument );
  _viewer->scene ( buildScene->buildScene ( options, 0x0 ) );
  [self setNeedsDisplay:YES];
}

// -----------------------------------------------------------------------------
//  Enable Per Facet Lighting
// -----------------------------------------------------------------------------
- (IBAction) enablePerFacetLighting:(id)sender
{
  Usul::Documents::Document::RefPtr document ( _iDocument.get() );
  // Return now if we didn't get a valid docuument.
  if( false == document.valid() )
    return;
  
  Usul::Documents::Document::Options &options = _iDocument->options();
  options["normals"] = "per-facet";
  options["colors"] = "per-facet";
  _viewer->setShadeModel ( osg::ShadeModel::FLAT );
  Usul::Interfaces::IBuildScene::ValidQueryPtr buildScene ( _iDocument );
  _viewer->scene ( buildScene->buildScene ( options, 0x0 ) );
  [self setNeedsDisplay:YES];
}


// -----------------------------------------------------------------------------
//  Turn on/off Hidden Lines in the Scene
// -----------------------------------------------------------------------------
- (IBAction) toggleHiddenLines:(id)sender
{
  if ( _viewer->hasHiddenLines() )
    _viewer->polygonMode( Usul::Interfaces::IPolygonMode::NONE );
  else
    _viewer->polygonMode( Usul::Interfaces::IPolygonMode::HIDDEN_LINES );
  
  [self setNeedsDisplay:YES];
}

// -----------------------------------------------------------------------------
//  Render the Model using a Surface Rendering
// -----------------------------------------------------------------------------
- (IBAction) enableSurfaceRender:(id)sender
{
  _viewer->polygonMode( Usul::Interfaces::IPolygonMode::NONE );
  [self setNeedsDisplay:YES];
}

// -----------------------------------------------------------------------------
//  Render with HiddenLines on
// -----------------------------------------------------------------------------
- (IBAction) enableHiddenLines:(id)sender
{
  _viewer->polygonMode( Usul::Interfaces::IPolygonMode::HIDDEN_LINES );   
  [self setNeedsDisplay:YES];
}

// -----------------------------------------------------------------------------
//  Render the Model using Wireframe rendering
// -----------------------------------------------------------------------------
- (IBAction) enableWireframeRender:(id)sender
{
  _viewer->polygonMode( Usul::Interfaces::IPolygonMode::WIRE_FRAME );
  [self setNeedsDisplay:YES];
}

// -----------------------------------------------------------------------------
//  Turn on/off the Lights in the Scene
// -----------------------------------------------------------------------------
- (IBAction) toggleLights:(id)sender
{
  _viewer->lighting( !_viewer->lighting() );
  [self setNeedsDisplay:YES];
}

#define TOGGLE_AXIS_ON @"Axis On"
#define TOGGLE_AXIS_OFF @"Axis Off"
// -----------------------------------------------------------------------------
//  Turn on/off the axis in the scene
// -----------------------------------------------------------------------------
- (IBAction) toggleAxis:(id)sender
{
  if (_viewer->isAxesShown()) {
    [sender setTitle:TOGGLE_AXIS_ON];
  } else {
    [sender setTitle:TOGGLE_AXIS_OFF];
  }
  _viewer->axesShown( !_viewer->isAxesShown() );
  [self setNeedsDisplay:YES];
}

// -----------------------------------------------------------------------------
//  Turn on/off the bounding box in the scene
// -----------------------------------------------------------------------------
- (IBAction) toggleBoundingBox:(id)sender
{
  _viewer->boundingBox( !_viewer->boundingBox() );
  [self setNeedsDisplay:YES];
}

// -----------------------------------------------------------------------------
//  Turn on/off the Bounding Sphere in the scene
// -----------------------------------------------------------------------------
- (IBAction) toggleBoundingSphere:(id)sender
{
  _viewer->boundingSphere( !_viewer->boundingSphere() );
  [self setNeedsDisplay:YES];
}

// -----------------------------------------------------------------------------
//  Turn on/off TwoSided Lighting in the scene
// -----------------------------------------------------------------------------
- (IBAction) toggleTwoSidedLighting:(id)sender
{
  _viewer->twoSidedLighting( !_viewer->twoSidedLighting() );
  [self setNeedsDisplay:YES];
}

// -----------------------------------------------------------------------------
//  Turn on/off the use of Display Lists
// -----------------------------------------------------------------------------
- (IBAction) toggleDisplayLists:(id)sender
{
  _viewer->useDisplayLists( !_viewer->useDisplayLists() );
  Usul::Interfaces::IBuildScene::ValidQueryPtr buildScene ( _iDocument );
  _viewer->scene ( buildScene->buildScene ( Usul::Documents::Document::Options(), 0x0 ) );
}


// -----------------------------------------------------------------------------
//    Receives Events from an NSColorPanel and Changes the Background
// -----------------------------------------------------------------------------
- (IBAction)updateBackgroundColor:(id)sender
{
  NSColor *color =  [[NSColorPanel sharedColorPanel] color];
  const osg::Vec4 c  ( [color redComponent], [color greenComponent], 
                        [color blueComponent], 1.0 );
  _viewer->backgroundColor( c, OsgTools::Render::Viewer::Corners::ALL );
  [self setNeedsDisplay:YES];
}

// -----------------------------------------------------------------------------
//    sets up the NSColorPanel to send events to this class
// -----------------------------------------------------------------------------
- (void) activateColorPanel:(id)sender
{
  [[NSColorPanel sharedColorPanel] makeKeyAndOrderFront:self];
  [[NSColorPanel sharedColorPanel] setTarget:self];
  [[NSColorPanel sharedColorPanel] setContinuous:YES];
  [[NSColorPanel sharedColorPanel] setAction:@selector(updateBackgroundColor:)];
}

// -----------------------------------------------------------------------------
//  Turn on/off the automatic Spinning of the Model
// -----------------------------------------------------------------------------
- (IBAction) toggleModelSpin:(id)sender
{
  bool spin = Usul::Shared::Preferences::instance().getBool ( Usul::Registry::Keys::ALLOW_SPIN );
  Usul::Shared::Preferences::instance().setBool ( Usul::Registry::Keys::ALLOW_SPIN, !spin );
}

// -----------------------------------------------------------------------------
//  Turn on/off the use of LODS
// -----------------------------------------------------------------------------
- (IBAction) toggleLOD:(id)sender
{
  typedef Usul::Shared::Preferences Prefs;
  bool lod = Usul::Shared::Preferences::instance().getBool ( Usul::Registry::Keys::LOW_LODS );
  Usul::Shared::Preferences::instance().setBool ( Usul::Registry::Keys::LOW_LODS, !lod );
}

// -----------------------------------------------------------------------------
//  Fit the model to the scene
// -----------------------------------------------------------------------------
- (IBAction) fitModel:(id)sender
{
  NSLog(@"  fitModel");
  _viewer->camera ( OsgTools::Render::Viewer::FIT );
  //_viewer->render();
  [self setNeedsDisplay:YES];
}

// -----------------------------------------------------------------------------
//  Reset the Camera to starting position
// -----------------------------------------------------------------------------
- (IBAction) resetView:(id)sender
{
   _viewer->camera ( OsgTools::Render::Viewer::RESET );
  [self setNeedsDisplay:YES];
}

- (IBAction) saveSceneAsImage:(id)sender  {
  void* contextInfo;
  NSSavePanel* aPanel = [NSSavePanel savePanel];
  [aPanel beginSheetForDirectory:NULL
                            file:NULL
                  modalForWindow:_window
                   modalDelegate:self
                  didEndSelector:@selector(outputSavePanelDidEnd:returnCode:contextInfo:)
                     contextInfo:contextInfo];
}

// -----------------------------------------------------------------------------
//  Called when the SavePanel ends for saving the Scene as an Image File
// -----------------------------------------------------------------------------
- (void)outputSavePanelDidEnd:(NSSavePanel *)sheet returnCode:(int)returnCode contextInfo:(void *)contextInfo
{
  if( returnCode == NSFileHandlingPanelOKButton ){
   // _viewer->numRenderPasses(8);
    [self display];
    std::string filename ( [[sheet filename] cString] );
    _viewer->writeImageFile(filename);
   // _viewer->numRenderPasses(1);
    [self setNeedsDisplay:YES];
  }
}


@end
