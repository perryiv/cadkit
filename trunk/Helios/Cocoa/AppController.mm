

#import "AppController.h"

#include "Usul/Components/Manager.h"
#include "Usul/App/Controller.h"
#include "Usul/Threads/Mutex.h"
#include "Threads/OpenThreads/Mutex.h"




@implementation AppController



// -----------------------------------------------------------------------------
//    Over Rider from standard behavior. We do not want to automatically open a 
//     new window
// -----------------------------------------------------------------------------
- (BOOL) applicationShouldOpenUntitledFile:(NSApplication *)theApplication
{
  return NO;
}

// -----------------------------------------------------------------------------
//    Delegate Method to open files that are dropped on the dock icon 
//     or Finder Icon
// -----------------------------------------------------------------------------
- (BOOL) application:(NSApplication *)theApplication
            openFile:(NSString *)filename
{
  NSDocumentController *dc;
  id doc;
  
  dc = [NSDocumentController sharedDocumentController];
  doc = [dc openDocumentWithContentsOfFile:filename display:YES];
  
  return ( doc != nil);
}

- (void)applicationWillFinishLaunching:(NSNotification *)aNotification
{
  NSLog (@"applicationWillFinishLaunching");
  //Set the Thread Mutex...
  Usul::Threads::SetMutexFactory factory ( &Threads::OT::newOpenThreadsMutex );

  [self loadAllBundles];
  
#ifdef _DEBUG
  Usul::Components::Manager::instance().addPluginExtension ( "plugd" );
#else 
  Usul::Components::Manager::instance().addPluginExtension ( "plug" );
#endif
  Usul::App::Controller::instance().loadPlugins();

  
}

///////////////////////////////////////////////////////////////////////////////
//
//  Load all the bundles
//
///////////////////////////////////////////////////////////////////////////////

-(void) loadAllBundles
{
    NSMutableArray *instances;                                         // 1
    NSMutableArray *bundlePaths;
    NSEnumerator *pathEnum;
    NSString *currPath;
    NSBundle *currBundle;
    Class currPrincipalClass;
    id currInstance;

    bundlePaths = [NSMutableArray array];

    if(!instances)
    {
        instances = [[NSMutableArray alloc] init];
    }

    [bundlePaths addObjectsFromArray:[self allBundles]];               // 2

    pathEnum = [bundlePaths objectEnumerator];

    while(currPath = [pathEnum nextObject])
    {

        currBundle = [NSBundle bundleWithPath:currPath];               // 3

        if(currBundle)
        {
            currPrincipalClass = [currBundle principalClass];          // 4
            if(currPrincipalClass)
            {
                currInstance = [[currPrincipalClass alloc] init];      // 5
                if(currInstance)

                {
                    [instances addObject:[currInstance autorelease]];
                }
            }
        }
    }
}

-(NSMutableArray *) allBundles
{
  NSArray *librarySearchPaths;
  NSEnumerator *searchPathEnum;
  NSString *currPath;
  NSMutableArray *bundleSearchPaths = [NSMutableArray array];
  NSMutableArray *allBundles = [NSMutableArray array];

  librarySearchPaths = NSSearchPathForDirectoriesInDomains(NSLibraryDirectory, NSAllDomainsMask - NSSystemDomainMask, YES);

  searchPathEnum = [librarySearchPaths objectEnumerator];

  while(currPath = [searchPathEnum nextObject])
  {
     [bundleSearchPaths addObject:
            [currPath stringByAppendingPathComponent:appSupportSubpath]];

  }

  [bundleSearchPaths addObject: [[NSBundle mainBundle] builtInPlugInsPath]];

 

  searchPathEnum = [bundleSearchPaths objectEnumerator];

  while(currPath = [searchPathEnum nextObject])
  {
     NSDirectoryEnumerator *bundleEnum;
     NSString *currBundlePath;

     bundleEnum = [[NSFileManager defaultManager]
           enumeratorAtPath:currPath];

     if(bundleEnum)
     {
       while(currBundlePath = [bundleEnum nextObject])
       {
         if([[currBundlePath pathExtension] isEqualToString:ext])
          {
            [allBundles addObject:[currPath
                           stringByAppendingPathComponent:currBundlePath]];
          }
       }
    }
  }

  return allBundles;
}

@end
