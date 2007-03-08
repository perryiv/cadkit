
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "DistributedGlue.h"

#include "Threads/OpenThreads/Mutex.h"
#include "Usul/Threads/Mutex.h"

#include "Minerva/Core/Layers/Layer.h"

#include "Usul/Strings/ManagedToNative.h"

#include <string>

using namespace DT::Minerva::Plugins::Document::Glue;


///////////////////////////////////////////////////////////////////////////////
//
//  Set for multi-threaded. This is global because the sooner we set this, 
//  the better. Setting in main() may be too late.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Threads::SetMutexFactory factory ( &Threads::OT::newOpenThreadsMutex );


namespace Detail
{
  template < class NativeStrings >
  DistributedGlue::Strings^ toManagedStrings( const NativeStrings& nativeStrings )
  {
    DistributedGlue::Strings ^strings = gcnew DistributedGlue::Strings;

    for( typename NativeStrings::const_iterator iter = nativeStrings.begin(); iter != nativeStrings.end(); ++iter )
    {
      strings->Add ( gcnew System::String ( iter->c_str() ) );
    }

    return strings;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

DistributedGlue::DistributedGlue() : _controller ( 0x0 ),
_host( nullptr ),
_database( nullptr ),
_user( nullptr ),
_password( nullptr )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

DistributedGlue::~DistributedGlue()
{
  this->disconnect();
}


void DistributedGlue::connect( )
{
  try
  {
    std::string database ( Usul::Strings::convert ( _database ) );
    std::string user ( Usul::Strings::convert ( _user ) );
    std::string password ( Usul::Strings::convert ( _password ) );
    std::string host ( Usul::Strings::convert ( _host ) );
    _controller = new ::Minerva::Core::GUI::Controller( database, user, password, host );
    Usul::Pointers::reference(_controller);
  }
  catch ( const std::exception &e )
  {
    System::Console::WriteLine( "Error 2003950136:" + gcnew System::String ( e.what() ) );
  }
  catch ( ... )
  {
    System::Console::WriteLine( "Error 2032387636: Unknown exception caught." );
  }
}


void DistributedGlue::disconnect()
{
  try
  {
    Usul::Pointers::unreference(_controller);
    _controller = 0x0;
  }
  catch ( const std::exception &e )
  {
    System::Console::WriteLine( "Error 2003950136:" + gcnew System::String ( e.what() ) );
  }
  catch ( ... )
  {
    System::Console::WriteLine( "Error 2032387636: Unknown exception caught." );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  
//
///////////////////////////////////////////////////////////////////////////////

int DistributedGlue::connectToSession(System::String ^name)
{
  try
  {
    return _controller->connectToSession( Usul::Strings::convert( name ) );
  }
  catch ( const std::exception &e )
  {
    System::Console::WriteLine( "Error 2003950136:" + gcnew System::String ( e.what() ) );
  }
  catch ( ... )
  {
    System::Console::WriteLine( "Error 2032387636: Unknown exception caught." );
  }
  
  return -1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  
//
///////////////////////////////////////////////////////////////////////////////

void DistributedGlue::deleteSession()
{
  try
  {
    _controller->deleteSession();
  }
  catch ( const std::exception &e )
  {
    System::Console::WriteLine( "Error 2003950136:" + gcnew System::String ( e.what() ) );
  }
  catch ( ... )
  {
    System::Console::WriteLine( "Error 2032387636: Unknown exception caught." );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  
//
///////////////////////////////////////////////////////////////////////////////

DistributedGlue::Strings^ DistributedGlue::getAvailableSessions()
{
  try
  {
    ::Minerva::Core::GUI::Controller::Strings result ( _controller->getAvailableSessions() );
    return Detail::toManagedStrings( result );
  }
  catch ( const std::exception &e )
  {
    System::Console::WriteLine( "Error 2003950136:" + gcnew System::String ( e.what() ) );
  }
  catch ( ... )
  {
    System::Console::WriteLine( "Error 2032387636: Unknown exception caught." );
  }
  
  return gcnew Strings();
}


///////////////////////////////////////////////////////////////////////////////
//
//  
//
///////////////////////////////////////////////////////////////////////////////

void DistributedGlue::removeLayer( CadKit::Interfaces::ILayer ^ layer )
{
  try
  {
    DT::Minerva::Interfaces::ILayer ^layerPtr = dynamic_cast < DT::Minerva::Interfaces::ILayer ^ > ( layer );

    if( nullptr != layerPtr )
    {
      ::Minerva::Core::Layers::Layer::RefPtr base ( reinterpret_cast < ::Minerva::Core::Layers::Layer * > ( layerPtr->layerPtr().ToPointer() ) );
      _controller->removeLayer( base->layerID() );
    }
  }
  catch ( const std::exception &e )
  {
    System::Console::WriteLine( "Error 2003950136:" + gcnew System::String ( e.what() ) );
  }
  catch ( ... )
  {
    System::Console::WriteLine( "Error 2032387636: Unknown exception caught." );
  }
}




///////////////////////////////////////////////////////////////////////////////
//
//  
//
///////////////////////////////////////////////////////////////////////////////

void DistributedGlue::modifyLayer( CadKit::Interfaces::ILayer ^layer )
{
  try
  {
    DT::Minerva::Interfaces::ILayer ^layerPtr = dynamic_cast < DT::Minerva::Interfaces::ILayer ^ > ( layer );

    if( nullptr != layerPtr )
    {
      ::Minerva::Core::Layers::Layer::RefPtr base ( reinterpret_cast < ::Minerva::Core::Layers::Layer * > ( layerPtr->layerPtr().ToPointer() ) );
      _controller->modifyLayer( base.get() );  
    }
  }
  catch ( const std::exception &e )
  {
    System::Console::WriteLine( "Error 2003950136:" + gcnew System::String ( e.what() ) );
  }
  catch ( ... )
  {
    System::Console::WriteLine( "Error 2032387636: Unknown exception caught." );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  
//
///////////////////////////////////////////////////////////////////////////////

void DistributedGlue::startAnimation(float speed, bool accumulate, bool timeWindow, int numDays)
{
  try
  {
    _controller->startAnimation(speed, accumulate, true, timeWindow, numDays );
  }
  catch ( const std::exception &e )
  {
    System::Console::WriteLine( "Error 2003950136:" + gcnew System::String ( e.what() ) );
  }
  catch ( ... )
  {
    System::Console::WriteLine( "Error 2032387636: Unknown exception caught." );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  
//
///////////////////////////////////////////////////////////////////////////////

void DistributedGlue::stopAnimation()
{
  try
  {
    _controller->stopAnimation();
  }
  catch ( const std::exception &e )
  {
    System::Console::WriteLine( "Error 1762543886:" + gcnew System::String ( e.what() ) );
  }
  catch ( ... )
  {
    System::Console::WriteLine( "Error 1785512636: Unknown exception caught." );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  
//
///////////////////////////////////////////////////////////////////////////////


void DistributedGlue::showLayer ( CadKit::Interfaces::ILayer ^ layer )
{
  try
  {
    DT::Minerva::Interfaces::ILayer ^layerPtr = dynamic_cast < DT::Minerva::Interfaces::ILayer ^ > ( layer );

    if( nullptr != layerPtr )
    {
      ::Minerva::Core::Layers::Layer::RefPtr base ( reinterpret_cast < ::Minerva::Core::Layers::Layer * > ( layerPtr->layerPtr().ToPointer() ) );
      _controller->showLayer( base.get() );
    }
  }
  catch ( const std::exception &e )
  {
    System::Console::WriteLine( "Error 3256653520:" + gcnew System::String ( e.what() ) );
  }
  catch ( ... )
  {
    System::Console::WriteLine( "Error 3285091020: Unknown exception caught." );
  }
}
