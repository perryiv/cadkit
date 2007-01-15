
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Decision Theater at Arizona State University
//  All rights reserved.
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "UnmanagedProgress.h"

#include "Usul/Interfaces/IUnknown.h"

using namespace DT::Minerva::Plugins::Document::Glue;

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( UnmanagedProgress , UnmanagedProgress::BaseClass );


UnmanagedProgress::UnmanagedProgress() :
_updateCallback( 0x0 ),
_totalCallback( 0x0 )
{
}

UnmanagedProgress::~UnmanagedProgress()
{
}

// Show the progress bar
void UnmanagedProgress::showProgressBar()
{
}

// Set the total of progress bar
void UnmanagedProgress::setTotalProgressBar ( unsigned int value )
{
  if( _totalCallback )
    _totalCallback( value );
}

// Update the progress bar
void UnmanagedProgress::updateProgressBar ( unsigned int value )
{
  if( _updateCallback )
    _updateCallback( value );
}

// Hide the progress bar
void UnmanagedProgress::hideProgressBar()
{
}

void UnmanagedProgress::setUpdateCallback(managedCallback callback)
{
  _updateCallback = callback;
}


void UnmanagedProgress::setTotalCallback(managedCallback callback)
{
  _totalCallback = callback;
}

///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *UnmanagedProgress::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::IProgressBar::IID:
    return static_cast < Usul::Interfaces::IProgressBar * > ( this );
  default:
    return 0x0;
  }
  
}
