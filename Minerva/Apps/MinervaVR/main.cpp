
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "MinervaVR.h"

#include "VRV/Core/Program.h"

#include "Usul/Threads/SetThreadFactory.h"

#include "Threads/OpenThreads/Mutex.h"


///////////////////////////////////////////////////////////////////////////////
//
//  Set for multi-threaded. This is global because the sooner we set this, 
//  the better. Setting in main() may be too late.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Threads::SetMutexFactory  mutexFactory  ( &Threads::OT::newOpenThreadsMutex );

int main( int argc, char* argv[] )
{
  VRV::Core::Program < MinervaVR > program ( "Minerva" );
  return program.run ( argc, argv );
}
