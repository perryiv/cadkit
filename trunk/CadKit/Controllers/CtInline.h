
///////////////////////////////////////////////////////////////////////////////
//
//  BSD License
//  http://www.opensource.org/licenses/bsd-license.html
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without 
//  modification, are permitted provided that the following conditions are met:
//
//  - Redistributions of source code must retain the above copyright notice, 
//    this list of conditions and the following disclaimer. 
//
//  - Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution. 
//
//  - Neither the name of the CAD Toolkit nor the names of its contributors may
//    be used to endorse or promote products derived from this software without
//    specific prior written permission. 
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
//  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
//  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
//  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
//  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
//  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
//  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
//  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
//  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
//  POSSIBILITY OF SUCH DAMAGE.
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

  // Get the necessary interface.
  SlQueryPtr<ICommandLine> commandLine ( ICommandLine::IID, controller );
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
  SlQueryPtr<ITranslator> translator ( ITranslator::IID, controller );
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
