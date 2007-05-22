
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Base class for all commands.
//
///////////////////////////////////////////////////////////////////////////////

#include "Helios/Qt/Commands/Command.h"
#include "Helios/Qt/Commands/Target.h"
#include "Helios/Qt/Tools/Icon.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Trace/Trace.h"

#include "QtGui/QAction"

using namespace CadKit::Helios::Commands;

USUL_IMPLEMENT_TYPE_ID ( Command );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Command::Command ( Usul::Interfaces::IUnknown *caller ) : BaseClass ( caller ),
  _target ( 0x0 )
{
  USUL_TRACE_SCOPE;

  // Make target.
  _target = new Target ( this );

  // Make the action point it to the target.
  //std::auto_ptr<QAction> action ( new QAction ( "&Open", _target ) );
  //action->setShortcut ( QKeySequence ( "Ctrl+O" ) );
  //action->setStatusTip ( "Open existing document" );
  //CadKit::Helios::Tools::Icon::set ( "openDocument.png", action.get() );
  //_target->connect ( action.get(), SIGNAL ( triggered() ), _target, SLOT ( _slot() ) );
  //action.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Command::~Command()
{
  USUL_TRACE_SCOPE;
  Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( this, &Command::_destroy ), "2611738985" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destroy.
//
///////////////////////////////////////////////////////////////////////////////

void Command::_destroy()
{
  USUL_TRACE_SCOPE;
  _target = 0x0;
}
