
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  CtInline: Inline functions.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_CONTROLLER_INLINE_FUNCTIONS_H_
#define _CADKIT_CONTROLLER_INLINE_FUNCTIONS_H_

#ifdef _WIN32
# pragma warning(disable:4786) // Truncated debug names.
#endif

#include "Interfaces/ICommandLine.h"
#include "Interfaces/ITranslator.h"

#include "Standard/SlQueryPtr.h"
#include "Standard/SlAssert.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include <list>
# include <string>
# include <iostream>
#endif

namespace CadKit
{
///////////////////////////////////////////////////////////////////////////////
//
//  Main function for a translator.
//
///////////////////////////////////////////////////////////////////////////////

inline bool translate ( CadKit::IUnknown *controller, 
                        CadKit::IUnknown *source, 
                        CadKit::IUnknown *target, 
                        int argc, 
                        char **argv, 
                        bool keepGoing )
{
  // It's ok to have a null target.
  if ( NULL == controller || NULL == source )
  {
    SL_ASSERT ( 0 ); // Heads up.
    return false;
  }

  // We reference all three interfaces to make sure they aren't deleted as we
  // loop through the input files. The caller may not have referenced them yet.
  SlRefPtr<IUnknown> dummy1 ( controller );
  SlRefPtr<IUnknown> dummy2 ( source );
  SlRefPtr<IUnknown> dummy3 ( target );

  // Get the necessary interface.
  SlQueryPtr<ICommandLine> commandLine ( controller );
  if ( commandLine.isNull() )
  {
    std::cout << "Failed to obtain interface to ICommandLine from given controller." << std::endl;
    return false;
  }

  // Check the arguments.
  if ( false == commandLine->checkArguments ( argc, (const char **) argv ) )
  {
    std::cout << commandLine->getUsageString ( argv[0], "jt" ).c_str() << std::endl;
    return false;
  }

  // Parse the arguments.
  ICommandLine::Args args;
  commandLine->parseArguments ( argc, (const char **) argv, args );

  // Get the necessary interface.
  SlQueryPtr<ITranslator> translator ( controller );
  if ( translator.isNull() )
  {
    std::cout << "Failed to obtain interface to ITranslator from given controller." << std::endl;
    return false;
  }

  // Loop through all the input files.
  for ( ICommandLine::Args::iterator f = args.begin(); f != args.end(); ++f )
  {
    // Translate the files.
    bool result = translator->translate ( *f, source, target );

    // If there's an error, only return if the caller wants us to.
    if ( false == result && false == keepGoing )
      return false;
  }

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Main function for a translator.
//
///////////////////////////////////////////////////////////////////////////////

template <class C, class S, class T> inline bool translate ( C *controller, S *source, T *target, int argc, char **argv, bool keepGoing )
{
  // It's ok to have a null target.
  if ( NULL == controller || NULL == source )
  {
    SL_ASSERT ( 0 ); // Heads up.
    return false;
  }

  // Call the other one.
  return CadKit::translate ( controller->queryInterface ( CadKit::IUnknown::IID ),
                             source->queryInterface ( CadKit::IUnknown::IID ),
                             ( target ) ? target->queryInterface ( CadKit::IUnknown::IID ) : NULL,
                             argc,
                             argv,
                             keepGoing );
}


}; // namespace CadKit


#endif // _CADKIT_CONTROLLER_INLINE_FUNCTIONS_H_
