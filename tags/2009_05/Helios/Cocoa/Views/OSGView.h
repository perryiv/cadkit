#ifndef __OSGView_H
#define __OSGView_H 

#import <Cocoa/Cocoa.h>

#include "osg/ref_ptr"
#include "osgUtil/SceneView"

#include "OsgTools/Render/Viewer.h"
#include "CocoaProgressBar.h"
#include "Usul/Documents/Document.h"

@interface OSGView : NSOpenGLView
{

  OsgTools::Render::Viewer::RefPtr    _viewer;
  Usul::Documents::Document::RefPtr _iDocument;
  
  NSLock      *renderLock;
  NSTimer    *_spinTimer;
  NSTimer    *_animationTimer;
  
  BOOL _osgInitialized;
  BOOL _runModelFlyin;
}

- (id) initWithFrame:(NSRect)frameRect;

- (void) setDocument:(Usul::Documents::Document *)document;

- (BOOL) initializeOSG;

- (void) cleanUpOSG;

- (NSLock *) renderLock;

- (void) runModelFlyin:(BOOL)value;
// -----------------------------------------------------------------------------
//  Animation Methods
// -----------------------------------------------------------------------------

- (void) startAnimation:(double)interval;
- (void) onTimeoutAnimation:(NSTimer *)timer;

- (void) startSpin:(double)interval;
- (void) stopSpin;
- (void) onTimeoutSpin:(NSTimer *)timer;

///////////////////////////////////////////////////////////////////////////////
//
//  Pass Through Methods to OsgTools/Render/Viewer so we can easily hook these
//     up to GUI widgets
//
///////////////////////////////////////////////////////////////////////////////

  // Hide/Show lights in the scene.
- (IBAction) toggleLights:(id)sender;

  /// Set rendering mode to Surface/Wireframe/HiddenLines
- (IBAction) enableSurfaceRender:(id)sender;
- (IBAction) enableWireframeRender:(id)sender;
- (IBAction) enableHiddenLines:(id)sender;
- (IBAction) toggleHiddenLines:(id)sender; /// done sort of

  // Set the shading model.
- (IBAction) toggleShadeModel:(id)sender;  /// Done
- (IBAction) enablePerVertexLighting:(id)sender;
- (IBAction) enablePerFacetLighting:(id)sender;

// Get/Set show axes state.
- (IBAction) toggleAxis:(id)sender;

// Get/Set the Low LODS
- (IBAction) toggleLOD:(id)sender;

  // Set/get the bounding-box state.
- (IBAction) toggleBoundingBox:(id)sender;

  // Set/get the bounding-sphere state.
- (IBAction) toggleBoundingSphere:(id)sender;
  // Set/Get two sided lighting
- (IBAction) toggleTwoSidedLighting:(id)sender;
  // Over ride the Global setting for display Lists
- (IBAction) toggleDisplayLists:(id)sender;

// Set the Background Color
- (IBAction) updateBackgroundColor:(id)sender;
- (IBAction) activateColorPanel:(id)sender;

  // Start/stop the spin.
- (IBAction) toggleModelSpin:(id)sender;

- (IBAction) fitModel:(id)sender;
- (IBAction) resetView:(id)sender;

//--Save the Current Screen as an Image
- (IBAction) saveSceneAsImage:(id)sender;

  // Add/Remove clipping plane
//TODO:: Implement this


@end


#endif