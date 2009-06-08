
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Gets application framework going.
//
///////////////////////////////////////////////////////////////////////////////

#include "AppFrameWork/Core/LifeCycle.h"
#include "AppFrameWork/Core/Program.h"
#include "AppFrameWork/Core/Application.h"
#include "AppFrameWork/Core/InitFunctor.h"
#include "AppFrameWork/Actions/OpenModelAction.h"
#include "AppFrameWork/Actions/QuitProgramAction.h"
#include "AppFrameWork/Actions/RunLocalEventLoop.h"
#include "AppFrameWork/Actions/RemoveMenuBar.h"
#include "AppFrameWork/Actions/CreateMenuBar.h"

#include "Usul/System/LastError.h"

#ifdef _MSC_VER
#include <crtdbg.h>
#endif


///////////////////////////////////////////////////////////////////////////////
//
//  Asserts if there is a system error.
//
///////////////////////////////////////////////////////////////////////////////

// Usul::System::LastError::Assert errorAssert; // Curious...


///////////////////////////////////////////////////////////////////////////////
//
//  Init functor.
//
///////////////////////////////////////////////////////////////////////////////

struct Init : public AFW::Core::InitFunctor
{
  Init(){}
  virtual void operator () ( AFW::Core::Application *app );
protected:
  virtual ~Init(){}
};


///////////////////////////////////////////////////////////////////////////////
//
//  Load actions.
//
///////////////////////////////////////////////////////////////////////////////

void Init::operator () ( AFW::Core::Application *app )
{
  if ( 0x0 == app )
    return;

#if 1

  AFW::Actions::Action::RefPtr pause ( new AFW::Actions::RunLocalEventLoop ( 1000 / 4 ) );
  app->eventAppend ( new AFW::Actions::OpenModelAction ( "../../../../models/task4/modelB_new_ratios.stl" ) );
  app->eventAppend ( new AFW::Actions::OpenModelAction ( "../../../../models/task4/modelB_new_ratios.tdf" ) );
  for ( unsigned int i = 0; i < 4; ++i )
  {
    app->eventAppend ( pause.get() );
    app->eventAppend ( new AFW::Actions::RemoveMenuBar );
    app->eventAppend ( pause.get() );
    app->eventAppend ( new AFW::Actions::CreateMenuBar );
  }
#if 0
  for ( unsigned int i = 0; i < 4; ++i )
  {
    app->eventAppend ( pause.get() );
    app->eventAppend ( new AFW::Actions::SetVisible ( new AFW::Algorithms::Find ( app, new AFW::Conditions::IsOfType<AFW::Core::StatusBar> ), true ) );
    app->eventAppend ( pause.get() );
    app->eventAppend ( new AFW::Actions::SetVisible ( new AFW::Algorithms::Find ( app, new AFW::Conditions::IsOfType<AFW::Core::StatusBar> ), false ) );
  }
  for ( unsigned int i = 0; i < 4; ++i )
  {
    app->eventAppend ( pause.get() );
    app->eventAppend ( new AFW::Actions::ToggleVisible ( new AFW::Algorithms::FindFirst ( app, new AFW::Conditions::IsOfType<AFW::Core::StatusBar> ) ) );
  }
#endif
  app->eventAppend ( pause.get() );
  app->eventAppend ( new AFW::Actions::QuitProgramAction );

#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Main function.
//
///////////////////////////////////////////////////////////////////////////////

int main ( int argc, char **argv )
{
#ifdef _MSC_VER
#if 0
  ::_CrtSetBreakAlloc ( 172 + 363 );
#endif
#endif

  const unsigned int num ( 1 );
  for ( unsigned int i = 0; i < num; ++i )
  {
    AFW::Core::ProgramLifeCycle program;
    {
      AFW::Core::InitFunctor::RefPtr init ( new Init );
      AFW::Core::Program::instance().execute ( argc, argv, init.get() );
    }
  }

  return 1;
}
