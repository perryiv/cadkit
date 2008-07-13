
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Helios/Qt/Views/OSG/Format.h"

#include "QtOpenGL/QGLFormat"

using namespace CadKit::Helios::Views::OSG;

namespace Detail
{
  QGLFormat *_format ( 0x0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the default format.
//
///////////////////////////////////////////////////////////////////////////////

QGLFormat CadKit::Helios::Views::OSG::defaultFormat ()
{
  if( 0x0 == Detail::_format )
  {
    Detail::_format = new QGLFormat;
    
    // Set our defaults.

    // Turn on alpha channel.
    Detail::_format->setAlpha ( true );

    // We want 8 bits per channel.
    Detail::_format->setAlphaBufferSize ( 8 );

    // Turn on the accumulation buffer.
    Detail::_format->setAccum ( true );

    // We want 8 bits per channel.
    Detail::_format->setAccumBufferSize ( 8 );
    
    // Ask for multisample.
    Detail::_format->setSampleBuffers ( true );
    Detail::_format->setSamples ( 4 );

    // Other than stereo, the rest of the QGLFormat defaults should be ok.
    // Do we need a way to change the pixel format on the fly?

    // Set the default to be this format.
    QGLFormat::setDefaultFormat ( *Detail::_format );
  }

  return *Detail::_format;
}
