
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "ReadFile.h"

#include "OsgTools/IO/Reader.h"

#include "Usul/Math/MinMax.h"
#include "Usul/Strings/Managed.h"

#include <limits>


using namespace CadKit::OSG::Glue;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

ReadFile::ReadFile() : 
  _progress ( nullptr ), 
  _delegate ( nullptr ), 
  _pin(),
  _startTime ( System::DateTime::Now )
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

CadKit::OSG::Glue::Node^ ReadFile::readNodeFile ( System::String ^name, System::Object^ caller )
{
  // Clear the members.
  this->clear();

  try
  {
    _startTime = System::DateTime::Now;
    _progress = dynamic_cast < CadKit::Interfaces::IProgressBar ^ > ( caller );

    // Make native function pointer to the progress interface.
    NativeProgressCallback progress ( this->_makeProgressCallback() );

    // Make the callback.
    OsgTools::IO::Reader::FunctionCallback callback ( progress );

    // Get file name.
    const std::string file ( Usul::Strings::convert ( name ) );

    // Read the file.
    OsgTools::IO::Reader reader;
    reader.callback ( &callback );
    reader.read ( file );

    // Save the node.
    CadKit::OSG::Glue::Node ^node = gcnew CadKit::OSG::Glue::Node( reader.node() );

    // Return the node.
    return node;
  }
  catch ( const std::exception &e )
  {
    System::String^ message = gcnew System::String ( ( e.what() ) ? e.what() : "" );
    throw gcnew System::Exception ( System::String::Format ( "Error 1431367384: {0}", message ) );
  }
  catch ( ... )
  {
    throw gcnew System::Exception ( System::String::Format ( "Error 1041641937: Failed to read file: {0}", name ) );
  }
  finally
  {
    // Clear the members.
    this->clear();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when there is progress.
//
///////////////////////////////////////////////////////////////////////////////

void ReadFile::_progressNotify ( const std::string &name, unsigned long bytes, unsigned long total )
{
  if ( nullptr != _progress )
  {
    System::String^ file = gcnew System::String ( name.c_str() );

    const float fraction ( static_cast<float> ( bytes ) / static_cast<float> ( total ) );
    System::String^ percent = System::String::Format ( "{0}", fraction.ToString ( "P" ) );

    System::TimeSpan span ( System::DateTime::Now.Subtract ( _startTime ) );
    System::TimeSpan remaining ( System::TimeSpan ( span.Ticks / fraction ).Subtract ( span ) );
    array<System::Char>^ chars = { '.' };
    array<System::String^>^ times = remaining.ToString()->Split ( chars );

    _progress->Text = System::String::Format ( "Reading: {0}       {1}       Time Remaining: {2}", file, percent, times[0] );
    _progress->Value = static_cast<int> ( bytes );
  }
}

  
///////////////////////////////////////////////////////////////////////////////
//
//  See if there is an appropriate reader.
//
///////////////////////////////////////////////////////////////////////////////

bool ReadFile::hasReader ( System::String^ name )
{
  const std::string file ( Usul::Strings::convert ( name ) );
  return OsgTools::IO::Reader::hasReader ( file );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return list of filters.
//
///////////////////////////////////////////////////////////////////////////////

CadKit::Interfaces::Filters^ ReadFile::filters()
{
  CadKit::Interfaces::Filters^ filters = gcnew CadKit::Interfaces::Filters();
  try
  {
    OsgTools::IO::Reader::Filters f ( OsgTools::IO::Reader::filters() );
    for ( OsgTools::IO::Reader::Filters::const_iterator i = f.begin(); i != f.end(); ++i )
    {
      filters->Add ( gcnew CadKit::Interfaces::Filter ( gcnew System::String ( i->first.c_str() ), gcnew System::String ( i->second.c_str() ) ) );
    }
  }
  catch ( const std::exception &e )
  {
    System::Console::WriteLine ( System::String::Format ( "Error 4193416035: {0}", gcnew System::String ( e.what() ) ) );
  }
  catch ( ... )
  {
    System::Console::WriteLine ( "Error 3853814285: Unnown exception caught when determining filters" );
  }
  return filters;
}
