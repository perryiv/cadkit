
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Functions shared among the tests.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _DATABASE_SQLITE_TEST_COMMON_FUNCTIONS_H_
#define _DATABASE_SQLITE_TEST_COMMON_FUNCTIONS_H_

#include <iostream>

namespace Helper {


///////////////////////////////////////////////////////////////////////////////
//
//  Print the result.
//
///////////////////////////////////////////////////////////////////////////////

template < class ResultPtr > inline void printResult ( ResultPtr result )
{
  if ( false == result.valid() )
    return;

  const unsigned int numColumns ( result->numColumns() );
  if ( 0 == numColumns )
    return;

  std::cout << "Row";
  for ( unsigned int j = 0; j < numColumns; ++j )
  {
    std::cout << ", " << result->columnName ( j );
  }
  std::cout << '\n';

  unsigned int row ( 0 );
  while ( true == result->prepareNextRow() )
  {
    std::cout << row++;
    for ( unsigned int i = 0; i < numColumns; ++i )
    {
      std::string value;
      *result >> value;
      std::cout << ", " << value;
    }
    std::cout << std::endl;
  }
  std::cout << std::flush;
}


} // namespace Helper


#endif // _DATABASE_SQLITE_TEST_COMMON_FUNCTIONS_H_
