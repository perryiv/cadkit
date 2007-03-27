
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Test program for SQLiteWrap.
//
///////////////////////////////////////////////////////////////////////////////

#include "SQLiteWrap/Vector.h"

#include "Usul/File/Path.h"
#include "Usul/File/Remove.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/IO/Redirect.h"
#include "Usul/Policies/Update.h"
#include "Usul/Threads/Mutex.h"

#include <iostream>
#include <limits>


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize mutex factory.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Threads::SetMutexFactory factory ( &Usul::Threads::newSingleThreadedMutexStub );


///////////////////////////////////////////////////////////////////////////////
//
//  Run a single test.
//
///////////////////////////////////////////////////////////////////////////////

template < class T > void _test()
{
  typedef SQLiteWrap::Vector<T> Vector;
  typedef typename Vector::ValueType ValueType;
  typedef typename Vector::SizeType SizeType;

  Vector a;
  a.append ( 10 );
  a.append ( 20 );
  a.append ( 30 );
  std::cout << "a:" << std::endl; a.print ( std::cout ); std::cout << std::endl;

  Vector b ( 4 );
  std::cout << "b:" << std::endl; b.print ( std::cout ); std::cout << std::endl;
  b.set ( 0, 11 );
  b.set ( 1, 12 );
  b.set ( 2, 13 );
  b.set ( 3, 14 );
  std::cout << "b:" << std::endl; b.print ( std::cout ); std::cout << std::endl;

  b = a;
  std::cout << "b:" << std::endl; b.print ( std::cout ); std::cout << std::endl;

  Vector c;
  const SizeType num ( std::numeric_limits<short>::max() );
  Usul::Policies::TimeBased update ( 1000 );
  ValueType temp = ValueType();
  for ( SizeType i = 0; i < num; ++i )
  {
    temp = static_cast<ValueType> ( 10 * i );
    c.append ( temp );
    if ( true == update() )
    {
      std::cout << "c[" << i << "] = " << temp << '\r' << std::flush;
    }
  }
  std::cout << std::endl;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Run the test.
//
///////////////////////////////////////////////////////////////////////////////

void _run()
{
  std::cout << "-------------- double ---------------" << std::endl;
  _test<double>();

  std::cout << "-------------- int ---------------" << std::endl;
  _test<int>();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Main function.
//
///////////////////////////////////////////////////////////////////////////////

int main ( int argc, char **argv )
{
  //Usul::IO::Redirect redirect ( Usul::File::base ( argv[0] ) + ".txt", true, true );
  Usul::Functions::safeCall ( _run, "1340615910" );
  return 0;
}
