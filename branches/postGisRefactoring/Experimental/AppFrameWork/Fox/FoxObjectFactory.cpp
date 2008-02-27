
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Window factory class.
//
///////////////////////////////////////////////////////////////////////////////

#include "AppFrameWork/Fox/FoxObjectFactory.h"
#include "AppFrameWork/Fox/FoxMainWindow.h"

#include "AppFrameWork/Core/Application.h"


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

FoxObjectFactory::FoxObjectFactory() : BaseClass()
{
  // Load factory functions.
  AFW::Core::Application::instance().factory ( typeid ( AFW::Core::MainWindow ), new FoxMainWindow::NewWindow );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

FoxObjectFactory::~FoxObjectFactory()
{
}