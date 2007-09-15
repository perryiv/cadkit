
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "MinervaReader.h"
#include "Minerva/Core/Commands/AddLayer.h"

#include "Usul/Interfaces/GUI/IProgressBarFactory.h"
#include "Usul/Factory/RegisterCreator.h"
#include "Usul/Factory/ObjectFactory.h"

#include "Serialize/XML/Deserialize.h"

using namespace Minerva::Document;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

MinervaReader::MinervaReader( const std::string& filename, Unknown* caller, MinervaDocument* doc ) :
_filename ( filename ),
_caller ( caller ),
_document ( doc )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

MinervaReader::~MinervaReader()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the file.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaReader::operator()()
{
  Usul::Factory::ObjectFactory::instance().add ( new Usul::Factory::TypeCreator<MinervaDocument> ( "MinervaDocument" ) );
  Serialize::XML::deserialize ( _filename, *_document );

  // Make a copy.
  typedef Minerva::Document::MinervaDocument::Layers Layers;
  Layers layers ( _document->layers() );

  // Clear what we have.
  _document->layers().clear ();

  // Check to see if our caller can create our progress bar.
  Usul::Interfaces::IProgressBarFactory::QueryPtr factory ( _caller );

  // Re-add as commands.  This will send them to the database if we should.
  for( Layers::iterator iter = layers.begin(); iter != layers.end(); ++iter )
  {
    Usul::Interfaces::IUnknown::QueryPtr progress ( factory.valid() ? factory->createProgressBar() : 0x0 );
    //Usul::Interfaces::IUnknown::QueryPtr progress;
    Minerva::Core::Commands::AddLayer::RefPtr command ( new Minerva::Core::Commands::AddLayer ( 0x0, *iter, progress.get () ) );
    command->execute ( _document->queryInterface ( Usul::Interfaces::IUnknown::IID ) );
  }
}
