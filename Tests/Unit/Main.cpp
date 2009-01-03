
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Main for test runner.
//
///////////////////////////////////////////////////////////////////////////////

#include "gtest/gtest.h"

#include "Usul/Factory/ObjectFactory.h"
#include "Usul/Jobs/Manager.h"
#include "Usul/Threads/Manager.h"
#include "Usul/Registry/Database.h"

int main ( int argc, char** argv )
{
  testing::InitGoogleTest(&argc, argv);

  const int result ( RUN_ALL_TESTS() );

  // Wait for all threads.
  //Usul::Jobs::Manager::instance().cancel();
  //Usul::Jobs::Manager::instance().wait();

  // Clear the registry.
  Usul::Registry::Database::destroy();

  // Clear the ObjectFactory.
  Usul::Factory::ObjectFactory::instance().clear();

  // Destroy the job manager.
  //Usul::Jobs::Manager::destroy();

  // Destroy the thread manager.
  //Usul::Threads::Manager::destroy();

  // Return the result.
  return result;
}
