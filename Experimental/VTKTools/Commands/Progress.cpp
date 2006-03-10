
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "Progress.h"

using namespace VTKTools::Commands;

Progress::Progress() :
_progress ( 0x0 )
{
}

Progress::~Progress()
{
}


void Progress::progressBar( Usul::Interfaces::IProgressBar* progressBar )
{
  _progress = progressBar;
}

void Progress::Execute ( vtkObject *caller, unsigned long eventId, void *data )
{
  if( eventId == vtkCommand::ProgressEvent )
  {
    double *progress ( reinterpret_cast < double * > ( data ) );

    if ( _progress.valid() )
      _progress->updateProgressBar ( *progress * 100 );
  }
}
