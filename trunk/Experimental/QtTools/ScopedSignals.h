
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Scope the blocking of signals.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __QT_TOOLS_SCOPED_SIGNALS_H__
#define __QT_TOOLS_SCOPED_SIGNALS_H__

#include "QtCore/QObject"

namespace QtTools
{
  struct ScopedSignals
  {
    ScopedSignals( QObject& object ) : _object ( object )
    {
      // Block signals.
      _object.blockSignals ( true );
    }
    ~ScopedSignals()
    {
      // Unblock signals.
      _object.blockSignals ( false );
    }
    
  private:
    QObject &_object;
  };
}

#endif // __QT_TOOLS_SCOPED_SIGNALS_H__
