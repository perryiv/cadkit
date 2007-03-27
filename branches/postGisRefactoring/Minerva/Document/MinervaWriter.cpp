
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "MinervaWriter.h"

using namespace Minerva::Document;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

MinervaWriter::MinervaWriter( const std::string& filename, Unknown* caller, const MinervaDocument* doc ) :
_filename ( filename ),
_caller ( caller ),
_document ( const_cast < MinervaDocument* > ( doc ) )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

MinervaWriter::~MinervaWriter()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the file.
//
///////////////////////////////////////////////////////////////////////////////

void MinervaWriter::operator()()
{
  Minerva::Core::registerFactories();
  Serialize::XML::Factory::instance().add ( new Serialize::XML::TypeCreator<MinervaDocument> ( "MinervaDocument" ) );
  Serialize::XML::serialize( "MinervaDocument", *_document, _filename );

  // Create the archive.
  /*std::ofstream fout ( _filename.c_str() );
  boost::archive::xml_oarchive oa ( fout );

  Minerva::Core::registerTypes( oa );
  oa.register_type< MinervaDocument > ();

  oa << boost::serialization::make_nvp( "Document", *_document );*/
}
