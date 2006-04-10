

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
#if 0
  #ifdef _DEBUG
  Usul::Components::Manager::instance().addPluginExtension ( "plugd" );
#else 
  Usul::Components::Manager::instance().addPluginExtension ( "plug" );
#endif
  Usul::App::Controller::instance().loadPlugins();
#endif
}

@end
