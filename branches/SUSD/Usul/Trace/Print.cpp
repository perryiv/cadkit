
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

#include "Usul/Trace/Print.h"

using namespace Usul::Trace;


///////////////////////////////////////////////////////////////////////////////
//
//  Static instance.
//
///////////////////////////////////////////////////////////////////////////////

Print *Print::_instance = 0x0;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Print::Print()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Print::~Print()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Single instance.
//
///////////////////////////////////////////////////////////////////////////////

Print &Print::instance()
{
  if ( 0x0 == _instance )
  {
    _instance = new Print;
  }
  return *_instance;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Close the file.
//
///////////////////////////////////////////////////////////////////////////////

void Print::close()
{
  Print *instance = _instance;
  _instance = 0x0;
  delete instance;
}