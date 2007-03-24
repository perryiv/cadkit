
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "MinervaReader.h"

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
  Minerva::Core::registerFactories();
  Serialize::XML::Factory::instance().add ( new Serialize::XML::TypeCreator<MinervaDocument> ( "MinervaDocument" ) );
  Serialize::XML::deserialize ( _filename, *_document );

  // Create the archive.
  /*std::ifstream fin ( _filename.c_str() );
  boost::archive::xml_iarchive ia ( fin );

  Minerva::Core::registerTypes( ia );
  ia.register_type< MinervaDocument > ();

  ia >> boost::serialization::make_nvp( "Document", *_document );*/
}
