
#define CADKIT_DEFINE_SL_VECTOR_STD_OSTREAM_OPERATOR
#define CADKIT_DEFINE_SL_MATRIX_STD_OSTREAM_FUNCTIONS
#define CADKIT_DEFINE_SL_MATRIX_ISTREAM_FUNCTIONS
#include "Standard/SlMatrix4.h"
#include "Standard/SlConstants.h"
using namespace CadKit;


void testMatrices()
{
  SlMatrix4d M1;
  std::cout << "M1 = \n" << M1 << std::endl;

  SlMatrix4d M2 ( SL_MATRIX4_IDENTITY_D );
  std::cout << "M2 = \n" << M2 << std::endl;

  SlMatrix4d M3 = SlMatrix4d::absolute ( M2 );
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
