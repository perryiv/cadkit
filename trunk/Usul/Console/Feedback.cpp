
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach.
//
///////////////////////////////////////////////////////////////////////////////

#include "Usul/Console/Feedback.h"

#include <iostream>

using namespace Usul::Console;

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( Feedback, Feedback::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Feedback::Feedback() :
	BaseClass(),
	_total ( 0 )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Feedback::~Feedback()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for given interface id.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown* Feedback::queryInterface ( unsigned long iid )
{
	switch ( iid )
	{
	case Usul::Interfaces::IUnknown::IID:
	case Usul::Interfaces::IStatusBar::IID:
		return static_cast < Usul::Interfaces::IStatusBar * > ( this );
	case Usul::Interfaces::IProgressBar::IID:
		return static_cast < Usul::Interfaces::IProgressBar * > ( this );
	default:
		return 0x0;
	}
}


///////////////////////////////////////////////////////////////////////////////
//
//  Show the progress bar.
//
///////////////////////////////////////////////////////////////////////////////

void Feedback::showProgressBar()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the total of progress bar.
//
///////////////////////////////////////////////////////////////////////////////

void Feedback::setTotalProgressBar ( unsigned int value )
{
	_total = value;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the progress bar.
//
///////////////////////////////////////////////////////////////////////////////

void Feedback::updateProgressBar ( unsigned int value )
{
	float percentage ( static_cast < float > ( value ) / _total );
	std::cout << static_cast < unsigned int > ( percentage * 100 ) << "%" << std::endl;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Hide the progress bar.
//
///////////////////////////////////////////////////////////////////////////////

void Feedback::hideProgressBar()
{
}
  

///////////////////////////////////////////////////////////////////////////////
//
//  Set the status bar text.
//
///////////////////////////////////////////////////////////////////////////////

void Feedback::setStatusBarText ( const std::string &text, bool force )
{
	std::cout << text << std::endl;
}
