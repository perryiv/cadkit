
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
//  Class for moving widgets.
//
///////////////////////////////////////////////////////////////////////////////

#include "Helios/Qt/Tools/Move.h"

#include "Usul/Trace/Trace.h"

#include "QtGui/QApplication"
#include "QtGui/QDesktopWidget"
#include "QtGui/QWidget"

using namespace CadKit::Helios::Tools;


///////////////////////////////////////////////////////////////////////////////
//
//  Move the widget to the center of the screen.
//
///////////////////////////////////////////////////////////////////////////////

void Move::center ( QWidget *widget )
{
  USUL_TRACE_SCOPE_STATIC;

  if ( 0x0 != widget )
  {
    const QRect rect ( QApplication::desktop()->screenGeometry() );
    widget->move ( rect.center() - QPoint ( widget->width() / 2, widget->height() / 2 ) );
  }
}
