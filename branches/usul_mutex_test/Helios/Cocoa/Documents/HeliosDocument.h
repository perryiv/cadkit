///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Mike Jackson
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __HeliosDocument_H
#define __HeliosDocument_H 


#import <Cocoa/Cocoa.h>


#include "OSGView.h"
#include "Helios/Plugins/TriangleModel/TriangleDocument.h"


@interface HeliosDocument : NSDocument
{
  IBOutlet OSGView *_osgView;
  IBOutlet NSTextField *_statusText;
  IBOutlet NSProgressIndicator *_progressBar;
  
  BOOL _deferFileLoad;
  
  TriangleDocument::RefPtr _tDoc;
  CocoaProgressBar::RefPtr _progressStatus;

}



// -----------------------------------------------------------------------------
//    Puts our Progress/Status Text Area into the Usul Resources
// -----------------------------------------------------------------------------
- (void) setCocoaProgressBar:(CocoaProgressBar *)progressBar;

- (void) updateProgressBar;



@end




#endif