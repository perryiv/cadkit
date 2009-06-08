
#ifdef _WIN32
#pragma warning(disable:4786) // Truncated debug names.
#endif

#include "Standard/SlMatrix44.h"
#include "Standard/SlMatrix44IO.h"
#include "Standard/SlMatrixStack.h"
#include "Standard/SlConstants.h"
#include <iostream>
#include <time.h>

using namespace CadKit;

#define RANDOM ( (Type) ( ::rand() / 100 ) )
#define RANDOM_VEC3 RANDOM, RANDOM, RANDOM
#define RANDOM_MATRIX\
  RANDOM, RANDOM, RANDOM, RANDOM, \
  RANDOM, RANDOM, RANDOM, RANDOM, \
  RANDOM, RANDOM, RANDOM, RANDOM, \
  RANDOM, RANDOM, RANDOM, RANDOM


void testMatrices()
{
  SlMatrix44d M1;
  std::cout << "A01: M1 = \n" << M1 << std::endl;

  SlMatrix44d M2 ( SL_MATRIX_44_IDENTITY_D );
  std::cout << "A02: M2 = \n" << M2 << std::endl;

  SlMatrix44d M3 = SlMatrix44d::absolute ( M2 );
  std::cout << "A03: M3 = \n" << M3 << std::endl;

  M3.absolute();
  std::cout << "A04: M3 = \n" << M3 << std::endl;

  M3.rotate ( SL_PI_OVER_4_D, SlVec3d ( 1.0, 0.0, 0.0 ) );
  std::cout << "A05: M3 = \n" << M3 << std::endl;

  SL_VERIFY ( M3.billboard() );
  std::cout << "A06: M3 = \n" << M3 << std::endl;

  M1.translate ( SlVec3d ( 1.0, 0.0, 0.0 ) );
  std::cout << "A07: M1 = \n" << M1 << std::endl;

  SlVec3d eye    ( 1.0, 1.0, 1.0 );
  SlVec3d center ( 0.0, 0.0, 0.0 );
  SlVec3d up     ( 0.0, 1.0, 0.0 );
  M1.lookAt ( eye, center, up );
  std::cout << "A08: M1 = \n" << M1 << std::endl;
}


void testMatrices2()
{
  ::srand ( (unsigned int) ::time ( 0x0 ) );

  typedef long Type;

  SlMatrix44<Type> M1 ( RANDOM_MATRIX );
  std::cout << "B01: M1 = " << std::endl;
  CadKit::write ( M1, stdout, "%10d" );
  std::cout << std::endl << std::endl;

  SlMatrix44<Type> M2 ( RANDOM_MATRIX );
  std::cout << "B03: M2 = " << std::endl;
  CadKit::write ( M2, stdout, "%10d" );
  std::cout << std::endl << std::endl;

  SlMatrix44<Type> M3 ( RANDOM_MATRIX );
  std::cout << "B04: M3 = " << std::endl;
  CadKit::write ( M3, stdout, "%10d" );
  std::cout << std::endl << std::endl;

  SlMatrix44<Type> M4 ( RANDOM_MATRIX );
  std::cout << "B05: M4 = " << std::endl;
  CadKit::write ( M4, stdout, "%10d" );
  std::cout << std::endl << std::endl;

  SL_ASSERT ( M1 * M2 != M2 * M1 );
  
  SlMatrix44<Type> A1 = M1 * M2;
  SL_ASSERT ( A1 * M3 == M1 * M2 * M3 );
  A1 *= M3;
  SL_ASSERT ( A1 * M4 == M1 * M2 * M3 * M4 );
  A1 *= M4;
  SL_ASSERT ( A1 == M1 * M2 * M3 * M4 );

  SlMatrix44<Type> A2 ( M1 * M2 * M3 * M4 );
  std::cout << "B06: A2 = " << std::endl;
  CadKit::write ( A2, stdout, "%15d" );
  std::cout << std::endl << std::endl;

  SlMatrix44<Type> A3 ( ( M1 * M2 ) * ( M3 * M4 ) );
  std::cout << "B07: A3 = " << std::endl;
  CadKit::write ( A3, stdout, "%15d" );
  std::cout << std::endl << std::endl;

  SL_ASSERT ( A2 == A3 );

  SlVec3<Type> v ( RANDOM_VEC3 );
  SL_ASSERT ( A1 * v == M1 * ( M2 * ( M3 * ( M4 * v ) ) ) );

  SlMatrixStack < SlMatrix44 < Type > > stack;
  stack.push ( M1 );
  stack.push ( M2 );
  stack.push ( M3 );
  stack.push ( M4 );

  SlMatrix44<Type> A4;
  stack.multiply ( A4 );
  SL_ASSERT ( A4 == M1 * M2 * M3 * M4 );
}


void testMatrices3()
{
  SlMatrix44<long> A 
  (
    1, 2, 3, 4,
    2, 3, 4, 5,
    3, 4, 5, 6,
    4, 5, 6, 7
  );

  SlMatrix44<long> B
  (
    1, 2, 1, 3,
    2, 3, 3, 1,
    4, 6, 5, 1,
    0, 4, 2, 3
  );

  SlMatrix44<long> C = A * B;
  std::cout << "C07: C = " << std::endl;
  CadKit::write ( C, stdout, "%15d" );
  std::cout << std::endl << std::endl;

  SlMatrix44<long> D = B * A;
  std::cout << "C08: D = " << std::endl;
  CadKit::write ( D, stdout, "%15d" );
  std::cout << std::endl << std::endl;
}


int main ( int argc, char **argv )
{
  std::cout.sync_with_stdio();

  testMatrices();
  testMatrices2();
  testMatrices3();
  
  // Wait for user to press the 'any' key.
  std::cin.get();

  return 1;
}
