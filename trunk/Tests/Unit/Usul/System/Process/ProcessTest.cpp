
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "Usul/System/Process.h"

#include "gtest/gtest.h"

#include <iostream>

TEST(ProcessTest,Test01)
{
  std::string childProcessName ( "./UsulProcessChild" );
  
#ifdef _DEBUG
  childProcessName += "d";
#endif
  
  Usul::System::Process::RefPtr process ( new Usul::System::Process ( childProcessName, "" ) );
  
  EXPECT_TRUE ( process->id() > 0 );
  EXPECT_TRUE ( process->isRunning() );
  
  process->stop();

  EXPECT_FALSE ( process->isRunning() );
}
