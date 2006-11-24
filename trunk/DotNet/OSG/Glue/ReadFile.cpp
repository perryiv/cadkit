
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "ReadFile.h"
#include "Reader.h"

#include "Usul/Strings/ManagedToNative.h"


namespace CadKit {
namespace OSG {
namespace Glue {


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

ReadFile::ReadFile() : _progress ( nullptr ), _delegate ( nullptr ), _pin()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

ReadFile::~ReadFile()
{
  try
  {
    this->clear();
  }

  catch ( ... )
  {
    System::Console::WriteLine ( "Error 3737000921: Exception caught when clearing members" );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear this instance.
//
///////////////////////////////////////////////////////////////////////////////

void ReadFile::clear()
{
  _progress = nullptr;
  _delegate = nullptr;
  if ( _pin.IsAllocated )
    _pin.Free();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return a callback function that is connected to the progress interface.
//  See: http://www.openscenegraph.org/index.php?page=Tutorials.LoadingProgress
//
///////////////////////////////////////////////////////////////////////////////

ReadFile::NativeProgressCallback ReadFile::_makeProgressCallback() 
{
  // Make delegate.
  _delegate = gcnew CallbackDelegate ( this, &ReadFile::_progressNotify );

  // Pin it so that we can get the internal function pointer.
  _pin = System::Runtime::InteropServices::GCHandle::Alloc ( _delegate );

  // Get the delegate's function pointer.
  System::IntPtr fun = System::Runtime::InteropServices::Marshal::GetFunctionPointerForDelegate ( _delegate );

  // Return the function.
  return reinterpret_cast < NativeProgressCallback > ( fun.ToPointer() );
}

  
///////////////////////////////////////////////////////////////////////////////
//
//  Read file with progress notification.
//  See: http://www.openscenegraph.org/index.php?page=Tutorials.LoadingProgress
//
///////////////////////////////////////////////////////////////////////////////

CadKit::OSG::Glue::Node^ ReadFile::readNodeFile( System::String ^name, System::Object^ caller )
{
  this->clear();
  _progress = dynamic_cast < CadKit::Interfaces::IProgressBar ^ > ( caller );

  // Make native function pointer to the progress interface.
  NativeProgressCallback progress ( ReadFile::_makeProgressCallback() );

  // Get file name.
  const std::string file ( Usul::Strings::convert ( name ) );

  // Declare reader.
  CadKit::OSG::Glue::Reader reader;
  reader.callback ( progress );
  reader.read ( file );

  // Save the model.
  CadKit::OSG::Glue::Node ^node = gcnew CadKit::OSG::Glue::Node();
  node->node ( reader.node() );

  // Clear the members.
  this->clear();

  // Return the node.
  return node;
}

  
///////////////////////////////////////////////////////////////////////////////
//
//  Called when there is progress.
//
///////////////////////////////////////////////////////////////////////////////

void ReadFile::_progressNotify()
{
  if ( nullptr != _progress )
    _progress->Value = 10;
}


}
}
}
