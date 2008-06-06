//
//  IVEDocument.h
//  Helios
//
//  Created by Michael Jackson on 5/12/06.
//  Copyright 2006 IMTS. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "HeliosDocument.h"
#include "Helios/Plugins/SceneModel/SceneDocument.h"

@interface IVEDocument : HeliosDocument {

  
  SceneDocument::RefPtr _sceneDoc;
}

@end
