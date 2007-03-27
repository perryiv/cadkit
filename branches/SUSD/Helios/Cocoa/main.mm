//
//  main.m
//  Test
//
//  Created by Michael A Jackson on 12/22/05.
//  Copyright __MyCompanyName__ 2005 . All rights reserved.
//

#import <Cocoa/Cocoa.h>

#include "Usul/Threads/Mutex.h"
#include "Threads/OpenThreads/Mutex.h"

Usul::Threads::SetMutexFactory factory ( &Threads::OT::newOpenThreadsMutex );


int main(int argc, char *argv[])
{
    return NSApplicationMain(argc, (const char **) argv);
}
