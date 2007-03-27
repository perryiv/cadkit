#include "Standard/SlMatrixNN.h"
#include "Standard/SlMatrixNNIO.h"
#include "Standard/SlConstants.h"
#include <iostream>

using namespace CadKit;


void testMatrices()
{
  const unsigned int size1 ( 6 );
  double m1[size1 * size1] = 
  {
    1.0, 2.0, 3.0, 4.0, 3.0, 2.0,
    5.0, 6.0, 7.0, 8.0, 7.0, 6.0,
    9.0, 8.0, 7.0, 6.0, 5.0, 4.0,
    5.0, 4.0, 3.0, 2.0, 1.0, 0.0,
    5.0, 6.0, 7.0, 8.0, 7.0, 6.0,
    8.0, 8.0, 4.0, 6.0, 2.0, 4.0
  };

  SlMatrixNNd M1 ( size1, m1 );
  std::cout << "A01: M1 = \n" << M1 << std::endl;

  M1.transpose();
  std::cout << "A02: M1 = \n" << M1 << std::endl;

  // Make a copy because we are going to invert it.
  SlMatrixNNd IM1 ( M1 );

  bool result ( IM1.invert() );
  std::cout << "A03: Inverse exists? " << ( ( result ) ? "yes" : "no" ) << std::endl;

  if ( result )
    std::cout << "A04: Inverse of M1 = \n" << IM1 << std::endl;

  SL_ASSERT ( m1 == M1.detach() );
}


int main ( int argc, char **argv )
{
  testMatrices();
  
  // Wait for user to press the 'any' key.
  std::cin.get();

  return 1;
}
