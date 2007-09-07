
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "MinervaWriter.h"
#include "Minerva/Core/Serialize.h"

#include "Usul/Factory/RegisterCreator.h"
#include "Usul/Factory/ObjectFactory.h"

#include "Serialize/XML/Serialize.h"

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
  Usul::Factory::ObjectFactory::instance().add ( new Usul::Factory::TypeCreator<MinervaDocument> ( "MinervaDocument" ) );
  Serialize::XML::serialize( "MinervaDocument", *_document, _filename );
}
