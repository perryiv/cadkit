
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Core/Functions/ReadFile.h"
#include "Minerva/Core/Factory/Readers.h"

#include "Usul/Components/Manager.h"
#include "Usul/Documents/Manager.h"
#include "Usul/File/Path.h"
#include "Usul/Interfaces/GUI/IProgressBarFactory.h"

///////////////////////////////////////////////////////////////////////////////
//
//  Read the file.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown* Minerva::Core::Functions::readFile ( const std::string& filename )
{
  const std::string ext ( Usul::File::extension ( filename ) );
  
  // Check the registered readers first.
  Usul::Interfaces::IUnknown::QueryPtr unknown ( Minerva::Core::Factory::Readers::instance().create ( ext ) );
  
  // If we didn't find one, ask the document manager.
  if ( false == unknown.valid() )
  {
    unknown = Usul::Interfaces::IUnknown::QueryPtr ( Usul::Documents::Manager::instance().find ( filename ).document.get() );
  }
  
  Usul::Interfaces::IRead::QueryPtr read ( unknown );
  if ( read.valid() )
  {
    Usul::Interfaces::IProgressBarFactory::QueryPtr factory ( Usul::Components::Manager::instance().getInterface ( Usul::Interfaces::IProgressBarFactory::IID ) );
    Usul::Interfaces::IUnknown::QueryPtr unknown ( factory->createProgressBar().get() );
    read->read ( filename, 0x0, unknown.get() );
  }
  
  read = static_cast<Usul::Interfaces::IRead*> ( 0x0 );
  return unknown.release();
}
