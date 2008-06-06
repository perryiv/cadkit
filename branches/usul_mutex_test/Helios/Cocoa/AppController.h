/* AppController */

#import <Cocoa/Cocoa.h>

@interface AppController : NSObject
{
  NSMutableArray *_bundles;
}

-(void)loadAllBundles;
-(NSMutableArray *) allBundlePaths;
@end
