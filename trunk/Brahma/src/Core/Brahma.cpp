
#include "Brahma.h"

#include "Serialize/XML/TypeWrapper.h"

#include "Usul/Pointers/Pointers.h"

void Brahma::Core::Brahma::load( const std::string &config )
{
  _inputConfig = config;
}

void Brahma::Core::Brahma::serialize( const std::string& config )
{
  Serialize::XML::serialize( config, "objects", _baseObjects.begin(), _baseObjects.end() ); 
}

void Brahma::Core::Brahma::deserialize()
{ 
  std::string config;
  Usul::File::contents( _inputConfig, false, config );
  std::istringstream in ( config );    
  Serialize::XML::deserialize( in, _baseObjects );
}

Brahma::Core::Brahma::BaseObject Brahma::Core::Brahma::query( const std::string &name )
{
  return 0x00;
}

void Brahma::Core::Brahma::clean()
{
  Serialize::XML::Factory::instance ( 0x0 );
}