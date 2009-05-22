
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
#include "Usul/Registry/Database.h"

#include "Usul/Exceptions/Exception.h"

#include "Usul/Signals/Actions.h"
#include "Usul/Signals/Handler.h"


int main ( int argc, char** argv )
{
  testing::InitGoogleTest(&argc, argv);

  // Trapping signals.
  typedef Usul::Signals::Actions::PrintMessage PrintMessage;
  typedef Usul::Signals::Actions::PrintStackTrace PrintStackTrace;
  typedef Usul::Signals::Actions::Pair<PrintMessage,PrintStackTrace> PrintPair;
  typedef Usul::Signals::Actions::Exit Exit;
  typedef Usul::Signals::Actions::Pair<PrintPair,Exit> SignalAction;
  USUL_DECLARE_SIGNAL_HANDLER ( SignalAction, SegmentationViolation );
  USUL_DECLARE_SIGNAL_HANDLER ( SignalAction, AbortSignal );
  USUL_DECLARE_SIGNAL_HANDLER ( SignalAction, FloatingPointException  );
  USUL_DECLARE_SIGNAL_HANDLER ( SignalAction, BusError  );

  const int result ( RUN_ALL_TESTS() );

  // Clear the registry.
  Usul::Registry::Database::destroy();

  // Clear the ObjectFactory.
  Usul::Factory::ObjectFactory::instance().clear();

  // Return the result.
  return result;
}
