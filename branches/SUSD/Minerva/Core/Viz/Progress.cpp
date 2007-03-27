
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Core/Viz/Progress.h"

#include <iostream>

using namespace Minerva::Core::Viz;

USUL_IMPLEMENT_IUNKNOWN_MEMBERS( Progress, Progress::BaseClass );

Progress::Progress() : BaseClass()
{
}

Progress::~Progress()
{
}

// Show the progress bar
void Progress::showProgressBar()
{
}
	
// Set the total of progress bar
void Progress::setTotalProgressBar ( unsigned int value )
{
}

// Update the progress bar
void Progress::updateProgressBar ( unsigned int value )
{
  if( value % 5 == 0 )
    std::cerr << value << std::endl;
}

// Hide the progress bar
void Progress::hideProgressBar()
{
}

Usul::Interfaces::IUnknown * Progress::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::IProgressBar::IID:
    return static_cast < Usul::Interfaces::IProgressBar* > ( this );
  }

  return 0x0;
}

