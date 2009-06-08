
#include "Usul/Types/Types.h"

#include <stdlib.h>
#include <io.h>
#include <time.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <ctime>


int main ( int argc, char **argv )
{
  // Check arguments.
  if ( argc < 3 )
  {
    std::cout << "Usage: " << argv[0] << " <filename> <bytes>" << std::endl;
    return 1;
  }

  // Grab the arguments.
  const char *filename = argv[1];
  typedef Usul::Types::Uint64 Uint64;
  const Uint64 bytes ( static_cast<Uint64> ( ::atof ( argv[2] ) ) );

  // Make sure the size is not negative.
  if ( bytes < 0 )
  {
    std::cout << "Error: given size (bytes) is less than zero" << std::endl;
    return 1;
  }

  // Make sure the file does not exist.
  if ( 0 == ::_access ( filename, 0 ) )
  {
    std::cout << "Error: file '"<< filename << "' already exists" << std::endl;
    return 1;
  }

  // Open the file.
  std::ofstream out ( filename, std::ios::binary );

  // Make sure it opened.
  if ( false == out.is_open() )
  {
    std::cout << "Error: failed to open '"<< filename << "' for writing" << std::endl;
    return 1;
  }

  // Seed random number generator.
  ::srand ( static_cast<int> ( ::time ( 0x0 ) ) );

  // Assign random bytes to vector.
  typedef std::vector<int> Data;
  Data data ( 4096 );
  for ( Data::size_type i = 0; i < data.size(); ++i )
    data[i] = ::rand();

  // Initialize clock and get current time.
  ::clock();
  Uint64 lastTime ( ::clock() );

  // Loop through all the bytes.
  Uint64 remaining = bytes;
  while ( remaining > 0 )
  {
    // Determine size to write.
    const std::streamsize sizeToWrite ( static_cast<std::streamsize> ( std::min<Uint64> ( data.size() * sizeof ( Data::size_type ), remaining ) ) );

    // Write the bytes to the file.
    out.write ( reinterpret_cast < const char * > ( &data[0] ), sizeToWrite );

    // Decrement remaining amount.
    remaining -= sizeToWrite;

    // Feedback.
    Uint64 currentTime ( ::clock() );
    Uint64 elapsedTime ( currentTime - lastTime );
    if ( elapsedTime > 1000 )
    {
      lastTime = currentTime;
      std::cout << "Remaining: " << remaining << '\r';
    }
  }

  // Clear the line.
  std::cout << std::endl;

  // It worked.
  return 0;
}
