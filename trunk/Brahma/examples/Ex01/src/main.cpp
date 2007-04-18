
#include "Core/Brahma.h"

#include "Scene/Node.h"

#include "Usul/Threads/Mutex.h"

const std::string INPUT_FILE_NAME( "input.xml" );
const std::string OUTPUT_FILE_NAME( "output.xml" );

Usul::Threads::SetMutexFactory factory ( &Usul::Threads::newSingleThreadedMutexStub );

int main( int argc, char** argv )
{
  Brahma::Core::Brahma::RefPtr brahma( new Brahma::Core::Brahma() );

  brahma->registerType< Brahma::Scene::Node >( "Node" );

  brahma->load( INPUT_FILE_NAME );

  brahma->deserialize();

  brahma->serialize( OUTPUT_FILE_NAME );

  return 1;
}
