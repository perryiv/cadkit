//
//  IVEDocument.m
//  Helios
//
//  Created by Michael Jackson on 5/12/06.
//  Copyright 2006 __MyCompanyName__. All rights reserved.
//

#import "IVEDocument.h"


@implementation IVEDocument

// -----------------------------------------------------------------------------
//  OverRide from NSDocument to read our file
// -----------------------------------------------------------------------------
- (BOOL)readFromFile:(NSString *)fileName ofType:(NSString *)type
{
  NSLog(@"Reading from IVE File");
  [self setFileName:fileName];
  [self setFileType:type];
  
  CocoaProgressBar::RefPtr progPtr ( new CocoaProgressBar (self, _progressBar, _statusText) );
  [self setCocoaProgressBar:progPtr.get()];
  
  const std::string fn ( [[self fileName] cString] );
  _tDoc = new SceneDocument();
  _tDoc.get()->read(fn, 0x0);
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




@end
