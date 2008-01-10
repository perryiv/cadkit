
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Mike Jackson
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#import "AppController.h"
#import "Foundation/NSPathUtilities.h"

#include "Usul/Components/Manager.h"
#include "Usul/App/Controller.h"
#include "Usul/Threads/Mutex.h"
#include "Usul/Threads/Manager.h"

#include "Threads/OpenThreads/Mutex.h"
#include "Threads/OpenThreads/Thread.h"

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


///////////////////////////////////////////////////////////////////////////////
//
//  The application has finished loading.
//
///////////////////////////////////////////////////////////////////////////////

- (void)applicationWillFinishLaunching:(NSNotification *)aNotification
{
  NSLog (@"applicationWillFinishLaunching");
  
  // Set for multi threading.
  Usul::Threads::SetMutexFactory factory ( &Threads::OT::newOpenThreadsMutex );
  Usul::Threads::Manager::instance().factory ( &Threads::OT::newOpenThreadsThread );

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
//  See: http://developer.apple.com/documentation/Cocoa/Conceptual/LoadingCode/Tasks/LoadingBundles.html#//apple_ref/doc/uid/20001273-99488
//
///////////////////////////////////////////////////////////////////////////////

-(void) loadAllBundles
{
  //NSBundle *currBundle;
  //Class currPrincipalClass;
  //id currInstance;

  NSMutableArray *bundlePaths = [NSMutableArray array];

  // Initalize the list of bundles
  if( nil == _bundles)
  {
    _bundles = [[NSMutableArray alloc] init];
  }

  // Get all the bundle paths.  The filenames will end in .bundle
  [bundlePaths addObjectsFromArray:[self allBundlePaths]];

  // Enumerator to loop over every path.
  NSEnumerator *pathEnum = [bundlePaths objectEnumerator];
  
  NSString *currPath = nil;

  // For each path...
  while(currPath = [pathEnum nextObject])
  {
    // Get the bundle.
    NSBundle *bundle = [NSBundle bundleWithPath:currPath];
    
    // Make sure it's valid.
    if ( nil != bundle )
    {
      // Load the bundle.
      [bundle load];
      [_bundles addObject:bundle];
    }
    
    #if 0
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
    #endif
  }
}

-(NSMutableArray *) allBundlePaths
{
  NSArray *librarySearchPaths;
  NSEnumerator *searchPathEnum;
  NSString *currPath;
  NSMutableArray *bundleSearchPaths = [NSMutableArray array];
  NSMutableArray *allBundles = [NSMutableArray array];
  NSString *ext = @"bundle";
  NSString *appSupportSubpath = @"Application Support/XHelios/PlugIns";

  librarySearchPaths = NSSearchPathForDirectoriesInDomains(NSLibraryDirectory, (NSSearchPathDomainMask) (NSAllDomainsMask - NSSystemDomainMask), YES);

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
