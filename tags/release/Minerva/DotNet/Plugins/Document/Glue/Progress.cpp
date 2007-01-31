
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Decision Theater at Arizona State University
//  All rights reserved.
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Progress.h"

using namespace DT::Minerva::Plugins::Document::Glue;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Progress::Progress(CadKit::Threads::Jobs::Progress ^progress)
{
  _progress = progress;
  _progress->Minimum = 0;

  _updateDelegate = gcnew CallbackDelegate(this, &Progress::update);
  _totalDelegate = gcnew CallbackDelegate(this, &Progress::total);

  _unmanagedProgress = new UnmanagedProgress();
  Usul::Pointers::reference(_unmanagedProgress);

  // Pin the pointers so they don't get moved on us by the GC.
  _updateDelegateHandle = System::Runtime::InteropServices::GCHandle::Alloc(_updateDelegate);
  _totalDelegateHandle = System::Runtime::InteropServices::GCHandle::Alloc(_totalDelegate);
  
  System::IntPtr ptr1 = System::Runtime::InteropServices::Marshal::GetFunctionPointerForDelegate(_updateDelegate);
  _unmanagedProgress->setUpdateCallback( (managedCallback) ptr1.ToPointer() );
  System::IntPtr ptr2 = System::Runtime::InteropServices::Marshal::GetFunctionPointerForDelegate(_totalDelegate);
  _unmanagedProgress->setTotalCallback( (managedCallback) ptr2.ToPointer() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Progress::~Progress()
{
  Usul::Pointers::unreference(_unmanagedProgress);
  _unmanagedProgress = 0x0;

  _updateDelegateHandle.Free();
  _totalDelegateHandle.Free();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the progress.
//
///////////////////////////////////////////////////////////////////////////////

void Progress::update( int i )
{
  _progress->Value = i;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the total.
//
///////////////////////////////////////////////////////////////////////////////

void Progress::total( int i )
{
  _progress->Maximum = i;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the unmanaged progress implementation.
//
///////////////////////////////////////////////////////////////////////////////

UnmanagedProgress* Progress::unmanagedProgress()
{
  return _unmanagedProgress;
}
