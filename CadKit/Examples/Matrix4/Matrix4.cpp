#include "Standard/SlMatrix44.h"
#include "Standard/SlMatrix44IO.h"
#include "Standard/SlConstants.h"
#include <iostream>

using namespace CadKit;


void testMatrices()
{
  SlMatrix44d M1;
  std::cout << "M1 = \n" << M1 << std::endl;

  SlMatrix44d M2 ( SL_MATRIX_44_IDENTITY_D );
  std::cout << "M2 = \n" << M2 << std::endl;

  SlMatrix44d M3 = SlMatrix44d::absolute ( M2 );
  std::cout << "M3 = \n" << M3 << std::endl;

  M3.absolute();
  std::cout << "M3 = \n" << M3 << std::endl;

  M3.rotate ( SL_PI_OVER_4_D, SlVec3d ( 1.0, 0.0, 0.0 ) );
  std::cout << "M3 = \n" << M3 << std::endl;

  SL_VERIFY ( M3.billboard() );
  std::cout << "M3 = \n" << M3 << std::endl;

  M1.translate ( SlVec3d ( 1.0, 0.0, 0.0 ) );
  std::cout << "M1 = \n" << M1 << std::endl;

  SlVec3d eye    ( 1.0, 1.0, 1.0 );
  SlVec3d center ( 0.0, 0.0, 0.0 );
  SlVec3d up     ( 0.0, 1.0, 0.0 );
  M1.lookAt ( eye, center, up );
}


int main ( int argc, char **argv )
{
  testMatrices();
  
  // Wait for user to press the 'any' key.
  std::cin.get();

  return 1;
}
