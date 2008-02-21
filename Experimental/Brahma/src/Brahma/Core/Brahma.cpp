
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Aashish Chaudhary
//
///////////////////////////////////////////////////////////////////////////////

#include "Brahma.h"

#include "Serialize/XML/TypeWrapper.h"

#include "Usul/Pointers/Pointers.h"

///////////////////////////////////////////////////////////////////////////////
//
// Get base objects. 
//
///////////////////////////////////////////////////////////////////////////////

const Brahma::Core::Brahma::BaseObjects& Brahma::Core::Brahma::objects() const
{
  return _baseObjects;
}


///////////////////////////////////////////////////////////////////////////////
//
// Set config file.
//
///////////////////////////////////////////////////////////////////////////////

void Brahma::Core::Brahma::setConfig( const std::string &config )
{
  _inputConfig = config;
}


///////////////////////////////////////////////////////////////////////////////
//
// Serialize. 
//
///////////////////////////////////////////////////////////////////////////////

void Brahma::Core::Brahma::serialize( const std::string& config )
{
  Serialize::XML::serialize( config, "objects", _baseObjects.begin(), _baseObjects.end() ); 
}


///////////////////////////////////////////////////////////////////////////////
//
// Deserialize. 
//
///////////////////////////////////////////////////////////////////////////////

void Brahma::Core::Brahma::deserialize()
{ 
  std::string config;
  Usul::File::contents( _inputConfig, false, config );
  std::istringstream in ( config );    
  Serialize::XML::deserialize( in, _baseObjects );
}


///////////////////////////////////////////////////////////////////////////////
//
// Query for a object. 
//
///////////////////////////////////////////////////////////////////////////////

Brahma::Core::Brahma::BaseObject Brahma::Core::Brahma::query( const std::string &name )
{
  return 0x00;
}


///////////////////////////////////////////////////////////////////////////////
//
// Releasing the resources. 
//
///////////////////////////////////////////////////////////////////////////////

void Brahma::Core::Brahma::clean()
{
  Serialize::XML::Factory::instance ( 0x0 );
}